#include "engine.h"
#include "chess.h"
#include <random>
#include <vector>


Move Engine::randomMove(vector<Move> legalMoves){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distrib(0, legalMoves.size()-1);

    int random_variable = distrib(gen);

    return legalMoves[random_variable];
}

int Engine::evaluateBoard(ChessBoard& board, PieceColor engineColor){
    int score = 0;
    int temp_score = 0;
    Piece piece;

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            piece = board.board[i][j];

            if (piece.type != Empty){
                    switch(piece.type) {
                        case PieceType::Pawn:
                            temp_score = 100;
                            break;
                        case PieceType::Knight:
                            temp_score = 300;
                            break;
                        case PieceType::Bishop:
                            temp_score = 300;
                            break;
                        case PieceType::Rook:
                            temp_score = 500;
                            break;
                        case PieceType::Queen:
                            temp_score = 900;
                            break;
                        case PieceType::King:
                            temp_score = 0;
                            break;
                        default:
                            temp_score = 0;
            }
            if (piece.color == engineColor){
                score += temp_score;
            }
            else {
                score -= temp_score;
            }
        }
    }  
}
 return score;
}

Move Engine::greedyMove(vector<Move> legalMoves, ChessBoard& chessBoard){
    Move bestMove;
    int bestScore = -9999999;
    int score;
    for (Move legalMove : legalMoves){
        chessBoard.makeMove(legalMove);
        score = evaluateBoard(chessBoard, chessBoard.board[legalMove.toRow][legalMove.toCol].color);
        chessBoard.undoMove();
        if (score > bestScore){
            bestMove = legalMove;
            bestScore = score;
        }
        // for randomness
        else if (score == bestScore){
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<int> distrib(0, 10);

            int random_variable = distrib(gen);
            if (random_variable == 10){
                bestMove = legalMove;
                bestScore = score;
            }
        }
    }

    return bestMove;
   
}