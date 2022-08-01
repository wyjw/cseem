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

    this->caches = std::vector<CacheLine>();
    this->caches.resize(num_blocks);
    this->tag_offset = set_offset + set_bits;
}

Cache::~Cache ()
{
}

int Cache::check_cache_hit(unsigned addr, int type) {
    auto set = get_set(addr);
    auto tag = get_tag(addr);
    auto base = set * associativity;
    std::span local{this->caches.data() + base, associativity}; //TODO: why?

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
        index = 0;
    }
    else {
        std::vector<unsigned *> priority;
        for (auto obj : this->caches) {
            priority.push_back(&obj.CacheMeta.__lru_count);
        } 
        assert(priority.size() == this->caches.size() == num_blocks);
        auto max_element = std::ranges::max_element(priority);
        index = std::distance(begin(priority), max_element);
        // dirty_wb = local_dirty[index];
    }
    std::vector<unsigned *> tags;
    // std::vector<bool *> dirtys;
    for (auto obj : this->caches) {
        tags.push_back(&obj.__tag);
        // dirtys.push_back(&obj.dirty);
    }
    auto tag = this->get_tag(addr);
    *tags[index] = tag;
    // dirtys[index] = type; 

    this->do_updates(addr, index);
    
    return index;
}

int Cache::do_updates(unsigned addr, int index) {
    std::vector<unsigned *> priority;
    for (auto obj : this->caches) {
        priority.push_back(&obj.CacheMeta.__lru_count);
    }
     
    if (!(priority.size() == this->caches.size() && this->caches.size() == num_blocks)) {
        throw std::runtime_error("Cache not right size!");
    }
    
    for (auto p : priority) { 
        if (*p <= *priority[index] && *p < associativity) return *p + 1;
        else return *p;
    };

    return 0; 
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

// pretty print the cache state
void Cache::dump_state() {
    // std::cout << "" << std::endl;
    std::vector<std::string> columns;
    std::vector<int> column_widths;
    std::string separator = "|";
    int table_width;

    columns.push_back("Index");
    column_widths.push_back(4);
    table_width += 4 + separator.size();

    columns.push_back("Valid");
    column_widths.push_back(4);
    table_width += 4 + separator.size();

    columns.push_back("Tag");
    column_widths.push_back(32);
    table_width += 32 + separator.size();

    columns.push_back("Data");
    column_widths.push_back(64);
    table_width += 64 + separator.size();

    columns.push_back("DirtyBit");
    column_widths.push_back(4);
    table_width += 4 + separator.size();

    int len = 0;
    for (auto c : caches) {
        len++;
        for (auto i = 0; i <= columns.size(); i++) {
            // columns[i] 
            // column_widths[i]

            std::cout << std::setw(column_widths[i]);
            std::string val;
            if (columns[i] == "Index") {
                std::cout << len;
            }
            else if (columns[i] == "Valid") {
                std::cout << c.valid;
            } else if (columns[i] == "Tag") {
                std::cout << c.__tag;
            } else if (columns[i] == "Data") {
                std::cout << c.buf;
            } else if (columns[i] == "DirtyBit") {
                std::cout << c.dirty; 
            }
        }
    } 
}
