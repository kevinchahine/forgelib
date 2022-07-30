#include "Clock.h"

#include <iomanip>
#include <sstream>

#include <guten/termcolor/termcolor.hpp>

using namespace std;

namespace forge
{
	void clock::stop()
	{
		whites_timer.pause();

		blacks_timer.pause();

		curr_state = STATE::STOP;
	}

	void clock::resume()
	{
		if (is_whites_turn()) {
			whites_timer.resume();
		}
		else {
			blacks_timer.resume();
		}

		curr_state = STATE::START;
	}
	
	void clock::reset_all()
	{
		// Reassign all fields with default values
		(*this) = clock{};
	}
	
	void clock::synchronize(
		std::chrono::high_resolution_clock::duration whites_time,
		std::chrono::high_resolution_clock::duration white_inc,
		std::chrono::high_resolution_clock::duration blacks_time,
		std::chrono::high_resolution_clock::duration black_inc)
	{
		this->whites_timer.expires_from_now(whites_time);
		this->whites_increment = chrono::duration_cast<chrono::milliseconds>(white_inc);
		this->blacks_timer.expires_from_now(blacks_time);
		this->blacks_increment = chrono::duration_cast<chrono::milliseconds>(black_inc);
	}
	
	void forge::clock::click()
	{
		curr_state = STATE::START;

		// Which player was thinking before the clock was clicked?
		if (is_whites_turn()) {
			// White clicked the clock so white was thinking.
			// Subtract that amount of time from white's time.
			whites_timer.pause();
			blacks_timer.resume();

			// Give white their increment (Fischer time)
			whites_timer.add_time(whites_increment);
		}
		else {
			// Black clicked the clock so black was thinking.
			// Subtract that amount of time from black's time.
			blacks_timer.pause();
			whites_timer.resume();

			// Give black their increment (Fishcer time)
			blacks_timer.add_time(blacks_increment);
		}
	}

	bool clock::is_whites_turn() const
	{
		return whites_timer.is_resumed();
	}
	
	bool clock::is_blacks_turn() const
	{
		return !is_whites_turn();
	}
} // namespace forge

ostream & operator<<(ostream & os, const forge::clock & clock)
{
	stringstream bss;
	stringstream wss;

	os << "Black: " << clock.get_black_timer() << '\t' << " White: " << clock.get_white_timer() << endl;

	return os;
}