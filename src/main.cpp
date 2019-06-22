#include <iostream>
#include <initializer_list>
#include "gameside.h"

int main(int argc, char **argv){
    // stub
    //
    Gameside p1 = Gameside(10);

    std::cout << p1.getPlayerField();

    p1.addOneBoat(std::list<Type::Pos> {
        Type::Pos(5,5),
        Type::Pos(5,6),
        Type::Pos(5,7)
    });

    p1.addOneBoat(std::list<Type::Pos> {
        Type::Pos(3,5),
        Type::Pos(3,6),
        Type::Pos(3,7)
    });

    /* testing hit system */
    for(int i = 0; i < 5; i++) {
        std::pair<bool, Boat> res = p1.didHit(Type::Pos(3, 5+i));
        std::cout << "Has hit? " << (res.first ? "Yes" : "No") << "\t";

        std::cout << (!res.second.isNull() ? "Discovered!" : "No discovery!") << "\n";
    }

    std::cout << "---------actual player field-------------------\n";
    std::cout << p1.getPlayerField();
    std::cout << "---------enemy field---------------------------\n";
    std::cout << p1.getEnemyField();

    return 0;
}
