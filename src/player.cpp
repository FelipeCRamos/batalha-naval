#include "player.h"

#define COLORS true
#define CANVAS_SIZE 10

Player::Player()
/*{{{*/
{
    // initialize dimension 
    this->m_dim = CANVAS_SIZE;

    // initialize matrix
    field = new Type::Field*[CANVAS_SIZE];
    enemyField = new Type::Field*[CANVAS_SIZE];

    for(int i = 0; i < CANVAS_SIZE; i++) {
        field[i] = new Type::Field[CANVAS_SIZE];
        enemyField[i] = new Type::Field[CANVAS_SIZE];
    }

    for(int line = 0; line < CANVAS_SIZE; line++) {
        for(int col = 0; col < CANVAS_SIZE; col++) {
            field[line][col] = Type::Field::hidden;
            enemyField[line][col] = Type::Field::hidden;
        }
    }
}
/*}}}*/

Player::~Player()
/*{{{*/
{
    for(int line = 0; line < this->m_dim; line++) {
        delete[] this->field[line];
        delete[] this->enemyField[line];
    }

    delete[] this->enemyField;
    delete[] this->field;
}
/*}}}*/

std::string Player::getEnemyField()
/*{{{*/
{
    return getField(this->enemyField);
}
/*}}}*/

std::string Player::getPlayerField()
/*{{{*/
{
    return getField(this->field);
}
/*}}}*/

std::string Player::getField(Type::Field **canvas)
/*{{{*/
{
    std::stringstream buf("");

    for(int line = 0; line < this->m_dim; line++) {

        // print line number
        buf << char('A' + line) << " ";

        for(int col = 0; col < this->m_dim; col++) {
            switch(canvas[line][col]) {
                case Type::Field::boat:
                    if(COLORS)
                        buf << "\e[32m#\e[0m "; // green #
                    else
                        buf << "# ";
                    break;

                case Type::Field::boat_disc:
                    if(COLORS)
                        buf << "\e[31m%\e[0m "; // green #
                    else
                        buf << "% ";
                    break;

                case Type::Field::hidden:
                    if(COLORS)
                        buf << "\e[2m-\e[0m ";  // hidden -
                    else
                        buf << "- ";
                    break;

                case Type::Field::discovered:
                    if(COLORS)
                        buf << "\e[1m+\e[0m ";  // bold +
                    else
                        buf << "+ ";
                    break;

                case Type::Field::wrong:
                    if(COLORS)
                        buf << "\e[31mx\e[0m "; // red x
                    else
                        buf << "x ";
                    break;

                default:
                    if(COLORS)
                        buf << "\e[31mER\e[0m"; // red ER
                    else
                        buf << "ER";
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
/*}}}*/

bool Player::addBoat(std::list<Type::Pos> boatBody)
/*{{{*/
{
    // first, check if does not overlap any exiting boat:
    for( auto &bodypart : boatBody ) {
        for( auto &otherBoat : this->m_boats ) {
            if(otherBoat.isBody(bodypart)) {
                return false;
            }
        }
    }

    // then, add-it
    this->m_boats.push_back(Boat(boatBody));

    // add to the field matrix
    for( auto &bodypart : boatBody ) {
        this->num_of_boats += 1;
        field[bodypart.line][bodypart.col] = Type::Field::boat;
    }
    return true;
}
/*}}}*/

Type::Pos Player::getRandomPlay()
/*{{{*/
{
    int line, col;    
    bool found = false;

    do {
        unsigned x = std::chrono::system_clock::now().time_since_epoch().count();

        unsigned y = std::chrono::system_clock::now().time_since_epoch().count();

        std::mt19937 l(x);
        std::mt19937 c(l());
        std::mt19937 z(c() + l());

        int r1 = z() % 10, r2 = l() % 10;

        if((r1 > 0) && (r2 > 0)) {
            // check (l,c) if wasn't played already
            auto pos = Type::Pos(r1 % 10, r2 % 10);
            if(std::find(history.begin(), history.end(), pos) == history.end()) {
                found = true;
                return pos;
            }
        }
    } while(!found);
}
/*}}}*/

bool Player::addRandomBoat(size_t boat_size) 
/*{{{*/
{
    boat_size -= 1;
    int bcol, ecol, blin, elin;
    bool found = false;
    int counter = 0;
    while(!found) {
        unsigned x = std::chrono::system_clock::now().time_since_epoch().count();
        unsigned y = std::chrono::system_clock::now().time_since_epoch().count();

        std::mt19937 x1(x);
        std::mt19937 y1(y);
        
        int line = x1() % 10, col = y1() % 10;

        bool horizontal = line % 2;
        
        if( line < 10 && line > 0 ) {
            if( col < 10 && col > 0 ) {
                if(horizontal) {
                    // check horizontal line
                    // for right
                    if(col + boat_size < 10) {
                        bcol = col;
                        blin = line;
                        ecol = col + boat_size;
                        elin = line;
                        found = true;
                    // for left
                    } else if(col - boat_size >= 0) {
                        bcol = col;
                        blin = line;
                        ecol = col - boat_size;
                        elin = line;
                        found = true;
                    }
                } else {
                    // check vertical line
                    if( line + boat_size < 10 ) {
                        // for down
                        bcol = col;
                        blin = line;
                        ecol = col;
                        elin = line + boat_size;
                        found = true;
                    } else if(line - boat_size < 10) {
                        // for up
                        bcol = col;
                        blin = line;
                        ecol = col;
                        elin = line - boat_size;
                        found = true;
                    }
                }
            }
        }
    }

    // printf(" (%c, %i) -> (%c, %i)\n", 'a' + blin, bcol, 'a' + elin, ecol);

    int iline = std::min(blin, elin), fline = std::max(blin, elin);
    int icol = std::min(bcol, ecol), fcol = std::max(bcol, ecol);

    // printf(" (%c, %i) -> (%c, %i)\n", 'a' + iline, icol, 'a' + fline, fcol);

    std::list<Type::Pos> buf;
    for(int line = iline; line <= fline; line++) {
        for(int col = icol; col <= fcol; col++) {
            buf.push_back(Type::Pos(line, col));
        }
    }

    return addBoat(buf);
}
/*}}}*/

std::pair<bool, Boat> Player::didHit(Type::Pos guess)
/*{{{*/
{

    // Checa se uma posição (guess) atingiu o meu navio
    for(auto &boat : this->m_boats) {
        if(boat.isBody(guess)) {
            this->field[guess.line][guess.col] = Type::Field::boat_disc;

            if( std::find(enemyHistory.begin(), enemyHistory.end(), guess) == enemyHistory.end() ) {
                this->enemyScore += 1;
                enemyHistory.push_back(guess);
            }

            if(boat.hasBeenFound()) {
                // retorna que atingiu, e todo o navio afundado
                return std::make_pair(true, boat.getBody());    // return all
            } else {
                // retorna que sim, sem nenhum navio afundado
                return std::make_pair(true, Boat());    // return with null boat
            }
            break;
        }
    }

    this->field[guess.line][guess.col] = Type::Field::discovered;
    return std::make_pair(false, Boat());   // return with nothing
}
/*}}}*/

void Player::storePosition(Type::Pos hitEnemyPos, Boat boat)
/*{{{*/
{
    auto res = std::find(history.begin(), history.end(), hitEnemyPos);

    // only give the point if he did not already matched that position
    if(res == history.end()) {
        this->myScore += 1;
        history.push_back(hitEnemyPos);
    }

    if(!boat.isNull()) {
        for(auto &pos : boat.getBody()) {
            this->enemyField[pos.line][pos.col] = Type::Field::boat;
        }
    } else {
        if((hitEnemyPos.line >= 0 && hitEnemyPos.line < this->m_dim) && 
            (hitEnemyPos.col >= 0 && hitEnemyPos.col < this->m_dim)) {
            this->enemyField[hitEnemyPos.line][hitEnemyPos.col] = Type::Field::discovered;
        } else {
            printf("ERROR: Position (%i, %i) invalid!", hitEnemyPos.line, hitEnemyPos.col);
        }
    }
}
/*}}}*/

void Player::storeError(Type::Pos hitPos)
/*{{{*/
{
    if((hitPos.line >= 0 && hitPos.line < this->m_dim) && 
            (hitPos.col >= 0 && hitPos.col < this->m_dim)) {
        this->enemyField[hitPos.line][hitPos.col] = Type::Field::wrong;
    } else {
        printf("ERROR: Position (%i, %i) invalid!", hitPos.line, hitPos.col);
    }
}
/*}}}*/

size_t Player::getScore() 
/*{{{*/
{
    return this->myScore;
}
/*}}}*/

size_t Player::getEnemyScore() 
/*{{{*/
{
    return this->enemyScore;
}
/*}}}*/

bool Player::isGameOver()
/*{{{*/
{
    if(this->enemyScore >= num_of_boats || this->myScore >= num_of_boats) {
        return true;
    } else {
        return false;
    }
}
/*}}}*/

bool Player::otherPlayerWin()
/*{{{*/
{
    return this->enemyScore > this->myScore ? true : false;
}
/*}}}*/

std::string Player::showStats()
/*{{{*/
{
    std::stringstream buf;

    buf << "YOU " << (otherPlayerWin() ? "\e[1;31mLOST!" : "\e[1;32mWON") << "\e[0m\n";
    buf << "Your score: " << getScore() << std::endl;
    buf << "Enemy score: " << getEnemyScore() << std::endl;

    return buf.str();
}
/*}}}*/
