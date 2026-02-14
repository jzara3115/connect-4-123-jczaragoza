#pragma once
#include "Game.h"

const int CONNECT_FOUR_COLS = 7;
const int CONNECT_FOUR_ROWS = 6;

class ConnectFour : public Game
{
public:
    ConnectFour();
    ~ConnectFour();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;

    void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid*       getGrid() override { return _grid; }

private:
    Bit*        PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int x, int y) const;
    int         getLowestAvailableRow(int column) const;
    bool        checkWinFrom(int x, int y, int dx, int dy) const;
    int         evaluatePosition(const std::string& state) const;
    int         minimax(std::string state, int depth, int alpha, int beta, bool maximizing);
    int         getBestColumn(std::string& state);

    Grid*       _grid;
};
