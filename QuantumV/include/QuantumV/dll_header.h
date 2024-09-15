#pragma once

#ifdef QV_SHARED
	#ifdef QV_PLATFORM_WINDOWS
		#ifdef QV_EXPORT
			#define QV_API __declspec(dllexport)
			#else 
			#define QV_API __declspec(dllimport)
		#endif
	#else
		#error QuantumV supports msvc only for now
		#define QV_API
	#endif
#endif