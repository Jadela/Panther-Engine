#pragma once

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class Buffer
	{
	public:
		virtual ~Buffer() {};

		void CopyTo(const void* a_SourceStart, size_t a_SizeInBytes);

	protected:
		enum class BufferType : ubyte8 { ConstantBuffer, UploadBuffer, Undefined };

		Buffer();
		Buffer(size_t a_BufferSize, BufferType a_Type);

		BufferType m_BufferType = BufferType::Undefined;
		ubyte8* m_CPUBuffer = nullptr;
		size_t m_BufferSize = 0;
	private:
		Buffer(const Buffer&) = delete;
	};
}