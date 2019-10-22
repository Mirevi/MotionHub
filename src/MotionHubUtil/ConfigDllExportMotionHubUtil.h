#ifndef MotionHubUtil_EXPORTS
	#define MotionHubUtil_DLL_import_export __declspec(dllimport)
#else
	#define MotionHubUtil_DLL_import_export __declspec(dllexport)
#endif