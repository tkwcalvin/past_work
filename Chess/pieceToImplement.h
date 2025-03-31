//  Implement the 4 Piece-derived classes here
// All classes will inherit the NamedPiece class 
// and implement the remaining pure virtual functions

template <char N, int F, int R>
class Leaper : public NamedPiece<N> {
public:
    Leaper(Color color) : NamedPiece<N>(color) {}

    virtual Leaper* clone() const override {
        Leaper* result =  new Leaper<N, F, R>(this->getColor());
        result->setPosition(this->getPosition());
        return result;
    }
    bool isValidPosition(const int& f, const int& r) const {
        return (f >= 0 && f < NUM_FILES && r >= 0 && r < NUM_RANKS);
    }
    virtual BooleanMap getMoves(const Board& board) const override {
        BooleanMap moves;
        //cout << "Leaper class "  << this->getName() << endl;
        int fileDirection = (this->isWhite() ? 1 : -1);
        //cout << "move!" << endl;
        int destFile = this->position.file + fileDirection * F;
        int destRank = this->position.rank + fileDirection * R;

        if (isValidPosition(destFile, destRank)) {
            //cout << "valid" << endl;
            const Piece* piece = board.piece(static_cast<_FILE>(destFile), static_cast<_RANK>(destRank));
            if (!piece || piece->isOpponent(*this)) { // empty or oponent
                //cout << "can go!" << endl;
                moves.cell(static_cast<_FILE>(destFile), static_cast<_RANK>(destRank)) = true;
            }
        }

        return moves;
    }
};


template <char N, int F, int R, int RANGE>
class Rider : public NamedPiece<N> {
public:
    Rider(Color color) : NamedPiece<N>(color) {}

    virtual Rider* clone() const override {
        Rider* result = new Rider<N, F, R, RANGE>(this->getColor());
        result->setPosition(this->getPosition());
        return result;
    }
    bool isValidPosition(const int& f, const int& r) const {
        return (f >= 0 && f < NUM_FILES && r >= 0 && r < NUM_RANKS);
    }
    virtual BooleanMap getMoves(const Board& board) const override {
        BooleanMap moves;
        //cout << "Rider class "  << this->getName() << endl;
        int fileDirection = (this->isWhite() ? 1 : -1);

        for (int i = 1; i <= RANGE; ++i) {
            int destFile = this->position.file + fileDirection * F * i;
            int destRank = this->position.rank + fileDirection * R * i;
            if (isValidPosition(destFile, destRank)) {
                const Piece* piece = board.piece(static_cast<_FILE>(destFile), static_cast<_RANK>(destRank));
                if (!piece || piece->isOpponent(*this)) { //empty or opponent
                    moves.cell(static_cast<_FILE>(destFile), static_cast<_RANK>(destRank)) = true;
                    if (piece) {
                        break; // Stop if there is a piece (capture or block)
                    }
                } else if(!piece || piece->isWhite() == this->isWhite()){
                    break; // Stop if the piece is the same color (block)
                }
            } else {
                break; // Stop if the destination is outside the board
            }
        }

        return moves;
    }
};


//Compound class
template <char N, typename P1, typename P2>
class Compound: public NamedPiece<N>{
    public:
        Compound(Color color): NamedPiece<N>(color){}

        virtual Piece* clone() const{
            return new Compound<N,P1,P2>(*this);
        }

        virtual BooleanMap getMoves(const Board& board) const{
            P1* t1=new P1(this->getColor());
            P2* t2=new P2(this->getColor());
            t1->setPosition(this->getPosition());
            t2->setPosition(this->getPosition());

            Board tb1=board.getTempBoard(t1, t1->getPosition());
            Board tb2=board.getTempBoard(t2, t2->getPosition());

            BooleanMap m1=t1->getMoves(tb1);
            BooleanMap m2=t2->getMoves(tb2);
            return m1 |= m2;
        }

};


// Divergent class
template <char N, typename M, typename C>
class Divergent: public NamedPiece<N>{
    public:
        Divergent(Color color): NamedPiece<N>(color){}

        virtual Piece* clone() const{
            return new Divergent<N,M,C>(*this);
        }

        virtual BooleanMap getMoves(const Board& board) const{
            M* m=new M(this->getColor());
            C* c=new C(this->getColor());

            m->setPosition(this->getPosition());
            m->setPosition(this->getPosition());

            Board tm1=board.getTempBoard(m, this->getPosition());
            Board tc2=board.getTempBoard(c, this->getPosition());

            BooleanMap op=board.getOpponentMap(this->getColor());

            BooleanMap mm=m->getMoves(tm1);
            BooleanMap mc=c->getMoves(tc2);

            BooleanMap mv=mm &= (~op);
            BooleanMap cp=mc &= op;
          
            return mv |= cp;
        }
};
