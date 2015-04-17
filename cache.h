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

 //Simulation Globals
unsigned long long int l1_ikickouts;
unsigned long long int l1_ikickouts_dirty;
unsigned long long int l1_itransfers;
unsigned long long int l1_ikickouts_flush;
unsigned long long int l1_ikickouts;
unsigned long long int l1_dkickouts_dirty;
unsigned long long int l1_dtransfers;
unsigned long long int l1_dkickouts_flush;
unsigned long long int l1_dtotal_req;
unsigned long long int l1_itotal_req;
unsigned long long int l2_total_req;
unsigned long long int l2_kickouts;
unsigned long long int l2_kickouts_dirty;
unsigned long long int l2_transfers;
unsigned long long int l2_kickouts_flush;

float read_ref_perc;
float write_ref_perc;
float inst_ref_perc;
float read_cycle_perc;
float write_cycle_perc;
float inst_cycle_perc;
float avg_cyc_write;
float avg_cyc_inst;
float avg_cyc_read;

unsigned long long int avg_cyc_exec_time;
float avg_cyc_cpi;
unsigned long long int avg_cyc_exec_misalign;
float avg_cyc_cpi_misalign;
unsigned long long int l1_icost;
unsigned long long int l1_dcost;
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
#define L1_DEFAULT_CACHE_ASSOC 1
#define L1_HIT_TIME 1
#define L1_MISS_TIME 1
#define L1_OFFSET 5

//Default L2 Cache Parameters
#define L2_DEFAULT_CACHE_SIZE 32768
#define L2_DEFAULT_CACHE_BLOCK_SIZE 64
#define L2_DEFAULT_CACHE_ASSOC 1
#define L2_HIT_TIME 5
#define L2_MISS_TIME 7
#define L2_TRANSFER_TIME 5
#define L2_BUS_WIDTH 16
#define MEM_TO_L2 160
#define L2_TO_L1 10
#define L2_OFFSET 6

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
int l1_cache_assoc;
int l2_cache_assoc;

 //Simulation Globals
unsigned long long int execution_time;
unsigned long long int flush_time;
unsigned long long int inst_refs;
unsigned long long int read_refs;
unsigned long long int write_refs;
unsigned long long int read_cycle;
unsigned long long int write_cycle;
unsigned long long int inst_cycle;
unsigned long long int l1_imisses;
unsigned long long int l1_ihits;
unsigned long long int l1_dmisses;
unsigned long long int l1_dhits;
unsigned long long int l2_hits;
unsigned long long int l2_misses;
unsigned long long int total_inst;

float l1_ihit_rate;
float l1_imiss_rate;
float l1_dhit_rate;
float l1_dmiss_rate;
float l2_hit_rate;
float l2_miss_rate;

int l1_cache_lines;
int l2_cache_lines;
int shift_amount_l1;
int shift_amount_l2;

//Cache Entry Structure
typedef struct cache_entry {
    unsigned long long int tag;
    unsigned long long int address;
    int dirty;
    int valid;
    struct cache_entry *next;
    struct cache_entry *prev;
} cache_entry;

struct cache_entry **l1_icache;
struct cache_entry **l1_dcache;
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
