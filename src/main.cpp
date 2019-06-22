#include <iostream>
#include "gameside.h"

int main(int argc, char **argv){
    Gameside p1 = Gameside(10);

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

    std::string command_buf;
    while(true) {
        /* MAIN LOOP */
        system("clear");

        std::cout << "Previous command: '" << command_buf << "'.\n";
        std::cout << "-----------------------------------------------\n";
        std::cout << ">> Player Field:\n";
        std::cout << p1.getPlayerField();
        std::cout << "-----------------------------------------------\n";
        std::cout << ">> Enemy Field:\n";
        std::cout << p1.getEnemyField();
        std::cout << "-----------------------------------------------\n";

        std::cout << "> ";
        std::cin >> command_buf;
    }

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
