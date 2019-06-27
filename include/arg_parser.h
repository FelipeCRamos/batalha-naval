#ifndef _arg_parser_h_
#define _arg_parser_h_

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <utility>

class Args {
    private:
        std::vector<std::string> elements;

    public:
        Args(int argc, char **argv) {
            for( int i = 1; i < argc; i++ ) {
                elements.push_back(std::string(argv[i]));
            }
        }

        bool checkArg(std::string argument) {
            if(std::find(elements.begin(), elements.end(), argument) != elements.end()) {
                return true;
            }
            return false;
        }

        std::string getArg(std::string argument) {
            std::string buf = "";
            auto res = std::find(elements.begin(), elements.end(), argument);

            if(res != elements.end()) {
                ++res;
                return *res;

            }
            return buf;
        }
};

#endif
