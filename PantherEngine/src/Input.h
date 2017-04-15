#pragma once
#include "Keys.h"
#include "Vector.h"

#include <unordered_map> 

namespace Panther
{
	class Input
	{
	public:
		static void Initialize();

		static void KeyDown(Key a_Key);
		static void KeyUp(Key a_Key);
		static void MouseMove(int32 a_X, int32 a_Y);

		static bool GetKey(Key a_Key) { return s_KeyStates[a_Key]; };
		static Vector GetMousePosition() { return s_MousePosition; }

	private:
		static std::unordered_map<Key, bool> s_KeyStates;
		static Vector s_MousePosition;
	};
}
