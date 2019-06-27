/* C++ */
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <list>
#include <initializer_list>
#include <utility>
#include <thread>

/* Socket related */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Project */
#include "player.h"
#include "parse.h"
#include "types.h"
#include "boat.h"
#include "arg_parser.h"

/* Definitions */
#define S_SIZE 1024
#define BACKLOG 5   // How many pending connections queue will hold
#define BOT_TIME 200 // ms

/*  Socket type for the connection */
enum SocketType {
    CLIENT,
    SERVER
};

int main(int argc, char **argv){
    // store and get arguments
    Args args = Args(argc, argv);


    SocketType socketMode;                  //!< Will hold which the program is
    // Args paramethers
    bool isServer = args.checkArg("-s");    // -s will trigger server mode
    bool isClient = args.checkArg("-c");    // -c will trigger client mode
    bool isAutomatic = args.checkArg("-a"); // -a will trigger auto mode

    std::cout << "Is automatic? " << isAutomatic << std::endl;

    if( isServer ^ isClient ) {
        if(isServer) socketMode = SERVER;
        if(isClient) socketMode = CLIENT;
    } else {
        std::cerr << "You need to specify at least if the program will act as"
            << " a server or a client (with -s or -c args).\n";
        return 1;
    }

    std::string server_ip = args.getArg("--server-ip").empty() ? "127.0.0.1" : args.getArg("--server-ip");

    size_t server_port;
    try {
        server_port = args.getArg("-p").empty() ? 8787 : std::stoi(args.getArg("-p"));
    } catch (...) {
        server_port = 8787;
    }

    // Control variables
    bool gameRunning = true;

    /* ---------------------------------------------------------------------- */
    /* -- PLAYER THINGS ----------------------------------------------------- */
    /* ---------------------------------------------------------------------- */
    Player player = Player();

    /* AUTO ADD RANDOM BOATS TO THE PLAYER */
    typedef std::chrono::high_resolution_clock myclock;
    for( int i = 1; i <= 5; i++ ) {
        int qntd = 0;

        switch(i) {
            case 2: qntd = 4; break;
            case 3: qntd = 3; break;
            case 4: qntd = 2; break;
            case 5: qntd = 1; break;
            default: qntd = 0; break;
        }

        for(int j = 0; j < qntd; j++) {
            bool result = false;
            do {
                // will try to add a boat (will fail if the boat is on top of other)
                // until it makes
                result = player.addRandomBoat(i);
            } while(!result);
        }
    }

    if( socketMode == CLIENT ) {
        /* ------------------------------------------------------------------ */
        /* -- CLIENT MODE --------------------------------------------------- */
        /* ------------------------------------------------------------------ */
        char serverResponse[S_SIZE];

        // creates the socket
        int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        
        char hostname[S_SIZE], ipaddress[S_SIZE];   //!< Placeholder for host & ip
        struct hostent *hostIP;                     //!< Placeholder for the IP Addr

        // If the gethostname returns a name then the program will get the IP
        if( gethostname(hostname, sizeof(hostname)) == 0 ) {
            hostIP = gethostbyname(hostname);
        } else {
            std::cerr << "ERROR: FCC4539 IP Address not found!" << std::endl;
        }

        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(server_port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if(inet_pton(AF_INET, server_ip.c_str(), &serverAddress.sin_addr) <= 0) {
            std::cerr << "Invalid IP Address!\n";
        }
        
        connect(socketDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

        // display some info about connection (debug)
        std::cout << "Connected with:\n";
        printf("Local Host: %s\n", inet_ntoa(*(struct in_addr*)hostIP->h_addr));
        printf("Remote Host: %s:", inet_ntoa(serverAddress.sin_addr));
        printf("%d\n", server_port);

        std::string sendBack = "";
        bool isMyTurn = false;

        while(gameRunning && !player.isGameOver()) {
            system("clear");

            std::cout << "Previous response: " << serverResponse << std::endl;
            std::cout << "---------------------------------\n";
            std::cout << "\e[1m" << "My score: " << player.getScore() << "\e[0m" << std::endl;
            std::cout << "\e[32m" << "Player field:" << "\e[0m" << std::endl;
            std::cout << "---------------------------------\n";
            std::cout << player.getPlayerField() << std::endl;
            std::cout << "---------------------------------\n\n";

            std::cout << "\e[1m" << "Enemy score: " << player.getEnemyScore() << "\e[0m" << std::endl;
            std::cout << "\e[31m" << "Enemy field:" << "\e[0m" << std::endl;
            std::cout << "---------------------------------\n";
            std::cout << player.getEnemyField() << std::endl;
            std::cout << "---------------------------------\n";

            // recieve the data from server 
            recv(socketDescriptor, serverResponse, sizeof(serverResponse), 0);
            // std::cout << "Other player sent: " << serverResponse << std::endl;
            // std::cout << "---------------------------------\n";

            std::stringstream atkResponse;
            if( strlen(serverResponse) == 2 ) {
                // it's an attack!                 
                // std::cout << "IDENTIFIED AN ATTACK!\n";
                atkResponse << serverResponse;

                int line, col;
                parsePosition(serverResponse, line, col);
                Type::Pos pos = Type::Pos(line, col);

                std::pair<bool, Boat> resp = player.didHit(pos);

                if(!resp.first) {   // attack didn't hit my boat!
                    atkResponse << "x";
                    isMyTurn = true;
                } else if (resp.second.isNull()) {  // attack hit, but didn't sunk
                    atkResponse << "#";
                    isMyTurn = false;
                } else { // attack sunk the boat!
                    char atkAns;
                    switch(resp.second.getBody().size()) {
                        case 2: atkAns = 'S'; break;
                        case 3: atkAns = 'D'; break;
                        case 4: atkAns = 'C'; break;
                        case 5: atkAns = 'P'; break;
                    }
                    atkResponse << atkAns;
                    isMyTurn = false;
                }

            } else if(strlen(serverResponse) == 3) {
                // it's the response of an attack!

                int line, col;
                parsePosition(serverResponse, line, col);
                Type::Pos pos = Type::Pos(line, col);

                switch(serverResponse[2]) {
                    case 'x':
                        isMyTurn = false;
                        player.storeError(pos);
                        break;
                    case '#':
                    case 'P':
                    case 'C':
                    case 'D':
                    case 'S':
                        isMyTurn = true;
                        player.storePosition(pos, Boat());
                        break;
                }

                sendBack = "?";
            }


            std::string counterResponse;
            if(sendBack.empty() && isMyTurn && atkResponse.str().empty()) {
                if(isAutomatic) {
                    auto pos = player.getRandomPlay();

                    std::stringstream ss;
                    ss << char('a' + pos.line) << pos.col;
                    counterResponse = ss.str();
                    std::this_thread::sleep_for(std::chrono::milliseconds(BOT_TIME));
                } else {
                    counterResponse = getPlay();
                }
            } else {
                if(!sendBack.empty()) {
                    counterResponse = sendBack;
                    sendBack = "";
                } else {
                    counterResponse = atkResponse.str();
                    atkResponse.str() = "";
                }
            }

            send(socketDescriptor, counterResponse.c_str(), sizeof(counterResponse), 0);
        }

        system("clear");
        std::cout << player.showStats();

        close(socketDescriptor);
        return 0;

    } else if ( socketMode == SERVER ) {
        /* ------------------------------------------------------------------ */
        /* -- SERVER MODE --------------------------------------------------- */
        /* ------------------------------------------------------------------ */

        char serverMessage[S_SIZE];
        char clientMessage[S_SIZE];

        // Creates the socket
        int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in serverAddress;

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(server_port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        bind(socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

        std::cout << "Waiting for a connection...\n";
        listen(socketDescriptor, BACKLOG);
        int clientSocketDescriptor = accept(socketDescriptor, NULL, NULL);

        // control variables
        std::string attackResponse = "";
        bool isMyTurn = true;

        system("clear");

        while(gameRunning && !player.isGameOver()) {

            std::cout << "\e[1m" << "My score: " << player.getScore() << "\e[0m" << std::endl;
            std::cout << "\e[32m" << "Player field:" << "\e[0m" << std::endl;
            std::cout << "---------------------------------\n";
            std::cout << player.getPlayerField() << std::endl;
            std::cout << "---------------------------------\n\n";

            std::cout << "\e[1m" << "Enemy score: " << player.getEnemyScore() << "\e[0m" << std::endl;
            std::cout << "\e[31m" << "Enemy field:" << "\e[0m" << std::endl;
            std::cout << "---------------------------------\n";
            std::cout << player.getEnemyField() << std::endl;
            std::cout << "---------------------------------\n";

            std::string msg;

            if(attackResponse.empty() && isMyTurn) {
                if(isAutomatic) {
                    auto pos = player.getRandomPlay();

                    std::stringstream ss;
                    ss << char('a' + pos.line) << pos.col;
                    msg = ss.str();
                    std::this_thread::sleep_for(std::chrono::milliseconds(BOT_TIME));
                } else {
                    msg = getPlay();
                }
                isMyTurn = false;
            } else {
                msg = attackResponse;
                attackResponse = "";
            }

            send(clientSocketDescriptor, msg.c_str(), sizeof(msg.c_str()), 0);
            recv(clientSocketDescriptor, &clientMessage, sizeof(clientMessage), 0);

            system("clear");

            std::cout << "Previous response: " << clientMessage << std::endl;
            std::cout << "---------------------------------\n";

            // interpret client message
            if(strlen(clientMessage) == 3) {
                // it's the response of an attack
                attackResponse = "";

                int line, col;
                parsePosition(clientMessage, line, col);
                Type::Pos pos = Type::Pos(line, col);

                switch(clientMessage[2]) {
                    case 'x':
                        isMyTurn = false;
                        player.storeError(pos);
                        break;
                    case '#':
                    case 'P':
                    case 'C':
                    case 'D':
                    case 'S':
                        isMyTurn = true;
                        player.storePosition(pos, Boat());
                        break;
                }

            } else if(strlen(clientMessage) == 2) {
                // it's an attack
                std::stringstream atkResponse;
                atkResponse << clientMessage;

                int line, col;
                parsePosition(clientMessage, line, col);
                Type::Pos atkPos = Type::Pos(line, col);

                std::pair<bool, Boat> atkResp = player.didHit(atkPos);

                if(!atkResp.first) { // attack didn't hit my boat!
                    atkResponse << "x";
                } else if (atkResp.second.isNull()) {   // attack hit, but not sunk
                    atkResponse << "#";
                } else {    // attack sunk my boat
                    char atkAns;
                    switch(atkResp.second.getBody().size()) {
                        case 2: atkAns = 'S'; break;
                        case 3: atkAns = 'D'; break;
                        case 4: atkAns = 'C'; break;
                        case 5: atkAns = 'P'; break;
                    }
                    atkResponse << atkAns;
                }

                // atkResponse << clientMessage << "x";
                attackResponse = atkResponse.str();
                std::cout << "attackResponse[2] = '" << attackResponse[2] << "'\n";
                if(attackResponse[2] == 'x') isMyTurn = true; else isMyTurn = false;
            }
        }

        // Exibit player stats, who won.. etc
        system("clear");
        std::cout << player.showStats();

        // Close connection
        close(socketDescriptor);
        return 0;
    }
}
