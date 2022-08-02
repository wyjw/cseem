#include "Cache.hpp"
#include "Parser.hpp"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <memory>

// Pass simple, one character command line options
// Taken from: https://stackoverflow.com/a/868894
class ArgParser {
    public:
        ArgParser (int &argc, char **argv) {
            for (int i = 1; i < argc; ++i) {
               this->tokens.push_back(std::string(argv[i])); 
            }
        }
        
        const std::string& getCmdOption (const std::string &option) const {
            std::vector<std::string>::const_iterator itr;
            itr = std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }

        bool cmdOptionExists(const std::string &option) const {
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                    != this->tokens.end();    
        }

    private:
        std::vector<std::string> tokens;
};

// Main runner
int main(int argc, char *argv[]) {
    // auto experiment = new SimulRun();
    auto parser = new Parser();
    auto argparser = ArgParser(argc, argv);
    
    // config options
    bool dump_table = false;
    bool print_stats = false;
    std::string format = "hw";

    if (argparser.cmdOptionExists("-h")) {
        auto _w = 40;
        std::cout << std::left << std::setw(_w) << "CSeme: the cache simulator!" << std::endl; 
        std::cout << std::setw(_w) << "----" << std::endl;
        std::cout << std::setw(_w) << "Help Section" << std::endl; 
        std::cout << std::left << std::setw(_w) << "-h: help" << std::endl; 
        std::cout << std::left << std::setw(_w) << "-f {filename}: (necessary)" << std::endl; 
        std::cout << std::left << std::setw(_w) << "-t {format}: (default = hw)" << std::endl; 
        std::cout << std::left << std::setw(_w) << "-d: dump table (default = false)" << std::endl; 
        std::cout << std::left << std::setw(_w) << "-s: print stats (default = false)" << std::endl; 
        exit(0);
    } 
    else { 
        if (argparser.cmdOptionExists("-d"))
            dump_table = true;
        if (argparser.cmdOptionExists("-s"))
            print_stats = true;
    }
    const std::string &filename = argparser.getCmdOption("-f");
    if (filename.empty()) {
        std::cout << "Please input a filename under -f" << std::endl;
        throw std::runtime_error("Filename is empty!"); 
    }
    
    const std::string &tmp_format = argparser.getCmdOption("-t");
    if (!tmp_format.empty()) {
        if (tmp_format != "hw" or tmp_format != "simple")
            throw std::runtime_error("Not valid format!"); 
        format = tmp_format;
    }

    auto c = std::make_unique<Cache>("cache1", 1, 16, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    parser->Load(filename, c, "hw");

    if (dump_table)
        c->dump_state();

    if (print_stats)
        c->dump_stats();
}
