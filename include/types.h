#ifndef _types_h_
#define _types_h_

// abreviate
#define uint unsigned int

/* Status: DONE */
namespace Type {

    /* field types, used on draw functions */
    enum Field {
        boat = '#',
        hidden = '-',
        discovered = '+'
    };

    /* Position system */
    typedef struct Pos_s {
        uint line;  // holds line
        uint col;   // holds column

        /* default constructor, permit syntax like: Pos(10, 20) */
        Pos_s(uint line, uint col) : line(line), col(col) {/* */};
    } Pos;
}

#endif
