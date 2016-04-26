#pragma once
#include "../../Panther_Core/src/Core.h"
#include "../../Panther_Core/src/Keys.h"

namespace Panther
{
	class Renderer;

	class Scene
	{
	public:
		Scene() = delete;
		Scene(Panther::Renderer& renderer);
		virtual ~Scene() {}

		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Update(float a_DT) = 0;
		virtual void Render() = 0;
		virtual void OnResize(uint32 a_Width, uint32 a_Height) = 0;
		virtual void OnKeyDown(Key a_Key, uint32 a_Character, KeyState a_KeyState, bool a_Ctrl, bool a_Shift, bool a_Alt) = 0;
		virtual void OnKeyUp(Key a_Key, uint32 a_Character, KeyState a_KeyState, bool a_Ctrl, bool a_Shift, bool a_Alt) = 0;
		virtual void OnMouseMove(int32 a_DeltaX, int32 a_DeltaY, bool a_LMBDown, bool a_RMBDown) = 0;

	protected:
		Panther::Renderer& m_Renderer;
		DirectX::XMINT2 m_MousePositionDelta;

		void UpdateMouseDelta(DirectX::XMINT2 a_NewMousePosition);
	private:
		DirectX::XMINT2 m_PreviousMousePosition;
	};
}