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
    printf("SIMULATION RESULTS\n");
    printf("\n");
    printf("Memory system:\n");
    printf("    Dcache size = %d : ways = %d : block size = %d\n", a,b,c);
    printf("    Icache size = %d : ways = %d : block size = %d\n", a,b,c);
    printf("    L2-cache size = %d : ways = %d : block size = %d\n",a,b,c);
    printf("    Memory ready time = %d : chunksize = %d : chunktime = %d\n",a,b,c);
    printf("\n");
    printf("Execute time =          %d;  Total refs = %d\n",a,b);
    printf("Flush time =            %d\n",a);
    printf("Inst refs = %d;  Data refs = %d\n",a,b);
    printf("\n");
    printf("Number of reference types:  [Percentage]\n");
    printf("    Reads  =           %d    [%3.1f%]\n",a,b);
    printf("    Writes =           %d    [%3.1f%]\n",a,b);
    printf("    Inst.  =           %d    [%3.1f%]\n",a,b);
    printf("    Total  =          %d\n",a+b+c);
    printf("\n");
    printf("Total cycles for activities:  [Percentage]\n");
    printf("    Reads  =           %d    [%3.1f%]\n",a,b);
    printf("    Writes =           %d    [%3.1f%]\n"a,b);
    printf("    Inst.  =           %d    [%3.1f%]\n"a,b);
    printf("    Total  =           %d\n",a+b+c);
    printf("\n");
    printf("Average cycles per activity:\n");
    printf("    Read =  N/A; Write = %3.1f; Inst. = %3.1f\n",a,b);
    printf("Ideal: Exec. Time = %d; CPI =  %2.1f\n",a,b);
    printf("Ideal mis-aligned: Exec. Time = %d; CPI =  %2.1f\n",a,b);
    printf("\n");
    printf("Memory Level:  L1i\n");
    printf("    Hit Count = %d  Miss Count = %d\n",a,b);
    printf("    Total Requests = %d\n",a);
    printf("    Hit Rate = %3.1f%   Miss Rate = %3.1f%\n"a,b);
    printf("    Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n",a,b,c);
    printf("    Flush Kickouts = %d\n");
    printf("\n");
    printf("Memory Level:  L1d\n");
    printf("    Hit Count = %d  Miss Count = %d\n",a,b);
    printf("    Total Requests = %d\n",a);
    printf("    Hit Rate = %3.1f%   Miss Rate = %3.1f%\n",a,b);
    printf("    Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n",a,b,c);
    printf("    Flush Kickouts = %d\n",a);
    printf("\n");
    printf("Memory Level:  L2\n");
    printf("    Hit Count = %d  Miss Count = %d\n",a,b);
    printf("    Total Requests = %d\n",a);
    printf("    Hit Rate =  %3.1%   Miss Rate = %4.1f%\n"
    printf("    Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n",a,b,c);
    printf("    Flush Kickouts = %d\n");
    printf("\n");
    printf("L1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n",a,b,c);
    printf("L2 cache cost = $%d;  Memory cost = $%d  Total cost = $%d\n",a,b,c);
    printf("Flushes = %d : Invalidates = %d\n",a,b);
    printf("\n");
    printf("-------------------------------------------------------------\n");
    printf("\n");
    printf("Cache final contents - Index and Tag values are in HEX\n");
    printf("\n");
    printf("Memory Level:  L1i\n")
    printf("Index:    %x | V:%d D:%d Tag:    %x | V:%d D:%d Tag: - |\n",a,b,c,d,e,f); 
    printf("Index:    %x | V:%d D:%d Tag:    %x | V:%d D:%d Tag: - |\n",a,b,c,d,e,f);
    printf("\n");
    printf("Memory Level:  L1d\n");
    printf("Index:    %x | V:%d D:%d Tag:    %x | V:%d D:%d Tag: - |\n",a,b,c,d,e,f); 
    printf("\n");
    printf("Memory Level:  L2\n");
    printf("Index:   %x | V:%d D:%d Tag:     %x |\n",a,b,c,d); 
    printf("Index:   %x | V:%d D:%d Tag:     %x |\n",a,b,c,d); 
    printf("Index:   %x | V:%d D:%d Tag:     %x |\n",a,b,c,d);


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

