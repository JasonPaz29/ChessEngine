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
		PieceColor attacker;
		for (int row = 0; row < 8; row++){
			for (int col = 0; col < 8; col++){
				Piece piece = board[row][col];
				if (piece.type == King && piece.color == color){
					if (piece.color == WhitePiece){
						attacker = BlackPiece;
					}
					else {
						attacker = WhitePiece;
					}
					return isSquareAttacked(row, col, attacker);
				}
			}
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
		UndoInfo oldMove = moveHistory.back();
		moveHistory.pop_back();
		//Undo everything
		if (oldMove.move.isCastle){
				// King side
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
		}
		whiteCanCastleKingSide = oldMove.whiteCanCastleKingSide;
		whiteCanCastleQueenSide = oldMove.whiteCanCastleQueenSide;
		blackCanCastleKingSide = oldMove.blackCanCastleKingSide;
		blackCanCastleQueenSide = oldMove.blackCanCastleQueenSide;
		hasEnPassantTarget = oldMove.hadEnPassantTarget;
		enPassantTargetCol = oldMove.enPassantTargetCol;
		enPassantTargetRow = oldMove.enPassantTargetRow;
		turn = oldMove.turn;
		if (isInsideBoard(oldMove.capturedRow, oldMove.capturedCol)) {
    		board[oldMove.capturedRow][oldMove.capturedCol] = oldMove.capturedPiece;
			if (oldMove.hadEnPassantTarget){
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
		int direction;
		UndoInfo undo;
		undo.move = move;
		undo.whiteCanCastleKingSide = whiteCanCastleKingSide;
		undo.whiteCanCastleQueenSide = whiteCanCastleQueenSide;
		undo.blackCanCastleKingSide = blackCanCastleKingSide;
		undo.blackCanCastleQueenSide = blackCanCastleQueenSide;
		undo.hadEnPassantTarget = hasEnPassantTarget;
		undo.enPassantTargetCol = enPassantTargetCol;
		undo.enPassantTargetRow = enPassantTargetRow;
		undo.turn = turn;
		if (!isInsideBoard(move.fromRow, move.fromCol) || !isInsideBoard(move.toRow, move.toCol)){
			return;
		}

		Piece movingPiece = board[move.fromRow][move.fromCol];
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
			undo.movedPiece = king;
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
				whiteCanCastleKingSide = false;
				whiteCanCastleQueenSide = false;
			}
			else {
				blackCanCastleKingSide = false;
				blackCanCastleQueenSide = false;
			}
		}
		
		else{
			Piece temp = movingPiece;
			Piece captured = isInsideBoard(move.toRow, move.toCol) ? board[move.toRow][move.toCol] : Piece{Empty, NoColor};
			undo.capturedPiece = board[move.toRow][move.toCol];
			if (undo.capturedPiece.type == Empty){
				undo.capturedCol = -1;
				undo.capturedRow = -1;
			}
			else{
			undo.capturedRow = move.toRow;
			undo.capturedCol = move.toCol;
			}
			if (temp.type != Empty){
				temp.selected = true;
			}
			if (temp.type == King){
				if (temp.color == WhitePiece){
					whiteCanCastleKingSide = false;
					whiteCanCastleQueenSide = false;
				}
				else{
					blackCanCastleKingSide = false;
					blackCanCastleQueenSide = false;
				}
			}
			else if (temp.type == Rook){
				if (temp.color == WhitePiece){
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
			else if (temp.type == Pawn){
				if (abs(move.toRow - move.fromRow) == 2){
					hasEnPassantTarget = true;
					enPassantTargetCol = move.fromCol;
					enPassantTargetRow = (move.toRow + move.fromRow) / 2;
				}
			}

			if (captured.type == Rook){
				if (captured.color == WhitePiece){
					if (move.toRow == 7 && move.toCol == 7){
						whiteCanCastleKingSide = false;
					}
					else if (move.toRow == 7 && move.toCol == 0){
						whiteCanCastleQueenSide = false;
					}
				}
				else if (captured.color == BlackPiece){
					if (move.toRow == 0 && move.toCol == 7){
						blackCanCastleKingSide = false;
					}
					else if (move.toRow == 0 && move.toCol == 0){
						blackCanCastleQueenSide = false;
					}
				}
			}

			else if (isEnPassantCapture){
				if (temp.color == WhitePiece){
					direction = -1;
				}
				else {
					direction = 1;
				}
				undo.capturedPiece = board[move.toRow-direction][move.toCol];
				undo.capturedCol = move.toCol;
				undo.capturedRow = move.toRow-direction;
				board[move.toRow-direction][move.toCol] = Piece{Empty, NoColor};
			}
				
	// 2. Updating Positions
			if (temp.selected) {
				// Stop dragging when mouse button is released
					if (move.toRow >= 0 && move.toRow < 8 && move.toCol >= 0 && move.toCol < 8) {
							board[move.toRow][move.toCol] = temp;
							undo.movedPiece = board[move.fromRow][move.fromCol];
							board[move.fromRow][move.fromCol] = Piece{Empty, NoColor};
							if (move.promotion != Empty && temp.type == Pawn){
								board[move.toRow][move.toCol].type = move.promotion;
							}
						}
						else {
							board[move.fromRow][move.fromCol] = temp;
						}
						
					}
					temp.selected = false;
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
		ChessBoard simulatedBoard = *this;
    	simulatedBoard.makeMoveUnchecked(move);

    	return !simulatedBoard.isKingInCheck(color);
	}

	vector<Move> ChessBoard::generateLegalMoves(PieceColor color){
		vector<Move> legalMoves;
		float squareSize = 750.0 / 8.0;
		for (int fromRow = 0; fromRow < 8; fromRow++){
			for (int fromCol = 0; fromCol < 8; fromCol++){
				Piece piece = board[fromRow][fromCol];

				if (piece.color != color){
					continue;
				}
				for (int toRow = 0; toRow < 8; toRow++){
					for (int toCol = 0; toCol < 8; toCol++){
						Move baseMove = {fromRow, fromCol, toRow, toCol, Empty, false, false};
						Move move = {fromRow, fromCol, toRow, toCol, Empty, isCastleMove(baseMove), isEnPassant(baseMove)};
						if (isLegalMove(move, color)){
							if (isPromotionMove(baseMove)){
								addPromotionMoves(legalMoves, baseMove);
							}
							else {
								legalMoves.push_back(move);
							}
						}
					}
				}
			}
		}
		return legalMoves;
	}

	bool ChessBoard::isCheckMate(vector<Move> legalMoves, PieceColor kingColor){
		return isKingInCheck(kingColor) && legalMoves.empty();
	}

	bool ChessBoard::isStaleMate(vector<Move> legalMoves, PieceColor kingColor){
		return !isKingInCheck(kingColor) && legalMoves.empty();
	}