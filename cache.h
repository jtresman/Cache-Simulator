/* ***************************************************
 * cache.h
 * This is the header file for our two caches. It defines
 *  defaults and the structs for the two caches. Also includes
 *  defaults for Main Memory.
 *
 * Author: Jacob Resman and Healy Fuess
 */

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
