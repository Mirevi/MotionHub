#ifndef NetworkManagement_EXPORTS
	#define NetworkManagement_DLL_import_export __declspec(dllimport)
#else
	#define NetworkManagement_DLL_import_export __declspec(dllexport)
#endif