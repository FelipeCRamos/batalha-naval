#ifndef _gameside_h_
#define _gameside_h_

#include <list>     // std::list
#include <fstream>  // std::ifstream
#include <iostream> // std::cout
#include <sstream>  // std::stringstream

#include "boat.h"
#include "types.h"

#define CANVAS_SIZE 16

/* Status: IN PROGRESS */
class Gameside {
    private:
        
        uint m_dim;                 // dimentions for the board
        Type::Field **field;
        Type::Field **enemyField;
        std::list<Boat> m_boats;    // list of active boats

        /* Get a field representation with a string obj */
        std::string getField(Type::Field **canvas);

    public:
        /* Basic constructor */
        Gameside(uint dimension);
        ~Gameside();

        /* Add boats to the game */
        bool readFromFile(std::ifstream &file);          // by a file
        bool addOneBoat(std::list<Type::Pos> boatBody);   // add only one boat
        bool random();                              // add randomly

        /* Asks if a determined position has been hit */
        std::pair<Boat, Type::Pos> didRandomHit();
        std::pair<bool, Boat> didHit(Type::Pos guess);
        
        /* Store that the player was hit */
        void storePosition(Type::Pos hitEnemyPos, bool didHit);

        std::string getPlayerField();
        std::string getEnemyField();

};

#endif
