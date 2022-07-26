#pragma once

#include <stdint.h>
#include <functional>	// for std::hash<>

namespace forge
{
	// Number of moves played
	// 0 - no pieces have moved yet. (Whites thinking)
	// 1 - white made its first move. (Blacks thinking)
	// 2 - black made its first move. (Whites thinking)
	// even numbers - (whites turn, whites thinking)
	// odd numbers - (blacks turn, blacks thinking)
	class MoveCounter
	{
	public:

		void reset() { count = 0; }

		bool isWhitesTurn() const { return count % 2 == 0; }
		bool isBlacksTurn() const { return !isWhitesTurn(); }

		int halfmoves() const { return count; }
		int fullmoves() const { return count % 2; }

		const MoveCounter & operator++() { count++; return *this; }
		MoveCounter operator++(int) { auto temp = *this; count++; return temp; }

	public:
		// half move count. 
		// 0 at beginning of game before white plays.
		// 1 immediately after white makes 1st move.
		// 2 immediately after black makes 1st move.
		// 
		// Internal counts		FEN counts
		// half		full		full
		// moves	moves		moves
		// 0		0			1
		// 1		0			1
		// 2		1			2
		// 3		1			2
		// 4		2			3
		// 5		2			3
		// 6		3			4
		// 7		3			4
		int count = 0;
	};
} // namespace forge

// --- Inject hash into std namespace ---
namespace std
{
	template<> struct hash<forge::MoveCounter>
	{
		std::size_t operator()(const forge::MoveCounter& b) const noexcept
		{
			return b.count;
		}
	};
}