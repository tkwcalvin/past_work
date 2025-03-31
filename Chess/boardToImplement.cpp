#include <iostream>
#include "board.h"
#include "piece.h"
using namespace std;


Board::Board(const Board& board)
{
    
    for (int i=0; i<NUM_RANKS; i++){
        for (int j=0; j<NUM_FILES; j++){
            if (board.pieces[i][j]!=nullptr){
                pieces[i][j]=board.pieces[i][j]->clone();
            }
            else{
                pieces[i][j]=nullptr;
            }
        }
    }
    
    isWhiteTurn=board.isWhiteTurn;

    if (board.selectedPiece!=nullptr){
        
        selectedPiece=piece(board.selectedPiece->getPosition());
        
    }

    moveMap=board.moveMap;

    for (int k=0; k<2; k++){
        if (board.royalPieces[k]!=nullptr){
            royalPieces[k]=piece(board.royalPieces[k]->getPosition());
        }
        
    }


}


Board::~Board() 
{
    for (int i=0; i<NUM_RANKS; i++){
        for (int j=0; j<NUM_FILES; j++){
            if (pieces[i][j]!=nullptr){
                delete pieces[i][j];
            }
            
        }
    }
}



void Board::move(const Position& destPos)
{
    // Safeguard against misusage of move()
    if (!selectedPiece) {
        cout << "ERROR: Piece not selected, cannot call move()" << endl;
        return;
    }

    if (piece(destPos)!=nullptr){
        delete piece(destPos);
    }
    Position current=selectedPiece->getPosition();
    pieces[current.rank][current.file]=nullptr;
    pieces[destPos.rank][destPos.file]=selectedPiece;
    moveMap=selectedPiece->getMoves(*this);
    isWhiteTurn= !isWhiteTurn;
   
}


BooleanMap Board::getAttackingMap() const
{
  
    BooleanMap nm;
    for (int i=0; i<NUM_RANKS; i++){
        for (int j=0; j<NUM_FILES; j++){
            if (pieces[i][j]!=nullptr){
                if (isWhiteTurn==pieces[i][j]->isWhite()){
                    nm |= getOpponentMap(isWhiteTurn) &= pieces[i][j]->getMoves(*this);
                }  
            } 
        }
    }
    return nm;
}


void Board::validateMoveMap()
{
  
    Position rp;
    if (isWhiteTurn){
        rp=royalPieces[0]->getPosition();
    }
    else{
        rp=royalPieces[1]->getPosition();
    }
    for (int i=0; i<NUM_RANKS; i++){
        for (int j=0; j<NUM_FILES; j++){
            Board tb(*this);
            Position ps{static_cast<_FILE>(j),static_cast<_RANK>(i)};
            tb.move(ps);
            BooleanMap am=tb.getAttackingMap();
            
            if (am.cell(rp)==true){
                moveMap.cell(ps)=false;
            }
            else{
                moveMap.cell(ps)=true;
            }
            
        }
    }
}
