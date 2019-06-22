#ifndef _boat_h_
#define _boat_h_

#include <list>                 // std::list
#include <initializer_list>     // std::initializer_list
#include <algorithm>            // std::find
#include "types.h"              // Type::*

/* Boat class, will represent each boat and it's statuses */
/* Status: DONE */
class Boat {
    private:
        std::list<Type::Pos> boatBody;            // holds all boat body parts
        size_t isFound = 0;                       // is marked when found
        bool isNil = false;

    public:
        /* Constructors */
        Boat(std::list<Type::Pos> body);
        Boat(std::initializer_list<Type::Pos> positions);
        Boat(){ isNil = true; };    // special null constructor

        /* Tell's if the boat is a NULL Boat */
        bool isNull() { return isNil; }

        /* Get all the boat body */
        std::list<Type::Pos> getBody(); 

        /* Tell's if a (x,y) is a part of the body of that boat */
        bool isBody(Type::Pos guess);

        /* Get boat size */
        size_t getSize();

        /* If isBody find all bodypart, is automatically marked found */
        bool hasBeenFound();
};

#endif
