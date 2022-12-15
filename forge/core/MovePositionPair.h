#pragma once

#include "forge/core/Move.h"
#include "forge/core/Position.h"

namespace forge
{
	class MovePositionPair
	{
	public:
		MovePositionPair() = default;
		MovePositionPair(Move move, const Position & position) :
			move(move),
			position(position) {}
		MovePositionPair(const MovePositionPair &) = default;
		MovePositionPair(MovePositionPair &&) noexcept = default;
		~MovePositionPair() noexcept = default;
		MovePositionPair & operator=(const MovePositionPair &) = default;
		MovePositionPair & operator=(MovePositionPair &&) noexcept = default;

		bool operator==(const MovePositionPair & pair) const;
		bool operator<(const MovePositionPair& pair) const;

		size_t hash() const;

		friend std::ostream& operator<<(std::ostream& os, const MovePositionPair& pair) {
			os << pair.move << '\t' << pair.position.toFEN();

			return os;
		}

		friend std::istream& operator>>(std::istream& is, MovePositionPair& pair) {
			std::string str;

			is >> str;
			
			pair.move = Move(str);

			std::getline(is, str);

			pair.position.fromFEN(str);

			return is;
		}

	public:
		Move move;
		Position position;
	};
} // namespace forge