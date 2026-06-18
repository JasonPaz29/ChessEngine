#include <raylib.h>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

enum PieceColor {
	WhitePiece, // = 0
	BlackPiece, // = 1
	NoColor
};

enum PieceType {
	King, // = 0
	Queen,
	Bishop,
	Knight,
	Rook,
	Pawn,
	Empty
};


struct Piece {
	PieceType type; // 0 = K, 1 = Q, 2 = B, 3 = N, 4 = R, 5 = P
	PieceColor color; // 0 = W, 1 = B
	Rectangle GetSourceRect(float pieceWidth, float pieceHeight) {
		return {
		type * pieceWidth,
		color * pieceHeight,
		pieceWidth,
		pieceHeight
		};
	}
	Rectangle GetDestRect(float squareSize, int col, int row){
		return {
		100 + squareSize * col,
		100 + squareSize * row,
		squareSize,
		squareSize
		};
	}
	bool selected;
};

struct Move {
	int fromRow;
	int fromCol;
	int toRow;
	int toCol;
};


struct ChessBoard {
	//Square size
	float size = 750.0 / 8.0;
	float x = 100.0f;
	float y = 100.0f;
	Piece board[8][8];	
	bool isFriendyPiece(int row, int col, PieceColor color);
	void DisplayBoard(){
		int color_turn = 0;
		Color color = {255, 255, 255, 255};
		for (int i = 0; i < 8; i++){
			for (int j = 0; j < 8; j++){
				if (color_turn == 0){
					color = {255, 255, 255, 255};
				}
				else if (color_turn == 1){
					color = {135, 206, 235, 255};
				}
				DrawRectangle(x + (j * size), y + (i * size), size, size, color);
				color_turn = 1 - color_turn;
			}
			color_turn = 1 - color_turn;	
		}
	}

	void NestBoard(){
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

	void DisplayPieces(Texture2D chessPieces){
		Vector2 origin = {0.0f, 0.0f};
		float pieceWidth = chessPieces.width / 6.0;
		float pieceHeight = chessPieces.height / 2.0;
		float scale = 0.5f;

		for (int row = 0; row < 8; row++){
			for (int col = 0; col < 8; col++){
				Piece piece = board[row][col];
				if (piece.type != Empty){
					DrawTexturePro(chessPieces, piece.GetSourceRect(pieceWidth, pieceHeight), piece.GetDestRect(size, col, row), origin, 0.0f, WHITE);
				}
			}
		}
	}

	void DisplayCheckHighlights(){
		for (int row = 0; row < 8; row++){
			for (int col = 0; col < 8; col++){
				Piece piece = board[row][col];
				if (piece.type == King && isKingInCheck(piece.color)){
					Vector2 center = {x + (col * size) + (size / 2), y + (row * size) + (size / 2)};
					DrawCircleGradient(center, size * 0.45f, {255, 0, 0, 180}, {255, 0, 0, 0});
				}
			}
		}
	}

	bool isFriendlyPiece(int row, int col, PieceColor color){
		if (!isInsideBoard(row, col)){
			return false;
		}
		if (board[row][col].color == color){
			return true;
		}
		return false;
	}

	bool isInsideBoard(int row, int col){
		return (row > -1 && row < 8 && col > -1 && col < 8);
	}

	bool isPathClear(Move move){
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

	bool isEnemyPiece(int row, int col, PieceColor color){
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

	bool isValidKnightMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		return ((rowDifference == 2 && colDifference == 1) || (rowDifference == 1 && colDifference == 2));
	}


	bool isValidPawnMove(Piece piece, Move move){
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
		else if (colDifference == 0 && rowDifference == direction * 2 && isPathClear(move)){
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
		return false;
	}

	bool isValidBishopMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		if (rowDifference != colDifference){
			return false;
		}

		return isPathClear(move);
	}

	bool isValidRookMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		if (rowDifference != 0 && colDifference != 0){
			return false;
		}

		return isPathClear(move);

	}

	bool isValidQueenMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		if (rowDifference != colDifference && rowDifference != 0 && colDifference != 0){
			return false;
		}

		return isPathClear(move);
	}

	bool isValidKingMove(Move move){
		int rowDifference = abs(move.toRow - move.fromRow);
		int colDifference = abs(move.toCol - move.fromCol);

		if (rowDifference > 1 || colDifference > 1){
			return false;
		}
		return (rowDifference == colDifference || rowDifference == 0 || colDifference == 0);
	}

	bool isAttackedByKnight(int targetRow, int targetCol, PieceColor color){
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

	bool isAttackedByPawn(int targetRow, int targetCol, PieceColor color){
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

	bool isAttackedByKing(int targetRow, int targetCol, PieceColor color){
		for (int rowOffset = -1; rowOffset < 2; rowOffset++){
			for (int colOffset = -1; colOffset < 2; colOffset++){
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

	bool isAttackedDiagonally(int targetRow, int targetCol, PieceColor color){
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
	
	bool isAttackedStraight(int targetRow, int targetCol, PieceColor color){
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
	
	bool isSquareAttacked(int row, int col, PieceColor color){
		if (!isInsideBoard(row, col)){
			return false;
		}

		return (isAttackedByKing(row, col, color) ||
				isAttackedByPawn(row, col, color) ||
				isAttackedByKnight(row, col, color)|| 
				isAttackedStraight(row, col, color) ||
				isAttackedDiagonally(row, col, color));
	}

	bool isKingInCheck(PieceColor color){
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

	void makeMoveUnchecked(Move move, float squareSize){
		if (move.fromRow >= 0 && move.fromRow < 8 && move.fromCol >= 0 && move.fromCol < 8){
			Piece temp = board[move.fromRow][move.fromCol];
			if (temp.type != Empty){
				temp.selected = true;
			}
	// 2. Updating Positions
			if (temp.selected) {
				// Stop dragging when mouse button is released
					if (move.toRow >= 0 && move.toRow < 8 && move.toCol >= 0 && move.toCol < 8) {
							board[move.toRow][move.toCol] = temp;
							board[move.fromRow][move.fromCol] = Piece{Empty, NoColor};
						}
						else {
							board[move.fromRow][move.fromCol] = temp;
						}
						
					}
					temp.selected = false;
			}
	}

	bool isPseudoLegal(Move move){
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
	


	bool isLegalMove(Move move, PieceColor color, float squareSize){
		if (!isPseudoLegal(move)){
			return false;
		}
		ChessBoard simulatedBoard = *this;
    	simulatedBoard.makeMoveUnchecked(move, squareSize);

    	return !simulatedBoard.isKingInCheck(color);
	}

	vector<Move> generateLegalMoves(PieceColor color){
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
						Move move = {fromRow, fromCol, toRow, toCol};
						if (isLegalMove(move, color, squareSize)){
							legalMoves.push_back(move);
						}
					}
				}
			}
		}
		return legalMoves;
	}

	bool isCheckMate(vector<Move> legalMoves, PieceColor kingColor){
		return isKingInCheck(kingColor) && legalMoves.empty();
	}

	bool isStaleMate(vector<Move> legalMoves, PieceColor kingColor){
		return !isKingInCheck(kingColor) && legalMoves.empty();
	}

	void isGameOver(){
		DrawText("It's checkmate...", 190, 200, 100, RED);
	}
};


int main(){
	ChessBoard chessBoard;	
	chessBoard.NestBoard();
	float squareSize = 750.0 / 8.0;
	Vector2 mouseOffset = {0.0f, 0.0f};
	Piece temp;
	PieceColor opposingColor;
	Move currentMove = {-1, -1, -1, -1};
	int turn = 0; // 0 = White, 1 = Black
	bool gameOver = false;
	
	InitWindow(1000, 1000, "Chess");
	SetTargetFPS(60);
	Texture2D chessPieces = LoadTexture("resources/Chess_Pieces_Sprite.svg.png");
	// Game Loop
	while (WindowShouldClose() == false)
	{	Vector2 mouse = GetMousePosition();
		// 1. Event handling
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
			currentMove.fromCol = (mouse.x - 100) / squareSize;
			currentMove.fromRow = (mouse.y - 100) / squareSize;	
		}

		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
			if (chessBoard.isInsideBoard(currentMove.fromRow, currentMove.fromCol)){
				currentMove.toRow = (mouse.y - 100) / squareSize;
				currentMove.toCol = (mouse.x - 100) / squareSize;
				temp = chessBoard.board[currentMove.fromRow][currentMove.fromCol];
				if ((temp.color == WhitePiece && turn == 0) || (temp.color == BlackPiece && turn == 1)){
					if (temp.color == WhitePiece){
						opposingColor = BlackPiece;
					}
					else{
					opposingColor = WhitePiece;
					}
				if (chessBoard.isLegalMove(currentMove, temp.color, squareSize) && chessBoard.isInsideBoard(currentMove.toRow, currentMove.toCol)){
					chessBoard.makeMoveUnchecked(currentMove, squareSize);
					if (chessBoard.isCheckMate(chessBoard.generateLegalMoves(opposingColor), opposingColor)){
						gameOver = true;
					}
					turn = 1 - turn;
				}
				else {
					continue;
				}
				}
				
			}
	}
		// 3. Drawing
		BeginDrawing();
		ClearBackground(BLACK);
		chessBoard.DisplayBoard();
		chessBoard.DisplayCheckHighlights();
		chessBoard.DisplayPieces(chessPieces);
		if (gameOver){
			chessBoard.isGameOver();
		}
		EndDrawing();
	}
	UnloadTexture(chessPieces);
	CloseWindow();
	return 0;
	}
