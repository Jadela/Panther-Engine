#include "Buffer.h"

#include <stdexcept>

namespace Panther
{
	uint32 Buffer::GetSlot()
	{
		if (m_ReservedSlot >= m_NumElements)
			throw std::runtime_error("Panther Buffer ERROR: No more available slots, increase a_NumElements while constructing this Buffer!");

		return m_ReservedSlot++;
	}

	void Buffer::CopyTo(int32 a_ElementIndex, const void* a_SourceStart, size_t a_SizeInBytes)
	{
		if (m_BufferType != BufferType::ConstantBuffer)
			throw std::runtime_error("Panther Buffer ERROR: Tried CopyTo on buffer that is not of type ConstantBuffer!");

		if (m_CPUBuffer == nullptr)
			throw std::runtime_error("Panther Buffer ERROR: Tried copying to buffer that is uninitialized!");

		if (a_SizeInBytes > m_ElementSize)
			throw std::runtime_error("Panther Buffer ERROR: Source size is larger than buffer size, will cause buffer overrun!");

		memcpy(&m_CPUBuffer[a_ElementIndex * m_ElementSize], a_SourceStart, a_SizeInBytes);
	}
	
	Buffer::Buffer(uint32 a_NumElements, size_t a_ElementSize, BufferType a_Type) :
		m_BufferType(a_Type),
		m_NumElements(a_NumElements),
		m_ElementSize(a_ElementSize)
	{}
}