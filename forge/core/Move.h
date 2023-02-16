#pragma once

#include "forge/core/BoardSquare.h"
#include "forge/core/Piece.h"

#include <bitset>

namespace forge
{
	const std::bitset<16> from_mask =      0b0000'000000'111111;
	const std::bitset<16> to_mask =		   0b0000'111111'000000;
	const std::bitset<16> promotion_mask = 0b1111'000000'000000;

	// Represents a chess move
	// Stores:
	//	coordinates where piece is moving from
	//	coordinates where piece is moving to
	//	promotion piece (if applicable)
	class Move
	{
	public:
		Move() = default;
		Move(BoardSquare from, BoardSquare to) :
			m_val((to.val() << 6) | from.val()) {}
		Move(BoardSquare from, BoardSquare to, pieces::Piece promotion) :
			m_val((promotion.val().to_ulong() << 12) | (to.val() << 6) | (from.val())) {}
		// Constructs move based on string
		// string can be stored in PGN or LAN notation
		Move(const std::string & notation);
		Move(const Move &) = default;
		Move(Move &&) noexcept = default;
		~Move() noexcept = default;
		Move & operator=(const Move &) = default;
		Move & operator=(Move &&) noexcept = default;

		bool operator==(const Move & rhs) const { return m_val == rhs.m_val; }
		bool operator!=(const Move & rhs) const { return !(*this == rhs); }

		// Returns coordinates of where piece is moving from
		BoardSquare from() const 
		{
			uint16_t v = (uint16_t) (m_val & from_mask).to_ulong();

			return BoardSquare((uint8_t) v);
		}
		
		// Sets coordinates of where piece is moving from
		void from(BoardSquare pos) 
		{
			std::bitset<16> posBits = pos.val() /*<< 0*/;

			m_val = (m_val & ~from_mask) | posBits;
		}
		
		// Sets coordinates of where piece is moving from
		// using file and rank as characters
		// Make sure file is lower case
		// file = { 'a' - 'h' }
		// rank = { '1' - '8' }
		// If file or rank are out of bounds, sets this object to invalid.
		// !!! Only use characters not integers
		void from(char file, char rank);

		// Returns coordinates of where piece is moving to
		BoardSquare to() const 
		{
			uint16_t v = (uint16_t) (m_val & to_mask).to_ulong() >> 6;

			return BoardSquare((uint8_t) v);
		}
		
		// Sets coordinates of where piece is moving to
		void to(BoardSquare pos)
		{
			std::bitset<16> posBits = pos.val() << 6;

			m_val = (m_val & ~to_mask) | posBits;
		}

		// Sets coordinates of where piece is moving to
		// using file and rank as characters
		// Make sure file is lower case
		// file = { 'a' - 'h' }
		// rank = { '1' - '8' }
		// If file or rank are out of bounds, sets this object to invalid.
		// !!! Only use characters not integers
		void to(char file, char rank);
		
		// Returns promotion piece (could return pieces::Empty if move is not a promotion)
		pieces::Piece promotion() const 
		{
			uint16_t v = (uint16_t) (m_val & promotion_mask).to_ulong() >> 12;

			return pieces::Piece((uint8_t) v);
		}
		
		// Set promotion piece
		// Make sure piece is of the correct color
		// and is any of:
		//	- Queen
		//	- Rook
		//	- Bishop
		//	- Knight
		//	- Empty (Optional: Should be empty if Move does not represent a pawn promotion)
		// and None of:
		//	- King
		//	- Pawn
		void promotion(pieces::Piece piece)
		{
			std::bitset<16> promotionBits = piece.val().to_ullong() << 12;

			m_val = (m_val & ~promotion_mask) | promotionBits;
		}

		// Sets promotion piece to one cooresponding to 
		// 'promotionCh'. 
		// Move will be set to invalid if 'promotionCh' does not
		// coorespond to a QRBN piece or if 'to' component doesn't
		// point to a promotional rank.
		// Color of promotion is determined automatically based on 'to'
		// component
		// 
		// Make sure to call this method only after the 'to' component has be set to a promotion rank.
		void promotion(char promotionCh);

		// Determines if the Move object refers to only part of a move
		// where either the 'from' or 'to' component is specified
		// Useful with user interfaces where the user wants to specify their move 
		// in 2 steps: One for the coordinate of the piece their moving (from) and
		// the second for the coordinate their moving to (to).
		// 
		// ex: 
		// 1.) User is prompted to make a move.
		// 2.) User selects 'e4'
		// 3.) Display highlights 'e4' square and all the legal moves for that piece
		// 4.) User selects 'e5'
		// 5.) Game applies the move 'e4e5'
		// 
		// Partial moves are determined by setting 'to' and 'from' components to the same BoardSquare
		bool isPartial() const { return to() == from(); }

		// Returns true if promotion component has been set to a piece other than empty.
		bool isPromotion() const { return promotion() != pieces::empty; }

		// Determines if this move has been set to invalid.
		// 
		// A Move object must be set to invalid by calling .setInvalid()
		// 
		// Warning: It is possible to store an 'invalid' or 'impossible' 
		// Move which can never occur in a game.
		// But this method is only intended to work when the object has been set to invalid 
		// by calling .setInvalid()
		bool isInvalid() const { return promotion().isPawn(); }

		// Determines if this move is valid.
		bool isValid() const { return !isInvalid(); }

		// Used to identify a move as invalid by setting promotion component to a pawn.
		// When setting back to a valid move, be sure to set the promotion component
		// as well.
		void setInvalid() { promotion(pieces::whitePawn); }

		//std::string toPGN(const Board & board) const;

		// Returns Move as a string represented in 'Long Algebreic Notation' (LAN)
		// ex: 'e4e5'		piece at e4 will move up to e5
		// ex: 'e7e8Q'		pawn at e7 will be promoted to Q on e8 (PAWN move only)
		// ex: 'e7d8Q'		pawn at e7 will be promoted to Q and capture piece on d8 (PAWN capture only)
		std::string toLAN() const;

		// Calculates a hash for this move
		size_t hash() const;

		// prints move in Long Algebraic Notation (LAN)
		// To print using PGN notation use the .toPGN() method instead
		friend std::ostream & operator<<(std::ostream & os, const Move & move);

		// Converts stream characters in LAN format to a Move.
		// If only the first coordinate is specified, then move will be interpreted as a partial move
		// Parses token until a space character is reached or new line
		// Case insensitive
		// Do not put space between 'to' and 'from' coordinates
		// ex:	e4		# partial move
		//		e4e5	# to and from are both specified
		//		a7a8q	# pawn promotes to a Queen on a8
		//		e4 e5	# Bad, will only extract 'e4' from stream as a partial move.
		// 
		// WARNING: Does not support PGN because PGN requires a Board to remove ambiguities
		friend std::istream & operator>>(std::istream & is, Move & move);

	protected:
		// bits 0...5	- from [ row (3-bits) ][ col (3-bits) ]
		// bits 6...11	- to   [ row (3-bits) ][ col (3-bits) ]
		// bits 12...15 - promotion [ piece val (4-bits) ]
		std::bitset<16> m_val = 0;
	};
} // namespace forge