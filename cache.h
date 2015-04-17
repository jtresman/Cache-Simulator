/* ***************************************************
 * cache.h
 * This is the header file for our two caches. It defines
 *  defaults and the structs for the two caches. Also includes
 *  defaults for Main Memory.
 *
 * Author: Jacob Resman and Healy Fuess
 *
 ******************************************************
 */


unsigned long long int l1_I_kickouts;
unsigned long long int l1_I_kickouts_dirty;
unsigned long long int l1_I_transfers;
unsigned long long int l1_I_kickouts_flush;
unsigned long long int l1_D_kickouts;
unsigned long long int l1_D_kickouts_dirty;
unsigned long long int l1_D_transfers;
unsigned long long int l1_D_kickouts_flush;
unsigned long long int l1_D_total_req;
unsigned long long int l1_I_total_req;
unsigned long long int l2_total_req;
unsigned long long int l2_kickouts;
unsigned long long int l2_kickouts_dirty;
unsigned long long int l2_transfers;
unsigned long long int l2_kickouts_flush;
unsigned long int l1_I_Misses;
unsigned long long int l1_I_Hits;
unsigned long int l1_D_Misses;
unsigned long long int l1_D_Hits;
unsigned long long int l2_Hits;
unsigned long long int l2_Misses;

//Percentage parameters
float read_ref_perc;
float write_ref_perc;
float inst_ref_perc;
float read_cycle_perc;
float write_cycle_perc;
float inst_cycle_perc;
float avg_cyc_write;
float avg_cyc_inst;

unsigned long long int avg_cyc_exec_time;
float avg_cyc_cpi;
unsigned long long int avg_cyc_exec_misalign;
float avg_cyc_cpi_misalign;

//Cost parameters
unsigned long long int l1_I_cost;
unsigned long long int l1_D_cost;
unsigned long long int l2_cost;
unsigned long long int mem_cost;
unsigned long long int tot_cost;
unsigned long long int flushes;
unsigned long long int invalidates;

// Default Cache Parameters
#define WORD_SIZE 4
#define WORD_SIZE_OFFSET 2

//Default L1 Cache Parameters
#define L1_DEFAULT_CACHE_SIZE 8192
#define L1_DEFAULT_CACHE_BLOCK_SIZE 32
#define L1_DEFAULT_CACHE_ASSOC 2
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
#define	MEM_READY 30
#define MEM_CHUNKTIME 15
#define MEM_CHUNCKSIZE 8

//Setup Default Cache Parameters
int l1_cache_size;
int l2_cache_size;
int l1_cache_block_size;
int l2_cache_block_size; 
// int l1_words_per_block = L1_DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
// int l2_words_per_block = L2_DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;

//Cache association
int l1_cache_assoc;
int l2_cache_assoc;

 //Simulation Globals

unsigned long long int l1_hit_time = L1_HIT_TIME;
unsigned long long int l1_miss_time = L1_MISS_TIME;
unsigned long long int l2_hit_time = L2_MISS_TIME;
unsigned long long int l2_miss_time = L2_MISS_TIME;
unsigned long long int l2_transfer_time = L2_TRANSFER_TIME;
unsigned long long int l2_bus_width = L2_BUS_WIDTH;
unsigned long long int execution_time;
unsigned long long int flush_time;
unsigned long long int inst_refs;
unsigned long long int data_refs;
unsigned long long int read_refs;
unsigned long long int write_refs;
unsigned long long int read_cycle;
unsigned long long int write_cycle;
unsigned long long int inst_cycle;
unsigned long long int l1_I_misses;
unsigned long long int l1_I_hits;
unsigned long long int l1_D_misses;
unsigned long long int l1_D_hits;
unsigned long long int l2_hits;
unsigned long long int l2_misses;
unsigned long long int total_inst;

float l1_I_hit_rate;
float l1_I_miss_rate;
float l1_D_hit_rate;
float l1_D_miss_rate;
float l2_hit_rate;
float l2_miss_rate;

int l1_cache_lines;
int l2_cache_lines;

//Cache Entry Structure
typedef struct cache_entry {
    unsigned long long int tag;
    unsigned long long int address;
    int dirty;
    int valid;
    struct cache_entry *next;
    struct cache_entry *prev;
} cache_entry;

struct cache_entry **l1_I_cache;
struct cache_entry **l1_D_cache;
struct cache_entry **l2_cache;

// Function Prototypes
void init_cache();
void perform_access(unsigned long long int addr, unsigned int byteSize, char op);
void flush();
void insert_inst(unsigned long long int addr);
void write_data(unsigned long long int addr);
void read_data(unsigned long long int addr, int dirty);
void insert_l2(unsigned long long int addr, int dirty);
int  check_inst_cache(unsigned long long int addr);
int  check_data_cache(unsigned long long int addr);
int  check_l2_cache(unsigned long long int addr);
int  is_dirty(unsigned long long int addr);
void adjust_LRU_l1i(unsigned long long int addr);
void adjust_LRU_l1d(unsigned long long int addr);
void adjust_LRU_l2(unsigned long long int addr);
void print_stats();
