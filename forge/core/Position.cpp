#include "forge/core/Position.h"

#include <sstream>

using namespace std;

namespace forge
{
	Position::Position(const std::string& fen)
	{
		this->fromFEN(fen);
	}

	void Position::reset()
	{
		cout << "Remplce calls to Position::reset() with Position::setupNewGame()" << endl;

		setupNewGame();
	}

	void Position::setupNewGame()
	{
		m_board.placeAllPieces();

		m_fiftyMoveRule.reset();

		m_moveCounter.reset();
	}

	void Position::clear()
	{
		m_board.reset();

		m_fiftyMoveRule.reset();

		m_moveCounter.reset();
	}

	void Position::fromFEN(const std::string& fen)
	{
		// https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation?msclkid=f2073544cd8311eca1516d444ecd6e97
		
		// TODO: finish this method

		istringstream ss{ fen };

		// 1.) --- Board and Pieces ---
		{
			Board& b = this->board();

			// Move Kings to lower right corner so that they don't interfer with piece placement
			b.place<pieces::WhiteKing>(BoardSquare{ 7, 7 });
			b.place<pieces::BlackKing>(BoardSquare{ 7, 7 });

			BoardSquare bs{ 0, 0 };
			char ch;

			while (ss.peek() != ' ') {
				ch = ss.get();

				if (isdigit(ch)) {
					int nSpaces = ch - '0';

					for (int count = 0; count < nSpaces; count++) {
						b.place<pieces::Empty>(bs.right(count));
					}

					bs = bs.right(nSpaces);
				}
				else if (isalpha(ch)) {
					bool isWhite = isupper(ch);

					b.placePiece(bs, pieces::Piece{ ch, isWhite });

					bs = bs.rightOne();
				}
				//else if (ch == '/') {
				//	bs = BoardSquare{ bs.row() + 1, 0 };
				//}
			}

			// No need to skip space explicitly.
			// stream will do it automatically.
		}

		// 2.) --- Active Piece (Who's turn is it?) ---
		char activePiece;
		ss >> activePiece;
		activePiece = tolower(activePiece);
		bool isWhite = (activePiece == 'w');

		// 3.) --- Castling Rights ---
		// TODO: Castling: Do this
		string castling;
		ss >> castling;

		// 4.) --- Enpassent ---
		// TODO: Enpassent: Do this
		string enpassent;
		ss >> enpassent;

		// 5.) --- 50 Move Rule ---
		int fiftyMoveCount;
		ss >> fiftyMoveCount;
		this->m_fiftyMoveRule.count(fiftyMoveCount);

		// 6.) --- Full move count ---
		int fullMoveCount;
		ss >> fullMoveCount;
		this->m_moveCounter.count = fullMoveCount - 1 + (isWhite ? 0 : 1);
	}

	string Position::toFEN() const
	{
		stringstream ss;

		// 1.) --- Board and Pieces ---
		{
			const Board& b = this->board();
			uint16_t emptyCount = 0;

			for (uint16_t row = 0; row < 8; row++) {
				for (uint16_t col = 0; col < 8; col++) {
					BoardSquare bs{ row, col };

					if (b.isEmpty(bs)) {
						emptyCount++;
					}
					else {
						if (emptyCount != 0) {
							ss << emptyCount;
						}
						emptyCount = 0;

						char ch = ' ';

						if (b.isKing(bs)) ch = 'k';
						else if (b.isQueen(bs)) ch = 'q';
						else if (b.isBishop(bs)) ch = 'b';
						else if (b.isKnight(bs)) ch = 'n';
						else if (b.isRook(bs)) ch = 'r';
						else if (b.isPawn(bs)) ch = 'p';

						if (b.isWhite(bs)) ch = toupper(ch);

						ss << ch;
					} // is not empty
				} // for (col = 0

				if (emptyCount != 0) {
					ss << emptyCount;
					emptyCount = 0;
				}

				if (row != 7) {
					ss << '/';
				}
			} // for (row = 0

			ss << ' ';
		} // Board and Pieces

		// 2.) --- Active Piece (Who's turn is it?) ---
		ss << (this->moveCounter().isWhitesTurn() ? 'w' : 'b') << ' ';

		// 3.) --- Castling Rights ---
		// TODO: Castling: Do this
		ss << "- ";	// For now

		// 4.) --- Enpassent ---
		// TODO: Enpassent: Do this
		ss << "- ";	// For now

		// 5.) --- 50 Move Rule ---
		ss << this->fiftyMoveRule().count() << ' ';

		// 6.) --- Full move count ---
		ss << (this->moveCounter().count / 2) + 1;

		return ss.str();
	}

	template<> void Position::move<pieces::WhiteKing>(Move move)
	{
#ifdef _DEBUG
		if (m_board.at(move.from()).isKing() == false) {
			std::cout << "Error " << __FUNCTION__ << " line " << __LINE__
				<< ": This method only moves Kings\n";
		}
#endif // _DEBUG

		m_board.move<pieces::WhiteKing>(move);

		m_fiftyMoveRule.update();
		// TODO: Castling

		m_moveCounter++;
	}

	template<> void Position::move<pieces::BlackKing>(Move move)
	{
#ifdef _DEBUG
		if (m_board.at(move.from()).isKing() == false) {
			std::cout << "Error " << __FUNCTION__ << " line " << __LINE__
				<< ": This method only moves Kings\n";
		}
#endif // _DEBUG

		m_board.move<pieces::BlackKing>(move);

		m_fiftyMoveRule.update();
		// TODO: Castling

		m_moveCounter++;
	}

	template<> void Position::move<pieces::King>(Move move)
	{
#ifdef _DEBUG
		if (m_board.at(move.from()).isKing() == false) {
			std::cout << "Error " << __FUNCTION__ << " line " << __LINE__
				<< ": This method only moves Kings\n";
		}
#endif // _DEBUG

		if (m_board.isWhite(move.from())) {
			this->move<pieces::WhiteKing>(move);
		}
		else {
			this->move<pieces::BlackKing>(move);
		}
	}

	template<> void Position::move<pieces::Queen>(Move move)
	{
		// --- Was this a capture? ---
		if (m_board.isOccupied(move.to()))
			m_fiftyMoveRule.pieceCaptured();	// Yes. Capture occured

		m_board.move<pieces::Queen>(move);

		m_fiftyMoveRule.update();

		m_moveCounter++;
	}

	template<> void Position::move<pieces::Bishop>(Move move)
	{
		// --- Was this a capture? ---
		if (m_board.isOccupied(move.to()))
			m_fiftyMoveRule.pieceCaptured();	// Yes. Capture occured

		m_board.move<pieces::Bishop>(move);

		m_fiftyMoveRule.update();

		m_moveCounter++;
	}

	template<> void Position::move<pieces::Knight>(Move move)
	{
		// --- Was this a capture? ---
		if (m_board.isOccupied(move.to()))
			m_fiftyMoveRule.pieceCaptured();	// Yes. Capture occured

		m_board.move<pieces::Knight>(move);

		m_fiftyMoveRule.update();

		m_moveCounter++;
	}

	template<> void Position::move<pieces::QBN_Piece>(Move move)
	{
		// --- Was this a capture? ---
		if (m_board.isOccupied(move.to()))
			m_fiftyMoveRule.pieceCaptured();	// Yes. Capture occured

		m_board.move<pieces::QBN_Piece>(move);

		m_fiftyMoveRule.update();

		m_moveCounter++;
	}

	template<> void Position::move<pieces::Rook>(Move move)
	{
		// --- Was this a capture? ---
		if (m_board.isOccupied(move.to()))
			m_fiftyMoveRule.pieceCaptured();	// Yes. Capture occured

		m_board.move<pieces::Rook>(move);

		m_fiftyMoveRule.update();

		// TODO: Don't forget castling

		m_moveCounter++;
	}

	template<> void Position::move<pieces::WhitePawn>(Move move)
	{
#ifdef _DEBUG 
		if (m_board.pawns()[move.from()] == false || m_board.whites()[move.from()] == false)
			std::cout << "Error " << __FILE__ << " line " << __LINE__
			<< ": This method only moves white pawns\n";
#endif

		// --- Was this a capture? ---
		if (m_board.isOccupied(move.to()))
			m_fiftyMoveRule.pieceCaptured();	// Yes. Capture occured

		m_board.place<pieces::Empty>(move.from(), bool());	// bool() is a place holder

		if (move.to().isTopRank()) {m_board.placePiece(move.to(), move.promotion()); }
		else m_board.place<pieces::WhitePawn>(move.to());

		m_fiftyMoveRule.pawnHasMoved();
		m_fiftyMoveRule.update();
		// TODO: ENPASSENT

		m_moveCounter++;
	}

	template<> void Position::move<pieces::BlackPawn>(Move move)
	{
#ifdef _DEBUG 
		if (m_board.pawns()[move.from()] == false || m_board.blacks()[move.from()] == false)
			std::cout << "Error " << __FUNCTION__ << " line " << __LINE__
			<< ": This method only moves black pawns\n";
#endif
		
		// --- Was this a capture? ---
		if (m_board.isOccupied(move.to()))
			m_fiftyMoveRule.pieceCaptured();	// Yes. Capture occured

		m_board.place<pieces::Empty>(move.from(), bool());	// bool() is a place holder

		if (move.to().isBotRank()) { m_board.placePiece(move.to(), move.promotion()); }
		else m_board.place<pieces::BlackPawn>(move.to());

		m_fiftyMoveRule.pawnHasMoved();
		m_fiftyMoveRule.update();
		// TODO: Still need enpassent 

		m_moveCounter++;
	}

	template<> void Position::move<pieces::Pawn>(Move move)
	{
		if (m_board.isWhite(move.from()))
			this->move<pieces::WhitePawn>(move);
		else
			this->move<pieces::BlackPawn>(move);
	}

	// Moves any piece on the board.
	// Works for captures and push moves as well.
	// For support for castling, see Position::move<pieces::King>()
	// For support for en passent, see Position::move<pieces::Pawn>();
	// For support for promotions, see Position::move<pieces::Pawn>();
	template<> void Position::move<pieces::Piece>(Move move)
	{
		pieces::Piece p = m_board.at(move.from());

		if (p.isPawn()) {
			this->move<pieces::Pawn>(move);
		}
		else if (p.isQueen() || p.isBishop() || p.isKnight()) {
			this->move<pieces::QBN_Piece>(move);
		}
		else if (p.isRook()) {
			this->move<pieces::Rook>(move);
		}
		else if (p.isKing()) {
			this->move<pieces::King>(move);
		}
		else {
			cout << __FILE__ << " line " << __LINE__ << ": Piece \'" << p << "\' does not have a conditional" << endl;
		}
	}

	std::ostream& operator<<(std::ostream& os, const Position& pos)
	{
		os << pos.toFEN();

		return os;
	}
	
	std::istream& operator>>(std::istream& is, Position& pos)
	{
		string fen;

		cout << __FUNCTION__ << "This doesn't work yet" << endl;
		pos.fromFEN(fen);

		return is;
	}
} // namespace forge
