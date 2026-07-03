#include <raylib.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include "chess.h"
#include "engine.h"

struct Render {
	float size = 750.0 / 8.0;
	float x = 100.0f;
	float y = 100.0f;	
	Rectangle GetSourceRect(Piece piece, float pieceWidth, float pieceHeight) {
		return {
		piece.type * pieceWidth,
		piece.color * pieceHeight,
		pieceWidth,
		pieceHeight
		};
	}
	Rectangle GetDestRect(int col, int row){
		return {
		x + size * col,
		y + size * row,
		size,
		size
		};
	}
	void DisplayBoard(ChessBoard& chessBoard){
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

	void DisplayPieces(ChessBoard& chessBoard, Texture2D chessPieces){
		Vector2 origin = {0.0f, 0.0f};
		float pieceWidth = chessPieces.width / 6.0;
		float pieceHeight = chessPieces.height / 2.0;

		for (int row = 0; row < 8; row++){
			for (int col = 0; col < 8; col++){
				Piece piece = chessBoard.board[row][col];
				if (piece.type != Empty){
					DrawTexturePro(chessPieces, GetSourceRect(piece, pieceWidth, pieceHeight), GetDestRect(col, row), origin, 0.0f, WHITE);
				}
			}
		}
	}

	Rectangle GetPromotionButtonRect(ChessBoard& chessBoard, int index){
		float buttonSize = 82.0f;
		float totalWidth = buttonSize * 4.0f;
		float startX = x + ((size * 8.0f) - totalWidth) / 2.0f;
		float startY = y + (size * 3.5f);

		return {
			startX + (buttonSize * index),
			startY,
			buttonSize,
			buttonSize
		};
	}

	PieceType PromotionTypeAtIndex(int index){
		PieceType promotionTypes[4] = {Queen, Rook, Bishop, Knight};
		return promotionTypes[index];
	}

	PieceType GetPromotionChoice(ChessBoard& chessBoard, Vector2 mouse){
		for (int i = 0; i < 4; i++){
			if (CheckCollisionPointRec(mouse, GetPromotionButtonRect(chessBoard, i))){
				return PromotionTypeAtIndex(i);
			}
		}

		return Empty;
	}

	void DisplayPromotionMenu(ChessBoard& chessBoard, Texture2D chessPieces, PieceColor color){
		Vector2 origin = {0.0f, 0.0f};
		float pieceWidth = chessPieces.width / 6.0f;
		float pieceHeight = chessPieces.height / 2.0f;

		Rectangle firstButton = GetPromotionButtonRect(chessBoard, 0);
		DrawRectangle(firstButton.x - 10, firstButton.y - 10, (firstButton.width * 4) + 20, firstButton.height + 20, {20, 20, 20, 230});
		DrawRectangleLinesEx({firstButton.x - 10, firstButton.y - 10, (firstButton.width * 4) + 20, firstButton.height + 20}, 2, WHITE);

		for (int i = 0; i < 4; i++){
			Rectangle button = GetPromotionButtonRect(chessBoard, i);
			Piece option = Piece{PromotionTypeAtIndex(i), color};
			Rectangle destination = {
				button.x + 8,
				button.y + 8,
				button.width - 16,
				button.height - 16
			};

			Color buttonColor = CheckCollisionPointRec(GetMousePosition(), button) ? Color{80, 80, 80, 255} : Color{45, 45, 45, 255};
			DrawRectangleRec(button, buttonColor);
			DrawRectangleLinesEx(button, 2, WHITE);
			DrawTexturePro(chessPieces, GetSourceRect(option, pieceWidth, pieceHeight), destination, origin, 0.0f, WHITE);
		}
	}

	void DisplayCheckHighlights(ChessBoard& chessBoard){
		for (int row = 0; row < 8; row++){
			for (int col = 0; col < 8; col++){
				Piece piece = chessBoard.board[row][col];
				if (piece.type == King && chessBoard.isKingInCheck(piece.color)){
					Vector2 center = {x + (col * size) + (size / 2), y + (row * size) + (size / 2)};
					DrawCircleGradient(center, size * 0.45f, {255, 0, 0, 180}, {255, 0, 0, 0});
				}
			}
		}
	}

	void Checkmate(){
		DrawText("It's checkmate...", 190, 200, 100, RED);
	}

	void StaleMate(){
		DrawText("It's stalemate...", 190, 200, 100, RED);
	}
};


int main(){
	ChessBoard chessBoard;	
	Engine engine;
	Render render;
	chessBoard.NestBoard();
	float squareSize = 750.0 / 8.0;
	bool waitingForPromotion = false;
	int promotionRow = -1;
	int promotionCol = -1;
	Vector2 mouseOffset = {0.0f, 0.0f};
	Piece temp;
	PieceColor opposingColor = NoColor;
	PieceColor promotionColor = NoColor;
	Move currentMove = {-1, -1, -1, -1, Empty, false, false};
	bool gameOver = false;
	bool stalemate = false;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> colorChoice(0, 1);
	PieceColor playerColor = colorChoice(gen) == 0 ? WhitePiece : BlackPiece;
	PieceColor engineColor = chessBoard.opposite(playerColor);
	
	InitWindow(1000, 1000, "Chess");
	SetTargetFPS(60);
	Texture2D chessPieces = LoadTexture("resources/Chess_Pieces_Sprite.svg.png");
	// Game Loop
	while (WindowShouldClose() == false)
	{	Vector2 mouse = GetMousePosition();
		// 1. Event handling
		if (!waitingForPromotion && IsKeyPressed(KEY_LEFT) && !chessBoard.moveHistory.empty()){
			chessBoard.undoMove();
			gameOver = false;
			stalemate = false;
		}

		if (!gameOver && waitingForPromotion){
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
				PieceType promotedType = render.GetPromotionChoice(chessBoard, mouse);
				if (promotedType != Empty){
					chessBoard.PromotePawn(promotionRow, promotionCol, promotedType);
					waitingForPromotion = false;

					if (chessBoard.isCheckMate(chessBoard.generateLegalMoves(opposingColor), opposingColor)){
						gameOver = true;
					}
					else if (chessBoard.isStaleMate(chessBoard.generateLegalMoves(opposingColor), opposingColor)){
						gameOver = true;
						stalemate = true;
					}
				}
			}
		}
		else if (!gameOver && chessBoard.turn == playerColor) {
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
				currentMove.fromCol = (mouse.x - 100) / squareSize;
				currentMove.fromRow = (mouse.y - 100) / squareSize;	
			}

			if (!waitingForPromotion && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
				if (chessBoard.isInsideBoard(currentMove.fromRow, currentMove.fromCol)){
					currentMove.toRow = (mouse.y - 100) / squareSize;
					currentMove.toCol = (mouse.x - 100) / squareSize;
					temp = chessBoard.board[currentMove.fromRow][currentMove.fromCol];
					if (temp.color == playerColor && temp.color == chessBoard.turn){
						opposingColor = chessBoard.opposite(temp.color);
						if (chessBoard.isInsideBoard(currentMove.toRow, currentMove.toCol) && chessBoard.isLegalMove(currentMove, temp.color)){
							bool isPromotion = chessBoard.isPromotionMove(currentMove);
							chessBoard.makeMove(currentMove);
							if (isPromotion){
								waitingForPromotion = true;
								promotionRow = currentMove.toRow;
								promotionCol = currentMove.toCol;
								promotionColor = temp.color;
							}
							else {
								if (chessBoard.isCheckMate(chessBoard.generateLegalMoves(opposingColor), opposingColor)){
									gameOver = true;
								}
								else if (chessBoard.isStaleMate(chessBoard.generateLegalMoves(opposingColor), opposingColor)){
									gameOver= true;
									stalemate = true;
								}
							}
						}
					}
				}
			}
		}
		else if (!gameOver && !waitingForPromotion && chessBoard.turn == engineColor){
			vector<Move> legalMoves = chessBoard.generateLegalMoves(chessBoard.turn);
			if (chessBoard.isCheckMate(legalMoves, chessBoard.turn)){
				gameOver = true;
			}
			else if (chessBoard.isStaleMate(legalMoves, chessBoard.turn)){
				gameOver = true;
				stalemate = true;
			}
			else if (!legalMoves.empty()){
				Move bestMove = engine.bestMove(legalMoves, chessBoard.turn, chessBoard);
				if (!chessBoard.isLegalMove(bestMove, chessBoard.turn)){
					bestMove = legalMoves[0];
				}
				if (chessBoard.makeMove(bestMove)){
					PieceColor nextTurn = chessBoard.turn;
					if (chessBoard.isCheckMate(chessBoard.generateLegalMoves(nextTurn), nextTurn)){
						gameOver = true;
					}
					else if (chessBoard.isStaleMate(chessBoard.generateLegalMoves(nextTurn), nextTurn)){
						gameOver = true;
						stalemate = true;
					}
				}
			}
		}

		// 3. Drawing
		BeginDrawing();
		ClearBackground(BLACK);
		render.DisplayBoard(chessBoard);
		render.DisplayCheckHighlights(chessBoard);
		render.DisplayPieces(chessBoard, chessPieces);
		if (waitingForPromotion){
			render.DisplayPromotionMenu(chessBoard, chessPieces, promotionColor);
		}
		if (gameOver && !stalemate){
			render.Checkmate();
		}
		else if (gameOver && stalemate){
			render.StaleMate();
		}
		EndDrawing();
	}
	UnloadTexture(chessPieces);
	CloseWindow();
	return 0;
	}
