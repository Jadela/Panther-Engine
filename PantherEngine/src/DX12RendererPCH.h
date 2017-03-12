#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers.
#endif

// System includes
#include <windows.h>

// DirectX includes
#include <d3d12.h>
#include <dxgi1_5.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dx12.h>
#include <d3d12sdklayers.h>

// STD includes
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <algorithm>

// Windows Runtime Template Library
#include <wrl.h>

#ifdef _DEBUG 
#include <Initguid.h>
#include <dxgidebug.h>
#endif

// Link library dependencies
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
