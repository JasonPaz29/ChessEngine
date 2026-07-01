#pragma once

#include "chess.h"

struct Engine {
    Move randomMove(vector<Move> legalMoves);
    int evaluateBoard(ChessBoard& board, PieceColor engineColor);
    Move greedyMove(vector<Move> legalMoves, ChessBoard& chessBoard);
};