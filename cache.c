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

 execution_time = 0;

void init_cache() {

    int i,j;
    l1_cache_lines = l1_cache_size/l1_cache_block_size/l1_cache_assoc;
    l2_cache_lines = l2_cache_size/l2_cache_block_size/l2_cache_assoc;


    l1_D_cache = malloc(sizeof(cache_entry *)*(l1_cache_lines));
    l1_I_cache = malloc(sizeof(cache_entry *)*(l1_cache_lines));
    l2_cache  = malloc(sizeof(cache_entry *)*(l2_cache_lines));

    //Define Cleared Cache Entry
    cache_entry *blank_entry;
    cache_entry *curr;

    //Setup Empty L1 Inst and Data Cache
    for ( i = 0; i < l1_cache_lines; i++){
        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        blank_entry->tag = 0;
        blank_entry->prev = NULL;
        l1_I_cache[i] = blank_entry;
        curr = l1_I_cache[i];
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
        l1_D_cache[i] = blank_entry;
        curr = l1_D_cache[i];
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
                l1_I_misses++;
                if (check_l2_cache(addr) != 1){
                    l2_misses++;
                    insert_l2(addr,0);
                    insert_inst(addr);
                    //Copy from MM to L2 and L1
                    
                } else {
                    l2_hits ++;
                    adjust_LRU_l2(addr); //Add Function to Adjust LRU L2
                    insert_inst(addr);
                    l1_I_hits += byteRefs;
                    //Copy from L2
                }
                
            } else {
                l1_I_hits += byteRefs;
                adjust_LRU_l1i(addr);       //Add Function to Adjust LRU L1i
                execution_time += l1_hit_time;    //Execution Time from L1

            } 
            break;
        case 'W':
            write_refs++;
            if(check_data_cache(addr) != 1){
                l1_D_misses++;
                if (check_l2_cache(addr) != 1){
                    l2_misses++;
                    insert_l2(addr, 0);
                    write_data(addr);
                    l1_D_hits += byteRefs;
                    //Copy from MM to L1 and L2
                } else {
                    write_data(addr);
                    adjust_LRU_l2(addr);  //Add Function to Adjust LRU L2
                    l2_hits ++;
                    l1_D_hits += byteRefs;
                    //Copy from L2
                }
            } else {
                //Add Function to just Mark Dirty
                adjust_LRU_l1d(addr); //Add Function to Adjust LRU L1d
                l1_D_hits += byteRefs;
            } 
            break;
        case 'R':
            read_refs++;
            if(check_data_cache(addr) != 1){
                l1_D_misses++;
                if (check_l2_cache(addr) != 1){
                    l2_misses++;
                    insert_l2(addr, 0);
                    read_data(addr, 0);
                    execution_time += MEM_SENDADDR + MEM_READY + (MEM_CHUNKTIME * (l2_cache_lines/MEM_CHUNKSIZE)) + l1_miss_time + l2_miss_time;  //Execution Time from MM
                    
                } else {
                    read_data(addr, is_dirty(addr));
                    l2_hits ++;
                    execution_time += l2_hit_time;
                }
                 
                l1_D_hits ++;
            } else {
                execution_time += l1_hit_time;  //Execution Time from L1
                l1_D_hits += byteRefs;
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
            l1_I_cache[index]->tag = tag;
            l1_I_cache[index]->address = addr;
            break;
        case 2:
            index = (addr>>5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            curr = l1_I_cache[index];
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
            l1_I_cache[index] = new;
            break;
        case 4:
            index = (addr>>5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 11;
            l1_I_cache[index]->tag = tag;
            l1_I_cache[index]->address = addr;
            break;
        case 8:
            index = (addr>>5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 10;
            l1_I_cache[index]->tag = tag;
            l1_I_cache[index]->address = addr;
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
            if(l1_D_cache[index]->tag == 0){
                l1_D_cache[index]->tag = tag;
                l1_D_cache[index]->address = addr;
                l1_D_cache[index]->dirty = 1;
            } else {
                //Insert Into L2 W is Dirty
                if (l1_D_cache[index]->dirty){
                    insert_l2(l1_D_cache[index]->address, l1_D_cache[index]->dirty);
                    //Calcuate Write to L2   
                }
                l1_D_cache[index]->tag = tag;
                l1_D_cache[index]->dirty = 1;
                l1_D_cache[index]->address = addr;
            } 
            break;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            if(l1_D_cache[index]->tag == 0){
                l1_D_cache[index]->tag = tag;
                l1_D_cache[index]->address = addr;
                l1_D_cache[index]->dirty = 1;
            } else {
                curr = l1_D_cache[index];
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
                l1_D_cache[index] = new;
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
            if(l1_D_cache[index]->tag == 0){
                l1_D_cache[index]->tag = tag;
                l1_D_cache[index]->address = addr;
                l1_D_cache[index]->dirty = dirty;
            } else {
                //Write back to L2 if dirty
                if (l1_D_cache[index]->dirty == 1){
                    insert_l2(l1_D_cache[index]->address, l1_D_cache[index]->dirty);
                    //Calcuate Write to L2
                }
                // printf("Insert Into L2 R\n");
                l1_D_cache[index]->tag = tag;
                l1_D_cache[index]->address = addr;
                l1_D_cache[index]->dirty = dirty;
            } 
            break;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            if(l1_D_cache[index]->tag == 0){
                l1_D_cache[index]->tag = tag;
                l1_D_cache[index]->address = addr;
                l1_D_cache[index]->dirty = 1;
            } else {
                curr = l1_D_cache[index];
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
                l1_D_cache[index] = new;
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
            // printf("I Tag: %Lx  Index: %d Valid: %d\n",tag, index, l1_I_cache[index]->tag == tag);
            return l1_I_cache[index]->tag == tag;
            break;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            curr = l1_I_cache[index];
            // printf("l1_I_cache Tag: %Lx\n", l1_I_cache[index]->tag );
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
            // printf("D Tag: %Lx  Index: %d Valid: %d\n",tag, index, l1_D_cache[index]->tag == tag);
            return l1_D_cache[index]->tag == tag;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            curr = l1_D_cache[index];
            // printf("l1_I_cache Tag: %Lx\n", l1_I_cache[index]->tag );
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

    int index;
    int i;
    unsigned long long int tag;
    cache_entry *curr;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 13;
            // printf("Index for L2: %x\n", (addr >> 6) & (l2_cache_lines-1));
            // printf("I Tag: %Lx  Index: %d Valid: %d\n",tag, index, l1_I_cache[index]->tag == tag);
            return l2_cache[index]->tag == tag;
            break:
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            curr = l1_I_cache[index];
            // printf("l1_I_cache Tag: %Lx\n", l1_I_cache[index]->tag );
            // printf("I Tag: %Lx  Index: %x Valid: %d\n",tag, index, curr->tag == tag);
            for (i = 0; i < l1_cache_assoc; i++){
                if (curr->tag == tag){
                    curr->prev->next = curr->next;
                    curr->next->prev = curr->prev;
                    curr->next = l1_I_cache[index];
                    curr->prev = NULL;
                }
                    return 1;
            }
                curr = curr->next;
            
            return 0;
            break;
        case 4:
        default: 
            printf("Default values \n");
    }

    

    return -1;

}

void adjust_LRU_l1d(unsigned long long int addr){

  int index;
    unsigned long long int tag;
    int i;
    cache_entry *curr;
    cache_entry *new;

    // double temp = (log ((double) l1_cache_lines/2) / log (2.0));
    // int tag_offset = (int)( (5 + (temp) ) - 2 );
    // printf("Shift : %d\n", tag_offset);

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //5 = offset bits
            tag = addr >> 13;
            // printf("Index for L2: %x\n", (addr >> 6) & (l2_cache_lines-1));
            // printf("D Tag: %Lx  Index: %d Valid: %d\n",tag, index, l1_D_cache[index]->tag == tag);
            return l1_D_cache[index]->tag == tag;
        case 2:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr >> 12;
            curr = l1_D_cache[index];
            // printf("l1_I_cache Tag: %Lx\n", l1_I_cache[index]->tag );
            // printf("D Tag: %Lx  Index: %x Valid: %d\n",tag, index, curr->tag == tag);
            for (i = 0; i < l1_cache_assoc; i++){
                if (curr->tag == tag){
                    curr->prev->next = curr->next;
                    curr->next->prev = curr->prev;
                    curr->next = l1_D_cache[index];
                    curr->prev = NULL;
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

void adjust_LRU_l2(unsigned long long int addr){

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

void print_stats() {

    int i;
    cache_entry *curr;

    if(l1_cache_assoc != 1){
      i = 1;
    }

    else i = 0;

    l1_I_cost = l1_D_cost = 100*((l1_cache_size)/(4 * 1024)) + i*100*(l1_cache_assoc - (l1_cache_assoc/2))*((l1_cache_size)/(4 * 1024));
    l2_cost = 50*((l2_cache_size)/(64 * 1024)) + i*50*(l2_cache_assoc - (l1_cache_assoc/2)); 

    // default mem_ready cost is $50 and default mem_chunksize costs $25
    mem_cost = 75;

    //print calculations
// tot_ref_type = read_refs + write_refs + inst_ref_type;
// tot_cycle = read_cycle + write_cycle + inst_cycle;
// read_ref_perc = (read_refs/tot_ref_type);
// write_ref_perc = (write_refs/tot_ref_type);
// inst_ref_perc = (inst_ref_type/tot_ref_type);
// read_cycle_perc = (read_cycle/tot_cycle);
// write_cycle_perc = (write_cycle/tot_cycle);
// inst_cycle_perc = (inst_cycle/tot_cycle);

/************************************************************/

    printf("SIMULATION RESULTS\n");
    printf("\n");
    printf("Memory system:\n");
    printf("    Dcache size = %d : ways = %d : block size = %d\n", l1_cache_size,l1_cache_assoc,l1_cache_block_size);
    printf("    Icache size = %d : ways = %d : block size = %d\n", l1_cache_size,l1_cache_assoc,l1_cache_block_size);
    printf("    L2-cache size = %d : ways = %d : block size = %d\n",l2_cache_size,l2_cache_assoc,l2_cache_block_size);
    printf("    Memory ready time = %d : chunksize = %d : chunktime = %d\n",MEM_READY,MEM_CHUNKSIZE,MEM_CHUNKTIME);
    printf("\n");

    //printf("Execute time =          %Lx;  Total refs = %Lx\n",execution_time,totalReads + totalWrites);
    //printf("Flush time =            %Lx\n",flush_time);
    //printf("Inst refs = %Lx;  Data refs = %Lx\n",inst_refs,data_refs);
    //printf("\n");
    // printf("Number of reference types:  [Percentage]\n");
    // printf("    Reads  =           %Lx    [%3.1f%%]\n",read_refs,read_ref_perc);
    // printf("    Writes =           %Lx    [%3.1f%%]\n",write_refs,write_ref_perc);
    // printf("    Inst.  =           %Lx    [%3.1f%%]\n",inst_ref_type,inst_ref_perc);
    // printf("    Total  =          %Lx\n",tot_ref_type);
    // printf("\n");
    printf("Total cycles for activities:  [Percentage]\n");
    printf("    Reads  =           %Lx    [%3.1f%%]\n",read_cycle,read_cycle_perc);
    printf("    Writes =           %Lx    [%3.1f%%]\n",write_cycle,write_cycle_perc);
    printf("    Inst.  =           %Lx    [%3.1f%%]\n",inst_cycle,inst_cycle_perc);
    printf("    Total  =           %Lx\n",read_cycle + write_cycle + inst_cycle);
    // printf("\n");
    // printf("Average cycles per activity:\n");
    // printf("    Read =  N/A; Write = %3.1f; Inst. = %3.1f\n",avg_cyc_write,avg_cyc_inst);
    // printf("Ideal: Exec. Time = %Lx; CPI =  %2.1f\n",avg_cyc_exec_time,avg_cyc_cpi);
    // printf("Ideal mis-aligned: Exec. Time = %Lx; CPI =  %2.1f\n",avg_cyc_exec_misalign,avg_cyc_cpi_misalign);
    // printf("\n");
    // printf("Memory Level:  L1i\n");
    printf("    Hit Count = %Lx  Miss Count = %Lx\n",l1_I_hits,l1_I_misses);
    printf("    Total Requests = %Lx\n",l1_I_total_req);
    // printf("    Hit Rate = %3.1f%%   Miss Rate = %3.1f%%\n",l1_I_hit_rate,l1_I_miss_rate);
    // printf("    Kickouts = %Lx; Dirty kickouts = %Lx; Transfers = %Lx\n",l1_I_kickouts,l1_I_kickouts_dirty,l1_I_transfers);
    // printf("    Flush Kickouts = %Lx\n", l1_I_kickouts_flush);
    // printf("\n");
    // printf("Memory Level:  L1d\n");
    // printf("    Hit Count = %Lx  Miss Count = %Lx\n",l1_D_hits,l1_D_misses);
    printf("    Total Requests = %Lx\n",l1_D_total_req);
    // printf("    Hit Rate = %3.1f%%   Miss Rate = %3.1f%%\n",l1_D_hit_rate,l1_I_miss_rate);
    // printf("    Kickouts = %Lx; Dirty kickouts = %Lx; Transfers = %Lx\n",l1_I_kickouts,l1_I_kickouts_dirty,l1_I_transfers);
    // printf("    Flush Kickouts = %Lx\n",l1_D_kickouts_flush);
    // printf("\n");
    // printf("Memory Level:  L2\n");
    // printf("    Hit Count = %Lx  Miss Count = %Lx\n",l2_hits,l2_misses);
    // printf("    Total Requests = %Lx\n",l2_total_req);
    // printf("    Hit Rate =  %3.1f%%   Miss Rate = %4.1f%%\n",l2_hit_rate,l2_miss_rate);
    // printf("    Kickouts = %Lx; Dirty kickouts = %Lx; Transfers = %Lx\n",l2_kickouts,l2_kickouts_dirty,l2_transfers);
    // printf("    Flush Kickouts = %Lx\n",l2_kickouts_flush);
    // printf("\n");
    // printf("L1 cache cost (Icache $%Lx) + (Dcache $%Lx) = $%Lx\n",l1_I_cost,l1_D_cost,l1_I_cost + l1_D_cost);
    // printf("L2 cache cost = $%Lx;  Memory cost = $%Lx  Total cost = $%Lx\n",l2_cost,mem_cost,l1_I_cost + l1_D_cost + l2_cost + mem_cost);
    // printf("Flushes = %Lx : Invalidates = %Lx\n",flushes,invalidates);

    //Add When Fixed
    
    // printf("Execute time =          %Lu;  Total refs = %Lu\n",execution_time,read_refs + write_refs+inst_refs);
    // printf("Inst refs = %Lu;  Data refs = %Lu\n",inst_refs,read_refs+write_refs);

    printf("Number of reference types:  \n");
    printf("    Reads  =           %Lu\n",read_refs);
    printf("    Writes =           %Lu\n",write_refs);
    printf("    Inst.  =           %Lu\n",inst_refs);
    printf("    Total  =           %Lu\n",read_refs+ write_refs + inst_refs);
    printf("\n");
    printf("Memory Level:  L1i\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l1_I_hits,l1_I_misses);
    printf("Memory Level:  L1d\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l1_D_hits,l1_D_misses);
    printf("Memory Level:  L2\n");
    printf("    Hit Count = %Lu  Miss Count = %Lu\n",l2_hits,l2_misses);


    printf("-------------------------------------------------------------\n");
    printf("\n");
    printf("Cache final contents - Index and Tag values are in HEX\n");
    printf("\n");
    printf("Memory Level:  L1i\n");
    
    for (i = 0; i < l1_cache_lines; i++){
        curr = l1_I_cache[i];
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
        curr = l1_D_cache[i];
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

