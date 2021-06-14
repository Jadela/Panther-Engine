#include "Texture.h"
#include <stdexcept>

// NOTE (JDL): Disable wcstombs warning
#pragma warning (disable: 4996)

namespace Panther
{
	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
	}

	void Texture::LoadTarga(const std::wstring& a_Path)
	{
		// http://www.rastertek.com/dx11s2tut05.html
		struct TargaHeader
		{
			uint8 data1[12];
			uint16 width;
			uint16 height;
			uint8 bpp;
			uint8 data2;
		};

		int32 error, bpp, imageSize;
		FILE* filePtr;
		size_t count;
		TargaHeader targaFileHeader;

		char filestring[256];
		std::wcstombs(filestring, a_Path.c_str(), 256);
		// Open the targa file for reading in binary.
		error = fopen_s(&filePtr, filestring, "rb");
		if (error != 0)
		{
			throw std::runtime_error("Panther Texture ERROR: Could not open texture file.");
		}

		// Read in the file header.
		count = fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
		if (count != 1)
		{
			throw std::runtime_error("Panther Texture ERROR: Reading targa header failed.");
		}

		// Get the important information from the header.
		m_Height	= (uint32)targaFileHeader.height;
		m_Width		= (uint32)targaFileHeader.width;
		bpp			= (uint32)targaFileHeader.bpp;

		// Check that it is 32 bit and not 24 bit.
		if (bpp != 32)
		{
			throw std::runtime_error("Panther Texture ERROR: Tried loading texture with bit-depth other than 32, not supported.");
		}

		// Calculate the size of the 32 bit image data.
		imageSize = m_Width * m_Height * 4;

		// Allocate memory for the targa image data.
		std::unique_ptr<uint8[]> temp(new uint8[imageSize]);
		m_Data = std::make_unique<uint8[]>(imageSize);
		if (!m_Data || !temp)
		{
			throw std::runtime_error("Panther Texture ERROR: Memory allocation failed.");
		}

		// Read in the targa image data.
		count = fread(temp.get(), 1, imageSize, filePtr);
		if (count != imageSize)
		{
			throw std::runtime_error("Panther Texture ERROR: image size mismatches from header info.");
		}

		// Close the file.
		error = fclose(filePtr);
		if (error != 0)
		{
			throw std::runtime_error("Panther Texture ERROR: Closing file failed.");
		}

		// Flip targa image
		uint32 i, j, k, rowStart, columnStart;
		// Initialize the index into the targa data since targa data is stored upside down.
		k = (m_Width * m_Height * 4) - (m_Width * 4);

		// Load the targa data into the texture now.
		for (j = 0; j < m_Height; j++)
		{
			// Set the beginning of the row.
			rowStart = j * m_Width * 4;

			for (i = 0; i < m_Width; i++)
			{
				// Set the beginning of the column.
				columnStart = i * 4;

				// Copy the data in.
				m_Data.get()[rowStart + columnStart + 0] = temp.get()[k + 2];  // Red.
				m_Data.get()[rowStart + columnStart + 1] = temp.get()[k + 1];  // Green.
				m_Data.get()[rowStart + columnStart + 2] = temp.get()[k + 0];  // Blue
				m_Data.get()[rowStart + columnStart + 3] = temp.get()[k + 3];  // Alpha

				// Increment the index into the targa data.
				k += 4;
			}

			// Set the targa data index back to the preceding row at the beginning of the column since its reading it in upside down.
			k -= (m_Width * 8);
		}
	}

	void Texture::LoadFromDisk(const std::wstring& a_Path)
	{
		size_t lastDot = a_Path.rfind(L".");
		if (lastDot == std::string::npos) 
		{
			throw std::runtime_error("Panther Texture ERROR: File path is missing '.tga' extension.");
		}

		std::wstring extension = a_Path.substr(lastDot);
		if (extension == L".tga")
			LoadTarga(a_Path);
	}
}