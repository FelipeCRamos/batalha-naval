#ifndef _debug_h_
#define _debug_h_

#define DEBUG

/* Status: DONE */

void dmsg(std::string msg) {
#ifdef DEBUG
    std::cout << "> DEBUG: " << msg << std::endl;
#endif
}

#endif