#include <string>

#ifndef PROJECTNAME
#define PROJECTNAME cseem
#endif

using namespace PROJECTNAME;

// Main class for the cache
class Cache {
    public:
    enum ReplacementPolicy {
        PolicyLRU,
        PolicyFIFO,
        PolicyRandom,
    };
   
    enum WritePolicy {
        PolicyWriteback
    };

    class CacheLine {
        friend class Cache;

        unsigned __tag;
        union {
          unsigned __count;
          unsigned __lru_count;
          unsigned __fifo_count;
        } CacheMeta;

        unsigned *buf;
        unsigned flag;
    };

    // Constructor
    Cache (const std::string &name, 
            unsigned num_sets, 
            unsigned num_ways, 
            unsigned block_size, 
            ReplacementPolicy rp, 
            WritePolicy wp);
    ~Cache();
};

// Run one iteration of an experiment
class SimulRun {
    public:
    SimulRun();
    ~SimulRun();

    float hit_rate() {
        return hit_count / (hit_count + miss_count);
    };

    private: 
    int hit_count;
    int miss_count;
};

// Parser
class Parser {
    
};

class LDFMParser : public Parser {
    
};
