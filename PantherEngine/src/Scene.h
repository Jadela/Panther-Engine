#pragma once
#include "Core.h"
#include "Keys.h"
#include "Vector.h"

#include <memory>
#include <unordered_map> 

namespace Panther
{
	class CommandList;
	class Renderer;
	class TextureManager;

	class Scene
	{
	public:
		Scene() = delete;
		Scene(Renderer& renderer);
		virtual ~Scene();

		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Update(float a_DT) = 0;
		virtual void Render(CommandList& a_CommandList) = 0;
		virtual void OnResize(uint32 a_Width, uint32 a_Height) = 0;
		void OnKeyDown(Key a_Key);
		void OnKeyUp(Key a_Key);
		virtual void OnMouseMove(int32 a_DeltaX, int32 a_DeltaY, bool a_LMBDown, bool a_RMBDown) = 0;

	protected:
		bool GetKey(Key a_Key) { return m_KeyStates[a_Key]; };
		void UpdateMouseDelta(Vector a_NewMousePosition);

		Renderer& m_Renderer;
		std::unique_ptr<TextureManager> m_TextureManager;

		Vector m_MousePositionDelta;

	private:
		std::unordered_map<Key, bool> m_KeyStates;
		Vector m_PreviousMousePosition;
	};
}