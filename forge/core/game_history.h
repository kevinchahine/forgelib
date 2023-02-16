#pragma once

#include <forge/core/MovePositionPair.h>

#include <vector>
#include <string>
#include <sstream>

namespace forge
{
	// Stores a sequence of moves and their cooresponding Positions.
	class game_history : public std::vector<MovePositionPair>
	{
	private:
		using super_t = std::vector<MovePositionPair>;

	public:
		// Optimize:
		// emplace_back() 
		// push_back()
		// See Optimizations.txt Search game_history
		const MovePositionPair & current() const { return this->back(); }
		MovePositionPair& current() { return this->back(); }
		
		// Applies move and appends to game history.
		// Warning: Only works if there is atleast 1 MovePositionPair in the game_history
		void push_back(const Move& move) {
			const MovePositionPair& curr = this->current();

			Position pos = curr.position;

			pos.move<pieces::Piece>(move);

			static_cast<super_t *>(this)->emplace_back(move, pos);// call emplace_back of parent class
		}

		// Push the latest move and position. These then become the current move/position.
		void push_back(const MovePositionPair & pair) {
			static_cast<super_t *>(this)->push_back(pair);// call emplace_back of parent class
		}

		// Save to a file
		void save(const std::string& filename);

		// Load from a file
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