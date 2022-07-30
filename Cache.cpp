#include "Cache.hpp"

Cache::Cache (const std::string &name, 
        unsigned associativity, 
        // unsigned num_ways, 
        unsigned block_size,
        unsigned capacity, 
        ReplacementPolicy rp, 
        WritePolicy wp)
{
    this->block_size = block_size;
    this->associativity = associativity;
    this->capacity = capacity;
    this->rp = PolicyLRU;
    this->wp = PolicyWriteback; 
    
    this->set_offset = std::popcount(block_size - 1);
    this->sets = capacity / (block_size * associativity);
    this->set_mask = sets - 1;
    this->set_bits = std::popcount(set_mask);
    this->num_blocks = capacity / block_size;
    caches.resize(num_blocks);
    this->tag_offset = set_offset + set_bits;
}

int Cache::check_cache_hit(unsigned addr, int type) {
    auto set = get_set(addr);
    auto tag = get_tag(addr);
    auto base = set * associativity;
    std::span local{caches.data() + base, associativity}; //TODO: why?

    int invalid_index = 1;
    int index;
    bool hit;

    for (auto i = 0;  i < local.size(); i++) {
        if (!local[i].valid) {
            auto invalid_index = i;
            continue;
        }
        auto ltag = local[i].__tag;
        if (tag != ltag) {
            // not valid tag
            continue;
        }

        hit = true;
        index = i;

        // update dirty flag, if needed
        local[i].dirty |= type;

        break; 
    }
    return hit;
}

unsigned Cache::get_set(unsigned address) {
    auto shifted_address = address >> this->set_offset;
    return shifted_address & set_mask;
}

unsigned Cache::get_tag(unsigned address) {
    return address >> this->tag_offset;
}

int Cache::get_free_line(unsigned addr, int invalid_index) {
    auto dirty_wb = false;
   // try to get an invalid index
    int index;
    if (true) {
        auto index = 0;
    }
    else {
        std::vector<unsigned *> priority;
        for (auto obj : this->caches) {
            priority.push_back(&obj.CacheMeta.__lru_count);
        } 
        assert(priority.size() == this->caches.size() == num_blocks);
        auto max_element = std::ranges::max_element(priority);
        auto index = std::distance(begin(priority), max_element);
        // dirty_wb = local_dirty[index];
    }
    std::vector<unsigned *> tags;
    // std::vector<bool *> dirtys;
    for (auto obj : this->caches) {
        tags.push_back(&obj.__tag);
        // dirtys.push_back(&obj.dirty);
    }
    auto tag = this->get_tag(addr);
    tags[index] = &tag;
    // dirtys[index] = type; 

    this->do_updates(addr, index); 
}

int Cache::do_updates(unsigned addr, int index) {
    std::vector<unsigned *> priority;
    for (auto obj : this->caches) {
        priority.push_back(&obj.CacheMeta.__lru_count);
    } 
    assert(priority.size() == this->caches.size() == num_blocks);
    
    for (auto p : priority) { 
        if (*p <= *priority[index] && *p < associativity) return *p + 1;
        else return *p;
    };
}

int Cache::do_cache_op(unsigned addr, int is_read)
{
    auto index = this->check_cache_hit(addr);
    if (index) {
        this->__stats.cache_hit_count++;
    }
    else {
        auto index = this->get_free_line(addr);
        // this->set_cache_item(item, addr);
        if (is_read) {
            this->__stats.cache_read_count++;
        }
        else {
            this->__stats.cache_write_count++;
        }
        this->__stats.cache_miss_count++;
    }
    return 0;
}

void Cache::dump_stats() {
    std::cout << "CACHE SETTINGS:" << std::endl;
    std::cout << "HITS:" << this->__stats.cache_hit_count << std::endl;
    std::cout << "MISS:" << this->__stats.cache_miss_count << std::endl;
    std::cout << "READS:" << this->__stats.cache_read_count << std::endl;
    std::cout << "WRITES:" << this->__stats.cache_write_count << std::endl;
}
