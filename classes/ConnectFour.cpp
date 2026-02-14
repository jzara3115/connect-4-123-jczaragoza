#include "ConnectFour.h"
#include <algorithm>
#include <climits>

ConnectFour::ConnectFour()
{
    _grid = new Grid(CONNECT_FOUR_COLS, CONNECT_FOUR_ROWS);
}

ConnectFour::~ConnectFour()
{
    delete _grid;
}

Bit* ConnectFour::PieceForPlayer(const int playerNumber)
{
    Bit *bit = new Bit();
    // Ai uses yellow, Player uses red
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    bit->setScale(0.8f);
    return bit;
}

void ConnectFour::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = CONNECT_FOUR_COLS;
    _gameOptions.rowY = CONNECT_FOUR_ROWS;
    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

// Handle clicking on an empty column to drop a piece
bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    ChessSquare* square = static_cast<ChessSquare*>(&holder);
    int column = square->getColumn();
    
    int row = getLowestAvailableRow(column);
    if (row == -1) {
        return false;
    }

    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    ChessSquare* targetSquare = _grid->getSquare(column, row);
    
    ImVec2 startPos = ImVec2(targetSquare->getPosition().x, _grid->getSquare(column, 0)->getPosition().y - 100);
    bit->setPosition(startPos);
    
    // Place bit in target square and animate it falling
    targetSquare->setBit(bit);
    bit->moveTo(targetSquare->getPosition());
    
    endTurn();
    return true;
}

bool ConnectFour::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    return false;
}

// Clean up
void ConnectFour::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

int ConnectFour::getLowestAvailableRow(int column) const
{
    for (int row = CONNECT_FOUR_ROWS - 1; row >= 0; row--) {
        ChessSquare* square = _grid->getSquare(column, row);
        if (square && !square->bit()) {
            return row;
        }
    }
    return -1; 
}

// Helps get owner at position
Player* ConnectFour::ownerAt(int x, int y) const
{
    if (x < 0 || x >= CONNECT_FOUR_COLS || y < 0 || y >= CONNECT_FOUR_ROWS) {
        return nullptr;
    }
    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

// Check for 4 in a row starting from position (x,y) in direction (dx,dy)
bool ConnectFour::checkWinFrom(int x, int y, int dx, int dy) const
{
    Player* player = ownerAt(x, y);
    if (!player) return false;
    
    for (int i = 1; i < 4; i++) {
        if (ownerAt(x + i*dx, y + i*dy) != player) {
            return false;
        }
    }
    return true;
}

Player* ConnectFour::checkForWinner()
{
    // Check all positions and directions for 4 in a row
    for (int y = 0; y < CONNECT_FOUR_ROWS; y++) {
        for (int x = 0; x < CONNECT_FOUR_COLS; x++) {
            Player* player = ownerAt(x, y);
            if (!player) continue;
            
            if (x <= CONNECT_FOUR_COLS - 4 && checkWinFrom(x, y, 1, 0)) {
                return player;
            }
            if (y <= CONNECT_FOUR_ROWS - 4 && checkWinFrom(x, y, 0, 1)) {
                return player;
            }
            if (x <= CONNECT_FOUR_COLS - 4 && y <= CONNECT_FOUR_ROWS - 4 && checkWinFrom(x, y, 1, 1)) {
                return player;
            }
            if (x >= 3 && y <= CONNECT_FOUR_ROWS - 4 && checkWinFrom(x, y, -1, 1)) {
                return player;
            }
        }
    }
    return nullptr;
}

bool ConnectFour::checkForDraw()
{
    for (int x = 0; x < CONNECT_FOUR_COLS; x++) {
        if (getLowestAvailableRow(x) != -1) {
            return false; 
        }
    }
    return true;
}

std::string ConnectFour::initialStateString()
{
    return std::string(CONNECT_FOUR_COLS * CONNECT_FOUR_ROWS, '0');
}

std::string ConnectFour::stateString()
{
    std::string s(CONNECT_FOUR_COLS * CONNECT_FOUR_ROWS, '0');
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * CONNECT_FOUR_COLS + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

void ConnectFour::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * CONNECT_FOUR_COLS + x;
        if (index < s.length()) {
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                square->setBit(PieceForPlayer(playerNumber-1));
            } else {
                square->setBit(nullptr);
            }
        }
    });
}

int ConnectFour::evaluatePosition(const std::string& state) const
{
    int score = 0;
    
    // Check all possible positions
    for (int y = 0; y < CONNECT_FOUR_ROWS; y++) {
        for (int x = 0; x < CONNECT_FOUR_COLS; x++) {
            for (int dir = 0; dir < 4; dir++) {
                int dx = 0, dy = 0;
                if (dir == 0) { dx = 1; dy = 0; } // Horizontal
                else if (dir == 1) { dx = 0; dy = 1; } // Vertical
                else if (dir == 2) { dx = 1; dy = 1; } // Diagonal down-right
                else { dx = -1; dy = 1; } // Diagonal down-left
                
                int aiCount = 0, oppCount = 0, empty = 0;
                bool valid = true;
                
                for (int i = 0; i < 4; i++) {
                    int nx = x + i*dx;
                    int ny = y + i*dy;
                    if (nx < 0 || nx >= CONNECT_FOUR_COLS || ny < 0 || ny >= CONNECT_FOUR_ROWS) {
                        valid = false;
                        break;
                    }
                    char c = state[ny * CONNECT_FOUR_COLS + nx];
                    if (c == '2') aiCount++;
                    else if (c == '1') oppCount++;
                    else empty++;
                }
                
                if (!valid) continue;
                
                // Score this window
                if (aiCount == 4) score += 100000;
                else if (aiCount == 3 && empty == 1) score += 100;
                else if (aiCount == 2 && empty == 2) score += 10;
                
                if (oppCount == 4) score -= 100000;
                else if (oppCount == 3 && empty == 1) score -= 90; // Prioritize blocking
                else if (oppCount == 2 && empty == 2) score -= 10;
            }
        }
    }
    
    for (int y = 0; y < CONNECT_FOUR_ROWS; y++) {
        if (state[y * CONNECT_FOUR_COLS + 3] == '2') score += 3;
    }
    
    return score;
}

// Minimax with alpha-beta pruning
int ConnectFour::minimax(std::string state, int depth, int alpha, int beta, bool maximizing)
{
    bool hasMove = false;
    for (int x = 0; x < CONNECT_FOUR_COLS; x++) {
        for (int y = CONNECT_FOUR_ROWS - 1; y >= 0; y--) {
            if (state[y * CONNECT_FOUR_COLS + x] == '0') {
                hasMove = true;
                break;
            }
        }
        if (hasMove) break;
    }
    
    if (depth == 0 || !hasMove) {
        return evaluatePosition(state);
    }
    
    if (maximizing) {
        int maxEval = INT_MIN;
        for (int x = 0; x < CONNECT_FOUR_COLS; x++) {
            int row = -1;
            for (int y = CONNECT_FOUR_ROWS - 1; y >= 0; y--) {
                if (state[y * CONNECT_FOUR_COLS + x] == '0') {
                    row = y;
                    break;
                }
            }
            if (row == -1) continue;
            
            std::string newState = state;
            newState[row * CONNECT_FOUR_COLS + x] = '2'; 
            
            int eval = minimax(newState, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int x = 0; x < CONNECT_FOUR_COLS; x++) {
            // Find lowest available row in column
            int row = -1;
            for (int y = CONNECT_FOUR_ROWS - 1; y >= 0; y--) {
                if (state[y * CONNECT_FOUR_COLS + x] == '0') {
                    row = y;
                    break;
                }
            }
            if (row == -1) continue; // Column full
            
            // Make move
            std::string newState = state;
            newState[row * CONNECT_FOUR_COLS + x] = '1'; 
            
            int eval = minimax(newState, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

// Get best column for AI to play
int ConnectFour::getBestColumn(std::string& state)
{
    int bestCol = -1;
    int bestScore = INT_MIN;
    int depth = 5; 
    
    for (int x = 0; x < CONNECT_FOUR_COLS; x++) {
        // Find lowest available row in column
        int row = -1;
        for (int y = CONNECT_FOUR_ROWS - 1; y >= 0; y--) {
            if (state[y * CONNECT_FOUR_COLS + x] == '0') {
                row = y;
                break;
            }
        }
        if (row == -1) continue; // Column full
        
        std::string newState = state;
        newState[row * CONNECT_FOUR_COLS + x] = '2'; 
        
        int score = minimax(newState, depth - 1, INT_MIN, INT_MAX, false);
        
        if (score > bestScore) {
            bestScore = score;
            bestCol = x;
        }
    }
    
    return bestCol;
}

void ConnectFour::updateAI() 
{
    // Don't move during animation
    bool hasMovingPiece = false;
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        if (square->bit() && square->bit()->getMoving()) {
            hasMovingPiece = true;
        }
    });
    
    if (hasMovingPiece) {
        return;
    }
    
    std::string state = stateString();
    int bestCol = getBestColumn(state);
    
    if (bestCol != -1) {
        int row = getLowestAvailableRow(bestCol);
        if (row != -1) {
            ChessSquare* square = _grid->getSquare(bestCol, 0); // Top of column for click
            actionForEmptyHolder(*square);
        }
    }
}