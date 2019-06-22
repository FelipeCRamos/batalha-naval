#include "boat.h"

Boat::Boat(std::list<Type::Pos> body) {
    for(auto &it : body) {
        this->boatBody.push_back(it);
    }
}

Boat::Boat(std::initializer_list<Type::Pos> positions) {
    for(auto &it : positions) {
        this->boatBody.push_back(it);
    }
}

std::list<Type::Pos> Boat::getBody() {
    return this->boatBody;
}

bool Boat::isBody(Type::Pos guess) {
    for( auto &body : this->boatBody ) {
        if( body.line == guess.line && body.col == guess.col ) {
            isFound++;  // one more body discovered
            return true;
        }
    }
    return false;
}

size_t Boat::getSize() {
    return this->boatBody.size();
}

bool Boat::hasBeenFound() {
    if (this->boatBody.size() == isFound) {
        return true;
    } else {
        return false;
    }
}
