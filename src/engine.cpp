#include "engine.h"
#include "chess.h"
#include <random>
#include <vector>
#include <cmath>

Move Engine::randomMove(vector<Move> legalMoves){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distrib(0, legalMoves.size()-1);

    int random_variable = distrib(gen);

    return legalMoves[random_variable];
}

int Engine::evaluateBoard(ChessBoard& board, PieceColor engineColor, vector<Move> legalMoves){
    int score = 0;
    
    score += evaluateMaterial(board);
    score += evaluatePieceSquareTables(board);

    return score;


}

int Engine::evaluateMaterial(ChessBoard& board){
    int score = 0;
    int temp_score = 0;
    int true_score = 0;
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
            if (piece.color == WhitePiece){
                score += temp_score;
            }
            else {
                score -= temp_score;
            }
            true_score += temp_score;
        }
    }  
}   
if (true_score <= 2600){
    board.endgame = true;
}
return score;
}

int Engine::getPieceSquareValue(Piece piece, int row, int col, bool endgame){
    Engine engine;
    int tableRow = row;
    int score = 0;
    if (piece.color == BlackPiece){
        tableRow = 7 - row;
    }

    switch(piece.type){
         case PieceType::Pawn:
            return engine.pawnTable[tableRow][col];
        case PieceType::Knight:
            return engine.knightTable[tableRow][col];
        case PieceType::Bishop:
            return engine.bishopTable[tableRow][col];
        case PieceType::Rook:
            return engine.rookTable[tableRow][col];
        case PieceType::Queen:
            return engine.queenTable[tableRow][col];
        case PieceType::King:
            if (endgame){
                return engine.kingEndGameTable[tableRow][col];
            }
            return engine.kingMiddleGameTable[tableRow][col];
        default:
            return 0;
    }
}

int Engine::evaluatePieceSquareTables(ChessBoard& board){
    Engine engine;
   int score = 0;
    int temp_score = 0;
    Piece piece;

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            piece = board.board[i][j];

            if(piece.type == Empty){
                continue;
            }

            if (piece.type != Empty){
                temp_score = getPieceSquareValue(piece, i, j, board.endgame);
            }
            if (piece.color == WhitePiece){
                score += temp_score;
            }
            else{
                score -= temp_score;
            }
        }
    }
    return score;
}

int Engine::evaluateMobility(ChessBoard& board, vector<Move> legalMoves, PieceColor engineColor){
    PieceColor otherColor = board.opposite(engineColor);

    int sideMoveCount = legalMoves.size();
    int otherMoveCount = board.generateLegalMoves(otherColor).size();

    if (engineColor == WhitePiece){
        return (sideMoveCount - otherMoveCount) * 2;
    }
    else {
        return (otherMoveCount - sideMoveCount) * 2;
    }
}

// int Engine::evaluateRooks(ChessBoard& board){
//     int score = 0;
//     int rook_count = 0; // if 4 (2 W and 2 B) rooks are found, break out the loop NOTE: this might break if there are more than 4 rooks due to promotion.
//     for (int row = 0; row < 8; row++){
//         for (int col = 0; col < 8; col++){
//             if (rook_count == 4){
//                 return score;
//             }
//             Piece piece = board.board[row][col];
//             if (piece.type != Rook){
//                 continue;
//             }
//             else {
//                 //Check if the file is open
//                 if (isPathClear)
//             }
//         }
//     }
// }

Move Engine::greedyMove(vector<Move> legalMoves, ChessBoard& chessBoard){
    Move bestMove;
    int bestScore = -9999999;
    int score;
    for (Move legalMove : legalMoves){
        chessBoard.makeMove(legalMove);
        score = evaluateBoard(chessBoard, NoColor, legalMoves); // No Color is placeholder since we no longer use Greedy.
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

int Engine::miniMax(vector<Move> legalMoves, int depth, PieceColor maximizingColor, int alpha, int beta, ChessBoard& chessBoard){
    if (legalMoves.empty()){
        if(chessBoard.isKingInCheck(maximizingColor)){
            return maximizingColor == WhitePiece ? -100000000 : 100000000;
        }
    }
    if (legalMoves.size() == 0 or depth == 0){
        return evaluateBoard(chessBoard, maximizingColor, legalMoves);
    }

    if (maximizingColor == WhitePiece){
        PieceColor opposing = BlackPiece;
        int maxEval = -99999999;
        int eval = maxEval;
        for (Move legalMove: legalMoves){
            if (chessBoard.makeMove(legalMove)){
                vector<Move> nextMoves = chessBoard.generateLegalMoves(opposing);
                eval = miniMax(nextMoves, depth - 1, opposing, alpha, beta, chessBoard);
                chessBoard.undoMove();
            }
            maxEval = max(eval, maxEval);
            alpha = max(alpha, eval);
            if (beta <= alpha){
                break;
            }
        }
        return maxEval;
        
    }
    else{
        PieceColor opposing = WhitePiece;
        int minEval = 99999999;
        int eval = minEval;
        for (Move legalMove: legalMoves){
             if (chessBoard.makeMove(legalMove)){
                vector<Move> nextMoves = chessBoard.generateLegalMoves(opposing);
                eval = miniMax(nextMoves, depth - 1, opposing, alpha, beta, chessBoard);
                chessBoard.undoMove();
            }
            minEval = min(eval, minEval);
            beta = min(beta, eval);
            if (beta <= alpha){
                break;
            }
        }
        return minEval;
    }
}

Move Engine::bestMove(vector<Move> legalMoves, PieceColor engineColor, ChessBoard& chessBoard){
    Engine engine;
    // We want positive scores
    Move bestMove;
    PieceColor opponent = chessBoard.opposite(engineColor);
    if (engineColor == WhitePiece){
        int highestScore = -9999999;
        int score = highestScore;
        for (Move legalMove: legalMoves){
              if (chessBoard.makeMove(legalMove)) {
                vector<Move> opponentMoves = chessBoard.generateLegalMoves(opponent);
                score = miniMax(opponentMoves, 4, opponent, -9999999, 9999999, chessBoard);
                chessBoard.undoMove();
        }
        if (score > highestScore){
            bestMove = legalMove;
            highestScore = score;
        }
    }
}
    else {
        int lowestScore = 9999999;
        int score = lowestScore;
        for (Move legalMove: legalMoves){
              if (chessBoard.makeMove(legalMove)) {
                vector<Move> opponentMoves = chessBoard.generateLegalMoves(opponent);
                score = miniMax(opponentMoves, 4, opponent, -99999999, 999999999, chessBoard);
                chessBoard.undoMove();
            }
            if (score < lowestScore){
                bestMove = legalMove;
                lowestScore = score;
            }
        }
    }
    return bestMove;
}