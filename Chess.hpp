#ifndef CHESS_H
#define CHESS_H

#include <vector>

using namespace std;

class Square;

// Enum for piece types
enum class PieceType {
    King, Queen, Rook, Bishop, Knight, Pawn, None
};

// Enum for piece sides
enum class Side {
    White, Black, None
};

// Base class for a chess piece
class Piece {
public:
    Piece(Side side, PieceType type) : side(side), type(type) {}
    virtual ~Piece() {} // Virtual destructor for proper cleanup

    Side getSide() const { return side; }
    PieceType getType() const { return type; }

    // Pure virtual function for getting valid moves
    virtual vector<bool> getValidMoves(vector<Square>& board, Square currentPos) = 0;

protected:
    Side side;
    PieceType type;
};

// Square that stores piece information
class Square {
public:
    Square(int row, int col, Piece* piece, bool hasMoved, bool en_passent = false)
        : row(row), col(col), piece(piece), moved(hasMoved), en_passent(en_passent) {}

    virtual ~Square() {}

    PieceType getPieceType() { return piece->getType(); }
    Side getSide() { return piece->getSide(); }
    Piece* getPiece() { return piece; }
    int getCol() { return col; }
    int getRow() { return row; }
    bool getMoved() { return moved; }
    bool getEnPassent() { return en_passent; }

    void setPiece(Piece* newPiece) {
        piece = newPiece;
    }

    void setMoved(bool hasMoved) {
        moved = hasMoved;
    }

    void setEnPassent(bool en) {
        en_passent = en;
    }

protected:
    int row;
    int col;
    Piece* piece;
    bool moved;
    bool en_passent;
};

// Derived classes for each piece type
class King : public Piece {
public:
    King(Side side) : Piece(side, PieceType::King) {}
    vector<bool> getValidMoves(vector<Square>& board, Square selected) override;
};

class Queen : public Piece {
public:
    Queen(Side side) : Piece(side, PieceType::Queen) {}
    vector<bool> getValidMoves(vector<Square>& board, Square selected) override;
};

class Rook : public Piece {
public:
    Rook(Side side) : Piece(side, PieceType::Rook) {}
    vector<bool> getValidMoves(vector<Square>& board, Square selected) override;
};

class Bishop : public Piece {
public:
    Bishop(Side side) : Piece(side, PieceType::Bishop) {}
    vector<bool> getValidMoves(vector<Square>& board, Square selected) override;
};

class Knight : public Piece {
public:
    Knight(Side side) : Piece(side, PieceType::Knight) {}
    vector<bool> getValidMoves(vector<Square>& board, Square selected) override;
};

class Pawn : public Piece {
public:
    Pawn(Side side) : Piece(side, PieceType::Pawn) {}
    vector<bool> getValidMoves(vector<Square>& board, Square selected) override;
};

// Class for an empty piece (empty square)
class EmptyPiece : public Piece {
public:
    EmptyPiece() : Piece(Side::None, PieceType::None) {}
    vector<bool> getValidMoves(vector<Square>& squares, Square selected) override;
};

#endif // CHESS_H
