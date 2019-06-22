#include "gameside.h"
#include "debug.h"

Gameside::Gameside(uint dimension) {
    // initialize dimension 
    this->m_dim = dimension;

    // initialize matrix
    field = new Type::Field*[dimension];
    enemyField = new Type::Field*[dimension];

    for(int i = 0; i < dimension; i++) {
        field[i] = new Type::Field[dimension];
        enemyField[i] = new Type::Field[dimension];
    }

    for(int line = 0; line < dimension; line++) {
        for(int col = 0; col < dimension; col++) {
            field[line][col] = Type::Field::hidden;
            enemyField[line][col] = Type::Field::hidden;
        }
    }
    dmsg("Allocated gameside");
}

Gameside::~Gameside() {
    for(int line = 0; line < this->m_dim; line++) {
        delete[] this->field[line];
    }

    delete[] this->field;
    dmsg("Deallocated gameside");
}

std::string Gameside::getEnemyField() {
    return getField(this->enemyField);
}

std::string Gameside::getPlayerField() {
    return getField(this->field);
}

std::string Gameside::getField(Type::Field **canvas) {
    std::stringstream buf("");

    for(int line = 0; line < this->m_dim; line++) {

        // print line number
        buf << char('A' + line) << " ";

        for(int col = 0; col < this->m_dim; col++) {
            switch(canvas[line][col]) {
                case Type::Field::boat:
                    buf << "\e[32m#\e[0m "; // green #
                    break;

                case Type::Field::hidden:
                    buf << "\e[2m-\e[0m ";  // hidden -
                    break;

                case Type::Field::discovered:
                    buf << "\e[1m+\e[0m ";  // bold +
                    break;

                default:
                    buf << "\e[31mER\e[0m"; // red ER
            }
        }
        buf << "\n";
    }

    buf << "  ";

    for( int col = 0; col < this->m_dim; col++ ) {
        buf << col + 1 << (col < 9 ? " " : "");
    }

    buf << "\n";

    return buf.str();
}

bool Gameside::addOneBoat(std::list<Type::Pos> boatBody) {
    this->m_boats.push_back(Boat(boatBody));

    // add to the field matrix
    for( auto &bodypart : boatBody ) {
        field[bodypart.line][bodypart.col] = Type::Field::boat;
    }
}

std::pair<bool, Boat> Gameside::didHit(Type::Pos guess) {
    for(auto &boat : this->m_boats) {
        if(boat.isBody(guess)) {
            this->enemyField[guess.line][guess.col] = Type::Field::boat;

            if(boat.hasBeenFound())
                return std::make_pair(true, boat.getBody());    // return all

            else
                return std::make_pair(true, Boat());    // return with null boat

            break;
        }
    }

    this->enemyField[guess.line][guess.col] = Type::Field::discovered;
    return std::make_pair(false, Boat());   // return with nothing
}

