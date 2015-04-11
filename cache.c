/* ********************************************
 * 
 * cache.c
 *
 * This file does the cache manipulations.
 *
 * Date: 31 March 2015
 * 
 * Author: Jacob Resman and Healy Fuess
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


    // l1_dcache = malloc(sizeof(cache_entry)*(l1_cache_size/l1_cache_block_size/l1_cache_assoc));
    // l1_icache = malloc(sizeof(cache_entry)*(l1_cache_size/l1_cache_block_size/l1_cache_assoc));
    // l2_cache  = malloc(sizeof(cache_entry)*(l2_cache_size/l2_cache_block_size/l2_cache_assoc);

    //Define Cleared Cache Entry
    cache_entry *blank_entry;
    cache_entry *curr;

    //Setup Empty L1 Inst and Data Cache
    for ( i = 0; i < l1_cache_lines; i++){
        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        l1_icache[i] = blank_entry;
        curr = l1_icache[i];
        for (j = 0; j < l1_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            curr->next = blank_entry;
            curr->next->prev = curr;
            curr = curr->next;
        }

        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        l1_dcache[i] = blank_entry;
        curr = l1_dcache[i];
        for (j = 0; j < l1_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            curr->next = blank_entry;
            curr->next->prev = curr;
            curr = curr->next;
        }    
    }

    //Setup Empty L2 Cache
    for ( i = 0; i < l2_cache_lines; i++){
        blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
        l2_cache[i] = blank_entry;
        curr = l2_cache[i];
        for (j = 0; j < l2_cache_assoc-1; j++) {
            blank_entry = (cache_entry *)malloc(sizeof(cache_entry));
            curr->next = blank_entry;
            curr->next->prev = curr;
            curr = curr->next;
        }    
    }

    free(blank_entry);

}

void perform_access(unsigned long long int addr, unsigned int byteSize, char op) {

    int byteRefs = (byteSize/4) + (byteSize%4 ? 1 : 0); //Determine Number of Refs from bytes

    switch(op){
        case 'I':
            inst_refs++;
            if(check_inst_cache(addr) != 1){
                l1_imisses++;
                insert_inst(addr);
                execution_time += MEM_CHUNKTIME + L1_MISS_TIME + MEM_SENDADDR+MEM_READY;
            } else {
                l1_ihits += byteRefs;
                execution_time += (L1_HIT_TIME*byteRefs);
                //Do Hit Calculations
            } 
            break;
        case 'W':
            write_refs++;
            if(check_data_cache(addr) != 1){
                l1_dmisses++;
                if (check_l2_cache(addr) != 1){
                    l2_misses++;
                    execution_time += (MEM_CHUNKTIME*(byteSize/MEM_CHUNCKSIZE))
                        + L1_MISS_TIME + L2_MISS_TIME + MEM_SENDADDR + MEM_READY;
                    //Copy from MM
                } else {
                    l2_hits += byteRefs;
                    //Copy from L2
                }
                write_data(addr);
                //Do Hit Calculations and mark dirty
            } else {
                //Mark Dirty
                l1_dhits += byteRefs;
            } 
            break;
        // case 'R':
        //     read_refs++;
        //     if(check_data_cache(addr) != 1){
        //         l1_dmisses++;
        //         if (check_l2_cache(addr) != 1){
        //             l2_misses++;
        //             //Copy from MM
        //         } else {
        //             l2_hits += byteRefs;
        //             //Copy from L2
        //         }
        //         read_data(addr);
        //         //Do Hit Calculations and mark dirty
        //     } else {
        //         l1_dhits += byteRefs;
        //         execution_time += L1_HIT_TIME;
        //     } 
        //     break;
    }
}

void flush() {

    /* flush the cache */

}


void insert_inst(unsigned long long int addr) {

    int index;
    unsigned long long int tag;
    switch(l1_cache_assoc){
        case 1:
            index = (addr>>5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr & 0xfffffffff800;
            tag = tag >> 11;
            l1_icache[index]->tag = tag;
            break;
        case 2:
        case 4:
        default:
            printf("Default values \n");
    }
}

void write_data(unsigned long long int addr) {

    int index;
    unsigned long long int tag;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr & 0xfffffffff800;
            tag = tag >> 11;
            if(l1_dcache[index]->tag == 0){
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->address = addr;
                l1_dcache[index]->dirty = 1;
            } else {
                insert_l2(l1_dcache[index]);
                l1_dcache[index]->tag = tag;
                l1_dcache[index]->dirty = 1;
                l1_dcache[index]->address = addr;
            } 
            break;
        case 2:
        case 4:
        default:
            printf("Default values \n");
    }

}

void read_data(unsigned long long int addr) {

    // int index;
    // unsigned long long int tag;

    // switch(l1_cache_assoc){
    //     case 1:
    //         index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
    //         tag = addr & 0xfffffffff800;
    //         tag = tag >> 11;
    //         if(l1_dcache[index].tag == 0){
    //             l1_dcache[index].tag = tag;
    //             l1_dcache[index].address = addr;
    //         } else {
    //             l2_cache[index].tag = l1_dcache[index].tag;
    //             l1_dcache[index].tag = tag;
    //         } 
    //         break;
    //     case 2:
    //     case 4:
    //     default:
    //         printf("Default values \n");
    // }
}

void insert_l2(cache_entry *curr) {

    // int index;
    // unsigned long long int tag;

    // switch(l2_cache_assoc){
    //     case 1:
    //         index = (curr.address >> 6) & (l2_cache_lines-1); //cachelined - 1
    //         tag = curr.address & 0xfffffffff800;
    //         tag = tag >> 11;
    //         if(l2_cache[index].tag == 0){
    //             l2_cache[index] = curr;
    //         } else {
    //             //Calculate Write Back to MM
    //             l2_cache[index].tag = tag;
    //             l2_cache[index].dirty = tag;
    //         } 
    //         printf("Tag L2: %Lx\n", l2_cache[index].tag);
    //         break;
    //     case 2:
    //     case 4:
    //     default:
    //         printf("Default values \n");
    // }

}

int check_inst_cache(unsigned long long int addr){

    int index;
    unsigned long long int tag;

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //cachelined - 1
            tag = addr & 0xfffffffff800;
            tag = tag >> 11;
            printf("I Tag: %Lx  Index: %d Valid: %d\n",tag, index, l1_icache[index]->tag == tag);
            return l1_icache[index]->tag == tag;
            break;
        case 2:
        case 4:
        default: 
            printf("Default values \n");
    }

    return -1;
}

int check_data_cache(unsigned long long int addr){

    int index;
    unsigned long long int tag;
    // double temp = (log ((double) l1_cache_lines/2) / log (2.0));
    // int tag_offset = (int)( (5 + (temp) ) - 2 );
    // printf("Shift : %d\n", tag_offset);

    switch(l1_cache_assoc){
        case 1:
            index = (addr >> 5) & (l1_cache_lines-1); //5 = offset bits
            tag = addr & 0xfffffffff800;
            tag = tag >> 11;
            printf("D Tag: %Lx  Index: %d Valid: %d\n",tag, index, l1_dcache[index]->tag == tag);
            return l1_dcache[index]->tag == tag;
        case 2:
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
            tag = addr & 0xfffffffff800;
            tag = tag >> 15;
            return l2_cache[index]->tag == tag;
        case 2:
        case 4:
        default: 
            printf("Default values \n");
    }
    return -1;
}

void print_stats() {
    //Add When Fixed

    printf("Execute time =          %Lu;  Total refs = %Lu\n",execution_time,read_refs + write_refs+inst_refs);
    printf("Inst refs = %Lu;  Data refs = %Lu\n",inst_refs,read_refs+write_refs);

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

}

/************************************************************/
