#ifndef GestureManagement_EXPORTS
	#define GestureManagement_DLL_import_export __declspec(dllimport)
#else
	#define GestureManagement_DLL_import_export __declspec(dllexport)
#endif