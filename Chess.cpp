#include "Chess.hpp"

#include <iostream>

void returnAllFalse(vector<Square>& squares, vector<bool>& result);
void returnAllTrue(vector<Square>& squares, vector<bool>& result);
bool sameSide(Square selected, Square potential_move);
bool otherSide(Square selected, Square potential_move);
bool sameCol(Square selected, Square potential_move);
bool sameRow(Square selected, Square potential_move);
bool rowBlocked(vector<Square>& squares, Square& selected, Square& potential_move);
bool diagBlocked(vector<Square>& squares, Square& selected, Square& potential_move);
bool isAttacked(vector<Square>& squares, int square, Side side);
int rowDif(Square selected, Square potential_move);
int colDif(Square selected, Square potential_move);


// sets all legal moves to 0
void returnAllFalse(vector<Square>& squares, vector<bool>& result) {
	for (int i = 0; i < squares.size(); i++) {
		result.push_back(false);
	}
}
// sets all legal moves to 1
void returnAllTrue(vector<Square>& squares, vector<bool>& result) {
	for (int i = 0; i < squares.size(); i++) {
		result.push_back(true);
	}
}

// returns true if same side
bool sameSide(Square selected, Square potential_move) {
	if (selected.getSide() == potential_move.getSide())
		return true;
	else
		return false;
}

// returns true if other side
bool otherSide(Square selected, Square potential_move) {
	if ((selected.getSide() == Side::White && potential_move.getSide() == Side::Black) || 
		(selected.getSide() == Side::Black && potential_move.getSide() == Side::White))
		return true;
	else
		return false;
}

// returns true if same col
bool sameCol(Square selected, Square potential_move) {
	if (selected.getCol() == potential_move.getCol())
		return true;
	else
		return false;
}

// returns true if same col
bool sameRow(Square selected, Square potential_move) {
	if (selected.getRow() == potential_move.getRow())
		return true;
	else
		return false;
}

// returns true if square is blocked on row
bool rowBlocked(vector<Square>& squares, Square& selected, Square& potential_move) {

	bool result = false, enemy_found = false;
	int col_dif = colDif(selected, potential_move);
	int square = selected.getRow() * 8 + selected.getCol();

	if (col_dif > 0) {

		for (int i = 0; i < abs(col_dif); i++) {
			if (sameSide(squares[square], squares[square + (i + 1)]) || enemy_found) {
				result = true;
			}
			if (otherSide(squares[square], squares[square + (i + 1)]))
				enemy_found = true;
		}
	}
	else {
		for (int i = 0; i < abs(col_dif); i++) {
			if (sameSide(squares[square], squares[square - (i + 1)]) || enemy_found) {
				result = true;
			}
			if (otherSide(squares[square], squares[square - (i + 1)]))
				enemy_found = true;
		}
	}

	return result;

}

// returns true if square is blocked on column
bool colBlocked(vector<Square>& squares, Square& selected, Square& potential_move) {

	bool result = false, enemy_found = false;
	int row_dif = rowDif(selected, potential_move);
	int square = selected.getRow() * 8 + selected.getCol();

	if (row_dif > 0) {

		for (int i = 0; i < abs(row_dif); i++) {
			if (sameSide(squares[square], squares[square + 8 * (i + 1)]) || enemy_found) {
				result = true;
			}
			if (otherSide(squares[square], squares[square + 8 * (i + 1)]))
				enemy_found = true;
		}
	}
	else {
		for (int i = 0; i < abs(row_dif); i++) {
			if (sameSide(squares[square], squares[square - 8 * (i + 1)]) || enemy_found) {
				result = true;
			}
			if (otherSide(squares[square], squares[square - 8 * (i + 1)]))
				enemy_found = true;
		}
	}

	return result;

}

// returns true if square is blocked on diagonal
bool diagBlocked(vector<Square>& squares, Square& selected, Square& potential_move) {

	bool result = false, enemy_found = false;
	int row_dif = rowDif(selected, potential_move);
	int col_dif = colDif(selected, potential_move);
	int square = selected.getRow() * 8 + selected.getCol();

	if (row_dif == col_dif && row_dif > 0) {
		for (int i = 0; i < abs(row_dif); i++) {
			if (sameSide(squares[square], squares[square + 9 * (i + 1)]) || enemy_found)
				result = true;
			if (otherSide(squares[square], squares[square + 9 * (i + 1)]))
				enemy_found = true;
		}
	}
	else if (row_dif == col_dif && row_dif < 0) {
		for (int i = 0; i < abs(row_dif); i++) {
			if (sameSide(squares[square], squares[square - 9 * (i + 1)]) || enemy_found)
				result = true;
			if (otherSide(squares[square], squares[square - 9 * (i + 1)]))
				enemy_found = true;
		}
	}
	else if (row_dif != col_dif && row_dif > 0) {
		for (int i = 0; i < abs(row_dif); i++) {
			if (sameSide(squares[square], squares[square + 7 * (i + 1)]) || enemy_found)
				result = true;
			if (otherSide(squares[square], squares[square + 7 * (i + 1)]))
				enemy_found = true;
		}
	}
	else {
		for (int i = 0; i < abs(row_dif); i++) {
			if (sameSide(squares[square], squares[square - 7 * (i + 1)]) || enemy_found)
				result = true;
			if (otherSide(squares[square], squares[square - 7 * (i + 1)]))
				enemy_found = true;
		}
	}

	return result;


}

// checks if attacked
bool isAttacked(vector<Square>& squares, int square, Side side) {
	for (int i = 0; i < squares.size(); i++) {
		if (squares[i].getSide() != side && squares[i].getSide() != Side::None && squares[i].getPieceType() != PieceType::King) {
			vector<bool> opponent_moves = squares[i].getPiece()->getValidMoves(squares, squares[i]);
			if (opponent_moves[square]) {
				return true;
			}
		}
	}
	return false;
}

// returns row difference according to selected square
int rowDif(Square selected, Square potential_move) {
	return potential_move.getRow() - selected.getRow();
}
// returns col difference according to selected square
int colDif(Square selected, Square potential_move) {
	return potential_move.getCol() - selected.getCol();
}

// return 0 legal moves if empty piece
vector<bool> EmptyPiece::getValidMoves(vector<Square>& squares, Square selected) {
	
	vector<bool> result;
	returnAllFalse(squares, result);
	return result;

}

// get moves if pawn
vector<bool> Pawn::getValidMoves(vector<Square>& squares, Square selected) {
	vector<bool> result(squares.size(), false);
	int currentPos = selected.getRow() * 8 + selected.getCol();
	int direction = (selected.getSide() == Side::White) ? 1 : -1;

	for (int i = 0; i < squares.size(); i++) {
		int rowDif = squares[i].getRow() - selected.getRow();
		int colDif = squares[i].getCol() - selected.getCol();

		// Advance by one
		if (colDif == 0 && rowDif == direction && squares[i].getPieceType() == PieceType::None) {
			result[i] = true;
		}
		// Advance by two on first move
		if (colDif == 0 && rowDif == 2 * direction && !selected.getMoved() &&
			squares[i - direction * 8].getPieceType() == PieceType::None &&
			squares[i].getPieceType() == PieceType::None) {
			result[i] = true;
		}
		// Diagonal capture
		if (abs(colDif) == 1 && rowDif == direction && squares[i].getSide() != Side::None &&
			squares[i].getSide() != selected.getSide()) {
			result[i] = true;
		}
		// En passant capture
		if (abs(colDif) == 1 && rowDif == direction && squares[i - direction * 8].getEnPassent() &&
			squares[i - direction * 8].getPieceType() == PieceType::Pawn) {
			result[i] = true;
		}
	}
	return result;
}

// get moves if knight
vector<bool> Knight::getValidMoves(vector<Square>& squares, Square selected) {

	vector<bool> result;
	int row_dif, col_dif;

	for (int i = 0; i < squares.size(); i++) {
		// check if same side
		if (!sameSide(selected, squares[i])) {
			row_dif = rowDif(selected, squares[i]);
			col_dif =colDif(selected, squares[i]);
			if ((abs(row_dif) == 2 && abs(col_dif) == 1) || (abs(row_dif) == 1 && abs(col_dif) == 2))
				result.push_back(true);
			else
				result.push_back(false);
		}
		else
			result.push_back(false);

	}

	return result;
}

// get moves if bishop
vector<bool> Bishop::getValidMoves(vector<Square>& squares, Square selected) {

	vector<bool> result;
	int row_dif, col_dif;

	for (int i = 0; i < squares.size(); i++) {
		// check if same side
		if (!sameSide(selected, squares[i])) {
			int row_dif = rowDif(selected, squares[i]);
			int col_dif = colDif(selected, squares[i]);

			if (abs(row_dif) == abs(col_dif) && row_dif != 0) {
				if (!diagBlocked(squares, selected, squares[i]))
					result.push_back(true);
				else
					result.push_back(false);
			}
			else
				result.push_back(false);

		}
		else
			result.push_back(false);

	}

    return result;
}

// get moves if rook
vector<bool> Rook::getValidMoves(vector<Square>& squares, Square selected) {

	vector<bool> result;
	for (int i = 0; i < squares.size(); i++) {
		// check if same side
		if (!sameSide(selected, squares[i])) {
			int row_dif = rowDif(selected, squares[i]);
			int col_dif = colDif(selected, squares[i]);
			if ((abs(row_dif) > 0 && col_dif == 0))
				if (!colBlocked(squares, selected, squares[i]))
					result.push_back(true);
				else
					result.push_back(false);
			else if ((abs(col_dif) > 0 && row_dif == 0))
				if (!rowBlocked(squares, selected, squares[i]))
					result.push_back(true);
				else
					result.push_back(false);
			else
				result.push_back(false);
		}
		else
			result.push_back(false);

	}

	return result;
}

// get moves if king
vector<bool> King::getValidMoves(vector<Square>& squares, Square selected) {
	vector<bool> result(squares.size(), false);
	int king = selected.getRow() * 8 + selected.getCol();

	for (int i = 0; i < squares.size(); i++) {
		// Check if it's the same side
		if (!sameSide(selected, squares[i])) {
			int row_dif = rowDif(selected, squares[i]);
			int col_dif = colDif(selected, squares[i]);

			// Normal king moves
			if (abs(row_dif) <= 1 && abs(col_dif) <= 1) {
				result[i] = true;
			}
		}
	}

	// Castling logic
	if (!selected.getMoved()) {
		// Right side castling
		if (king % 8 <= 4 && squares[king + 1].getPieceType() == PieceType::None &&
			squares[king + 2].getPieceType() == PieceType::None && !squares[king + 3].getMoved() &&
			squares[king + 3].getPieceType() == PieceType::Rook && squares[king + 3].getSide() == selected.getSide()) {

			// Ensure the squares the king moves through are not attacked
			if (!isAttacked(squares, king, selected.getSide()) &&
				!isAttacked(squares, king + 1, selected.getSide()) &&
				!isAttacked(squares, king + 2, selected.getSide())) {
				result[king + 2] = true;
			}
		}

		// Left side castling
		if (king % 8 >= 3 && squares[king - 1].getPieceType() == PieceType::None &&
			squares[king - 2].getPieceType() == PieceType::None && squares[king - 3].getPieceType() == PieceType::None &&
			!squares[king - 4].getMoved() && squares[king - 4].getPieceType() == PieceType::Rook &&
			squares[king - 4].getSide() == selected.getSide()) {

			// Ensure the squares the king moves through are not attacked
			if (!isAttacked(squares, king, selected.getSide()) &&
				!isAttacked(squares, king - 1, selected.getSide()) &&
				!isAttacked(squares, king - 2, selected.getSide())) {
				result[king - 2] = true;
			}
		}
	}

	return result;
}

// get moves if queen
vector<bool> Queen::getValidMoves(vector<Square>& squares, Square selected) {

	vector<bool> result;
	for (int i = 0; i < squares.size(); i++) {
		// check if same side
		if (!sameSide(selected, squares[i])) {
			int row_dif = rowDif(selected, squares[i]);
			int col_dif = colDif(selected, squares[i]);
			if ((abs(row_dif) > 0 && col_dif == 0))
				if (!colBlocked(squares, selected, squares[i]))
					result.push_back(true);
				else
					result.push_back(false);
			else if (abs(col_dif) > 0 && row_dif == 0)
				if (!rowBlocked(squares, selected, squares[i]))
					result.push_back(true);
				else
					result.push_back(false);
			else if (abs(col_dif) == abs(row_dif))
				if (!diagBlocked(squares, selected, squares[i]))
					result.push_back(true);
				else
					result.push_back(false);
			else
				result.push_back(false);
		}
		else
			result.push_back(false);

	}
	return result;

}