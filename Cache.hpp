#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>
#include <span>
#include <iostream>
#include <bit>
#include <assert.h>
#include <algorithm>
#include <iomanip>

#ifndef PROJECTNAME
#define PROJECTNAME cseem
#endif

namespace PROJECTNAME {}
using namespace PROJECTNAME;

// Main class for the cache
class Cache {
    public:
    enum ReplacementPolicy {
        PolicyLRU,
        PolicyFIFO,
        PolicyRandom,
    }; 
    ReplacementPolicy rp;

    enum WritePolicy {
        PolicyWriteback
    };
    WritePolicy wp;

    class CacheLine {
        friend class Cache;
        
        public:
        unsigned __tag;
        struct {
          unsigned __count;
          unsigned __lru_count;
          unsigned __fifo_count;
        } CacheMeta;

        unsigned *buf;
        //unsigned flag;
        bool valid;
        bool dirty;
        
        CacheLine () {
            __tag = 0;
            CacheMeta.__count = 0;
            CacheMeta.__lru_count = 0;
            CacheMeta.__fifo_count = 0;
            buf = NULL;
            valid = false;
            dirty = false;
        }
    };

    // Configuration parameters
    unsigned block_size;
    unsigned associativity;
    unsigned capacity;
    unsigned miss_penalty;
    unsigned dirty_wb_penalty;
    
    unsigned set_offset;
    unsigned sets;
    unsigned set_mask;
    unsigned set_bits;
    unsigned num_blocks;
    unsigned tag_offset;

    unsigned clock_count;
    // offsets
    // |**** TAG ****|**** SET ****|**** OFFSET ****|

    unsigned get_set(unsigned address); 
    unsigned get_tag(unsigned address);

    std::vector<CacheLine> caches;

    // Constructor
    Cache (const std::string &name, 
            unsigned num_sets, 
            // unsigned num_ways, 
            unsigned block_size,
            unsigned capacity, 
            ReplacementPolicy rp, 
            WritePolicy wp);
    ~Cache();

    int check_cache_hit(unsigned addr, int* invalid_index, int* rindex, int type = 0);
    void dump_stats();
    void dump_state(bool meta = false);
    //int get_free_line(int level);
    int get_free_line(unsigned addr, int invalid_index=0);
    int do_updates(unsigned addr, int index);
    int do_updates_only_on_write(unsigned addr, int index);
    int do_cache_op(unsigned addr, int is_read);

    // Run one iteration of an experiment
    struct Stats {
        float hit_rate() {
            return (float)cache_hit_count / (float)(cache_hit_count + cache_miss_count);
        };
        int cache_hit_count;
        int cache_miss_count;
        int cache_read_count;
        int cache_write_count;
        int cache_store_hit_count;
        int cache_load_hit_count;
        int cache_store_miss_count;
        int cache_load_miss_count;
    };
    struct Stats __stats;
};
#endif
