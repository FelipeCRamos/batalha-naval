#ifndef _player_h_
#define _player_h_

#include <list>     // std::list
#include <fstream>  // std::ifstream
#include <iostream> // std::cout
#include <sstream>  // std::stringstream
#include <random>
#include <chrono>
#include <algorithm>

#include "boat.h"   // class Boat
#include "types.h"  // class Types

/* Status: IN PROGRESS */
class Player {
    private:
        uint m_dim;                 // dimentions for the board

        /* Fields 2d-arrays */
        Type::Field **field;
        Type::Field **enemyField;

        size_t num_of_boats = 0;

        size_t myScore = 0;
        size_t enemyScore = 0;

        std::list<Boat> m_boats;    // list of active boats

        /* Get a field representation with a string obj */
        std::string getField(Type::Field **canvas);
        std::list<Type::Pos> history;
        std::list<Type::Pos> enemyHistory;

    public:
        /* Basic constructor */
        Player();

        /* Basic destructor */
        ~Player();

        /* Add boats to the game */
        bool addBoat(std::list<Type::Pos> boatBody);
        bool addRandomBoat(size_t boat_size);

        Type::Pos GetRandomPlay()

        /* Asks if a determined position has been hit */
        std::pair<bool, Boat> didHit(Type::Pos guess);
        
        /* Store that the player was hit */
        void storePosition(Type::Pos hitEnemyPos, Boat boat);
        void storeError(Type::Pos hitPos);

        /* Return the representation of the player & enemy field's */
        std::string getPlayerField();
        std::string getEnemyField();

        /* Get player and enemy score */
        size_t getScore();
        size_t getEnemyScore();

        bool isGameOver();
        bool otherPlayerWin();

        std::string showStats();
};

#endif
