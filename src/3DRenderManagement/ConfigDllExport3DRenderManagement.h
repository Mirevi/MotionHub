#ifndef 3DRenderManagement_EXPORTS
	#define 3DRenderManagement_DLL_import_export __declspec(dllimport)
#else
	#define 3DRenderManagement_DLL_import_export __declspec(dllexport)
#endif