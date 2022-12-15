#pragma once

#include <forge/core/MovePositionPair.h>

#include <vector>
#include <string>
#include <sstream>

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

		void save(const std::string& filename);
		void load(const std::string& filename);

		friend std::ostream& operator<<(std::ostream& os, const game_history& history) {
			for (const MovePositionPair& pair : history) {
				os << pair << std::endl;
			}

			return os;
		}

		friend std::istream& operator>>(std::istream& is, game_history& history) {
			std::string line = " ";

			while (true) {
				std::getline(is, line);

				if (line.empty())
					break;

				std::istringstream iss(line);

				iss >> history.emplace_back();
			}

			return is;
		}

		void toPGN() const;
		void fromPGN() const;
	};
} // namespace forge