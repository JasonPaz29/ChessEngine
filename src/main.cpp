#include <raylib.h>
#include <iostream>
#include <cmath>
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


struct ChessBoard {
	//Square size
	float size = 750.0 / 8.0;
	float x = 100.0f;
	float y = 100.0f;
	Piece board[8][8];
	bool isFriendyPiece(int newRow, int newCol, PieceColor color);
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

	void DisplayPieces(){
		Vector2 origin = {0.0f, 0.0f};
		Texture2D chessPieces = LoadTexture("resources/Chess_Pieces_Sprite.svg.png");	
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

	bool isFriendlyPiece(int newRow, int newCol, PieceColor color){
		if (board[newRow][newCol].color == color){
			return true;
		}
		return false;
	}

	bool isInsideBoard(int row, int col){
		return (row > -1 && row < 8 && col > -1 && col < 8);
	}

	bool isPathClear(int oldRow, int oldCol, int newRow, int newCol){
		int rowStep = (newRow > oldRow) - (newRow < oldRow);
		int colStep = (newCol > oldCol) - (newCol < oldCol);

		int currentRow = oldRow + rowStep;
		int currentCol = oldCol + colStep;

		while (currentRow != newRow || currentCol != newCol){
			if (board[currentRow][currentCol].type != Empty){
				return false;
			}
			currentRow += rowStep;
			currentCol += colStep;
		}
		return true;
	}

	bool isEnemyPiece(int newRow, int newCol, PieceColor color){
		if (board[newRow][newCol].color == color){
			return false;
		}
		return true;
	}

	bool isValidKnightMove(int oldRow, int oldCol, int newRow, int newCol){
		int rowDifference = abs(newRow - oldRow);
		int colDifference = abs(newCol - oldCol);

		return ((rowDifference == 2 && colDifference == 1) || (rowDifference == 1 && colDifference == 2));
	}


	bool isValidPawnMove(Piece piece, int oldRow, int oldCol, int newRow, int newCol){
		int direction;
		if (piece.color == WhitePiece){
			direction = -1;
		}
		else {
			direction = 1;
		}

		int rowDifference = newRow - oldRow;
		int colDifference = abs(newCol - oldCol);

		if (colDifference == 0 && rowDifference == direction && board[newRow][newCol].type == Empty){
			return true;
		}
		else if (colDifference == 0 && rowDifference == direction * 2){
			if (rowDifference < 0 && oldRow == 6){
				return true;
			}
			else if (rowDifference > 0 && oldRow == 1){
				return true;
			}
		}
		else if (colDifference == 1 && rowDifference == direction && isEnemyPiece(newRow, newCol, piece.color)){
			return true;
		}
		return false;
	}

	bool isValidBishopMove(int oldRow, int oldCol, int newRow, int newCol){
		if (!isPathClear(oldRow, oldCol, newRow, newCol)){
			return false;
		}
		int rowDifference = abs(newRow - oldRow);
		int colDifference = abs(newCol - oldCol);

		return (rowDifference == colDifference);
	}

	bool isValidRookMove(int oldRow, int oldCol, int newRow, int newCol){
		if (!isPathClear(oldRow, oldCol, newRow, newCol)){
			return false;
		}
		int rowDifference = abs(newRow - oldRow);
		int colDifference = abs(newCol - oldCol);

		return (rowDifference == 0 || colDifference == 0);

	}

	bool isValidQueenMove(int oldRow, int oldCol, int newRow, int newCol){
		if (!isPathClear(oldRow, oldCol, newRow, newCol)){
			return false;
		}
		int rowDifference = abs(newRow - oldRow);
		int colDifference = abs(newCol - oldCol);

		return (rowDifference == colDifference || rowDifference == 0 || colDifference == 0);
	}

	bool isValidKingMove(int oldRow, int oldCol, int newRow, int newCol){
		int rowDifference = abs(newRow - oldRow);
		int colDifference = abs(newCol - oldCol);

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
		
		Piece piece = board[row][col];
		
		if (piece.color == color && piece.type == Knight){
			return true;
		}
	}
		return false;
    }

	bool isAttackedByPawn(int targetRow, int targetCol, PieceColor color){
		int offsets[4][2] = {
			{-1, 1}, {1, 1},
			{-1, -1}, {1, -1}
		};

		for (auto& offset: offsets){
			int row = targetRow + offset[0];
			int col = targetCol + offset[1];

			Piece piece = board[row][col];

		if (piece.color == color && piece.type == Pawn){
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
		return (isAttackedByKing(row, col, color) ||
				isAttackedByPawn(row, col, color) ||
				isAttackedByKnight(row, col, color)|| 
				isAttackedStraight(row, col, color) ||
				isAttackedDiagonally(row, col, color));
	}

	bool isPseudoLegal(int oldRow, int oldCol, int newRow, int newCol){
		Piece piece = board[oldRow][oldCol];
		if (isFriendlyPiece(newRow, newCol, piece.color)){
			return false;
		}
		else if (piece.type == Empty){
			return false;
		}
		switch (piece.type){
			case PieceType::Pawn:
				return isValidPawnMove(piece, oldRow, oldCol, newRow, newCol);
			case PieceType::Knight:
				return isValidKnightMove(oldRow, oldCol, newRow, newCol);
			case PieceType::Bishop:
				return isValidBishopMove(oldRow, oldCol, newRow, newCol);
			case PieceType::Rook:
				return isValidRookMove(oldRow, oldCol, newRow, newCol);
			case PieceType::Queen:
				return isValidQueenMove(oldRow, oldCol, newRow, newCol);
			case PieceType::King:
				return isValidKingMove(oldRow, oldCol, newRow, newCol);
			default:
				return false;
		}
	}
};


int main(){
	ChessBoard chessBoard;
	chessBoard.NestBoard();
	float squareSize = 750.0 / 8.0;
	Vector2 mouseOffset = {0.0f, 0.0f};
	Piece temp;
	int oldCol;
	int oldRow;
	
	InitWindow(1000, 1000, "Chess");
	SetTargetFPS(60);

	// Game Loop
	while (WindowShouldClose() == false)
	{	Vector2 mouse = GetMousePosition();
		// 1. Event handling
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
			
			oldCol = (mouse.x - 100) / squareSize;
			oldRow = (mouse.y - 100) / squareSize;
			if (oldRow >= 0 && oldRow < 8 && oldCol >= 0 && oldCol < 8){
				temp = chessBoard.board[oldRow][oldCol];
				if (temp.type != Empty){
					temp.selected = true;
					mouseOffset.x = mouse.x - (oldCol * squareSize);
					mouseOffset.y = mouse.y - (oldRow * squareSize);
				}
			}
		}

		// 2. Updating Positions
		 if (temp.selected) {
			int newRow;
			int newCol;
            newRow = (mouse.y - 100) / squareSize;
            newCol = (mouse.x - 100) / squareSize;

            // Stop dragging when mouse button is released
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
					if (chessBoard.isPseudoLegal(oldRow, oldCol, newRow, newCol)){
						chessBoard.board[newRow][newCol] = temp;
						chessBoard.board[oldRow][oldCol] = Piece{Empty, NoColor};
					}
					else {
						chessBoard.board[oldRow][oldCol] = temp;
					}
					
				}
                temp.selected = false;
            }
        }
		// 3. Drawing
		BeginDrawing();
		ClearBackground(BLACK);
		chessBoard.DisplayBoard();
		chessBoard.DisplayPieces();
		EndDrawing();
	}
	
	CloseWindow();
	return 0;
}