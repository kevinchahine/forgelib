#pragma once

#include <forge/core/MovePositionPair.h>

#include <vector>

namespace forge
{
	class game_history : public std::vector<MovePositionPair>
	{
	public:
		// Optimize:
		// emplace_back() 
		// push_back()
		// See Optimizations.txt Search game_history
		const MovePositionPair & current() const { return this->back(); }
		MovePositionPair& current() { return this->back(); }
		
		// Applies move and appends to game history.
		// Only works if there is atleast 1 MovePositionPair in the game_history
		void push_back(const Move& move) {
			const MovePositionPair& curr = this->current();

			Position pos = curr.position;

			pos.move<pieces::Piece>(move);

			this->emplace_back(move, pos);
		}

		void toPGN() const;
		void fromPGN() const;
	};
} // namespace forge