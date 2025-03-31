#include "userPiece.h"

// isRoyal(const Piece*)


bool isRoyal(const Piece* piece){
    return dynamic_cast<const King*>(piece) !=nullptr;
}





//  Implement any Pawn function(s) here if needed
Pawn::Pawn(Color color) : Divergent<'P', Leaper<'P',0,1>, Compound<'P', Rider<'p', 1, 1, 1>, Rider<'p', -1, 1, 1>>>(color) {}

BooleanMap Pawn::getMoves(const Board& board) const {
    BooleanMap moves;

    const Position currentPos = this->getPosition();
    int file = currentPos.file;
    int rank = currentPos.rank;
    if (this->isWhite()) {
        if(rank == 1){
            if(!board.piece(static_cast<_FILE>(file), static_cast<_RANK>(rank+1))){ //empty
                moves.cell(static_cast<_FILE>(file), static_cast<_RANK>(rank+1)) = true;
                if(!board.piece(static_cast<_FILE>(file), static_cast<_RANK>(rank+2))){
                    moves.cell(static_cast<_FILE>(file), static_cast<_RANK>(rank+2)) = true;
                }
            }
            
        }
        else{
            if(!board.piece(static_cast<_FILE>(file), static_cast<_RANK>(rank+1))){ //empty
                moves.cell(static_cast<_FILE>(file), static_cast<_RANK>(rank+1)) = true;
            }
        }

    } else {
        if(rank == 6){
            if(!board.piece(static_cast<_FILE>(file), static_cast<_RANK>(rank-1))){
                moves.cell(static_cast<_FILE>(file), static_cast<_RANK>(rank-1)) = true;
                if(!board.piece(static_cast<_FILE>(file), static_cast<_RANK>(rank-2))){
                    moves.cell(static_cast<_FILE>(file), static_cast<_RANK>(rank-2)) = true;
                }
            }
        }
        else{
            if(!board.piece(static_cast<_FILE>(file), static_cast<_RANK>(rank-1))){ //empty
                moves.cell(static_cast<_FILE>(file), static_cast<_RANK>(rank-1)) = true;
            }                
        }
    }

    if (this->isWhite()) {
        const Piece* upRight = board.piece(static_cast<_FILE>(file+1), static_cast<_RANK>(rank+1));
        const Piece* upLeft = board.piece(static_cast<_FILE>(file-1), static_cast<_RANK>(rank+1));
        
        if(upRight){
            if(upRight->isWhite() == this->isWhite()){}
            else{
                moves.cell(upRight->getPosition()) = true;
            }
        }

        if(upLeft){
            if(upLeft->isWhite() == this->isWhite()){}
            else{
                moves.cell(upLeft->getPosition()) = true;
            }
        }
    }
    else{
        const Piece* upRight = board.piece(static_cast<_FILE>(file-1), static_cast<_RANK>(rank-1));
        const Piece* upLeft = board.piece(static_cast<_FILE>(file+1), static_cast<_RANK>(rank-1));

        if(upRight){
            if(upRight->isWhite() == this->isWhite()){}
            else{
                moves.cell(upRight->getPosition()) = true;
            }
        }

        if(upLeft){
            if(upLeft->isWhite() == this->isWhite()){}
            else{
                moves.cell(upLeft->getPosition()) = true;
            }
        }

    }

    return moves;

}