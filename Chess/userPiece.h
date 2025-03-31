#ifndef __USERPIECE_H__
#define __USERPIECE_H__

#include "piece.h"

bool isRoyal(const Piece* piece);


using Knight = OmniLeaper<'N', 2, 1>;

using Rook = OmniRider<'R', 1, 0>;

// Define the 3 classes Bishop, Queen, King similar to above

using Bishop = OmniRider<'B', 1, 1>;
using Queen = Compound<'Q', Rook, Bishop>;

using King = Compound<'K',OmniRider<'K', 1, 1, 1>, OmniRider<'K', 1, 0, 1>>;                                                                    


//  Define the Pawn class



class Pawn : public Divergent<'P', Leaper<'P',0,1>, Compound<'P', Rider<'p', 1, 1, 1>, Rider<'p', -1, 1, 1>>> {
public:
    Pawn(Color color);
    virtual BooleanMap getMoves(const Board& board) const override;
};


#endif // __USERPIECE_H__