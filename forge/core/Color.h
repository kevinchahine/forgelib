#pragma once

namespace forge
{
	namespace colors
	{
		// Simple class to represent piece color.
		// TODO: Do we even use this class
		class Color
		{
		public:

		public:
			bool val = false;
		};

		class White : public Color {
		public:
			
		};
		
		class Black : public Color {
		public:
		};
		
		// TODO: Implement opposite functions. See reverse() in Direction.h 
		//template<White>
		//Black opposite() { return Black; }

	} // namespace colors
} // namespace forge