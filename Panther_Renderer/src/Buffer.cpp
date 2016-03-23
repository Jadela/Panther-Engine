#include "DX12RendererPCH.h"
#include "Buffer.h"

namespace Panther
{
	void Buffer::CopyTo(const void* a_SourceStart, size_t a_SizeInBytes)
	{
		if (m_BufferType != BufferType::ConstantBuffer)
			throw std::runtime_error("Panther Buffer ERROR: Tried CopyTo on buffer that is not of type ConstantBuffer!");

		if (m_CPUBuffer == nullptr)
			throw std::runtime_error("Panther Buffer ERROR: Tried copying to buffer that is uninitialized!");

		if (a_SizeInBytes > m_BufferSize)
			throw std::runtime_error("Panther Buffer ERROR: Source size is larger than buffer size, will cause buffer overrun!");

		memcpy(m_CPUBuffer, a_SourceStart, a_SizeInBytes);
	}

	Buffer::Buffer() {}

	Buffer::Buffer(size_t a_BufferSize, BufferType a_Type)
		: m_BufferSize(a_BufferSize), m_BufferType(a_Type)
	{}
}