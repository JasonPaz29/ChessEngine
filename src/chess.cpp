#include "chess.h"


PieceColor ChessBoard::opposite(PieceColor color){
		if (color == WhitePiece){
			return BlackPiece;
		}
		return WhitePiece;
	}
	void ChessBoard::NestBoard(){
		board[0][0] = Piece{Rook, BlackPiece};
		board[0][1] = Piece{Knight, BlackPiece};
		board[0][2] = Piece{Bishop, BlackPiece};
		board[0][3] = Piece{Queen, BlackPiece};
		board[0][4] = Piece{King, BlackPiece};
		board[0][5] = Piece{Bishop, BlackPiece};
		board[0][6] = Piece{Knight, BlackPiece};
		board[0][7] = Piece{Rook, BlackPiece};
		for (int i = 0; i < 8; i++){
			board[1][i] = Piece{Pawn, BlackPiece};
			board[2][i] = Piece{Empty, NoColor};
			board[3][i] = Piece{Empty, NoColor};
			board[4][i] = Piece{Empty, NoColor};
			board[5][i] = Piece{Empty, NoColor};
			board[6][i] = Piece{Pawn, WhitePiece};
		}
		board[7][0] = Piece{Rook, WhitePiece};
		board[7][1] = Piece{Knight, WhitePiece};
		board[7][2] = Piece{Bishop, WhitePiece};
		board[7][3] = Piece{Queen, WhitePiece};
			board[7][4] = Piece{King, WhitePiece};
			board[7][5] = Piece{Bishop, WhitePiece};
			board[7][6] = Piece{Knight, WhitePiece};
			board[7][7] = Piece{Rook, WhitePiece};
			whiteKingRow = 7;
			whiteKingCol = 4;
			blackKingRow = 0;
			blackKingCol = 4;
		}

	bool ChessBoard::isFriendlyPiece(int row, int col, PieceColor color){
		if (!isInsideBoard(row, col)){
			return false;
		}
		if (board[row][col].color == color){
			return true;
		}
		return false;
	}

	bool ChessBoard::isInsideBoard(int row, int col){
		return (row > -1 && row < 8 && col > -1 && col < 8);
	}

	bool ChessBoard::isPathClear(Move move){
		if (!isInsideBoard(move.fromRow, move.fromCol) || !isInsideBoard(move.toRow, move.toCol)){
			return false;
		}

		int rowStep = (move.toRow > move.fromRow) - (move.toRow < move.fromRow);
		int colStep = (move.toCol > move.fromCol) - (move.toCol < move.fromCol);

		int currentRow = move.fromRow + rowStep;
		int currentCol = move.fromCol + colStep;

		while (currentRow != move.toRow || currentCol != move.toCol){
			if (!isInsideBoard(currentRow, currentCol)){
				return false;
			}
			if (board[currentRow][currentCol].type != Empty){
				return false;
			}
			currentRow += rowStep;
			currentCol += colStep;
		}
		return true;
	}

	bool ChessBoard::isEnemyPiece(int row, int col, PieceColor color){
		if (!isInsideBoard(row, col)){
			return false;
		}
		if (board[row][col].color == color){
			return false;
		}

		if (board[row][col].type == Empty){
			return false;
		}

		return true;
	}

	bool ChessBoard::isValidKnightMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		return ((rowDifference == 2 && colDifference == 1) || (rowDifference == 1 && colDifference == 2));
	}

    void ChessBoard::generateKnightMoves(int fromRow, int fromCol, PieceColor color, vector<Move>& legalMoves){
        int offsets[8][2] = {
        {-2, -1}, {-2, 1},
        {-1, -2}, {-1, 2},
        {1, -2}, {1, 2},
        {2, -1}, {2, 1}
        };

	        for (auto& offset: offsets){
	            int offsetRow = offset[0];
	            int offsetCol = offset[1];
	            Move move = {fromRow, fromCol, fromRow + offsetRow, fromCol + offsetCol, Empty, false, false};
				if (!isInsideBoard(move.toRow, move.toCol)){
					continue;
				}
				Piece targetPiece = board[move.toRow][move.toCol];
				if (targetPiece.color == color || targetPiece.type == King){
					continue;
				}
				if (isValidKnightMove(move)){
					legalMoves.push_back(move);
				}
	        }
	    }

    void ChessBoard::generatePawnMoves(int fromRow, int fromCol, PieceColor color, vector<Move>& legalMoves){
        int direction;

        if (color == WhitePiece){
            direction = -1;
        }
        else {
            direction = 1;
        }

         int pawnRow = fromRow + direction;

    int pawnCols[2] = {
        fromCol + 1,
        fromCol - 1
    };

    for (int i = 0; i < 2; i++){
        int pawnCol = pawnCols[i];

        if (!isInsideBoard(pawnRow, pawnCol)){
            continue;
        }

	        Move move = {fromRow, fromCol, pawnRow, pawnCol, Empty, false, isEnPassant({fromRow, fromCol, pawnRow, pawnCol, Empty, false, false})};
			if (board[pawnRow][pawnCol].type == King){
				continue;
			}
	        if (isPromotionMove(move) && isValidPawnMove(board[fromRow][fromCol], move)){
	            addPromotionMoves(legalMoves, move);
	        }
	        else if (isValidPawnMove(board[fromRow][fromCol], move)){
            legalMoves.push_back(move);
        }
    }
    //Forward one space and 2 spaces
    Move move = {fromRow, fromCol, fromRow + direction, fromCol, Empty, false, false};
    Move otherMove = {fromRow, fromCol, fromRow + direction * 2, fromCol, Empty, false, false};
    if (isPromotionMove(move) && isValidPawnMove(board[fromRow][fromCol], move)){
        addPromotionMoves(legalMoves, move);
    }
		else if (isValidPawnMove(board[fromRow][fromCol], move)){
			legalMoves.push_back(move);
		}
		if (isValidPawnMove(board[fromRow][fromCol], otherMove)){
			legalMoves.push_back(otherMove);
		}

	    }

    void ChessBoard::generateKingMoves(int fromRow, int fromCol, PieceColor color, vector<Move>& legalMoves){
        for (int rowOffset = -1; rowOffset < 2; rowOffset++){
			for (int colOffset = -1; colOffset < 2; colOffset++){
				if (rowOffset == 0 && colOffset == 0){
					continue;
				}

				int row = fromRow + rowOffset;
				int col = fromCol + colOffset;

				if (!isInsideBoard(row, col)){
					continue;
				}

					Move move = {fromRow, fromCol, row, col, Empty, false, false};
					if (isValidKingMove(move) && !isFriendlyPiece(row, col, color)){
						if (board[row][col].type != King){
							legalMoves.push_back(move);
						}
					}
	                
				}
		}
        int kingCols[2] = {
        fromCol + 2,
        fromCol - 2
    };
    for (int i = 0; i < 2; i++){
        int kingCol = kingCols[i];

        Move move = {fromRow, fromCol, fromRow, kingCol, Empty, isCastleMove({fromRow, fromCol, fromRow, kingCol, Empty, false, false}), false};
		if (isValidCastle(move)){
        legalMoves.push_back(move);
		}
        
    }
    
    }
    void ChessBoard::generateSlidingMoves(int fromRow, int fromCol, PieceColor color, vector<Move>& legalMoves){
        Piece piece = board[fromRow][fromCol];
        // Do Diagnols first
        int diagnolDirections[4][2]{
			{-1, 1}, {1, 1},
			{-1, -1}, {1, -1}
		};
        if (piece.type != Rook){
			for (auto& direction : diagnolDirections){
				int row = fromRow + direction[0];
				int col = fromCol + direction[1];

				while (isInsideBoard(row, col)){
					Move move = {fromRow, fromCol, row, col, Empty, false, false};
                    Piece targetPiece = board[row][col];
				if (targetPiece.color == color){
					break;
				}
					else if (targetPiece.color == opposite(color)){
						if (targetPiece.type != King){
							legalMoves.push_back(move);
							break;
						}
					else {
						break;
					}
				}
				else if (targetPiece.color == NoColor){
				legalMoves.push_back(move);
				}
                else {
					break;
				}
				row += direction[0];
				col += direction[1];
				}
            
            }
			if (piece.type == Bishop){
                return;
        }
			
    }
        //Straight Path
        int straightDirections[4][2]{
			{-1, 0}, {0, 1},
			{0, -1}, {1, 0}
		};
		for (auto& direction : straightDirections){
			int row = fromRow + direction[0];
			int col = fromCol + direction[1];

			while (isInsideBoard(row, col)){
				Move move = {fromRow, fromCol, row, col, Empty, false, false};
				Piece targetPiece = board[row][col];
				if (targetPiece.color == color){
					break;
				}
					else if (targetPiece.color == opposite(color)){
						if (targetPiece.type != King){
							legalMoves.push_back(move);
							break;
						}
					else {
						break;
					}
				}
				else if (targetPiece.color == NoColor){	
				legalMoves.push_back(move);
				}
                else {
					break;
				}
			row += direction[0];
			col += direction[1];
			}
			}
			
			}

	

	bool ChessBoard::isValidPawnMove(Piece piece, Move move){
		if (!isInsideBoard(move.toRow, move.toCol)){
			return false;
		}

		int direction;
		if (piece.color == WhitePiece){
			direction = -1;
		}
		else {
			direction = 1;
		}

		int rowDifference = move.toRow - move.fromRow;
		int colDifference = abs(move.toCol - move.fromCol);

		if (colDifference == 0 && rowDifference == direction && board[move.toRow][move.toCol].type == Empty){
			return true;
		}
		else if (colDifference == 0 && rowDifference == direction * 2 && board[move.toRow][move.toCol].type == Empty && isPathClear(move)){
			if (rowDifference < 0 && move.fromRow == 6){
				return true;
			}
			else if (rowDifference > 0 && move.fromRow == 1){
				return true;
			}
		}
		else if (colDifference == 1 && rowDifference == direction && isEnemyPiece(move.toRow, move.toCol, piece.color)){
			return true;
		}
		else if (colDifference == 1 && rowDifference == direction && (move.toRow == enPassantTargetRow && move.toCol == enPassantTargetCol) && hasEnPassantTarget && board[move.toRow][move.toCol].type == Empty){
			return true;
		}
		return false;
	}

	bool ChessBoard::isEnPassant(Move move){
		if (!hasEnPassantTarget){
			return false;
		}

		return (abs(move.fromCol - move.toCol) == 1 && (move.toRow == enPassantTargetRow) && (move.toCol == enPassantTargetCol));
	}

	bool ChessBoard::isValidBishopMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		if (rowDifference != colDifference){
			return false;
		}

		return isPathClear(move);
	}

	bool ChessBoard::isValidRookMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		if (rowDifference != 0 && colDifference != 0){
			return false;
		}

		return isPathClear(move);

	}

	bool ChessBoard::isValidQueenMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		if (rowDifference != colDifference && rowDifference != 0 && colDifference != 0){
			return false;
		}

		return isPathClear(move);
	}
	bool ChessBoard::isCastleMove(Move move){
		if (!isInsideBoard(move.fromRow, move.fromCol) || !isInsideBoard(move.toRow, move.toCol)){
			return false;
		}

		Piece piece = board[move.fromRow][move.fromCol];

		return piece.type == King &&
           move.fromRow == move.toRow &&
           abs(move.toCol - move.fromCol) == 2;
}

	bool ChessBoard::isValidCastle(Move move){
		if (!isCastleMove(move)){
			return false;
		}

		Piece king = board[move.fromRow][move.fromCol];
		PieceColor kingColor = king.color;
		PieceColor attacker = (kingColor == WhitePiece) ? BlackPiece : WhitePiece;
		int homeRow = (kingColor == WhitePiece) ? 7 : 0;
		bool kingSide = move.toCol > move.fromCol;
		int rookCol = kingSide ? 7 : 0;

		if (move.fromRow != homeRow || move.fromCol != 4 || move.toRow != homeRow){
			return false;
		}

		if (kingSide && move.toCol != 6){
			return false;
		}

		if (!kingSide && move.toCol != 2){
			return false;
		}

		if (kingSide){
			if (kingColor == WhitePiece && !whiteCanCastleKingSide){
				return false;
			}
			if (kingColor == BlackPiece && !blackCanCastleKingSide){
				return false;
			}
		}
		else {
			if (kingColor == WhitePiece && !whiteCanCastleQueenSide){
				return false;
			}
			if (kingColor == BlackPiece && !blackCanCastleQueenSide){
				return false;
			}
		}

		Piece rook = board[homeRow][rookCol];
		if (rook.type != Rook || rook.color != kingColor){
			return false;
		}

		if (kingSide){
			if (board[homeRow][5].type != Empty || board[homeRow][6].type != Empty){
				return false;
			}
			if (isSquareAttacked(homeRow, 4, attacker) ||
				isSquareAttacked(homeRow, 5, attacker) ||
				isSquareAttacked(homeRow, 6, attacker)){
				return false;
			}
		}
		else {
			if (board[homeRow][1].type != Empty ||
				board[homeRow][2].type != Empty ||
				board[homeRow][3].type != Empty){
				return false;
			}
			if (isSquareAttacked(homeRow, 4, attacker) ||
				isSquareAttacked(homeRow, 3, attacker) ||
				isSquareAttacked(homeRow, 2, attacker)){
				return false;
			}
		}

		return true;
}

	bool ChessBoard::isValidKingMove(Move move){
		if (isCastleMove(move)){
			return isValidCastle(move);
		}
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		if (rowDifference > 1 || colDifference > 1){
			return false;
		}
		return (rowDifference == colDifference || rowDifference == 0 || colDifference == 0);
	}

	bool ChessBoard::isAttackedByKnight(int targetRow, int targetCol, PieceColor color){
		int offsets[8][2] = {
        {-2, -1}, {-2, 1},
        {-1, -2}, {-1, 2},
        {1, -2},  {1, 2},
        {2, -1},  {2, 1}
		};

		for (auto& offset : offsets){
			int row = targetRow + offset[0];
			int col = targetCol + offset[1];

		if (!isInsideBoard(row, col)){
			continue;
		}
		
		Piece piece = board[row][col];
		
		if (piece.color == color && piece.type == Knight){
			return true;
		}
	}
		return false;
    }

	bool ChessBoard::isAttackedByPawn(int targetRow, int targetCol, PieceColor color){
    int direction;

    if (color == WhitePiece){
        direction = -1;
    }
    else {
        direction = 1;
    }

    int pawnRow = targetRow - direction;

    int pawnCols[2] = {
        targetCol - 1,
        targetCol + 1
    };

    for (int i = 0; i < 2; i++){
        int pawnCol = pawnCols[i];

        if (!isInsideBoard(pawnRow, pawnCol)){
            continue;
        }

        Piece piece = board[pawnRow][pawnCol];

        if (piece.type == Pawn && piece.color == color){
            return true;
        }
    }

    return false;
}

	bool ChessBoard::isAttackedByKing(int targetRow, int targetCol, PieceColor color){
		for (int rowOffset = -1; rowOffset < 2; rowOffset++){
			for (int colOffset = -1; colOffset < 2; colOffset++){
				if (rowOffset == 0 && colOffset == 0){
					continue;
				}

				int row = targetRow + rowOffset;
				int col = targetCol + colOffset;

				if (!isInsideBoard(row, col)){
					continue;
				}

				Piece piece = board[row][col];

				if (piece.color == color && piece.type == King){
					return true;
				}

			}
		}
		return false;
	}

	bool ChessBoard::isAttackedDiagonally(int targetRow, int targetCol, PieceColor color){
		int directions[4][2]{
			{-1, 1}, {1, 1},
			{-1, -1}, {1, -1}
		};
			for (auto& direction : directions){
				int row = targetRow + direction[0];
				int col = targetCol + direction[1];

				while (isInsideBoard(row, col)){
					Piece piece = board[row][col];

					if (piece.type != Empty){
					if (piece.color == color && (piece.type == Bishop || piece.type == Queen)){
						return true;
					}
					break;
					
				}
				row += direction[0];
				col += direction[1];
				}
			
			}
			return false;
		}
	
	bool ChessBoard::isAttackedStraight(int targetRow, int targetCol, PieceColor color){
		int directions[4][2]{
			{-1, 0}, {0, 1},
			{0, -1}, {1, 0}
		};
		for (auto& direction : directions){
			int row = targetRow + direction[0];
			int col = targetCol + direction[1];

			while (isInsideBoard(row, col)){
				Piece piece = board[row][col];

				if (piece.type != Empty){
				if (piece.color == color && (piece.type == Rook || piece.type == Queen)){
					return true;
				}
				break;
				
			}
			row += direction[0];
			col += direction[1];
			}
			
			}
		return false;

	}
	
	bool ChessBoard::isSquareAttacked(int row, int col, PieceColor color){
		if (!isInsideBoard(row, col)){
			return false;
		}

		return (isAttackedByKing(row, col, color) ||
				isAttackedByPawn(row, col, color) ||
				isAttackedByKnight(row, col, color)|| 
				isAttackedStraight(row, col, color) ||
				isAttackedDiagonally(row, col, color));
	}

		bool ChessBoard::isKingInCheck(PieceColor color){
			if (color == WhitePiece){
				return isSquareAttacked(whiteKingRow, whiteKingCol, BlackPiece);
			}
			else if (color == BlackPiece){
				return isSquareAttacked(blackKingRow, blackKingCol, WhitePiece);
			}
			return false;
		}

	void ChessBoard::PromotePawn(int row, int col, PieceType promotedType){
		Piece pawn = board[row][col];

		if (pawn.type != Pawn){
			return;
		}

		if (promotedType != Queen &&
			promotedType != Rook &&
			promotedType != Knight &&
			promotedType != Bishop){
				return;
			}
		board[row][col] = Piece{promotedType, pawn.color};
	}

	bool ChessBoard::isPromotionMove(Move move){
		if (!isInsideBoard(move.fromRow, move.fromCol) ||
			!isInsideBoard(move.toRow, move.toCol)) {
			return false;
}
		Piece piece = board[move.fromRow][move.fromCol];

		if (piece.type != Pawn){
			return false;
		}
		if (piece.color == WhitePiece){
			if (move.toRow != 0){
				return false;
			}
		}
		else {
			if(move.toRow != 7){
				return false;
			}
		}
		return true;

	}

	void ChessBoard::undoMove(){
        if (moveHistory.empty()){
            return;
        }
		UndoInfo oldMove = moveHistory.back();
		moveHistory.pop_back();

		whiteCanCastleKingSide = oldMove.whiteCanCastleKingSide;
		whiteCanCastleQueenSide = oldMove.whiteCanCastleQueenSide;
		blackCanCastleKingSide = oldMove.blackCanCastleKingSide;
		blackCanCastleQueenSide = oldMove.blackCanCastleQueenSide;
			hasEnPassantTarget = oldMove.hadEnPassantTarget;
			enPassantTargetCol = oldMove.enPassantTargetCol;
			enPassantTargetRow = oldMove.enPassantTargetRow;
			whiteKingRow = oldMove.whiteKingRow;
			whiteKingCol = oldMove.whiteKingCol;
			blackKingRow = oldMove.blackKingRow;
			blackKingCol = oldMove.blackKingCol;
			turn = oldMove.turn;

		if (oldMove.move.isCastle){
			if (oldMove.move.toCol - oldMove.move.fromCol > 0){
				board[oldMove.move.fromRow][oldMove.move.fromCol] = oldMove.movedPiece;
				board[oldMove.move.fromRow][7] = Piece{Rook, oldMove.movedPiece.color};
				board[oldMove.move.toRow][oldMove.move.toCol] = Piece{Empty, NoColor};
				board[oldMove.move.toRow][5] = Piece{Empty, NoColor};
			}
			else{
				board[oldMove.move.fromRow][oldMove.move.fromCol] = oldMove.movedPiece;
				board[oldMove.move.fromRow][0] = Piece{Rook, oldMove.movedPiece.color};
				board[oldMove.move.toRow][oldMove.move.toCol] = Piece{Empty, NoColor};
				board[oldMove.move.toRow][3] = Piece{Empty, NoColor};
			}
			return;
		}

		if (isInsideBoard(oldMove.capturedRow, oldMove.capturedCol)) {
    		board[oldMove.capturedRow][oldMove.capturedCol] = oldMove.capturedPiece;
			if (oldMove.move.isEnPassant){
				board[oldMove.move.toRow][oldMove.move.toCol] = Piece{Empty, NoColor};
			}
		}
		else {
			board[oldMove.move.toRow][oldMove.move.toCol] = Piece{Empty, NoColor};
		} 
		board[oldMove.move.fromRow][oldMove.move.fromCol] = oldMove.movedPiece;

		
	}

	bool ChessBoard::makeMove(Move move){
		if (!isLegalMove(move, turn)){
			return false;
		}

		makeMoveUnchecked(move);
		turn = opposite(turn);

		return true;
	}

		void ChessBoard::makeMoveUnchecked(Move move){
			UndoInfo undo;
			undo.move = move;
			undo.whiteCanCastleKingSide = whiteCanCastleKingSide;
			undo.whiteCanCastleQueenSide = whiteCanCastleQueenSide;
			undo.blackCanCastleKingSide = blackCanCastleKingSide;
		undo.blackCanCastleQueenSide = blackCanCastleQueenSide;
			undo.hadEnPassantTarget = hasEnPassantTarget;
			undo.enPassantTargetCol = enPassantTargetCol;
			undo.enPassantTargetRow = enPassantTargetRow;
			undo.whiteKingRow = whiteKingRow;
			undo.whiteKingCol = whiteKingCol;
			undo.blackKingRow = blackKingRow;
			undo.blackKingCol = blackKingCol;
			undo.turn = turn;
			if (!isInsideBoard(move.fromRow, move.fromCol) || !isInsideBoard(move.toRow, move.toCol)){
				return;
			}

			Piece movingPiece = board[move.fromRow][move.fromCol];
			Piece capturedPiece = board[move.toRow][move.toCol];
			undo.movedPiece = movingPiece;
			undo.capturedPiece = capturedPiece;
			undo.capturedRow = capturedPiece.type == Empty ? -1 : move.toRow;
			undo.capturedCol = capturedPiece.type == Empty ? -1 : move.toCol;

			bool isEnPassantCapture = movingPiece.type == Pawn &&
									  hasEnPassantTarget &&
									  move.toRow == enPassantTargetRow &&
									  move.toCol == enPassantTargetCol &&
									  board[move.toRow][move.toCol].type == Empty &&
									  abs(move.toCol - move.fromCol) == 1;

			hasEnPassantTarget = false;
			enPassantTargetRow = -1;
			enPassantTargetCol = -1;

			if (isCastleMove(move)){
				undo.move.isCastle = true;
				Piece king = board[move.fromRow][move.fromCol];
				undo.capturedPiece = Piece{Empty, NoColor};
				undo.capturedCol = -1;
				undo.capturedRow = -1;
				int rookFromCol = (move.toCol > move.fromCol) ? 7 : 0;
				int rookToCol = (move.toCol > move.fromCol) ? move.toCol - 1 : move.toCol + 1;
			Piece rook = board[move.fromRow][rookFromCol];

				board[move.toRow][move.toCol] = king;
				board[move.fromRow][move.fromCol] = Piece{Empty, NoColor};
				board[move.fromRow][rookToCol] = rook;
				board[move.fromRow][rookFromCol] = Piece{Empty, NoColor};

				if (king.color == WhitePiece){
					whiteKingRow = move.toRow;
					whiteKingCol = move.toCol;
					whiteCanCastleKingSide = false;
					whiteCanCastleQueenSide = false;
				}
				else {
					blackKingRow = move.toRow;
					blackKingCol = move.toCol;
					blackCanCastleKingSide = false;
					blackCanCastleQueenSide = false;
					}
			}
			
			else{
					if (movingPiece.type == King){
						if (movingPiece.color == WhitePiece){
							whiteKingRow = move.toRow;
							whiteKingCol = move.toCol;
							whiteCanCastleKingSide = false;
							whiteCanCastleQueenSide = false;
					}
					else{
						blackKingRow = move.toRow;
						blackKingCol = move.toCol;
						blackCanCastleKingSide = false;
							blackCanCastleQueenSide = false;
						}
				}
				else if (movingPiece.type == Rook){
					if (movingPiece.color == WhitePiece){
						if (move.fromRow == 7 && move.fromCol == 7){
							whiteCanCastleKingSide = false;
						}
						else if (move.fromRow == 7 && move.fromCol == 0){
						whiteCanCastleQueenSide = false;
					}
				}
				else {
					if (move.fromRow == 0 && move.fromCol == 7){
						blackCanCastleKingSide = false;
					}
					else if (move.fromRow == 0 && move.fromCol == 0){
							blackCanCastleQueenSide = false;
						}
					}
					}
				else if (movingPiece.type == Pawn){
					if (abs(move.toRow - move.fromRow) == 2){
						hasEnPassantTarget = true;
						enPassantTargetCol = move.fromCol;
						enPassantTargetRow = (move.toRow + move.fromRow) / 2;
					}
				}

				if (capturedPiece.type == Rook){
					if (capturedPiece.color == WhitePiece){
						if (move.toRow == 7 && move.toCol == 7){
							whiteCanCastleKingSide = false;
						}
						else if (move.toRow == 7 && move.toCol == 0){
							whiteCanCastleQueenSide = false;
						}
					}
					else if (capturedPiece.color == BlackPiece){
						if (move.toRow == 0 && move.toCol == 7){
							blackCanCastleKingSide = false;
						}
						else if (move.toRow == 0 && move.toCol == 0){
						blackCanCastleQueenSide = false;
					}
				}
				}

				else if (isEnPassantCapture){
					int direction = movingPiece.color == WhitePiece ? -1 : 1;
					undo.move.isEnPassant = true;
					undo.capturedPiece = board[move.toRow-direction][move.toCol];
					undo.capturedCol = move.toCol;
					undo.capturedRow = move.toRow-direction;
					board[move.toRow-direction][move.toCol] = Piece{Empty, NoColor};
				}

				board[move.toRow][move.toCol] = movingPiece;
				board[move.fromRow][move.fromCol] = Piece{Empty, NoColor};
				if (move.promotion != Empty && movingPiece.type == Pawn){
					board[move.toRow][move.toCol].type = move.promotion;
				}
			}
				moveHistory.push_back(undo);
		}

	bool ChessBoard::isPseudoLegal(Move move){
		if (!isInsideBoard(move.fromRow, move.fromCol) || !isInsideBoard(move.toRow, move.toCol)){
			return false;
		}

		Piece piece = board[move.fromRow][move.fromCol];
		if (isFriendlyPiece(move.toRow, move.toCol, piece.color)){
			return false;
		}
		else if (piece.type == Empty){
			return false;
		}
		else if (board[move.toRow][move.toCol].type == King){
			return false;
		}
		switch (piece.type){
			case PieceType::Pawn:
				return isValidPawnMove(piece, move);
			case PieceType::Knight:
				return isValidKnightMove(move);
			case PieceType::Bishop:
				return isValidBishopMove(move);
			case PieceType::Rook:
				return isValidRookMove(move);
			case PieceType::Queen:
				return isValidQueenMove(move);
			case PieceType::King:
				return isValidKingMove(move);
			default:
				return false;
		}
	}
	
	void ChessBoard::addPromotionMoves(vector<Move>& legalMoves, Move baseMove){
		PieceType promotionTypes[4] = {Queen, Rook, Bishop, Knight};

		for (int i = 0; i < 4; i++){
			Move promotionMove = baseMove;
			promotionMove.promotion = promotionTypes[i];
			legalMoves.push_back(promotionMove);
		}
	}

	bool ChessBoard::isLegalMove(Move move, PieceColor color){
		if (!isPseudoLegal(move)){
			return false;
		}
		makeMoveUnchecked(move);
		bool legal = !isKingInCheck(color);
		undoMove();
    	return legal;
	}

	vector<Move> ChessBoard::generatePseudoLegalMoves(PieceColor color){
		vector<Move> pseudoLegalMoves;
		for (int fromRow = 0; fromRow < 8; fromRow++){
			for (int fromCol = 0; fromCol < 8; fromCol++){
				Piece piece = board[fromRow][fromCol];
                if (piece.color != color){
                    continue;
                }
                else {
                    switch (piece.type){
                        case PieceType::Pawn:
                            generatePawnMoves(fromRow, fromCol, color, pseudoLegalMoves);
                            break;
                        case PieceType::Knight:
                            generateKnightMoves(fromRow, fromCol, color, pseudoLegalMoves);
                            break;
                        case PieceType::Bishop:
                            generateSlidingMoves(fromRow, fromCol, color, pseudoLegalMoves);
                            break;
                        case PieceType::Rook:
                            generateSlidingMoves(fromRow, fromCol, color, pseudoLegalMoves);
                            break;
                        case PieceType::Queen:
                            generateSlidingMoves(fromRow, fromCol, color, pseudoLegalMoves);
                            break;
                        case PieceType::King:
                            generateKingMoves(fromRow, fromCol, color, pseudoLegalMoves);
                            break;
                        default:
                            continue;
                    }
                }
			}
		}
		return pseudoLegalMoves;
	}

	vector<Move> ChessBoard::generateLegalMoves(PieceColor color){
		vector<Move> pseudoLegalMoves = generatePseudoLegalMoves(color);
		vector<Move> legalMoves;
		legalMoves.reserve(pseudoLegalMoves.size());

		for (Move move: pseudoLegalMoves){
			makeMoveUnchecked(move);
			if (!isKingInCheck(color)){
				legalMoves.push_back(move);
			}
			undoMove();
		}
		return legalMoves;
	}

	bool ChessBoard::isCheckMate(vector<Move> legalMoves, PieceColor kingColor){
		return isKingInCheck(kingColor) && legalMoves.empty();
	}

	bool ChessBoard::isStaleMate(vector<Move> legalMoves, PieceColor kingColor){
		return !isKingInCheck(kingColor) && legalMoves.empty();
	}
