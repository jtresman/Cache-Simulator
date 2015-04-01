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

cache_entry **l1_icache;
cache_entry **l1_dcache;
cache_entry **l2_cache;

/************************************************************/

void init_cache() {
	
	//TODO Determine what Fully Associative Looks Like
	
    //Allocate Space for L1 Instruction Cache
	int rows = l1_cache_size/((4 * WORD_SIZE)*l1_cache_assoc);
	l1_icache = malloc(rows * sizeof(cache_entry *));
	for(i = 0; i < rows; i++) {
		l1_icache[i] = malloc(l1_cache_assoc * sizeof(cache_entry));
	}

    //Allocate Space for L1 Data Cache
	l1_dcache = malloc(rows * sizeof(cache_entry *));
	for(i = 0; i < rows; i++) {
		l1_dcache[i] = malloc(l1_cache_assoc * sizeof(cache_entry));
	}
	
	//Allocate Space for L2 Cache
	rows = l2_cache_size/((4 * WORD_SIZE)*l2_cache_assoc);
	l2_cache = malloc(rows * sizeof(cache_entry *));
	for(i = 0; i < rows; i++) {
		l2_cache[i] = malloc(l2_cache_assoc * sizeof(cache_entry));
	}
}

void perform_access(addr, access_type) {

  /* handle an access to the cache */

}

void flush() {

  /* flush the cache */

}


void insert(addr, op) {
  
}


void print_stats() {
  
  //TODO Change to Formating He Wants
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

void free_cache(){
	
	//Free L1 Data Cache
    int rows = l1_cache_size/((4 * WORD_SIZE)*l1_cache_assoc);
	for(i = 0; i < rows; i++) {
		free(l1_dcache[i]);
	}
	free(l1_dcache);
	
	//Free L1 Instruction Cache
	for(i = 0; i < rows; i++) {
		free(l1_icache[i]);
	}
	free(l1_icache);
	
	//Free L2 Cache
    rows = l2_cache_size/((4 * WORD_SIZE)*l2_cache_assoc);
	for(i = 0; i < rows; i++) {
		free(l2_cache[i]);
	}
	free(l2_cache);
	
	
	
}
/************************************************************/