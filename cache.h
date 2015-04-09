/* ***************************************************
 * cache.h
 * This is the header file for our two caches. It defines
 *  defaults and the structs for the two caches. Also includes
 *  defaults for Main Memory.
 *
 * Author: Jacob Resman and Healy Fuess
 */

 //Simulation Globals
unsigned long long int executionTime;
unsigned long long int flushTime;
unsigned long long int instRefs;
unsigned long long int dataRefs;
unsigned long long int totalReads;
unsigned long long int totalWrites;
unsigned long long int l1ItotalReq;
unsigned long long int l1DtotalReq;
unsigned long long int l2totalReq;

unsigned float l1IhitRate;
unsigned float l1ImissRate;
unsigned float l1DhitRate;
unsigned float l1DmissRate;
unsigned float l2hitRate;
unsigned float l2missRate;

unsigned long long int l1Ikickouts;
unsigned long long int l1Ikickouts_dirty;
unsigned long long int l1Itransfers;
unsigned long long int l1Ikickouts_flush;
unsigned long long int l1Dkickouts;
unsigned long long int l1Dkickouts_dirty;
unsigned long long int l1Dtransfers;
unsigned long long int l1Dkickouts_flush;
unsigned long long int l2kickouts;
unsigned long long int l2kickouts_dirty;
unsigned long long int l2transfers;
unsigned long long int l2kickouts_flush;
unsigned long int l1IMisses;
unsigned long long int l1IHits;
unsigned long int l1DMisses;
unsigned long long int l1DHits;
unsigned long long int l2Hits;
unsigned long long int l2Misses;

unsigned float read_ref_type;
unsigned float read_ref_perc;
unsigned float write_ref_type;
unsigned float write_ref_perc;
unsigned float inst_ref_type;
unsigned float inst_ref_perc;
unsigned float read_cycle;
unsigned float read_cycle_perc;
unsigned float write_cycle;
unsigned float write_cycle_perc;
unsigned float inst_cycle;
unsigned float inst_cycle_perc;
unsigned float avg_cyc_write;
unsigned float avg_cyc_inst;
unsigned float tot_ref_type;
unsigned float tot_cycle;

unsigned long long int avg_cyc_exec_time;
unsigned float avg_cyc_cpi;
unsigned long long int avg_cyc_exec_misalign;
unsigned float avg_cyc_cpi_misalign;
unsigned long long int l1Icost;
unsigned long long int l1Dcost;
unsigned long long int l2cost;
unsigned long long int memcost;
unsigned long long int totcost;
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

//Cache Entry Structure
typedef struct cache_entry {
    unsigned long long int tag;
    unsigned long long int address;
    int dirty;
    int valid;
    cache_entry *next;
    cache_entry *prev;
} cache_entry;

// Function Prototypes
void init_cache();
void perform_access(unsigned long long int addr, unsigned int byteSize, char op);
void flush();
void insert();
void dump_settings();
void print_stats();
int  check_inst_cache(unsigned long long int addr);
int check_data_cache(unsigned long long int addr);
int check_l2_cache(unsigned long long int addr);
void insert_inst(unsigned long long int addr);
void write_data(unsigned long long int addr);
void read_data(unsigned long long int addr);
void insert_l2(struct cache_entry);
