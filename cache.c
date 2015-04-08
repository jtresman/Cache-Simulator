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

//Setup Default Cache Parameters
static int l1_cache_size = L1_DEFAULT_CACHE_SIZE;
static int l2_cache_size = L2_DEFAULT_CACHE_SIZE; 
static int l1_cache_block_size = L1_DEFAULT_CACHE_BLOCK_SIZE;
static int l2_cache_block_size = L2_DEFAULT_CACHE_BLOCK_SIZE;
static int l1_words_per_block = L1_DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int l2_words_per_block = L2_DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int l1_cache_assoc = L1_DEFAULT_CACHE_ASSOC;
static int l2_cache_assoc = L2_DEFAULT_CACHE_ASSOC;

//Cache Globals
 //Simulation Globals
static unsigned long long int executionTime;
static unsigned long long int flushTime;
static unsigned long long int instRefs;
static unsigned long long int totalReads;
static unsigned long long int totalsWrites;
static unsigned long int l1IMisses;
static unsigned long long int l1IHits;
static unsigned long int l1DMisses;
static unsigned long long int l1DHits;
static unsigned long long int l2Hits;
static unsigned long long int l2Misses;

/************************************************************/

static cache_entry l1_icache[256];
static cache_entry l1_dcache[256];
static cache_entry l2_cache[512];

void init_cache() {

  int i;
  int l1CacheLines = l1_cache_size/l1_cache_block_size/l1_cache_assoc;
  int l2CacheLines = l2_cache_size/l2_cache_block_size/l2_cache_assoc;
	// l1_dcache = malloc(sizeof(cache_entry)*(l1_cache_size/l1_cache_block_size/l1_cache_assoc));
	// l1_icache = malloc(sizeof(cache_entry)*(l1_cache_size/l1_cache_block_size/l1_cache_assoc));
	// l2_cache  = malloc(sizeof(cache_entry)*(l2_cache_size/l2_cache_block_size/l2_cache_assoc);

  //Define Cleared Cache Entry
  struct cache_entry blankEntry;
  blankEntry.tag = 0;
  blankEntry.dirty = 0;
  blankEntry.valid = 0;
  blankEntry.address = 0;

  //Setup l1 Cache Entries For D-Map
  for ( i = 0; i < l1CacheLines; ++i){
      l1_icache[i] = blankEntry;
      l1_dcache[i] = blankEntry;
  }
		
  //Setup l2 Cache Entries for D-Map
  for ( i = 0; i < l2CacheLines; ++i){
      l2_cache[i] = blankEntry;
  }
  
}

void perform_access(unsigned long long int addr, unsigned int byteSize, char op) {

  	switch(op){
  		case 'I':
         if(check_inst_cache(addr) != 1){
  			 	 insert_inst(addr);
           //Do Hit Calculations
  			 } else {
            //Do Hit Calculations
         } 
  			 break;
  		case 'W':
         if(check_data_cache(addr) != 1){
           write_data(addr);
           //Do Hit Calculations and mark dirty
         } else {
            //Do Hit Calculations and mark dirty
         } 
         break;
  		case 'R':
         if(check_data_cache(addr) != 1){
           read_data(addr);
           //Do Hit Calculations
         } else {
            //Do Hit Calculations
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
  
  switch(l1_cache_assoc){
  	case 1:
        index = addr & 0x7f; //cachelined - 1
        tag = addr & 0xfffffffff800;
        tag = tag >> 11;
  			l1_icache[index].tag = tag;
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
    		index = addr & 0x7f; //cachelined - 1
        tag = addr & 0xfffffffff800;
        tag = tag >> 11;
        if(l1_dcache[index].tag == 0){
    			l1_dcache[index].tag = tag;
          l1_dcache[index].address = addr;
          l1_dcache[index].dirty = 1;
    		} else {
    			insert_l2(l1_dcache[index]);
    			l1_dcache[index].tag = tag;
          l1_dcache[index].dirty = 1;
          l1_dcache[index].address = addr;
    		} 
    		break;
    	case 2:
    	case 4:
    	default:
        printf("Default values \n");
  }

}

void read_data(unsigned long long int addr) {
    
    int index;
    unsigned long long int tag;
    
    switch(l1_cache_assoc){
      case 1:
        index = addr & 0x7f; //cachelined - 1
        tag = addr & 0xfffffffff800;
        tag = tag >> 11;
        if(l1_dcache[index].tag == 0){
          l1_dcache[index].tag = tag;
          l1_dcache[index].address = addr;
        } else {
          l2_cache[index].tag = l1_dcache[index].tag;
          l1_dcache[index].tag = tag;
        } 
        break;
      case 2:
      case 4:
      default:
        printf("Default values \n");
  }
}

void insert_l2(struct cache_entry) {
    
  //   int index;
  //   unsigned long long int tag;
    
  //   switch(l2_cache_assoc){
  //     case 1:
  //       index = cache_entry.address & 0x7f; //cachelined - 1
  //       tag = cache_entry.address & 0xfffffffff800;
  //       tag = tag >> 11;
  //       if(l2_cache[index].tag == 0){
  //         l2_cache[index].tag = tag;
  //       } else {
  //         //Calculate Write Back to MM
  //         l2_cache[index].tag = tag;
  //         l2_cache[index].dirty = tag;
  //       } 
  //       break;
  //     case 2:
  //     case 4:
  //     default:
  //       printf("Default values \n");
  // }

}

int check_inst_cache(unsigned long long int addr){
  
  int index;
  unsigned long long int tag;

	switch(l1_cache_assoc){
		case 1:
      index = addr & 0x7f; //cachelined - 1
      tag = addr & 0xfffffffff800;
      tag = tag >> 11;
			return l1_icache[index].tag == tag;
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

	switch(l1_cache_assoc){
		case 1:
			index = addr & 0x7f; //cachelined - 1
      tag = addr & 0xfffffffff800;
      tag = tag >> 11;
      return l1_dcache[index].tag == tag;
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
			index = addr & 0x7f; //cachelined - 1
      tag = addr & 0xfffffffff800;
      tag = tag >> 11;
      return l2_cache[index].tag == tag;
		case 2:
		case 4:
		default: 
      printf("Default values \n");
	}
  return -1;
}

void print_stats() {
    // printf("SIMULATION RESULTS\n");
    // printf("\n");
    // printf("Memory system:\n");
    // printf("    Dcache size = %d : ways = %d : block size = %d\n", a,b,c);
    // printf("    Icache size = %d : ways = %d : block size = %d\n", a,b,c);
    // printf("    L2-cache size = %d : ways = %d : block size = %d\n",a,b,c);
    // printf("    Memory ready time = %d : chunksize = %d : chunktime = %d\n",a,b,c);
    // printf("\n");
    // printf("Execute time =          %d;  Total refs = %d\n",a,b);
    // printf("Flush time =            %d\n",a);
    // printf("Inst refs = %d;  Data refs = %d\n",a,b);
    // printf("\n");
    // printf("Number of reference types:  [Percentage]\n");
    // printf("    Reads  =           %d    [%3.1f%]\n",a,b);
    // printf("    Writes =           %d    [%3.1f%]\n",a,b);
    // printf("    Inst.  =           %d    [%3.1f%]\n",a,b);
    // printf("    Total  =          %d\n",a+b+c);
    // printf("\n");
    // printf("Total cycles for activities:  [Percentage]\n");
    // printf("    Reads  =           %d    [%3.1f%]\n",a,b);
    // printf("    Writes =           %d    [%3.1f%]\n"a,b);
    // printf("    Inst.  =           %d    [%3.1f%]\n"a,b);
    // printf("    Total  =           %d\n",a+b+c);
    // printf("\n");
    // printf("Average cycles per activity:\n");
    // printf("    Read =  N/A; Write = %3.1f; Inst. = %3.1f\n",a,b);
    // printf("Ideal: Exec. Time = %d; CPI =  %2.1f\n",a,b);
    // printf("Ideal mis-aligned: Exec. Time = %d; CPI =  %2.1f\n",a,b);
    // printf("\n");
    // printf("Memory Level:  L1i\n");
    // printf("    Hit Count = %d  Miss Count = %d\n",a,b);
    // printf("    Total Requests = %d\n",a);
    // printf("    Hit Rate = %3.1f%   Miss Rate = %3.1f%\n"a,b);
    // printf("    Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n",a,b,c);
    // printf("    Flush Kickouts = %d\n");
    // printf("\n");
    // printf("Memory Level:  L1d\n");
    // printf("    Hit Count = %d  Miss Count = %d\n",a,b);
    // printf("    Total Requests = %d\n",a);
    // printf("    Hit Rate = %3.1f%   Miss Rate = %3.1f%\n",a,b);
    // printf("    Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n",a,b,c);
    // printf("    Flush Kickouts = %d\n",a);
    // printf("\n");
    // printf("Memory Level:  L2\n");
    // printf("    Hit Count = %d  Miss Count = %d\n",a,b);
    // printf("    Total Requests = %d\n",a);
    // printf("    Hit Rate =  %3.1%   Miss Rate = %4.1f%\n"
    // printf("    Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n",a,b,c);
    // printf("    Flush Kickouts = %d\n");
    // printf("\n");
    // printf("L1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n",a,b,c);
    // printf("L2 cache cost = $%d;  Memory cost = $%d  Total cost = $%d\n",a,b,c);
    // printf("Flushes = %d : Invalidates = %d\n",a,b);
    // printf("\n");
    // printf("-------------------------------------------------------------\n");
    // printf("\n");
    // printf("Cache final contents - Index and Tag values are in HEX\n");
    // printf("\n");
    // printf("Memory Level:  L1i\n")
    // printf("Index:    %x | V:%d D:%d Tag:    %x | V:%d D:%d Tag: - |\n",a,b,c,d,e,f); 
    // printf("Index:    %x | V:%d D:%d Tag:    %x | V:%d D:%d Tag: - |\n",a,b,c,d,e,f);
    // printf("\n");
    // printf("Memory Level:  L1d\n");
    // printf("Index:    %x | V:%d D:%d Tag:    %x | V:%d D:%d Tag: - |\n",a,b,c,d,e,f); 
    // printf("\n");
    // printf("Memory Level:  L2\n");
    // printf("Index:   %x | V:%d D:%d Tag:     %x |\n",a,b,c,d); 
    // printf("Index:   %x | V:%d D:%d Tag:     %x |\n",a,b,c,d); 
    // printf("Index:   %x | V:%d D:%d Tag:     %x |\n",a,b,c,d);
}

/************************************************************/
