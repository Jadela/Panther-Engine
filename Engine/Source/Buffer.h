#pragma once
#include "Core.h"

namespace Panther
{
	class Buffer
	{
	public:
		virtual ~Buffer() {};

		size_t GetSize() { return m_ElementSize * m_NumElements; }
		uint32 GetSlot();
		void CopyTo(int32 a_ElementIndex, const void* a_SourceStart, size_t a_SizeInBytes);

	protected:
		enum class BufferType : ubyte8 { ConstantBuffer, UploadBuffer, Undefined };

		Buffer(uint32 a_NumElements, size_t a_ElementSize, BufferType a_Type);

		ubyte8* m_CPUBuffer = nullptr;

	private:
		Buffer() = delete;
		Buffer(const Buffer&) = delete;

		BufferType m_BufferType = BufferType::Undefined;
		uint32 m_NumElements = 0;
		size_t m_ElementSize = 0;
		uint32 m_ReservedSlot = 0;
	};
}