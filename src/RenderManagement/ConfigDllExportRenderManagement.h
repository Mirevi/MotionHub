#ifndef RenderManagement_EXPORTS
	#define RenderManagement_DLL_import_export __declspec(dllimport)
#else
	#define RenderManagement_DLL_import_export __declspec(dllexport)
#endif