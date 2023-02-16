#pragma once

#include <iostream>
#include <bitset>
#include <functional>

#include <forge/core/Position.h>
#include "forge/core/game_history.h"
#include "forge/core/Node.h"
//#include "forge/search/MCTS_Node.h"
//#include "forge/search/MiniMaxNode.h"

namespace forge
{
	// Purpose: Represents a game state and calculates the game state of a Position.
	//	To calculate the game state of a Position, the GameHistory is needed. 
	//  * See game_history.h
	// 
	// Game States:
	//	- continue
	//		- whites turn
	//		- blacks turn
	//	- white wins
	//		- by resignation
	//		- by timeout (with sufficient material)
	//		- by checkmate
	//	- black wins
	//		- by resignation
	//		- by timeout (with sufficient material)
	//		- by checkmate
	//	- draw
	//		- by agreement
	//		- by repetition (the same Position has been reached 3 times)
	//		- by timeout (with insufficient material)
	//		- by 50 move rule
	//		- by insufficient material (even with time on the clock)
	// 
	// *** Draws by insufficient material follow USCF rules
	class GameState
	{
	public:
		// Clears GameState
		void reset() { (*this) = GameState(); }

		// TODO: Optimize: make an overload of init() which accepts the number of 
		// legal moves at that node to save on running movegen.
		
		// Determines game state
		template<class NODE_T>
		void init(const NodeTemplate<NODE_T>& node, const game_history & history = game_history());
		
		// Determines game state
		void init(const game_history& history);
		
		//void init(const ChessMatch & match);	// TODO:

		bool isGameOver() const { return state != STATE::CONTINUE; }
		bool isGameOn() const { return state == STATE::CONTINUE; }
		bool whiteWins() const { return state == STATE::WIN && player == PLAYER::WHITE; }
		bool blackWins() const { return state == STATE::WIN && player == PLAYER::BLACK; }
		bool isDraw() const { return state == STATE::DRAW; }

		// Returns:
		//	+1 if white wins
		//	-1 if black wins
		//	0  if draw
		// Undefined if gamestate is not terminal (win/loss/draw)
		int getValue(bool maximizeWhite) const {
			if (whiteWins()) return (maximizeWhite ? 1 : -1);
			if (blackWins()) return (maximizeWhite ? -1 : 1);
			else return 0;	// counts for draws and continues
		}

		friend std::ostream& operator<<(std::ostream& os, const GameState& gs)
		{
			if (gs.state != GameState::STATE::DRAW) {
				switch (gs.player)
				{
				case GameState::PLAYER::WHITE:	os << "White";	break;
				case GameState::PLAYER::BLACK:	os << "Black";	break;
				}
			}

			switch (gs.state)
			{
			case GameState::STATE::CONTINUE:	os << "'s turn";	break;
			case GameState::STATE::WIN:			os << " won";		break;
			case GameState::STATE::DRAW:		os << "Draw";		break;
			}

			switch (gs.reason)
			{
			case GameState::REASON::NOTHING:												break;
			case GameState::REASON::AGREEMENT:					os << " by aggreement";		break;
			case GameState::REASON::RESIGNATION:				os << " by resignation";	break;
			case GameState::REASON::CHECKMATE:					os << " by checkmate";		break;
			case GameState::REASON::STALEMATE:					os << " by stalemate";		break;
			case GameState::REASON::REPETITION:					os << " by repetition";		break;
			case GameState::REASON::FIFTY_MOVE_RULE:			os << " by 50 Move Rule";	break;
			case GameState::REASON::INSUFFICIENT_MATERIAL_ONLY: os << " by insufficient material";	break;
			case GameState::REASON::TIMEOUT_ONLY:				os << " by timeout";		break;
			case GameState::REASON::TIMEOUT_AND_INSUFFICIENT_MATERIAL:	os << " by timeout with insufficient material";  break;
			}

			return os;
		}

	private:
		// Calculates state of game as in: white wins, black wins, draw or continue.
		// params:
		//	- nLegalMoves - Number of legal moves at currPos
		//	- currPos - current position game
		//	- drawByRepetition - function that calculates a draw by repetition.
		//		Function will need to contain in some way all the moves or positions
		//		that lead to currPos, and return true if currPos has been found in 
		//		game history atleast 3 times (actually 2 times not counting 'currPos'
		//		itself.
		void calcGameState(int nLegalMoves, const Position& currPos, std::function<bool()>&& drawByRepetition);

		template<class NODE_T>
		static bool isDrawByRepetition(const NodeTemplate<NODE_T>& node);

		static bool isDrawByRepetition(const game_history& history);

		static bool isInsufficientMaterial(const Board& board);

		template<class NODE_T>
		static int16_t countMatches(const NodeTemplate<NODE_T> & node);

		static int16_t countMatches(const game_history & history);

	public:
		enum class PLAYER : bool {
			WHITE, BLACK
		} player;

		enum class STATE : uint8_t {
			CONTINUE, WIN, DRAW
			// No lose because a win for one player is a loss for the other
		} state;

		enum class REASON : uint8_t {
			NOTHING,			// (CONTINUE only) For when game is still playing.
			AGREEMENT,			// (DRAW only) Where both players agree on a draw 
			RESIGNATION,		// (WIN only) where one player resigns
			CHECKMATE,			// (WIN only)
			STALEMATE,			// (DRAW only) No pieces can move but King is not being attacked
			REPETITION,			// (DRAW only) 3 fold repetition
			FIFTY_MOVE_RULE,	// (DRAW only) 50 moves have passed without captures or pawn moves
			INSUFFICIENT_MATERIAL_ONLY,	// (DRAW only) Neither players have enough pieces to administer checkmate
			TIMEOUT_ONLY,		// (WINS only) One player ran out of time while the other has sufficient material to administer checkmate
			TIMEOUT_AND_INSUFFICIENT_MATERIAL,	// (DRAW only) One player ran out of time while other doesn't have enough material to administer checkmate
		} reason;
	};

	template<class NODE_T>
	void GameState::init(const NodeTemplate<NODE_T>& node, const game_history & history)
	{
//#ifdef _DEBUG
//		if (node.isExpanded() == false) {
//			std::cout << termcolor::red
//				<< "Error: node should be expanded before calling this method."
//				<< std::endl;
//		}
//#endif
		MoveGenerator2 gen;
		MoveList moves = gen.generate(node.position());
		
		std::function<bool()> drawByRepetition = [&]() {
			return 
				GameState::countMatches(node) + 
				GameState::countMatches(history) >= 3;
				//GameState::isDrawByRepetition(node) || 
				//GameState::isDrawByRepetition(history);
		};

		calcGameState(
			moves.size(),//node.children().size(),			// Number of legal moves
			node.position(),				// current position
			std::move(drawByRepetition));	// calculates draw by repetition using a Node tree
	}

	template<class NODE_T>
	bool GameState::isDrawByRepetition(const NodeTemplate<NODE_T>& node)
	{
		const Position& currPos = node.position();

		const NodeTemplate<NODE_T>* nPtr = &node;
		uint8_t matches = 0;

		// Skip current Node 
		nPtr = nPtr->parentPtr();

		// --- Search Tree ---
		// Iterate up the node tree to the root.
		while (nPtr != nullptr) {
			if (nPtr->position() == currPos) {
				matches++;

				if (matches >= 3) {
					return true;	// 3 matches found (DRAW by repetition)
				}
			}

			// Jump to parent of this node.
			nPtr = nPtr->parentPtr();
		}

		return false;	// did not find 3 matches (No DRAW)
	}

	// TODO: Optimize: Only search every other node. This is because a match is dependent of the player making the move.
	// !!! If searching every other node, then we need to skip the 1st 2 nodes instead of the 1st one only !!!
	template<class NODE_T>
	inline int16_t GameState::countMatches(const NodeTemplate<NODE_T> & node) {
		const Position & currPos = node.position();

		const NodeTemplate<NODE_T> * nPtr = &node;
		uint16_t matches = 0;

		// Skip current Node 
		nPtr = nPtr->parentPtr();

		// --- Search Tree ---
		// Iterate up the node tree to the root.
		while (nPtr != nullptr) {
			if (nPtr->position() == currPos) {
				matches++;

				if (matches >= 3) {
					break;
					//return true;	// 3 matches found (DRAW by repetition)
				}
			}

			// Jump to parent of this node.
			nPtr = nPtr->parentPtr();
		}

		return matches;	// did not find 3 matches (No DRAW)
	}
} // namespace forge