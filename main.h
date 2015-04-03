/* **************************************
 *
 * main.h
 *
 * Header for the main file.
 *
 * Date: 31 March 2015
 *
 * Author: Jacob Resman and Healy Fuess
 ******************************************
 */

//Simulation Globals
unsigned long long int executionTime;
unsigned long long int flushTime;
unsigned long long int instRefs;
unsigned long long int totalReads;
unsigned long long int totalsWrites;
unsigned long int l1IMisses;
unsigned long long int l1IHits;
unsigned long int l1DMisses;
unsigned long long int l1DHits;
unsigned long long int l2Hits;
unsigned long long int l2Misses;

// Default Cache Parameters
#define WORD_SIZE 4
#define WORD_SIZE_OFFSET 2

//Default L1 Cache Parameters
#define L1_DEFAULT_CACHE_SIZE 8192
#define L1_DEFAULT_CACHE_BLOCK_SIZE 32
#define L1_DEFAULT_CACHE_ASSOC 1
#define L1_HIT_TIME 1
#define L1_MISS_TIME 1

//Default L2 Cache Parameters
#define L2_DEFAULT_CACHE_SIZE 32768
#define L2_DEFAULT_CACHE_BLOCK_SIZE 64
#define L2_DEFAULT_CACHE_ASSOC 1
#define L2_HIT_TIME 5
#define L2_MISS_TIME 7
#define L2_TRANSFER_TIME 5
#define L2_BUS_WIDTH 16

//Default Main Memory Access Times
#define MEM_SENDADDR 10
#define MEM_READY 30
#define MEM_CHUNKTIME 15
#define MEM_CHUNCKSIZE 8
//Cache Entry Structure
typedef struct cache_entry {
    unsigned long long int tag;
    int valid;
    int dirty;
    struct cache_entry *next;
    struct cache_entry *prev;
} cache_entry;

//Function Prototypes
void init_cache();
void perform_access();
void flush();
void insert();
void dump_settings();
void print_stats();


//TODO Add the Rest of the Variables

