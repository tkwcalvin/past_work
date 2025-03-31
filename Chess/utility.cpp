#include <iostream>
#include "utility.h"
using namespace std;

ostream& operator<<(ostream& os, const Position& pos)
{
    os << static_cast<char>(pos.file + 'a') << static_cast<char>(pos.rank + '1');
    return os;
}

// Implement the 6 operator overloading functions of Position, Vector and BooleanMap

bool Position::operator==(const Position& pos) const{
    return (file==pos.file && rank==pos.rank);
}


Position Vector::operator+(const Position& pos) const{
    if ((pos.file+file>=0 && pos.file+file<=7)&&(pos.rank+rank>=0 && pos.rank+rank<=7)){
        Position ps{static_cast<_FILE>(pos.file+file),static_cast<_RANK>(pos.rank+rank)};
        return ps;
    }
    return pos;
}


Vector Vector::operator*(int mul) const{
    int f=file*mul;
    int r=rank*mul;
    Vector v{f,r};
    return v;
}


BooleanMap& BooleanMap::operator|=(const BooleanMap& other){
    for (int i=0; i<NUM_RANKS; i++){
        for (int j=0; j<NUM_FILES; j++){
            cells[i][j]=cells[i][j] || other.cells[i][j];
        }
    }
    return *this;
}

BooleanMap& BooleanMap::operator&=(const BooleanMap& other){
    for (int i=0; i<NUM_RANKS; i++){
        for (int j=0; j<NUM_FILES; j++){
            cells[i][j]=cells[i][j] && other.cells[i][j];
        }
    }
    return *this;
}


BooleanMap BooleanMap::operator~() const{
    BooleanMap nm;
    for (int i=0; i<NUM_RANKS; i++){
        for (int j=0; j<NUM_FILES; j++){
            nm.cells[i][j]=!cells[i][j];
        }
    }
    return nm;
}


