#ifndef TrackingManagement_EXPORTS
	#define TrackingManagement_DLL_import_export __declspec(dllimport)
#else
	#define TrackingManagement_DLL_import_export __declspec(dllexport)
#endif