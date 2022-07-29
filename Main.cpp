#include "Cache.hpp"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>

// Pass simple, one character command line options
// Taken from: https://stackoverflow.com/a/868894
class ArgParser {
    public:
        ArgParser (int &argc, char **argv) {
            for (int i; i < argc; ++i) {
                
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
int main(int argc, char **argv) {
    auto experiment = new SimulRun();
    auto parser = new Parser();
    auto argparser = ArgParser(argc, argv);

    if (argparser.cmdOptionExists("-h")) {
        
    }

    const std::string &filename = argparser.getCmdOption("-f");
    if (!filename.empty()) {
        throw Error(std::fmt("Filename is empty!")); 
    }

    parser->Load(filename);
}
