#pragma once
#include "../../Panther_Core/src/Core.h"
#include "../../Panther_Core/src/Keys.h"
#include "../../Panther_Core/src/Vector.h"

// Required otherwise the unique_ptr won't compile due destructor problem.
#include "../../Panther_Utilities/src/TextureManager.h"

namespace Panther
{
	class Renderer;

	class Scene
	{
	public:
		Scene() = delete;
		Scene(Renderer& renderer);
		virtual ~Scene() = default;

		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Update(float a_DT) = 0;
		virtual void Render() = 0;
		virtual void OnResize(uint32 a_Width, uint32 a_Height) = 0;
		virtual void OnKeyDown(Key a_Key, uint32 a_Character, KeyState a_KeyState, bool a_Ctrl, bool a_Shift, bool a_Alt) = 0;
		virtual void OnKeyUp(Key a_Key, uint32 a_Character, KeyState a_KeyState, bool a_Ctrl, bool a_Shift, bool a_Alt) = 0;
		virtual void OnMouseMove(int32 a_DeltaX, int32 a_DeltaY, bool a_LMBDown, bool a_RMBDown) = 0;

	protected:
		void UpdateMouseDelta(Vector2<int> a_NewMousePosition);

		Renderer& m_Renderer;
		std::unique_ptr<TextureManager> m_TextureManager;
		Vector2<int> m_MousePositionDelta;

	private:
		Vector2<int> m_PreviousMousePosition;
	};
}