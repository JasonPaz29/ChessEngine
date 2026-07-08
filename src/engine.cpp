#include "engine.h"
#include "chess.h"
#include <random>
#include <vector>
#include <cmath>
#include <iostream>
#include <chrono>

Move Engine::randomMove(vector<Move> legalMoves){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distrib(0, legalMoves.size()-1);

    int random_variable = distrib(gen);

    return legalMoves[random_variable];
}

int Engine::evaluateBoard(ChessBoard& board, PieceColor engineColor, vector<Move>& legalMoves){
    int score = 0;
    
    score += evaluateMaterial(board);
    score += evaluatePieceSquareTables(board);
    score += evaluateRooks(board);
    score += evaluateKingSafety(board, legalMoves, engineColor);

    return score;


}

int Engine::evaluateMaterial(ChessBoard& board){
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
            if (piece.color == WhitePiece){
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

bool Engine::isEndgame(ChessBoard& board){
    int materialScore = 0;

    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            Piece piece = board.board[row][col];

            switch(piece.type) {
                case PieceType::Pawn:
                    materialScore += 100;
                    break;
                case PieceType::Knight:
                case PieceType::Bishop:
                    materialScore += 300;
                    break;
                case PieceType::Rook:
                    materialScore += 500;
                    break;
                case PieceType::Queen:
                    materialScore += 900;
                    break;
                default:
                    break;
            }
        }
    }

    return materialScore <= 2600;
}

int Engine::getPieceSquareValue(Piece piece, int row, int col, bool endgame){
    int tableRow = row;
    int score = 0;
    if (piece.color == BlackPiece){
        tableRow = 7 - row;
    }

    switch(piece.type){
         case PieceType::Pawn:
            return pawnTable[tableRow][col];
        case PieceType::Knight:
            return knightTable[tableRow][col];
        case PieceType::Bishop:
            return bishopTable[tableRow][col];
        case PieceType::Rook:
            return rookTable[tableRow][col];
        case PieceType::Queen:
            return queenTable[tableRow][col];
        case PieceType::King:
            if (endgame){
                return kingEndGameTable[tableRow][col];
            }
            return kingMiddleGameTable[tableRow][col];
        default:
            return 0;
    }
}

int Engine::evaluatePieceSquareTables(ChessBoard& board){
   int score = 0;
    int temp_score = 0;
    Piece piece;
    bool endgame = isEndgame(board);

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            piece = board.board[i][j];

            if(piece.type == Empty){
                continue;
            }

            if (piece.type != Empty){
                temp_score = getPieceSquareValue(piece, i, j, endgame);
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

int Engine::evaluateRooks(ChessBoard& board){
    int score = 0;
    int rook_count = 0; // if 4 (2 W and 2 B) rooks are found, break out the loop NOTE: this might break if there are more than 4 rooks due to promotion.
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            if (rook_count == 4){
                return score;
            }
            Piece piece = board.board[row][col];
            if (piece.type != Rook){
                continue;
            }
            else {
                rook_count += 1;
                //Go to the bottom of the row, and then using the color we determine if the file is open, Using IsPathClear
                if (piece.color == WhitePiece){
                    Move pathMove = {0, col, 7, col, Empty, false, false};
                    if (board.isPathClear(pathMove)){
                        score += 40;
                    }
                    // Semi open files only have one ENEMY pawn in them.
                    else {
                        int enemyPawnCount = 0;
                        for (int clearRow = 0; clearRow < 8; clearRow++){
                            Piece targetPiece = board.board[clearRow][col];
                            // No extra pieces in open file (Contesting Open files should be added later)
                            if (targetPiece.type == Queen || targetPiece.type == Bishop || targetPiece.type == Rook || targetPiece.type == Knight){
                                break;
                            }
                            else if (targetPiece.type == Pawn){
                                if (targetPiece.color == board.opposite(piece.color)){
                                    enemyPawnCount++;
                                    if (enemyPawnCount > 1){
                                        break;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                        }
                        if (enemyPawnCount == 1){
                            score += 25;
                        }
                    }
                }
                else {
                    Move pathMove = {0, col, 7, col, Empty, false, false};
                    if (board.isPathClear(pathMove)){
                        score -= 40;
                    }
                    // Semi open files only have one ENEMY pawn in them.
                    else {
                        int enemyPawnCount = 0;
                        for (int clearRow = 0; clearRow < 8; clearRow++){
                            Piece targetPiece = board.board[clearRow][col];
                            // No extra pieces in open file (Contesting Open files should be added later)
                            if (targetPiece.type == Queen || targetPiece.type == Bishop || targetPiece.type == Rook || targetPiece.type == Knight){
                                break;
                            }
                            else if (targetPiece.type == Pawn){
                                if (targetPiece.color == board.opposite(piece.color)){
                                    enemyPawnCount++;
                                    if (enemyPawnCount > 1){
                                        break;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                        }
                        if (enemyPawnCount == 1){
                            score -= 25;
                        }
                    }
                }
                }
            }
        }
    return score;
}

int Engine::evaluateKingSafety(ChessBoard& board, vector<Move>& legalMoves, PieceColor engineColor){
    int score = 0;
    //Incentivize castling
    for (Move legalMove: legalMoves){
        if (legalMove.isCastle){
        if (engineColor == WhitePiece){
            score += 40;
        }
        else {
            score -= 40;
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

int Engine::miniMax(vector<Move>& legalMoves, int depth, PieceColor maximizingColor, int alpha, int beta, ChessBoard& chessBoard){
    nodesSearched++;

    if (legalMoves.empty()){
        if(chessBoard.isKingInCheck(maximizingColor)){
            return maximizingColor == WhitePiece ? -100000000 + depth : 100000000 - depth;
        }
        else if(!chessBoard.isKingInCheck(maximizingColor)){
            return 0;
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
                chessBoard.makeMoveUnchecked(legalMove);
                chessBoard.turn = chessBoard.opposite(chessBoard.turn);
                vector<Move> nextMoves = chessBoard.generateLegalMoves(opposing);
                eval = miniMax(nextMoves, depth - 1, chessBoard.turn, alpha, beta, chessBoard);
                chessBoard.undoMove();
            maxEval = max(eval, maxEval);
            alpha = max(alpha, eval);
            if (beta <= alpha){
                cutoffs++;
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
                chessBoard.makeMoveUnchecked(legalMove);
                chessBoard.turn = chessBoard.opposite(chessBoard.turn);
                vector<Move> nextMoves = chessBoard.generateLegalMoves(opposing);
                eval = miniMax(nextMoves, depth - 1, chessBoard.turn, alpha, beta, chessBoard);
                chessBoard.undoMove();
            minEval = min(eval, minEval);
            beta = min(beta, eval);
            if (beta <= alpha){
                cutoffs++;
                break;
            }
        }
        return minEval;
    }
}

Move Engine::searchBestMoveAtDepth(vector<Move>& legalMoves, PieceColor engineColor, ChessBoard& chessBoard, int depth, int& bestScore){
    Move bestMove = {-1, -1, -1, -1, Empty, false, false};
    PieceColor opponent = chessBoard.opposite(engineColor);

    if (engineColor == WhitePiece){
        int highestScore = -9999999;

        for (Move legalMove: legalMoves){
            chessBoard.makeMoveUnchecked(legalMove);
            chessBoard.turn = chessBoard.opposite(chessBoard.turn);
            vector<Move> opponentMoves = chessBoard.generateLegalMoves(opponent);
            int score = miniMax(opponentMoves, depth, chessBoard.turn, -9999999, 9999999, chessBoard);
            chessBoard.undoMove();

            if (score > highestScore){
                bestMove = legalMove;
                highestScore = score;
            }
        }

        bestScore = highestScore;
    }
    else {
        int lowestScore = 9999999;

        for (Move legalMove: legalMoves){
            chessBoard.makeMoveUnchecked(legalMove);
            chessBoard.turn = chessBoard.opposite(chessBoard.turn);
            vector<Move> opponentMoves = chessBoard.generateLegalMoves(opponent);
            int score = miniMax(opponentMoves, depth, chessBoard.turn, -99999999, 999999999, chessBoard);
            chessBoard.undoMove();

            if (score < lowestScore){
                bestMove = legalMove;
                lowestScore = score;
            }
        }

        bestScore = lowestScore;
    }

    return bestMove;
}

Move Engine::bestMove(vector<Move>& legalMoves, PieceColor engineColor, ChessBoard& chessBoard){
    nodesSearched = 0;
    cutoffs = 0;
    auto searchStart = std::chrono::steady_clock::now();
    int bestScore = 0;
    Move chosenMove = searchBestMoveAtDepth(legalMoves, engineColor, chessBoard, 4, bestScore);
    auto searchEnd = std::chrono::steady_clock::now();
    double elapsedSeconds = std::chrono::duration<double>(searchEnd - searchStart).count();
    double nps = elapsedSeconds > 0.0 ? nodesSearched / elapsedSeconds : 0.0;

    std::cout << "Depth: 4"
              << " | Eval: " << bestScore
              << " | Nodes searched: " << nodesSearched
              << " | Search time: " << elapsedSeconds
              << "s | NPS: " << nps
              << " | Cutoffs: " << cutoffs << std::endl;
    return chosenMove;
}
