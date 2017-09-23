// Copyright (c) 2017 Jasper de Laat

#pragma once

// Platforms not defined (by GENie) are set to 0.
#ifndef PLATFORM_WINDOWS
	#define PLATFORM_WINDOWS 0
#endif

#if PLATFORM_WINDOWS != 1
	#error Platform definitions are invalid, exactly one platform must be 1.
#endif

