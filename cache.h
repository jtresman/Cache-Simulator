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
#define L1_DEFAULT_CACHE_SIZE 9192
#define L1_DEFAULT_CACHE_BLOCK_SIZE 32
#define L1_DEFAULT_CACHE_ASSOC 1
#define L1_HIT_TIME 1
#define L1_MISS_TIME 1

//Default L2 Cache Parameters
#define L1_DEFAULT_CACHE_SIZE 32768
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


//Cache Line Structure
typedef struct cache_line_ {
  unsigned int tag;
  unsigned int index;
  unsigned int byte;
  int valid;
  int dirty;
  struct cache_line_ *LRU_next;
  struct cache_line_ *LRU_prev;
} cache_line_, *Pcache_line;

//Cache Structure
typedef struct cache_ {
  int size;			
  int associativity;		
  int n_sets;			
  unsigned index_mask;		
  int index_mask_offset;	
  Pcache_line *LRU_head;	
  Pcache_line *LRU_tail;	
  int *set_contents;		/* number of valid entries in set */
  int contents;			/* number of valid entries in cache */
} cache, *Pcache;

//Not Sure If I'll keep this yet
// typedef struct cache_stat_ {
  // int accesses;			/* number of memory references */
  // int misses;			/* number of cache misses */
  // int replacements;		/* number of misses that cause replacments */
  // int demand_fetches;		/* number of fetches */
  // int copies_back;		/* number of write backs */
// } cache_stat, *Pcache_stat;


/* function prototypes */
void set_cache_param();
void init_cache();
void perform_access();
void flush();
void delete();
void insert();
void dump_settings();
void print_stats();


/* macros */
#define LOG2(x) ((int) rint((log((double) (x))) / (log(2.0))))