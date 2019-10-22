#ifndef UIManagement_EXPORTS
	#define UIManagement_DLL_import_export __declspec(dllimport)
#else
	#define UIManagement_DLL_import_export __declspec(dllexport)
#endif