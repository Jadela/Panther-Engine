#include "Input.h"

namespace Panther
{
	std::unordered_map<Key, bool> Input::s_KeyStates;
	Vector Input::s_MousePosition;

	void Input::Initialize()
	{
		s_KeyStates.reserve(static_cast<size_t>(Key::KeyCount));
		for (int keyInt = (int)Key::None; keyInt != (int)Key::KeyCount; keyInt++)
		{
			s_KeyStates[static_cast<Key>(keyInt)] = false;
		}
	}

	void Input::KeyDown(Key a_Key)
	{
		s_KeyStates[a_Key] = true;
	}

	void Input::KeyUp(Key a_Key)
	{
		s_KeyStates[a_Key] = false;
	}

	void Input::MouseMove(int32 a_X, int32 a_Y)
	{
		s_MousePosition = Vector((float)a_X, (float)a_Y);
	}
}
