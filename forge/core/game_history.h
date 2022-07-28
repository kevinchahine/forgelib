#pragma once

#include <forge/core/MovePositionPair.h>

#include <queue>

namespace forge
{
	class game_history : public std::deque<MovePositionPair>
	{
	public:
		// Optimize:
		// emplace_back() 
		// push_back()
		// See Optimizations.txt Search game_history
		const MovePositionPair & current() const { return this->back(); }
		MovePositionPair& current() { return this->back(); }

		void toPGN() const;
		void fromPGN() const;
	};
} // namespace forge