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
 * TODO Fix Reference Calculation and Execution Time
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
        l1_icache[i] = blank_entry;
        curr = l1_icache[i];
        for (j = 0; j < l1_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            blank_entry->tag = 0;
            blank_entry->next = NULL;
            curr->next = blank_entry;
            blank_entry->prev = curr;
            curr = curr->next;
        }

        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        blank_entry->tag = 0;
        l1_dcache[i] = blank_entry;
        curr = l1_dcache[i];
        for (j = 0; j < l1_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            blank_entry->tag = 0;
            curr->next = blank_entry;
            curr->next->prev = curr;
            curr = curr->next;
        }    
    }

    //Setup Empty L2 Cache
    for ( i = 0; i < l2_cache_lines; i++){
        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        blank_entry->tag = 0;
        l2_cache[i] = blank_entry;
        curr = l2_cache[i];
        for (j = 0; j < l2_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            blank_entry->tag = 0;
            curr->next = blank_entry;
            curr->next->prev = curr;
            curr = curr->next;
        }    
    }

    // free(blank_entry);

}

void perform_access(unsigned long long int addr, unsigned int byteSize, char op) {

    int byteRefs = (byteSize/4) + (byteSize%4 ? 1 : 0); //Determine Number of Refs from bytes

    //Check to see if Unaligned or Not

    //Perform Access Based on Operation
    switch(op){
        case 'I':
            inst_refs++;
            // printf("ByteRefs: %d\n", byteRefs);
            if(check_inst_cache(addr) != 1){
                l1_imisses++;
                if (check_l2_cache(addr) != 1){
                    l2_misses++;
                    insert_l2(addr,0);
                    insert_inst(addr);
                    //Copy from MM to L2 and L1
                } else {
                    l2_hits ++;
                    //Add Function to Adjust LRU L2
                    insert_inst(addr);
                    l1_ihits += byteRefs;
                    //Copy from L2
                }
                
            } else {
                l1_ihits += byteRefs;
                //Add Function to Adjust LRU L1i
                //Execution Time from L1
            } 
            break;
        case 'W':
            write_refs++;
            if(check_data_cache(addr) != 1){
                l1_dmisses++;
                if (check_l2_cache(addr) != 1){
                    l2_misses++;
                    insert_l2(addr, 0);
                    write_data(addr);
                    l1_dhits += byteRefs;
                    //Copy from MM to L1 and L2
                } else {
                    write_data(addr);
                    //Add Function to Adjust LRU L2
                    l2_hits ++;
                    l1_dhits += byteRefs;
                    //Copy from L2
                }
            } else {
                //Add Function to just Mark Dirty
                //Add Function to Adjust LRU L1d
                l1_dhits += byteRefs;
            } 
            break;
        case 'R':
            read_refs++;
            if(check_data_cache(addr) != 1){
                l1_dmisses++;
                if (check_l2_cache(addr) != 1){
                    l2_misses++;
                    insert_l2(addr, 0);
                    read_data(addr, 0);
                    //Execution Time from MM
                } else {
                    read_data(addr, is_dirty(addr));
                    l2_hits ++;
                    //Execution Time from L2
                }
                // 
                l1_dhits ++;
            } else {
                //Execution Time from L1
                l1_dhits += byteRefs;
            } 
            break;
    }
}

void flush() {

    /* flush the cache */

}

void insert_inst(unsigned long long int addr) {

    int index;
    unsigned long long int tag;
    cache_entry * curr;
    cache_entry * new;

    switch(l1_cache_assoc){
        case 1:
            index = (addr>>5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 13;
            l1_icache[index]->tag = tag;
            l1_icache[index]->address = addr;
            break;
        case 2:
            index = (addr>>5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            curr = l1_icache[index];
            new = malloc(sizeof(cache_entry));
            //Setup New Cache Entry
            new->tag = tag;
            new->address = addr;
            new->next = curr;
            curr->prev = new;

            //Grab the Last Element
            while (curr->next != NULL){
                curr = curr->next;
            }

            curr->prev->next = NULL;
            free(curr);
            l1_icache[index] = new;
            break;
        case 4:
            index = (addr>>5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 11;
            l1_icache[index]->tag = tag;
            l1_icache[index]->address = addr;
            break;
        case 8:
            index = (addr>>5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 10;
            l1_icache[index]->tag = tag;
            l1_icache[index]->address = addr;
            break;
        default:
            printf("Default values \n");
            //Fully Associative
    }
}

void write_data(unsigned long long int addr) {

    int index;
    unsigned long long int tag;
    cache_entry * curr;
    cache_entry * new;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 13;
            if(l1_dcache[index]->tag == 0){
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->address = addr;
                l1_dcache[index]->dirty = 1;
            } else {
                //Insert Into L2 W is Dirty
                if (l1_dcache[index]->dirty){
                    insert_l2(l1_dcache[index]->address, l1_dcache[index]->dirty);
                    //Calcuate Write to L2   
                }
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->dirty = 1;
                l1_dcache[index]->address = addr;
            } 
            break;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            if(l1_dcache[index]->tag == 0){
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->address = addr;
                l1_dcache[index]->dirty = 1;
            } else {
                curr = l1_dcache[index];
                new = malloc(sizeof(cache_entry));
                //Setup New Cache Entry
                new->tag = tag;
                new->address = addr;
                new->next = curr;
                new->dirty = 1;
                curr->prev = new;

                //Grab the Last Element
                while (curr->next != NULL){
                    curr = curr->next;
                }

                if (curr->tag != 0 && curr->dirty){
                    //If the LRU is Dirty Write Back to L2
                    printf("We need to replace!\n");
                    insert_l2(curr->address, curr->dirty);
                    //Calcuate Write to L2   
                } 

                curr->prev->next = NULL;
                free(curr);
                l1_dcache[index] = new;
            } 
            break;
        case 4:
        default:
            printf("Default values \n");
    }
}

void read_data(unsigned long long int addr, int dirty) {

    int index;
    unsigned long long int tag;
    cache_entry *curr;
    cache_entry * new;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 13; //log2 l1_cache_lines -1 + offset
            if(l1_dcache[index]->tag == 0){
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->address = addr;
                l1_dcache[index]->dirty = dirty;
            } else {
                //Write back to L2 if dirty
                if (l1_dcache[index]->dirty == 1){
                    insert_l2(l1_dcache[index]->address, l1_dcache[index]->dirty);
                    //Calcuate Write to L2
                }
                // printf("Insert Into L2 R\n");
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->address = addr;
                l1_dcache[index]->dirty = dirty;
            } 
            break;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            if(l1_dcache[index]->tag == 0){
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->address = addr;
                l1_dcache[index]->dirty = 1;
            } else {
                curr = l1_dcache[index];
                new = malloc(sizeof(cache_entry));
                //Setup New Cache Entry
                new->tag = tag;
                new->address = addr;
                new->next = curr;
                curr->prev = new;

                //Grab the Last Element
                while (curr->next != NULL){
                    curr = curr->next;
                }

                if (curr->tag != 0 && curr->dirty){
                    //If the LRU is Dirty Write Back to L2
                    printf("We need to replace!\n");
                    insert_l2(curr->address, curr->dirty);
                    //Calcuate Write to L2   
                } 

                curr->prev->next = NULL;
                free(curr);
                l1_dcache[index] = new;
            }
            break;
        case 4:
        default:
            printf("Default values \n");
    }
}

void insert_l2(unsigned long long int addr, int dirty) {

    int index;
    unsigned long long int tag;

    switch(l2_cache_assoc){
        case 1:
            index = (addr >> 6) & (l2_cache_lines-1); //cachelined - 1
            tag = addr >> 15;
            // printf("Tag L2: %Lx\n", l2_cache[index]->tag);
            if(l2_cache[index]->tag == 0){
                l2_cache[index]->tag = tag;
                l2_cache[index]->dirty = dirty;
                l2_cache[index]->address = addr;
            } else {
                if (l2_cache[index]->dirty){
                    //Cacluate Write Back to MM
                }
                l2_cache[index]->tag = tag;
                l2_cache[index]->dirty = dirty;
                l2_cache[index]->address = addr;
            } 
            break;
        case 2:
        case 4:
        default:
            printf("Default values \n");
    }
}

int check_inst_cache(unsigned long long int addr){

    int index;
    int i;
    unsigned long long int tag;
    cache_entry *curr;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 13;
            // printf("Index for L2: %x\n", (addr >> 6) & (l2_cache_lines-1));
            // printf("I Tag: %Lx  Index: %d Valid: %d\n",tag, index, l1_icache[index]->tag == tag);
            return l1_icache[index]->tag == tag;
            break;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            curr = l1_icache[index];
            // printf("L1_iCache Tag: %Lx\n", l1_icache[index]->tag );
            // printf("I Tag: %Lx  Index: %x Valid: %d\n",tag, index, curr->tag == tag);
            for (i = 0; i < l1_cache_assoc; i++){
                if (curr->tag == tag){
                    return 1;
                }
                curr = curr->next;
            }
            return 0;
            break;
        case 4:
        default: 
            printf("Default values \n");
    }

    return -1;
}

int check_data_cache(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    int i;
    cache_entry *curr;

    // double temp = (log ((double) l1_cache_lines/2) / log (2.0));
    // int tag_offset = (int)( (5 + (temp) ) - 2 );
    // printf("Shift : %d\n", tag_offset);

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //5 = offset bits
            tag = addr >> 13;
            // printf("Index for L2: %x\n", (addr >> 6) & (l2_cache_lines-1));
            // printf("D Tag: %Lx  Index: %d Valid: %d\n",tag, index, l1_dcache[index]->tag == tag);
            return l1_dcache[index]->tag == tag;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            curr = l1_dcache[index];
            // printf("L1_iCache Tag: %Lx\n", l1_icache[index]->tag );
            // printf("D Tag: %Lx  Index: %x Valid: %d\n",tag, index, curr->tag == tag);
            for (i = 0; i < l1_cache_assoc; i++){
                if (curr->tag == tag){
                    return 1;
                }
                curr = curr->next;
            }
            return 0;
            break;
        case 4:
        default: 
            printf("Default values \n");
    }

    return -1;
}

int check_l2_cache(unsigned long long int addr){

    int index;
    unsigned long long int tag;

    switch(l2_cache_assoc){
        case 1:
            index = (addr >> 6) & (l2_cache_lines-1); //cachelined - 1
            tag = addr >> 15;
            // printf("L2 Curr: Index %x Tag: %Lu\n", index, l2_cache[index]->tag );
            // printf("L2 Tag: %Lx  Index: %x Valid: %d\n",tag, index, l2_cache[index]->tag == tag);
            return l2_cache[index]->tag == tag;
        case 2:
        case 4:
        default: 
            printf("Default values \n");
    }
    return -1;
}

int is_dirty(unsigned long long int addr){

    int index;

    switch(l2_cache_assoc){
        case 1:
            index = (addr >> 6) & (l2_cache_lines-1); //cachelined - 1
            return l2_cache[index]->dirty;
            break;
        case 2:
        case 4:
        default: 
            printf("Default values \n");
    }
    return -1;
}

void adjust_LRU_l1i(unsigned long long int addr){

}

void adjust_LRU_l1d(unsigned long long int addr){

}

void adjust_LRU_l2(unsigned long long int addr){

}

void print_stats() {
    //Add When Fixed

    int i;
    cache_entry *curr;

    printf("SIMULATION RESULTS\n");
    printf("\n");
    printf("Memory system:\n");
    printf("    Dcache size = %d : ways = %d : block size = %d\n", l1_cache_size,l1_cache_assoc,l1_cache_block_size);
    printf("    Icache size = %d : ways = %d : block size = %d\n", l1_cache_size,l1_cache_assoc,l1_cache_block_size);
    printf("    L2-cache size = %d : ways = %d : block size = %d\n",l2_cache_size,l2_cache_assoc,l2_cache_block_size);
    printf("    Memory ready time = %d : chunksize = %d : chunktime = %d\n",MEM_READY,MEM_CHUNCKSIZE,MEM_CHUNKTIME);
    printf("\n");

    // printf("Execute time =          %Lu;  Total refs = %Lu\n",execution_time,read_refs + write_refs+inst_refs);
    // printf("Inst refs = %Lu;  Data refs = %Lu\n",inst_refs,read_refs+write_refs);

    printf("Number of reference types:  \n");
    printf("    Reads  =           %Lu\n",read_refs);
    printf("    Writes =           %Lu\n",write_refs);
    printf("    Inst.  =           %Lu\n",inst_refs);
    printf("    Total  =           %Lu\n",read_refs+ write_refs + inst_refs);
    printf("\n");
    printf("Memory Level:  L1i\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l1_ihits,l1_imisses);
    printf("Memory Level:  L1d\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l1_dhits,l1_dmisses);
    printf("Memory Level:  L2\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l2_hits,l2_misses);


    printf("-------------------------------------------------------------\n");
    printf("\n");
    printf("Cache final contents - Index and Tag values are in HEX\n");
    printf("\n");
    printf("Memory Level:  L1i\n");
    
    for (i = 0; i < l1_cache_lines; i++){
        curr = l1_icache[i];
        if (curr->tag != 0){
            printf("Index:  %3x",i);
            printf(" |  Tag:    %Lx ", curr->tag);
            curr = curr->next;
            while (curr != NULL) {
                printf(" |  Tag:    %Lx ", curr->tag);
                curr = curr->next;
            }   
            printf("\n");
        }
    }

    printf("\n");
    printf("Memory Level:  L1d\n");
    for (i = 0; i < l1_cache_lines; i++){
        curr = l1_dcache[i];
        if (curr->tag != 0){
            printf("Index:  %3x",i);
            printf(" |  Tag:    %Lx ", curr->tag);
            curr = curr->next;
            while (curr != NULL) {
                printf(" |  Tag:    %Lx ", curr->tag);
                curr = curr->next;
            }   
            printf("\n");
        }
    }
    // printf("\n");
    // printf("Memory Level:  L2\n");
    // for (i = 0; i < l2_cache_lines; i++){
    //     curr = l2_cache[i];
    //     if (curr->tag != 0){
    //             printf("Index:  %3x",i);
    //             printf(" |  Tag:    %Lx \n", curr->tag);         
    //     }
    // }
}

/************************************************************/
