// Copyright (c) 2017 Jasper de Laat	

#pragma once

// Set build configurations not defined by GENie to 0.
#ifndef CONFIG_DEBUG
	#define CONFIG_DEBUG 0
#endif
#ifndef CONFIG_RELEASE
	#define CONFIG_RELEASE 0
#endif

#if CONFIG_DEBUG + CONFIG_RELEASE != 1
	#error Configuration definitions are invalid, exactly one configuration must be 1.
#endif

// Set options that rely solely on build configuration.
#if CONFIG_DEBUG
	#define ENABLE_ASSERTIONS 1
#elif CONFIG_RELEASE
	#define ENABLE_ASSERTIONS 0
#else
	#error Build configuration dependency check failed.
#endif