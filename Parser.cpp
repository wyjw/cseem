#include "Parser.hpp"

class Parser {
    void Parse(std::ifstream *f) {
        std::string line;
        while (std::getline(f, line))
        {
            std::istringstream iss(line);
            char a;
            std::hex b;

            if (!iss >> a >> b) { break; } // error 

            // do stuff with line
        }
        f.close();
    }
};
