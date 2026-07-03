#pragma once
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
	bool selected;
};

struct Move {
	int fromRow;
	int fromCol;
	int toRow;
	int toCol;
	PieceType promotion;
	bool isCastle;
	bool isEnPassant;
};

struct UndoInfo {
	Move move;

	Piece movedPiece;
	Piece capturedPiece;
	int capturedRow;
	int capturedCol;

	bool whiteCanCastleKingSide;
	bool whiteCanCastleQueenSide;
	bool blackCanCastleKingSide;
	bool blackCanCastleQueenSide;

	bool hadEnPassantTarget;
	int enPassantTargetRow;
	int enPassantTargetCol;

	PieceColor turn;
};

struct ChessBoard {
	Piece board[8][8];
	bool whiteCanCastleKingSide = true;
	bool whiteCanCastleQueenSide = true;
	bool blackCanCastleKingSide = true;
	bool blackCanCastleQueenSide = true;
	bool hasEnPassantTarget = false;
	int enPassantTargetRow = -1;
	int enPassantTargetCol = -1;
    bool endgame = false;
	PieceColor turn = WhitePiece;
	vector<UndoInfo> moveHistory;

	PieceColor opposite(PieceColor color);
	void NestBoard();
	bool isFriendlyPiece(int row, int col, PieceColor color);
	bool isInsideBoard(int row, int col);
	bool isPathClear(Move move);
	bool isEnemyPiece(int row, int col, PieceColor color);
	bool isValidKnightMove(Move move);
	bool isValidPawnMove(Piece piece, Move move);
    bool isValidBishopMove(Move move);
	bool isEnPassant(Move move);
	bool isValidRookMove(Move move);
	bool isValidQueenMove(Move move);
    void generateKnightMoves(int fromRow, int fromCol, PieceColor color, vector<Move>& legalMoves);
    void generatePawnMoves(int fromRow, int fromCol, PieceColor color, vector<Move>& legalMoves);
    void generateKingMoves(int fromRow, int fromCol, PieceColor color, vector<Move>& legalMoves);
    void generateSlidingMoves(int fromRow, int fromCol, PieceColor color, vector<Move>& legalMoves);
	bool isCastleMove(Move move);
	bool isValidCastle(Move move);
	bool isValidKingMove(Move move);
	bool isAttackedByKnight(int targetRow, int targetCol, PieceColor color);
	bool isAttackedByPawn(int targetRow, int targetCol, PieceColor color);
	bool isAttackedByKing(int targetRow, int targetCol, PieceColor color);
	bool isAttackedDiagonally(int targetRow, int targetCol, PieceColor color);
	bool isAttackedStraight(int targetRow, int targetCol, PieceColor color);
	bool isSquareAttacked(int row, int col, PieceColor color);
	bool isKingInCheck(PieceColor color);
	void PromotePawn(int row, int col, PieceType promotedType);
	bool isPromotionMove(Move move);
	void undoMove();
	bool makeMove(Move move);
	void makeMoveUnchecked(Move move);
	bool isPseudoLegal(Move move);
	void addPromotionMoves(vector<Move>& legalMoves, Move baseMove);
	bool isLegalMove(Move move, PieceColor color);
	vector<Move> generateLegalMoves(PieceColor color);
	bool isCheckMate(vector<Move> legalMoves, PieceColor kingColor);
	bool isStaleMate(vector<Move> legalMoves, PieceColor kingColor);
    
};