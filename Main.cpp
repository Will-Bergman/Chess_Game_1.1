#include <iostream>

#include <SFML/Graphics.hpp>
#include "SFML\Window.hpp"
#include "SFML\System.hpp"
#include "Chess.hpp"

using namespace sf;
using namespace std;

typedef Vector2f fpoint;
typedef Vector2i ipoint;

#define WIDTH 1280
#define HEIGHT 720

// board offset from top left
#define OFFSET_X 200.f
#define OFFSET_Y 40.f
#define SIZE 640

//
#define ROWS 8
#define COLS 8
#define SQUARE_W SIZE/ROWS
#define SQUARE_H SIZE/COLS 
#define SQUARE1 Vector2f(OFFSET_X, OFFSET_Y + SIZE - SQUARE_H)

void resetLegals(vector<bool>& legals);
void getLegals(vector<bool>& legals, vector<Square> squares, Square selected);
void showLegals(vector<bool> legals, vector<CircleShape>& grid);
void getRealLegals(vector<bool>& legals, vector<Square>& squares, int selected, int king_square, Piece* emptySquare);
void promote(vector<Square>& squares, Square& selected, Sprite& w_prom, Sprite& b_prom, fpoint mouse_pos, vector<RectangleShape> grid, RenderWindow& window, Mouse& mouse, fpoint size);
void makeMove(vector<Square>& squares, int selected, int clicked_on, Piece* emptySquare, bool& promotion);
bool isTurn(vector<Square> squares, int square_clicked_on, bool turn);
int getKingSquare(vector<Square> squares, bool turn);
int getSquare(fpoint mouse_pos, vector<RectangleShape> squares);


// updates textures based on square's piece
void setTextures(vector<Sprite>& sprites, vector<Square>& squares, Texture& emptyTexture,
    Texture& w_pawn, Texture& b_pawn,
    Texture& w_knight, Texture& b_knight,
    Texture& w_bish, Texture& b_bish,
    Texture& w_rook, Texture& b_rook,
    Texture& w_queen, Texture& b_queen,
    Texture& w_king, Texture& b_king) {
    for (int i = 0; i < sprites.size(); i++) {
        sprites[i].setPosition(SQUARE1.x + (i % 8) * SQUARE_W, SQUARE1.y - (i / 8) * SQUARE_H);
        sprites[i].setScale((float)SQUARE_W / (float)w_pawn.getSize().x, (float)SQUARE_H / (float)w_pawn.getSize().x);
        
        switch (squares[i].getPieceType()) {
        case(PieceType:: None):
            if (squares[i].getSide() == Side::None) {
                sprites[i].setTexture(emptyTexture);
            }
            break;
        case(PieceType::Pawn):
            if (squares[i].getSide() == Side::White)
                sprites[i].setTexture(w_pawn);
            else
                sprites[i].setTexture(b_pawn);
            break;
        case(PieceType::Knight):
            if (squares[i].getSide() == Side::White)
                sprites[i].setTexture(w_knight);
            else
                sprites[i].setTexture(b_knight);
            break;
        case(PieceType::Bishop):
            if (squares[i].getSide() == Side::White)
                sprites[i].setTexture(w_bish);
            else
                sprites[i].setTexture(b_bish);
            break;
        case(PieceType::Rook):
            if (squares[i].getSide() == Side::White)
                sprites[i].setTexture(w_rook);
            else
                sprites[i].setTexture(b_rook);
            break;
        case(PieceType::Queen):
            if (squares[i].getSide() == Side::White)
                sprites[i].setTexture(w_queen);
            else
                sprites[i].setTexture(b_queen);
            break;
        case(PieceType::King):
            if (squares[i].getSide() == Side::White)
                sprites[i].setTexture(w_king);
            else
                sprites[i].setTexture(b_king);
            break;
        }
        
    }
}

// resets legal moves grid
void resetLegals(vector<bool>& legals) {
    for (int i = 0; i < legals.size(); i++) {
        legals[i] = false;
    }
}

// returns grid of booleans with respective legal moves
void getLegals(vector<bool>& legals, vector<Square> squares, Square selected) {

    Piece* piece = selected.getPiece();
    vector<bool> validMoves = piece->getValidMoves(squares, selected);
    for (int i = 0; i < legals.size(); i++) {
        legals[i] = validMoves[i];
    }

}

// changes opacity of legal move grid
void showLegals(vector<bool> legals, vector<CircleShape>& grid) {

    for (int i = 0; i < legals.size(); i++) {
        if (legals[i])
            grid[i].setFillColor(Color(0, 0, 0, 100));
        else
            grid[i].setFillColor(Color(0, 0, 0, 0));
    }

}

// restricts legal moves based on king
void getRealLegals(vector<bool>& legals, vector<Square>& squares, int selected, int king_square, Piece* emptySquare) {
    // Backup the original board state
    vector<Square> original_squares = squares;
    Piece* captured_pawn = nullptr;
    int captured_pawn_pos;
    // Simulate each move and check if it puts the king in check
    for (int i = 0; i < legals.size(); i++) {
        if (legals[i]) {
            // Make the move
            Piece* movedPiece = squares[selected].getPiece();
            Piece* capturedPiece = squares[i].getPiece();
            squares[i].setPiece(movedPiece);
            squares[selected].setPiece(emptySquare);

            // Handle en passant special case
            if (squares[i].getEnPassent()) {
                // Determine the position of the captured pawn
                captured_pawn_pos = (movedPiece->getSide() == Side::White) ? i - 8 : i + 8;
                captured_pawn = squares[captured_pawn_pos].getPiece();

                // Remove the captured pawn from the board
                squares[captured_pawn_pos].setPiece(emptySquare);
            }

            // Determine the new king's position
            int new_king_square = (movedPiece->getType() == PieceType::King) ? i : king_square;

            // Check if the king is in check after the move
            bool king_in_check = false;
            for (int j = 0; j < squares.size(); j++) {
                if (squares[j].getSide() != squares[new_king_square].getSide() && squares[j].getSide() != Side::None) {
                    vector<bool> opponent_legals(squares.size(), false);
                    getLegals(opponent_legals, squares, squares[j]);
                    if (opponent_legals[new_king_square]) {
                        king_in_check = true;
                        break;
                    }
                }
            }

            // If the king is in check, the move is not legal
            if (king_in_check) {
                legals[i] = false;
            }

            // Revert the board to the original state
            squares[i].setPiece(capturedPiece);
            squares[selected].setPiece(movedPiece);

            // Restore en passant captured pawn if applicable
            if (squares[i].getEnPassent()) {
                int captured_pawn_pos = (movedPiece->getSide() == Side::White) ? i - 8 : i + 8;
                squares[captured_pawn_pos].setPiece(captured_pawn);
            }
        }
    }
}

// changes the board based on move
void makeMove(vector<Square>& squares, int selected, int clicked_on, Piece* emptySquare, bool& promotion) {

    cout << "made move";
    // Reset en passant flags
    for (Square& square : squares) {
        square.setEnPassent(false);
    }

    // Check if a pawn moved two squares forward
    if (squares[selected].getPieceType() == PieceType::Pawn && abs(clicked_on - selected) == 16) {
        squares[clicked_on].setEnPassent(true);
        cout << squares[clicked_on].getEnPassent();
    }

    // Handle en passant capture
    if (squares[selected].getPieceType() == PieceType::Pawn && abs(clicked_on - selected) % 8 != 0 &&
        squares[clicked_on].getPieceType() == PieceType::None) {
        int capturedPawnIndex = (squares[selected].getSide() == Side::White) ? clicked_on - 8 : clicked_on + 8;
        squares[capturedPawnIndex].setPiece(emptySquare);
    }

    // Move the piece
    squares[clicked_on].setPiece(squares[selected].getPiece());
    squares[clicked_on].setMoved(true);
    squares[selected].setPiece(emptySquare);
    squares[selected].setMoved(true);

    // check if promoted
    if (squares[clicked_on].getPieceType() == PieceType::Pawn) {
        // Determine if the pawn reaches the promotion rank
        int promotionRank = (squares[clicked_on].getSide() == Side::White) ? 7 : 0;
        if (squares[clicked_on].getRow() == promotionRank) {
            promotion = true;
        }
    }

    // Handle castling move
    if (squares[clicked_on].getPieceType() == PieceType::King && abs(clicked_on - selected) == 2) {
        if (clicked_on > selected) { // Castle right
            squares[clicked_on - 1].setPiece(squares[clicked_on + 1].getPiece());
            squares[clicked_on + 1].setPiece(emptySquare);
            squares[clicked_on + 1].setMoved(true);
        }
        else { // Castle left
            squares[clicked_on + 1].setPiece(squares[clicked_on - 2].getPiece());
            squares[clicked_on - 2].setPiece(emptySquare);
            squares[clicked_on - 2].setMoved(true);
        }
    }
}

// promotion function
void promote(vector<Square>& squares, Square& selected, Sprite& w_prom, Sprite& b_prom, fpoint mouse_pos, vector<RectangleShape> grid, RenderWindow& window, Mouse& mouse, fpoint size) {
    bool chosen = false, pressed = false;
    int promotionRank = (selected.getSide() == Side::White) ? 7 : 0;
    int promotionSquare = (selected.getSide() == Side::White) ? promotionRank * 8 : promotionRank * 8 + 7;

    // Determine the position to display the promotion options
    fpoint promotionPosition = grid[promotionSquare].getPosition();

    // Display the promotion options
    if (selected.getSide() == Side::White) {
        w_prom.setPosition(OFFSET_X + SIZE/2, OFFSET_Y + SIZE/ 2);
        window.draw(w_prom);
    }
    else {
        b_prom.setPosition(OFFSET_X + SIZE / 2, OFFSET_Y + SIZE / 2);
        window.draw(b_prom);
    }

    while (!chosen) {
        if (Mouse::isButtonPressed(Mouse::Left) && !pressed) {
            fpoint mouse_pos = fpoint((float)mouse.getPosition(window).x / size.x, (float)mouse.getPosition(window).y / size.y);
            int square = getSquare(mouse_pos, grid);
            pressed = true;
            cout << square;
            if (square != -1) {
                PieceType promotionPieceType;
                switch (square) {
                case 27:  // Assuming position 19 corresponds to promoting to Queen
                    promotionPieceType = PieceType::Bishop;
                    break;
                case 28:  // Assuming position 20 corresponds to promoting to Rook
                    promotionPieceType = PieceType::Knight;
                    break;
                case 35:  // Assuming position 27 corresponds to promoting to Bishop
                    promotionPieceType = PieceType::Rook;
                    break;
                case 36:  // Assuming position 28 corresponds to promoting to Knight
                    promotionPieceType = PieceType::Queen;
                    cout << "here";
                    break;
                default:
                    continue;  // If no valid promotion position clicked, continue waiting for valid input
                }

                // Promote the pawn to the chosen piece type
                chosen = true;
            }
        }
        if (pressed && !Mouse::isButtonPressed(Mouse::Left)) {
            pressed = false;
        }
    }
}


// Checks if square clicked is on correct turn
bool isTurn(vector<Square> squares, int square_clicked_on, bool turn) {
    if ((squares[square_clicked_on].getSide() == Side::White && turn) || (squares[square_clicked_on].getSide() == Side::Black && !turn))
        return true;
    else
        return false;
}

// Gets king square position
int getKingSquare(vector<Square> squares, bool turn) {
    if (turn) {
        for (int i = 0; i < squares.size(); i++) {
            if (squares[i].getPieceType() == PieceType::King && squares[i].getSide() == Side::White)
                return i;
        }
    }
    else {
        for (int i = 0; i < squares.size(); i++) {
            if (squares[i].getPieceType() == PieceType::King && squares[i].getSide() == Side::Black)
                return i;
        }
    }
}

// Gets # of square clicked on
int getSquare(fpoint mouse_pos, vector<RectangleShape> squares) {

    int col = 0, row = 0;
    if (mouse_pos.x < OFFSET_X || mouse_pos.x > (OFFSET_X + SIZE) || mouse_pos.y < OFFSET_Y || mouse_pos.y > (OFFSET_Y + SIZE)) {
        return -1;
    }
    for (int i = 0; i < COLS; i++) {
        if (mouse_pos.x >= squares[i].getPosition().x && mouse_pos.x < squares[i].getPosition().x + SQUARE_W)
            col = i;
    }
    for (int i = 0; i < ROWS; i++) {
        if (mouse_pos.y >= squares[i * 8].getPosition().y && mouse_pos.y < squares[i * 8].getPosition().y + SQUARE_H)
            row = 7 - i;
    }

    return (ROWS - row - 1) * ROWS + col;

}

int main()
{

    // Game Window
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Chess Game", Style::Default);

    // Create Pieces
    Piece* whiteRook = new Rook(Side::White);
    Piece* whiteKnight = new Knight(Side::White);
    Piece* whiteBishop = new Bishop(Side::White);
    Piece* whiteQueen = new Queen(Side::White);
    Piece* whiteKing = new King(Side::White);
    Piece* whitePawn = new Pawn(Side::White);

    Piece* blackRook = new Rook(Side::Black);
    Piece* blackKnight = new Knight(Side::Black);
    Piece* blackBishop = new Bishop(Side::Black);
    Piece* blackQueen = new Queen(Side::Black);
    Piece* blackKing = new King(Side::Black);
    Piece* blackPawn = new Pawn(Side::Black);

    Piece* emptySquare = new EmptyPiece();

    // Create board
        // 1st rank
    vector<Square> squares;
    squares.push_back(Square(0, 0, whiteRook, false));
    squares.push_back(Square(0, 1, whiteKnight, false));
    squares.push_back(Square(0, 2, whiteBishop, false));
    squares.push_back(Square(0, 3, whiteQueen, false));
    squares.push_back(Square(0, 4, whiteKing, false));
    squares.push_back(Square(0, 5, whiteBishop, false));
    squares.push_back(Square(0, 6, whiteKnight, false));
    squares.push_back(Square(0, 7, whiteRook, false));
    // 2nd rank
    for (int i = 0; i < 8; i++) {
        squares.push_back(Square(1, i, whitePawn, false));
    }
    // 3rd-6th ranks
    for (int i = 0; i < 32; i++) {
        squares.push_back(Square((i / 8) + 2, i % 8, emptySquare, false));
    }
    // 7th rank
    for (int i = 0; i < 8; i++) {
        squares.push_back(Square(6, i, blackPawn, false));
    }
    // 8th rank
    squares.push_back(Square(7, 0, blackRook, false));
    squares.push_back(Square(7, 1, blackKnight, false));
    squares.push_back(Square(7, 2, blackBishop, false));
    squares.push_back(Square(7, 3, blackQueen, false));
    squares.push_back(Square(7, 4, blackKing, false));
    squares.push_back(Square(7, 5, blackBishop, false));
    squares.push_back(Square(7, 6, blackKnight, false));
    squares.push_back(Square(7, 7, blackRook, false));

    // Game Mouse
    Mouse mouse;
    bool alrPressed = false;
    bool turn = true;
    bool promotion = false;
    int promotionRank, promotionSquare;
    Piece* promotionPieceType;

    // Game Border
    RectangleShape border(fpoint(SIZE, SIZE));
    border.setPosition(fpoint(OFFSET_X, OFFSET_Y));
    border.setFillColor(Color(0, 0, 0, 100));
    border.setOutlineThickness(20.f);
    border.setOutlineColor(Color(26, 3, 3, 255));

    // Highlighted Square
    RectangleShape highlighted_rec(fpoint(SQUARE_W, SQUARE_H));
    highlighted_rec.setPosition(SQUARE1);
    highlighted_rec.setFillColor(Color(255, 0, 0, 0));
    int selected = -1;
    bool highlighted = false;

    // Rectangle Grid
    vector<RectangleShape> grid(ROWS * COLS);
    for (int i = 0; i < squares.size(); i++) {
        grid[i].setSize(fpoint(SQUARE_W, SQUARE_H));
    }

    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {

            grid[8 * i + j].setPosition(fpoint(SQUARE1.x + (float)(j * SQUARE_W), SQUARE1.y - (float)(i * SQUARE_H)));
            if ((i + j) % 2 == 0)
                grid[8 * i + j].setFillColor(Color(186, 154, 115, 255));
            else
                grid[8 * i + j].setFillColor(Color(59, 48, 36, 255));

        }
    }

    // Load Game Pieces
    Texture w_pawn, b_pawn, w_knight, b_knight, w_bish, b_bish, w_rook, b_rook, w_queen, b_queen, w_king, b_king, w_prom, b_prom, emptyTexture;
    w_pawn.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\W_Pawn.png");
    b_pawn.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\B_Pawn.png");
    w_knight.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\W_Knight.png");
    b_knight.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\B_Knight.png");
    w_bish.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\W_Bish.png");
    b_bish.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\B_Bish.png");
    w_rook.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\W_Rook.png");
    b_rook.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\B_Rook.png");
    w_queen.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\W_Queen.png");
    b_queen.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\B_Queen.png");
    w_king.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\W_King.png");
    b_king.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\B_King.png");
    w_prom.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\W_Promotion.png");
    b_prom.loadFromFile("C:\\Users\\willi\\source\\repos\\Chess_Game_1.1\\B_Promotion.png");

    // Set the size of the empty texture (assuming 64x64 for example)
    emptyTexture.create(128, 128);

    // Fill the texture with a solid color (e.g., transparent)
    Image emptyImage;
    emptyImage.create(128, 128, Color::Transparent);
    emptyTexture.update(emptyImage);

    // Sprite Grid
    vector<Sprite> sprites(ROWS * COLS);
    setTextures(sprites, squares, emptyTexture,
        w_pawn, b_pawn,
        w_knight, b_knight,
        w_bish, b_bish,
        w_rook, b_rook,
        w_queen, b_queen,
        w_king, b_king);

    // promotions
    Sprite White_prom, Black_prom;
    White_prom.setTexture(w_prom);
    Black_prom.setTexture(b_prom);
    White_prom.setScale((float)SQUARE_W * 2 / (float)w_prom.getSize().x, (float)SQUARE_H * 2 / (float)w_prom.getSize().y);
    Black_prom.setScale((float)SQUARE_W * 2 / (float)b_prom.getSize().x, (float)SQUARE_H * 2 / (float)w_prom.getSize().y);

    // promotion outline
    RectangleShape prom_outline;
    prom_outline.setPosition(OFFSET_X + SIZE / 2 - SQUARE_W, OFFSET_Y + SIZE / 2 - SQUARE_H);
    prom_outline.setSize(fpoint((float)SQUARE_W * 2, (float)SQUARE_H * 2));
    prom_outline.setFillColor(Color(245, 155, 66, 255));
    prom_outline.setOutlineColor(Color(0, 0, 0, 255));
    prom_outline.setOutlineThickness(SQUARE_W * 0.25);

    // Legal Moves Grid
    vector<bool> legals;
    vector<CircleShape> legal_moves(ROWS * COLS);
    for (int i = 0; i < legal_moves.size(); i++) {
        legals.push_back(true);
        legal_moves[i].setRadius(20);
        legal_moves[i].setFillColor(Color(0, 0, 0, 0));
        legal_moves[i].setPosition(SQUARE1.x + (i % 8) * SQUARE_W + 0.25 * SQUARE_W, SQUARE1.y - (i / 8) * SQUARE_H + 0.25 * SQUARE_H);
    }

    // Loop
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

        }
        //----------------------------------------------------------------------------------------------------------
        // Update --------------------------------------------------------------------------------------------------

            // get mouse pos with according to window size
        fpoint size = fpoint((float)window.getSize().x / WIDTH, (float)window.getSize().y / HEIGHT);
        fpoint mouse_pos = fpoint((float)mouse.getPosition(window).x / size.x, (float)mouse.getPosition(window).y / size.y);

        if (!alrPressed && Mouse::isButtonPressed(Mouse::Left)) {

            // Find square clicked on (if any)
            int square_clicked_on = getSquare(mouse_pos, grid);
            if (square_clicked_on != -1) {

                // Check if there is a square already selected
                if (!highlighted && isTurn(squares, square_clicked_on, turn)) {

                    selected = square_clicked_on;
                    // set position of highlight
                    if ((squares[selected].getSide() == Side::White && turn) || (squares[selected].getSide() == Side::Black && !turn)) {
                        highlighted_rec.setPosition(grid[square_clicked_on].getPosition());
                        highlighted_rec.setFillColor(Color(255, 0, 0, 100));
                        highlighted = true;
                    }
                    else {
                        highlighted_rec.setFillColor(Color(255, 0, 0, 0));
                        highlighted = false;
                    }

                    // Find legal moves

                    getLegals(legals, squares, squares[selected]);
                    int king = getKingSquare(squares, turn);
                    getRealLegals(legals, squares, selected, king, emptySquare);

                    // Show legal moves
                    showLegals(legals, legal_moves);

                }
                else {

                    highlighted_rec.setFillColor(Color(255, 0, 0, 0));
                    highlighted = false;
                    for (int i = 0; i < squares.size(); i++) {
                        legal_moves[i].setFillColor(Color(0, 0, 0, 0));
                    }

                    // Make move if legal

                    if (legals[square_clicked_on] && selected != -1) {

                        promotion = false;
                        makeMove(squares, selected, square_clicked_on, emptySquare, promotion);
                        if (promotion) {
                            bool chosen = false, pressed = false;
                            promotionRank = (squares[square_clicked_on].getSide() == Side::White) ? 7 : 0;
                            promotionSquare = (squares[square_clicked_on].getSide() == Side::White) ? promotionRank * 8 : promotionRank * 8 + 7;

                            // Display the promotion options
                            if (squares[square_clicked_on].getSide() == Side::White) {
                                White_prom.setPosition(OFFSET_X + SIZE / 2 - SQUARE_W, OFFSET_Y + SIZE / 2 - SQUARE_H);
                            }
                            else {
                                Black_prom.setPosition(OFFSET_X + SIZE / 2 - SQUARE_W, OFFSET_Y + SIZE / 2 - SQUARE_H);
                            }

                            // Separate loop for promotion rendering
                            while (!chosen) {

                                // Clear window
                                window.clear();

                                // Draw the main game content here (board, pieces, etc.)
                                window.clear(Color(51, 48, 47, 255));

                                window.draw(border);
                                for (int i = 0; i < grid.size(); i++) {
                                    window.draw(grid[i]);
                                }
                                window.draw(highlighted_rec);
                                for (int i = 0; i < sprites.size(); i++) {
                                    window.draw(sprites[i]);
                                    window.draw(legal_moves[i]);
                                }
                                window.draw(prom_outline);
                                // Draw the promotion sprites
                                if (squares[square_clicked_on].getSide() == Side::White) {
                                    window.draw(White_prom);
                                }
                                else {
                                    window.draw(Black_prom);
                                }

                                // Display what was drawn
                                window.display();

                                // Check for user input or conditions to complete promotion
                                if (Mouse::isButtonPressed(Mouse::Left) && !pressed) {
                                    fpoint mouse_pos = fpoint((float)mouse.getPosition(window).x / size.x, (float)mouse.getPosition(window).y / size.y);
                                    int square = getSquare(mouse_pos, grid);
                                    pressed = true;

                                    if (square != -1) {
                                        
                                        if (squares[square_clicked_on].getSide() == Side::White) {
                                            switch (square) {
                                            case 27:  // Assuming position 19 corresponds to promoting to Queen
                                                promotionPieceType = whiteBishop;
                                                break;
                                            case 28:  // Assuming position 20 corresponds to promoting to Rook
                                                promotionPieceType = whiteKnight;
                                                break;
                                            case 35:  // Assuming position 27 corresponds to promoting to Bishop
                                                promotionPieceType = whiteRook;
                                                break;
                                            case 36:  // Assuming position 28 corresponds to promoting to Knight
                                                promotionPieceType = whiteQueen;
                                                break;
                                            default:
                                                continue;  // If no valid promotion position clicked, continue waiting for valid input
                                            }
                                        }
                                        else {
                                            switch (square) {
                                            case 27:  // Assuming position 19 corresponds to promoting to Queen

                                                promotionPieceType = blackBishop;
                                                break;
                                            case 28:  // Assuming position 20 corresponds to promoting to Rook
                                                promotionPieceType = blackKnight;
                                                break;
                                            case 35:  // Assuming position 27 corresponds to promoting to Bishop
                                                promotionPieceType = blackRook;
                                                break;
                                            case 36:  // Assuming position 28 corresponds to promoting to Knight
                                                promotionPieceType = blackQueen;
                                                break;
                                            default:
                                                continue;  // If no valid promotion position clicked, continue waiting for valid input
                                            }
                                        }
                                        // Promote the pawn to the chosen piece type
                                        squares[square_clicked_on].setPiece((promotionPieceType));

                                        chosen = true;
                                    }
                                }

                                if (pressed && !Mouse::isButtonPressed(Mouse::Left)) {
                                    pressed = false;
                                }
                            }

                            // Once promotion is complete, exit the promotion function and return to the main game loop
                        }

                        setTextures(sprites, squares, emptyTexture,
                            w_pawn, b_pawn,
                            w_knight, b_knight,
                            w_bish, b_bish,
                            w_rook, b_rook,
                            w_queen, b_queen,
                            w_king, b_king);

                        turn = !turn;
                        resetLegals(legals);

                    }

                }

            }

            // Toggle boolean
            alrPressed = true;
        }
        if (alrPressed && !Mouse::isButtonPressed(Mouse::Left)) {


            // Toggle boolean
            alrPressed = false;
        }

        //--------------------------------------------------------------------------------------
        // Draw --------------------------------------------------------------------------------
        window.clear(Color(51, 48, 47, 255));

        window.draw(border);
        for (int i = 0; i < grid.size(); i++) {
            window.draw(grid[i]);
        }
        window.draw(highlighted_rec);
        for (int i = 0; i < sprites.size(); i++) {
            window.draw(sprites[i]);
            window.draw(legal_moves[i]);
        }
        window.display();
        //----------------------------------------------------------------------------------------
    }
    return 0;

}