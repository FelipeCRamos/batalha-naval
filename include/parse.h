#ifndef _parse_h_
#define _parse_h_

void parsePosition(std::string st, int &l, int &c) {
    if(st.size() < 2) {
        return;
    }

    char comp;
    if(st[0] >= 'A' && st[0] <= 'Z') {
        comp = 'A';
    } else {
        comp = 'a';
    }

    char result = st[0] - comp;

    l = result;

    // treat number
    if(st[1] >= '1' && st[1] <= '9') {
        c = int(st[1] - '0') - 1;
    } else if (st[1] == '0') {
        c = 10 - 1;
    }
}

bool isChar(char c) {
    if((c >= 'a' && c <= 'j') || (c >= 'A' &&  c <= 'J')) {
        return true;
    } else {
        return false;
    }
}

bool isValidNum(char c) {
    if(c >= '0' && c <= '9') {
        return true;
    } else {
        return false;
    }
}

std::string getPlay() {
    bool hasWrong = false;
    bool isCorrect = false;
    std::string play;
    do {
        if(hasWrong) {
            std::cout << "Please, insert in the format: [A-J][0-9], ex: a9\n";
        }
        std::cout << "Insert play: ";
        std::cin >> play;
        if(play.size() == 2 && isChar(play[0]) && isValidNum(play[1]))
            isCorrect = true;
        else 
            hasWrong = true;
    } while (!isCorrect);

    return play;
}

#endif
