#include "game_history.h"

#include <fstream>

using namespace std;

namespace forge
{
	void game_history::save(const std::string& filename)
	{
		ofstream out(filename);

		if (!out) {
			cout << "Error: " << __FILE__ << " line " << __LINE__ << ": File could not be opened." << endl;
			return;
		}

		out << *this << endl;

		out.close();
	}
	
	void game_history::load(const std::string& filename)
	{
		ifstream in(filename);

		if (!in) {
			cout << "Error: " << __FILE__ << " line " << __LINE__ << ": File could not be opened." << endl;
			return;
		}

		in >> *this;
	}
} // namespace forge