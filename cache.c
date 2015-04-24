/* ********************************************
 * 
 * cache.c
 *
 * This file does the cache manipulations.
 *
 * Date: 31 March 2015
 * 
 * Author: Jacob Resman and Healy Fuess
 * 
 ***********************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "cache.h"

/************************************************************/

void init_cache() {

    int i,j;
    l1_cache_lines = l1_cache_size/l1_cache_block_size/l1_cache_assoc;
    l2_cache_lines = l2_cache_size/l2_cache_block_size/l2_cache_assoc;

    double temp = (log ((double) l1_cache_lines) / log (2.0));
    shift_amount_l1 = (int)( (L1_OFFSET + (temp) ));

    temp = (log ((double) l2_cache_lines) / log (2.0));
    shift_amount_l2 = (int)( (L2_OFFSET + (temp) ));

    l1_dcache = malloc(sizeof(cache_entry *)*(l1_cache_lines));
    l1_icache = malloc(sizeof(cache_entry *)*(l1_cache_lines));
    l2_cache  = malloc(sizeof(cache_entry *)*(l2_cache_lines));

    //Define Cleared Cache Entry
    cache_entry *blank_entry;
    cache_entry *curr;

    //Setup Empty L1 Inst and Data Cache
    for ( i = 0; i < l1_cache_lines; i++){
        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        blank_entry->tag = 0;
        blank_entry->prev = NULL;
        blank_entry->valid = 0;
        blank_entry->dirty = 0;
        l1_icache[i] = blank_entry;
        curr = l1_icache[i];
        for (j = 0; j < l1_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            blank_entry->tag = 0;
            blank_entry->valid = 0;
            blank_entry->dirty = 0;
            blank_entry->next = NULL;
            curr->next = blank_entry;
            blank_entry->prev = curr;
            curr = curr->next;
        }

        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        blank_entry->tag = 0;
        blank_entry->valid = 0;
        blank_entry->dirty = 0;
        l1_dcache[i] = blank_entry;
        curr = l1_dcache[i];
        for (j = 0; j < l1_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            blank_entry->tag = 0;
            blank_entry->valid = 0;
            blank_entry->dirty = 0;
            curr->next = blank_entry;
            curr->next->prev = curr;
            curr = curr->next;
        }    
    }

    //Setup Empty L2 Cache
    for ( i = 0; i < l2_cache_lines; i++){
        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        blank_entry->tag = 0;
        blank_entry->valid = 0;
        blank_entry->dirty = 0;
        l2_cache[i] = blank_entry;
        curr = l2_cache[i];
        for (j = 0; j < l2_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            blank_entry->tag = 0;
            blank_entry->valid = 0;
            blank_entry->dirty = 0;
            curr->next = blank_entry;
            curr->next->prev = curr;
            curr = curr->next;
        }    
    }
}

void perform_access(unsigned long long int addr, unsigned int byteSize, char op) {

    unsigned long long int endAddress = addr + (byteSize-1);
    unsigned long long int waStartAddress = (addr & 0xFFFFFFFFFFFC); //word aligned address
    unsigned long long int dirtyAddressl1;
    unsigned long long int dirtyAddressl2;

    // printf("Address: %Lx\n", addr );

    switch(op){
        case 'I':
        inst_refs++;
        flush_counter++;
        break;
        case 'W':
        write_refs++;
        break;
        case 'R':
        read_refs++;
    }

    while(waStartAddress <= endAddress) {
        //Perform Access Based on Operation
        // printf("Address Refernced: %Lx\n", waStartAddress);
        switch(op){
            case 'I':
                if(!check_inst_cache(waStartAddress)){
                    l1_imisses++;
                    execution_time += L1_MISS_TIME;
                    inst_cycle += L1_MISS_TIME;
                    if (!check_l2_cache(waStartAddress)){
                        //Copy from MM to L2 and L1
                        l2_misses++;
                        l2_transfers++;
                        l1_itransfers ++;
                        execution_time += L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        inst_cycle += L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;

                        if (is_dirtykickout_l2(waStartAddress)){
                            execution_time += MEM_TO_L2;
                            inst_cycle += MEM_TO_L2;
                            l2_transfers++;      
                        }

                        insert_l2(waStartAddress,0);
                        insert_inst(waStartAddress);
                    } else {
                        //Copy from L2 to L1
                        l2_hits ++;
                        l1_itransfers ++;
                        adjust_LRU_l2(waStartAddress);
                        insert_inst(waStartAddress);
                        execution_time += L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        inst_cycle += L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                    }

                } else {
                    l1_ihits ++;
                    adjust_LRU_l1i(waStartAddress);
                    //Execution Time from L1
                    execution_time += L1_HIT_TIME;
                    inst_cycle += L1_HIT_TIME;
                } 
                break;
            case 'W':
                // printf("Level L1d access addr = %Lx ", waStartAddress);
                if(check_data_cache(waStartAddress) != 1){
                    l1_dmisses++;
                    execution_time += L1_MISS_TIME;
                    write_cycle += L1_MISS_TIME;
                    l1_dtransfers++;

                    dirtyAddressl1 = is_dirtykickout_l1(waStartAddress);
                    dirtyAddressl2 = is_dirtykickout_l2(dirtyAddressl1);

                    //See if dirty kickouts occur
                    if (dirtyAddressl1){
                        
                        if (check_l2_cache(dirtyAddressl1)){
                            l2_hits++;                      
                            mark_dirty_l2(dirtyAddressl1);
                            adjust_LRU_l2(dirtyAddressl1);
                            execution_time += L2_TO_L1 + L2_HIT_TIME;
                            write_cycle += L2_TO_L1 + L2_HIT_TIME;
                        } else {
                            if (dirtyAddressl2){
                                execution_time += MEM_TO_L2;
                                write_cycle += MEM_TO_L2;
                            }
                            l2_transfers++;
                            l2_misses++;
                            insert_l2(dirtyAddressl1, 1);
                            execution_time += L2_TO_L1 + L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME;
                            write_cycle += L2_TO_L1 + L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME;
                        }
                    }

                    if (!check_l2_cache(waStartAddress)){
 
                        dirtyAddressl2 = is_dirtykickout_l2(waStartAddress);

                        if (dirtyAddressl2){
                            execution_time += MEM_TO_L2;
                            write_cycle += MEM_TO_L2;
                        }

                        //Execution Time from MM
                        l2_misses++;
                        l2_transfers++;
                        execution_time += L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        write_cycle += L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        insert_l2(waStartAddress, 0);
                        write_data(waStartAddress);
                    } else {
                        //Copy from L2 and Mark Dirty
                        l2_hits ++;
                        execution_time += L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        write_cycle += L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        write_data(waStartAddress);
                        adjust_LRU_l2(waStartAddress);
                    }

                } else {
                    //In Cache Just Mark Dirty
                    adjust_LRU_l1d(waStartAddress);
                    mark_dirty_l1(waStartAddress);
                    l1_dhits ++;
                    execution_time += L1_HIT_TIME;
                    write_cycle += L1_HIT_TIME;
                } 
                break;
            case 'R':
                // printf("Level L1d access addr = l2_kickouts%Lx ", waStartAddress);
                if(check_data_cache(waStartAddress) != 1){
                    l1_dmisses++;
                    execution_time += L1_MISS_TIME;
                    read_cycle += L1_MISS_TIME;
                    l1_dtransfers++;
                    // printf("L1 D MISS\n");

                    dirtyAddressl1 = is_dirtykickout_l1(waStartAddress);
                    dirtyAddressl2 = is_dirtykickout_l2(dirtyAddressl1);

                    //See if dirty kickouts occur
                    if (dirtyAddressl1){
                        
                        if (check_l2_cache(dirtyAddressl1)){
                            l2_hits++;
                            mark_dirty_l2(dirtyAddressl1);
                            adjust_LRU_l2(dirtyAddressl1);
                            execution_time += L2_TO_L1 + L2_HIT_TIME;
                            read_cycle += L2_TO_L1 + L2_HIT_TIME;
                        } else {
                            if (dirtyAddressl2){
                                execution_time += MEM_TO_L2;
                                read_cycle += MEM_TO_L2;
                            }
                            l2_transfers++;
                            insert_l2(dirtyAddressl1, 1);
                            execution_time += L2_TO_L1 + L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME;
                            read_cycle += L2_TO_L1 + L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME;
                            l2_misses++;
                        }
                    }

                    //Insert New Address   
                    if (!check_l2_cache(waStartAddress)){

                        dirtyAddressl2 = is_dirtykickout_l2(waStartAddress);

                        if (dirtyAddressl2){
                            execution_time += MEM_TO_L2;
                            read_cycle += MEM_TO_L2;
                            // l2_transfers++;
                        }
                        
                        l2_misses++;
                        l2_transfers++;
                        // printf("L2 MISS\n");
                        execution_time += L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        read_cycle += L2_MISS_TIME + MEM_TO_L2 + L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        insert_l2(waStartAddress, 0);
                        read_data(waStartAddress);
                        //Execution Time from MM
                    } else {
                        l2_hits ++;
                        // printf("L2 HIT\n");
                        execution_time += L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        read_cycle += L2_HIT_TIME + L2_TO_L1 + L1_HIT_TIME;
                        read_data(waStartAddress);
                        adjust_LRU_l2(waStartAddress);
                        //Execution Time from L2
                    }
                    
                } else {
                    //Execution Time from L1
                    // printf("L1 D HIT\n");
                    adjust_LRU_l1d(waStartAddress);
                    l1_dhits++;
                    execution_time += L1_HIT_TIME;
                    read_cycle += L1_HIT_TIME;
                } 

                break;
        }
        waStartAddress += 4;
    } 

    //Check if Flush is Necessary
    if (((flush_counter % (380000ul)) == 0) && flush_counter != 0){
       flush_counter = 0;
       flush();
    }
}

void flush() {

    flushes ++;
    invalidates ++;
    int i;
    cache_entry *curr;

    //L1 flush
    for (i = 0; i < l1_cache_lines; i++){
        curr = l1_icache[i];
        while(curr != NULL){
            curr->valid = 0;
            curr = curr->next;
        }
    }

    //L1 flush
    for (i = 0; i < l1_cache_lines; i++){
        curr = l1_dcache[i];
        while(curr != NULL){
            if (curr->dirty && curr->valid){
                
                if (check_l2_cache(curr->address)){
                    //May Need to Update LRU
                    mark_dirty_l2(curr->address);
                    flush_time += L2_TO_L1 + L2_HIT_TIME;
                    inst_cycle += L2_TO_L1 + L2_HIT_TIME;
                    execution_time += L2_TO_L1 + L2_HIT_TIME;
                    l2_hits ++;
                    // l2_transfers++;
                } else {
                    l2_misses++;
                    flush_time += MEM_TO_L2 + L2_MISS_TIME + L2_TO_L1;
                    inst_cycle +=  MEM_TO_L2 + L2_MISS_TIME + L2_TO_L1;
                    execution_time +=  MEM_TO_L2 + L2_MISS_TIME + L2_TO_L1;

                    if (is_dirtykickout_l2(curr->address)){
                        flush_time += MEM_TO_L2 + L2_HIT_TIME;
                        inst_cycle += MEM_TO_L2 + L2_HIT_TIME;
                        execution_time += MEM_TO_L2 + L2_HIT_TIME;
                        l2_transfers++;
                    }
                    insert_l2(curr->address, 1);
                    l2_transfers++;
                }

                l1_dtransfers++;
                l1_dkickouts_flush++;
            }
            curr->valid = 0;
            curr->dirty = 0;
            curr = curr->next;
        }
    }

    //L2 flush
    for (i = 0; i < l2_cache_lines; i++){
        curr = l2_cache[i];
        while(curr != NULL){
            if (curr->dirty == 1){
                flush_time += MEM_TO_L2;
                inst_cycle += MEM_TO_L2;
                execution_time += MEM_TO_L2;
                l2_transfers++;
                l2_kickouts_flush++;
            }
            curr->valid = 0;            
            curr->dirty = 0;
            curr = curr->next;
        }
    }
}

void insert_inst(unsigned long long int addr) {

    int index;
    unsigned long long int tag;
    cache_entry * curr;
    cache_entry * new;

    switch(l1_cache_assoc){
        case 1:
            index = (addr>>L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            if (l1_icache[index]->valid == 1){
                l1_ikickouts++;
            }
            l1_icache[index]->tag = tag;
            l1_icache[index]->address = addr;
            l1_icache[index]->valid = 1;
            l1_icache[index]->dirty = 0;
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr>>L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            curr = l1_icache[index];
            new = malloc(sizeof(cache_entry));
            //Setup New Cache Entry
            new->tag = tag;
            new->address = addr;
            new->valid = 1;
            new->dirty = 0;
            new->next = curr;
            curr->prev = new;

            //Grab the Last Element
            while (curr->next != NULL){
                curr = curr->next;
            }

            if (curr->valid){
                l1_ikickouts++;
            }

            curr->prev->next = NULL;
            free(curr);
            l1_icache[index] = new;
            break;

        default:
            printf("Strange Things Occured!\n");

    }
}

void write_data(unsigned long long int addr) {

    int index;
    unsigned long long int tag;
    cache_entry * curr;
    cache_entry * new;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;

            //Insert Into L2 W is Dirty
            if (l1_dcache[index]->dirty && l1_dcache[index]->valid){
                l1_dkickouts_dirty++;
                l1_dkickouts++;
            } else if (l1_dcache[index]->valid){
                l1_dkickouts++;
            }
            l1_dcache[index]->tag = tag;
            l1_dcache[index]->dirty = 1;
            l1_dcache[index]->valid = 1;
            l1_dcache[index]->address = addr;
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            if(l1_dcache[index]->valid == 0){
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->address = addr;
                l1_dcache[index]->dirty = 1;
                l1_dcache[index]->valid = 1;
            } else {
                curr = l1_dcache[index];
                new = malloc(sizeof(cache_entry));
                //Setup New Cache Entry
                new->tag = tag;
                new->address = addr;
                new->next = curr;
                new->dirty = 1;
                new->valid = 1;
                new->prev = NULL;
                curr->prev = new;

                //Grab the Last Element
                while (curr->next != NULL){
                    curr = curr->next;
                }

                if (curr->valid && curr->dirty){
                    l1_dkickouts_dirty++;
                    l1_dkickouts++;
                } else if (curr->valid){
                    l1_dkickouts++;
                }

                curr->prev->next = NULL;
                free(curr);
                l1_dcache[index] = new;
            } 
            break;
        default:
            printf("The Aliens Came!\n");
    }
}

void read_data(unsigned long long int addr) {

    int index;
    unsigned long long int tag;
    cache_entry * curr;
    cache_entry * new;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1; //log2 l1_cache_lines -1 + offset

            //Write back to L2 if dirty
            if (l1_dcache[index]->dirty && l1_dcache[index]->valid){
                l1_dkickouts_dirty++;
                l1_dkickouts++;
            } else if (l1_dcache[index]->valid){
                l1_dkickouts++;
            }

            l1_dcache[index]->tag = tag;
            l1_dcache[index]->address = addr;
            l1_dcache[index]->dirty = 0;
            l1_dcache[index]->valid = 1;
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            if(l1_dcache[index]->valid == 0){
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->address = addr;
                l1_dcache[index]->dirty = 0;
                l1_dcache[index]->valid = 1;
            } else {
                curr = l1_dcache[index];
                new = malloc(sizeof(cache_entry));
                //Setup New Cache Entry
                new->tag = tag;
                new->address = addr;
                new->valid = 1;
                new->dirty = 0;
                new->next = curr;
                curr->prev = new;

                //Grab the Last Element
                while (curr->next != NULL){
                    curr = curr->next;
                }

                if (curr->valid && curr->dirty){
                    //If the LRU is Dirty Write Back to L2
                    l1_dkickouts_dirty++;
                    l1_dkickouts++;
                    // insert_l2(curr->address, curr->dirty);
                } else if (curr->valid){
                    l1_dkickouts++;
                }

                curr->prev->next = NULL;
                free(curr);
                l1_dcache[index] = new;
            }
            break;

        default:
            printf("Full Associative Write Data\n");
    }
}

void insert_l2(unsigned long long int addr, int dirty) {

    int index;
    unsigned long long int tag;
    cache_entry *curr;
    cache_entry * new;

    switch(l2_cache_assoc){
        case 1:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l2;

            if (l2_cache[index]->dirty && l2_cache[index]->valid){
                l2_kickouts_dirty++;
                l2_kickouts++;
            } else if (l2_cache[index]->valid) {
                l2_kickouts++;
            }

            l2_cache[index]->tag = tag;
            l2_cache[index]->dirty = dirty;
            l2_cache[index]->address = addr;
            l2_cache[index]->valid = 1;
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l2;
            if(l2_cache[index]->valid == 0){
                l2_cache[index]->tag = tag;
                l2_cache[index]->address = addr;
                l2_cache[index]->dirty = dirty;
                l2_cache[index]->valid = 1;
            } else {
                // if (!check_l2_cache(addr)){
                    curr = l2_cache[index];
                    new = malloc(sizeof(cache_entry));
                    //Setup New Cache Entry
                    new->tag = tag;
                    new->address = addr;
                    new->next = curr;
                    new->dirty = dirty;
                    new->valid = 1;
                    curr->prev = new;

                    //Grab the Last Element
                    while (curr->next != NULL){
                        curr = curr->next;
                    }

                    if (curr->dirty && curr->valid){
                        l2_kickouts_dirty++;
                        l2_kickouts++;
                    } else if (curr->valid){
                        l2_kickouts++;
                    }

                    curr->prev->next = NULL;
                    free(curr);
                    l2_cache[index] = new;
                }
            // }
            break;
        default:
            printf("Shouldn't Happen2\n");
    }
}

int check_inst_cache(unsigned long long int addr){

    int index;
    int i;
    unsigned long long int tag;
    cache_entry *curr;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            return (l1_icache[index]->tag == tag) && l1_icache[index]->valid;
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            curr = l1_icache[index];
            for (i = 0; i < l1_cache_assoc; i++){
                if (curr->tag == tag && curr->valid){
                    return 1;
                }
                curr = curr->next;
            }
            return 0;
            break;
        default: 
            printf("Fully Associative Check Data \n");
    }

    return -1;
}

int check_data_cache(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    int i;
    cache_entry *curr;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //5 = offset bits
            tag = addr >> shift_amount_l1;
            return l1_dcache[index]->tag == tag && l1_dcache[index]->valid ;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            curr = l1_dcache[index];
            for (i = 0; i < l1_cache_assoc; i++){
                if (curr->tag == tag && curr->valid){
                    return 1;
                }
                curr = curr->next;
            }
            return 0;
            break;
        default: 
            printf("This shouldn't have happend.\n");
    }

    return -1;
}

int check_l2_cache(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    int i; 
    cache_entry * curr;

    switch(l2_cache_assoc){
        case 1:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l2;
            return (l2_cache[index]->tag == tag) &&  l2_cache[index]->valid ;
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l2;
            curr = l2_cache[index];
            for (i = 0; i < l2_cache_assoc; i++) {
                if (curr->tag == tag && curr->valid){
                    return 1;
                }
                curr = curr->next;
            }
            return 0;
            break;
        default: 
            printf("This shouldn't have happend\n");
    }
    return -1;
}

unsigned long long int is_dirtykickout_l2(unsigned long long int addr){

    int index;
    cache_entry * curr;
    int i;

    switch(l2_cache_assoc){
        case 1:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //cachelined - 1
            return (l2_cache[index]->dirty && l2_cache[index]->valid) ? l2_cache[index]->address: 0;
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //cachelined - 1
            curr = l2_cache[index];
            for (i = 0; i < l2_cache_assoc; i++){
                if (curr->next != NULL){
                    curr = curr->next;
                }
            }
            return (curr->dirty && curr->valid) ? curr->address: 0;
            break;
        default: 
            printf("This shouldn't have happend! \n");
    }
    return -1;
}   

unsigned long long int is_dirtykickout_l1(unsigned long long int addr){

    int index;
    cache_entry * curr;
    int i;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            // printf("Is Dirty L1? %Lx \n", l1_dcache[index]->dirty? l1_dcache[index]->address: 0 );
            return (l1_dcache[index]->dirty && l1_dcache[index]->valid)? l1_dcache[index]->address: 0;
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            curr = l1_dcache[index];
            for (i = 0; i < l1_cache_assoc; i++){
                if (curr->next != NULL){
                    curr = curr->next;
                }
            }
            return (curr->dirty && curr->valid) ? curr->address: 0;
            break;
        default: 
            printf("This shouldn't have happend! \n");
    }
    return -1;
}

void mark_dirty_l1(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    int i;
    cache_entry *curr;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //5 = offset bits
            tag = addr >> shift_amount_l1;
            if (l1_dcache[index]->valid && l1_dcache[index]->tag == tag){
                l1_dcache[index]->dirty = 1;
            } else {
                printf("This shouldn't have happen.\n");
            }
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            curr = l1_dcache[index];
            // printf("L1_iCache Tag: %Lx\n", l1_icache[index]->tag );
            // printf("D Tag: %Lx  Index: %x Valid: %d\n",tag, index, curr->tag == tag);
            for (i = 0; i < l1_cache_assoc; i++){
                if (curr->tag == tag && curr->valid){
                    curr->dirty = 1;
                    return;
                }
                curr = curr->next;
            }
            printf("This should't have happened.\n");
            break;
        default: 
            printf("This shouldn't have happend.\n");
    }
}

void mark_dirty_l2(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    int i;
    cache_entry *curr;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //5 = offset bits
            tag = addr >> shift_amount_l2;
            if (l2_cache[index]->valid && l2_cache[index]->tag == tag){
                 l2_cache[index]->dirty = 1;
             } 
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l2;
            curr = l2_cache[index];
            // printf("L1_iCache Tag: %Lx\n", l1_icache[index]->tag );
            // printf("D Tag: %Lx  Index: %x Valid: %d\n",tag, index, curr->tag == tag);
            for (i = 0; i < l2_cache_assoc; i++){
                if (curr->tag == tag && curr->valid){
                    curr->dirty = 1;
                }
                curr = curr->next;
            }
            break;
        default: 
            printf("This shouldn't have happend.\n");
    }
}

void adjust_LRU_l1i(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    cache_entry *curr;

    switch(l1_cache_assoc){
        case 1:
            //No need to do anything
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            curr = l1_icache[index];

            // printf("l1_I_cache Tag: %Lx\n", l1_I_cache[index]->tag );
            // printf("I Tag: %Lx  Index: %x Valid: %d\n",tag, index, curr->tag == tag);
            while (curr->tag != tag && curr->next != NULL) {
                curr = curr->next;            
            }

            if (l1_icache[index] != curr){

                curr->prev->next = curr->next;

                if (curr->next != NULL){
                    curr->next->prev = curr->prev;
                }

                curr->next = l1_icache[index];

                curr->next->prev = curr;
                curr->prev = NULL;
                l1_icache[index] = curr;
            } 
            break;
        default: 
            printf("Default values \n");
    }
}

void adjust_LRU_l1d(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    cache_entry * curr;
    // int i;

    switch(l1_cache_assoc){
        case 1:
            //No need to do anything
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L1_OFFSET) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l1;
            curr = l1_dcache[index];

            while (curr->tag != tag && curr->next != NULL){
                curr = curr->next;
            }

            if (l1_dcache[index] != curr){

                curr->prev->next = curr->next;

                if (curr->next != NULL){
                    curr->next->prev = curr->prev;
                }

                curr->next = l1_dcache[index];

                curr->next->prev = curr;
                curr->prev = NULL;
                l1_dcache[index] = curr;
            } 
            break;

        default: 
            printf("This shouldn't have happend!\n");
    }
}

void adjust_LRU_l2(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    cache_entry *curr;

    switch(l2_cache_assoc){
        case 1:
            //No need to do anything
            break;
        case 2:
        case 4:
        case 8:
        case 256:
            index = (addr >> L2_OFFSET) & (l2_cache_lines-1); //cachelined - 1
            tag = addr >> shift_amount_l2;
            curr = l2_cache[index];

            while (curr->tag != tag && curr->next != NULL){
                curr = curr->next;
            }

            if (l2_cache[index] != curr){

                curr->prev->next = curr->next;

                if (curr->next != NULL){
                    curr->next->prev = curr->prev;
                }

                curr->next = l2_cache[index];

                curr->next->prev = curr;
                curr->prev = NULL;
                l2_cache[index] = curr;
            }
            break;
        default: 
            printf("This shouldn't have happened.\n");
    }
}

void print_stats() {

    int i;
    int j;

    cache_entry *curr;

    if(l1_cache_assoc == 2) {
        i = 1;
    }

    if(l1_cache_assoc == 4) {
        i = 2;
    }

    if(l1_cache_assoc == 8) {
        i = 3;
    }

    if(l1_cache_assoc == 256) {
        i = 8;
    }

    else i = 0;

    if(l2_cache_assoc == 2) {
        j = 1;
    }

    if(l2_cache_assoc == 4) {
        j = 2;
    }

    if(l2_cache_assoc == 8) {
        j = 3;
    }

    if(l2_cache_assoc == 256) {
        j = 8;
    }

    else j = 0;


    l1_icost = l1_dcost = 100*((l1_cache_size)/(4 * 1024)) + i*100*((l1_cache_size)/(4 * 1024));
    l2_cost = 50*((l2_cache_size*l2_cache_assoc)/(64 * 1024)) + j*50;

    // default mem_ready cost is $50 and default mem_chunksize costs $25
    mem_cost = 75;

    //print calculations
    tot_ref_type = read_refs + write_refs + inst_refs;
    tot_cycle = read_cycle + write_cycle + inst_cycle;
    read_ref_perc    = 100*((float)read_refs/(float)tot_ref_type);
    write_ref_perc   = 100*((float)write_refs/(float)tot_ref_type);
    inst_ref_perc    = 100*((float)inst_refs/(float)tot_ref_type);
    read_cycle_perc  = 100*((float)read_cycle/(float)tot_cycle);
    write_cycle_perc = 100*((float)write_cycle/(float)tot_cycle);
    inst_cycle_perc  = 100*((float)inst_cycle/(float)tot_cycle);

    l1_ihit_rate  = 100*(float)l1_ihits / (float) (l1_imisses+l1_ihits);
    l1_imiss_rate = 100*(float)l1_imisses / (float) (l1_imisses+l1_ihits);
    l1_dhit_rate  = 100*(float)l1_dhits / (float) (l1_dmisses+l1_dhits);
    l1_dmiss_rate = 100*(float)l1_dmisses / (float) (l1_dmisses+l1_dhits);
    l2_hit_rate   = 100*(float)l2_hits / (float) (l2_misses+l2_hits);
    l2_miss_rate  = 100*(float)l2_misses / (float) (l2_misses+l2_hits);

    if(l1_cache_assoc != 1){
        i = 1;
    } else {
        i = 0;  
    } 

    l1_icost = l1_dcost = 100*((l1_cache_size)/(4 * 1024)) + i*100*(l1_cache_assoc - (l1_cache_assoc/2))*((l1_cache_size)/(4 * 1024));
    //TODO Fix L2 Cost Calc
    l2_cost = 50*((l2_cache_size)/(64 * 1024)) + i*50*(l2_cache_assoc - (l1_cache_assoc/2)); 

    // default mem_ready cost is $50 and default mem_chunksize costs $25
    mem_cost = 75;

    tot_ref_type = read_refs + write_refs + inst_refs;
    tot_cycle = read_cycle + write_cycle + inst_cycle;
    read_ref_perc    = 100*((float)read_refs/(float)tot_ref_type);
    write_ref_perc   = 100*((float)write_refs/(float)tot_ref_type);
    inst_ref_perc    = 100*((float)inst_refs/(float)tot_ref_type);
    read_cycle_perc  = 100*((float)read_cycle/(float)tot_cycle);
    write_cycle_perc = 100*((float)write_cycle/(float)tot_cycle);
    inst_cycle_perc  = 100*((float)inst_cycle/(float)tot_cycle);

    l1_ihit_rate  = 100*(float)l1_ihits / (float) (l1_imisses+l1_ihits);
    l1_imiss_rate = 100*(float)l1_imisses / (float) (l1_imisses+l1_ihits);
    l1_dhit_rate  = 100*(float)l1_dhits / (float) (l1_dmisses+l1_dhits);
    l1_dmiss_rate = 100*(float)l1_dmisses / (float) (l1_dmisses+l1_dhits);
    l2_hit_rate   = 100*(float)l2_hits / (float) (l2_misses+l2_hits);
    l2_miss_rate  = 100*(float)l2_misses / (float) (l2_misses+l2_hits);

    avg_cyc_read = (float)read_cycle/(float)read_refs;
    avg_cyc_write =  (float)write_cycle/(float)write_refs;
    avg_cyc_inst = (float)tot_cycle/(float)inst_refs;

    avg_cyc_exec_time = 2*inst_refs + read_refs + write_refs;
    avg_cyc_cpi = (float)avg_cyc_exec_time/inst_refs;

    printf("SIMULATION RESULTS\n");
    printf("\n");
    printf("Memory system:\n");
    printf("    Dcache size = %d : ways = %d : block size = %d\n", l1_cache_size,l1_cache_assoc,l1_cache_block_size);
    printf("    Icache size = %d : ways = %d : block size = %d\n", l1_cache_size,l1_cache_assoc,l1_cache_block_size);
    printf("    L2-cache size = %d : ways = %d : block size = %d\n",l2_cache_size,l2_cache_assoc,l2_cache_block_size);
    printf("    Memory ready time = %d : chunksize = %d : chunktime = %d\n",MEM_READY,MEM_CHUNCKSIZE,MEM_CHUNKTIME);
    printf("\n");

    printf("Execute time =   %7Lu;  Total refs = %7Lu\n",execution_time,read_refs+write_refs+inst_refs);
    printf("Flush time =     %7Lu\n",flush_time);
    printf("Inst refs = %Lu;  Data refs = %Lu\n",inst_refs,read_refs+write_refs);
    printf("\n");
    printf("Number of reference types:  [Percentage]\n");
    printf("    Reads  =        %7Lu [%3.1f%%]\n",read_refs, read_ref_perc);
    printf("    Writes =        %7Lu [%3.1f%%]\n",write_refs, write_ref_perc);
    printf("    Inst.  =        %7Lu [%3.1f%%]\n",inst_refs, inst_ref_perc);
    printf("    Total  =        %7Lu\n",read_refs+write_refs+inst_refs);
    printf("\n");
    printf("\n");
    printf("Total cycles for activities:  [Percentage]\n");

    printf("    Reads  =         %7Lu  [%3.1f%%]\n",read_cycle, read_cycle_perc);
    printf("    Writes =         %7Lu  [%3.1f%%]\n",write_cycle, write_cycle_perc);
    printf("    Inst.  =         %7Lu  [%3.1f%%]\n",inst_cycle, inst_cycle_perc);
    printf("    Total  =         %7Lu\n",read_cycle+write_cycle+inst_cycle);
    printf("\n");
    printf("Average cycles per activity:\n");
    printf("    Read =  %3.1f; Write = %3.1f; Inst. = %3.1f\n",avg_cyc_read, avg_cyc_write,avg_cyc_inst);
    printf("Ideal: Exec. Time = %Lu; CPI =  %2.1f\n",avg_cyc_exec_time,avg_cyc_cpi);
    printf("Ideal mis-aligned: Exec. Time = %Lu; CPI =  %2.1f\n",avg_cyc_exec_misalign,avg_cyc_cpi_misalign);
    printf("\n");
    printf("Memory Level:  L1i\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l1_ihits,l1_imisses);
    printf("    Total Requests = %Lu\n",l1_imisses+l1_ihits);
    printf("    Hit Rate = %3.1f%%   Miss Rate = %3.1f%%\n",l1_ihit_rate,l1_imiss_rate);
    printf("    Kickouts = %Lu; Dirty kickouts = %Lu; Transfers = %Lu\n",l1_ikickouts,l1_ikickouts_dirty,l1_itransfers);
    printf("    Flush Kickouts = %Lu\n", l1_ikickouts_flush);
    printf("\n");
    printf("Memory Level:  L1d\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l1_dhits,l1_dmisses);
    printf("    Total Requests = %Lu\n",l1_dhits+l1_dmisses);
    printf("    Hit Rate = %3.1f%%   Miss Rate = %3.1f%%\n",l1_dhit_rate,l1_dmiss_rate);
    printf("    Kickouts = %Lu; Dirty kickouts = %Lu; Transfers = %Lu\n",l1_dkickouts,l1_dkickouts_dirty,l1_dtransfers);
    printf("    Flush Kickouts = %Lu\n",l1_dkickouts_flush);
    printf("\n");
    printf("Memory Level:  L2\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l2_hits,l2_misses);
    printf("    Total Requests = %Lu\n",l2_hits+l2_misses);
    printf("    Hit Rate =  %3.1f%%   Miss Rate = %4.1f%%\n",l2_hit_rate,l2_miss_rate);
    printf("    Kickouts = %Lu; Dirty kickouts = %Lu; Transfers = %Lu\n",l2_kickouts,l2_kickouts_dirty,l2_transfers);
    printf("    Flush Kickouts = %Lu\n",l2_kickouts_flush);
    printf("\n");
    printf("L1 cache cost (Icache $%Lu) + (Dcache $%Lu) = $%Lu\n",l1_icost,l1_dcost,l1_icost + l1_dcost);
    printf("L2 cache cost = $%Lu;  Memory cost = $%Lu  Total cost = $%Lu\n",l2_cost,mem_cost,l1_icost + l1_dcost+l2_cost+mem_cost);
    printf("Flushes = %Lu : Invalidates = %Lu\n",flushes,invalidates);

    printf("-------------------------------------------------------------\n");
    printf("\n");
    printf("Cache final contents - Index and Tag values are in HEX\n");
    printf("\n");
    printf("Memory Level:  L1i\n");

    // for (i = 0; i < l1_cache_lines; i++){
    //     curr = l1_icache[i];
    //     if (curr->valid != 0){
    //         printf("Index:  %3x",i);
    //         printf(" | V:%d D:%d Tag: %11Lx ", curr->valid, curr->dirty, curr->tag);
    //         curr = curr->next;
    //         while (curr != NULL) {
    //             if (!curr->valid){
    //                 printf(" | V:0 D:0 Tag: - ");
    //             } else {
    //                 printf(" | V:%d D:%d Tag: %11Lx ", curr->valid, curr->dirty, curr->tag);
    //             }
    //             curr = curr->next;
    //         }   
    //         printf("\n");
    //     }
    // }

    // printf("\n");
    // printf("Memory Level:  L1d\n");
    // for (i = 0; i < l1_cache_lines; i++){
    //     curr = l1_dcache[i];
    //     if (curr->valid != 0){
    //         printf("Index:  %3x",i);
    //         printf(" | V:%d D:%d Tag: %11Lx ", curr->valid, curr->dirty, curr->tag);
    //         curr = curr->next;
    //         while (curr != NULL) {
    //             if (!curr->valid){
    //                 printf(" | V:0 D:0 Tag:- ");
    //             } else {
    //                 printf(" | V:%d D:%d Tag: %11Lx ", curr->valid, curr->dirty, curr->tag);
    //             }
    //             curr = curr->next;
    //         }   
    //         printf("\n");
    //     }
    // }

    // printf("\n");
    // printf("Memory Level:  L2\n");
    // for (i = 0; i < l2_cache_lines; i++){
    //     curr = l2_cache[i];
    //     if (curr->valid != 0){
    //         printf("Index:  %3x",i);
    //         printf(" | V:%d D:%d Tag: %11Lx ", curr->valid, curr->dirty, curr->tag);
    //         curr = curr->next;
    //         while (curr != NULL) {
    //             if (!curr->valid){
    //                 printf(" | V:0 D:0 Tag: - ");
    //             } else {
    //                 printf(" | V:%d D:%d Tag: %11Lx ", curr->valid, curr->dirty, curr->tag);
    //             }
    //             curr = curr->next;
    //         }   
    //         printf("\n");
    //     }
    // }
}

/************************************************************/
