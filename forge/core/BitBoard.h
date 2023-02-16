#pragma once

#include "forge/core/Direction.h"

#include <iostream>
#include <bitset>

#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG

namespace forge
{
	// --- Forward Declarations ---
	class BoardSquare;

	/*
	* Stores an 8x8 boolean array as a bitset<64> (64-bit integer).
	*
	* Runs some operations faster than arrays by using bitwise operations.
	* These bitwise operations can replace common array operations which can
	* only be executed using loops.
	* 
	* Each bit coordesponds to a cell on a chess board:
	*	 0  1  2  3  4  5  6  7
	*	 8  9 10 11 12 13 14 15
	*	16 17 18 19 20 21 22 23
	*	24 25 26 27 28 29 30 31
	*	32 33 34 35 36 37 38 39
	*	40 41 42 43 44 45 46 47
	*	48 49 50 51 52 53 54 55
	*	56 57 58 59 60 61 62 63
	*/
	class BitBoard : public std::bitset<64>
	{
	public:
		BitBoard() = default;
		BitBoard(unsigned long long l) : std::bitset<64>(l) {};
		BitBoard(const BitBoard & bb) = default;
		BitBoard(const std::bitset<64> & bset) : std::bitset<64>(bset) {};
		~BitBoard() noexcept = default;
		BitBoard & operator=(const BitBoard &) = default;

		// Access a single bit
		std::bitset<64>::reference operator[](size_t i) {
			return static_cast<bitset<64> &>(*this)[i];
		}

		// Access a single bit
		bool operator[](size_t i) const {
			return this->std::bitset<64>::operator[](i);
		}

		// Access a single bit
		std::bitset<64>::reference operator[](int i) {
			return static_cast<bitset<64> &>(*this)[i];
		}

		// Access a single bit
		bool operator[](int i) const {
			return this->std::bitset<64>::operator[](i);
		}

		// Access a single bit referenced by a BoardSquare
		std::bitset<64>::reference operator[](const BoardSquare & square) {
			return (*this)[square.val()];
		}
		
		// Access a single bit referenced by a BoardSquare
		bool operator[](const BoardSquare & square) const {
			return (*this)[square.val()];
		}

		// Rotates the BitBoard 180 degrees. 
		// Same as rotating pieces without rotating the chess board.
		// Does not flip bits (0 to 1, 1 to 0)
		BitBoard rotated() const;

		// --- Shift Operations ---

		// Shifts bits in a 2D way (Up, Down, Left, Right) as if bits were stored as a 2D 8x8 array.
		template<typename DIRECTION_T>
		void shift(uint8_t numberOfCells);/* {
			static_assert(true,
				"This method is the unspeciallized template. "
				"Don't call this method. "
				"Instead call a fully specialized overload instead.");
		}*/

		// Draws a line of 1s between begin and end.
		// Inclusive on begin
		// Exclusive on end
		// DIRECTION_T should be of a ray direction type
		//	ex: Up, Down, UL, DL. Not Horizontal or Vertical
		//	See Direction.h
		// Make sure the path between begin and end are in the same direction
		//	as RAY_DIRECTION_T.
		template<typename RAY_DIRECTION_T>
		static BitBoard mask(BoardSquare begin, BoardSquare end);

		// Draws a line of 1s from `center` to the edge of the board in the 
		// direction specified by `DIRECTION_T`.
		// *See above comments
		template<typename DIRECTION_T>
		static BitBoard mask(BoardSquare center);

		// Prints all 64 bits on the same line
		friend std::ostream & operator<<(std::ostream & os, const BitBoard & bb);

		// Print bits as a 2D 8x8 array of 1's and 0's
		void print(std::ostream & os = std::cout) const;

	private:
	}; // class BitBoard

	// -------------------------------- METHOD DEFINITIONS --------------------

	template<typename RAY_DIRECTION_T>
	BitBoard BitBoard::mask(BoardSquare begin, BoardSquare end) {
		// Make sure template parameter RAY_DIRECTION_T is a Ray and not something else.
		static_assert(std::is_base_of<directions::Ray, RAY_DIRECTION_T>(),
					  "RAY_DIRECTION_T must be a ray direction like forge::directions::Up or forge::directions::UL");

		#ifdef _DEBUG
		int rise = abs((int) begin.row() - (int) end.row());
		int run = abs((int) begin.col() - (int) end.col());

		if (rise == 0 || run == 0 || rise == run) {
		}
		else {
			std::cout << "Error: " << __FILE__ << " line " << __LINE__
				<< ": begin and end are not on the same lateral or diagonal\n";
		}
		#endif

		BitBoard bb;

		// TODO: Optimize: This can really be optimized. 
		// We shouldn't need a loop at all. Only bitwise operations.
		while (begin.operator!=(end)) {
			bb[begin] = 1;	// Set bit to 1

			begin = RAY_DIRECTION_T{}(begin);
		}

		return bb;
	}

	// Unspecialized definition
	//template<typename DIRECTION_T>
	//BitBoard BitBoard::mask(BoardSquare center);/*
	//{
	//	static_assert(true, "Call the explicit specializations instead of the primary specialization of the method");
	//
	//	BitBoard bb;
	//
	//	return bb;
	//}*/
} // namespace forge

// --- Inject hash into std namespace ---
namespace std
{
	template<> struct hash<forge::BitBoard>
	{
	public:
		std::size_t operator()(const forge::BitBoard & b) const noexcept {
			return std::hash<std::bitset<64>>{}(b);
		}
	};
}
