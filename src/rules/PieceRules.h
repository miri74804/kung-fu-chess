#pragma once

#include "PieceRule.h"

class RookRule : public PieceRule {
public:
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};

class BishopRule : public PieceRule {
public:
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};

class QueenRule : public PieceRule {
public:
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};

class KnightRule : public PieceRule {
public:
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};

class KingRule : public PieceRule {
public:
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};

class PawnRule : public PieceRule {
public:
	bool isValidMove(const Position& from, const Position& to, const Board& board) const override;
};
