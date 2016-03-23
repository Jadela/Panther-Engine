#pragma once

namespace Panther
{
	class Material;
	class Mesh;

	class CommandList
	{
	public:
		virtual ~CommandList();

		virtual void SetMaterial(Material& a_Material, bool a_ResetState) = 0;
		virtual void SetMesh(Mesh& a_Mesh) = 0;

	protected:
		CommandList();
		CommandList(const CommandList&);

	private:

	};
}