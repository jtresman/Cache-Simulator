/*****************************************************
 * Cache Simulator
 *
 * This program takes program traces that include a 
 *  Operation, Memory Address and Byte Size. It 
 *  keeps track of L1 data and instruction cache, L2
 *  cache and main memory.
 *
 *  20 March 2015
 *
 *  Author: Jacob Resman and Healy Fuess
 ****************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

//Setup Default Cache Parameters
static int l1_cache_size = L1_DEFAULT_CACHE_SIZE;
static int l2_cache_size = L2_DEFAULT_CACHE_SIZE; 
static int l1_cache_block_size = L1_DEFAULT_CACHE_BLOCK_SIZE;
static int l2_cache_block_size = L2_DEFAULT_CACHE_BLOCK_SIZE;
static int l1_words_per_block = L1_DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int l2_words_per_block = L2_DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int l1_cache_assoc = L1_DEFAULT_CACHE_ASSOC;
static int l2_cache_assoc = L2_DEFAULT_CACHE_ASSOC;

int main(int argc, char **argv){

    char op;
    unsigned long long int address;
    unsigned int bytesize;

    cache_entry l1_icache[l1_cache_size]; 
    cache_entry l1_dcahce[l1_cache_size];

    while(scanf("%c %Lx %d\n",&op,&address,&bytesize) == 3){

        printf("OP      : %c  ", op);
        printf("Address : %Lx ", address);
        printf("ByteSize: %d  ", bytesize); 

    }

    return 0;

}

void print_stats() {

    //TODO Change to Correct Formating   
    printf("\n*** CACHE STATISTICS ***\n");

    printf(" INSTRUCTIONS\n");
    printf("  accesses:  %d\n", cache_stat_inst.accesses);
    printf("  misses:    %d\n", cache_stat_inst.misses);
    if (!cache_stat_inst.accesses)
        printf("  miss rate: 0 (0)\n");
    else
        printf("  miss rate: %2.4f (hit rate %2.4f)\n",
                (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses,
                1.0 - (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses);
    printf("  replace:   %d\n", cache_stat_inst.replacements);

    printf(" DATA\n");
    printf("  accesses:  %d\n", cache_stat_data.accesses);
    printf("  misses:    %d\n", cache_stat_data.misses);
    if (!cache_stat_data.accesses)
        printf("  miss rate: 0 (0)\n");
    else
        printf("  miss rate: %2.4f (hit rate %2.4f)\n",
                (float)cache_stat_data.misses / (float)cache_stat_data.accesses,
                1.0 - (float)cache_stat_data.misses / (float)cache_stat_data.accesses);
    printf("  replace:   %d\n", cache_stat_data.replacements);

    printf(" TRAFFIC (in words)\n");
    printf("  demand fetch:  %d\n", cache_stat_inst.demand_fetches +
            cache_stat_data.demand_fetches);
    printf("  copies back:   %d\n", cache_stat_inst.copies_back +
            cache_stat_data.copies_back);
}

