#pragma once
#pragma warning (disable : 4251)	//--> It's for surpressing the wanring about the need of a dll-interface for the STL map class...
									//... but can be ignored, because theoretically no "client" of configReader will ...
									//... ever use the member m_startupConfig. This means, there is no need for exporting Map to the dll.
									//Before january 2017, it was a static public STL Map, which is now a private member and can't directly accessed
									//by "clients"/"users" of ConfigReader. The warning could be some kind of minor bug in the compiler

#include "ConfigDllExportMotionHubUtil.h"

#include <map>
#include <string>

#include "tinyxml2.h"

#include "Console.h"

class MotionHubUtil_DLL_import_export ConfigManager
{
	// #### CONSTRUCTOR & DESTRUCTOR ###############
public:
	ConfigManager();
	~ConfigManager();

	enum DebugLevel
	{
		NO_OUTPUT,
		NOT_IN_LOOPS,
		VERBOSE
	};

private:
	std::map<std::string, std::string> m_startupConfig;
	DebugLevel m_debugLevel;

	tinyxml2::XMLDocument m_xmlFile;
	tinyxml2::XMLElement const* m_startUpConfigNode;


public:
	DebugLevel getDebugLevel();
	//TODO: Remove Warning for not dll exported stl class ->look here: http://stackoverflow.com/questions/4145605/stdvector-needs-to-have-dll-interface-to-be-used-by-clients-of-class-xt-war
	std::string getStringFromStartupConfig(std::string mapKeyIn);
	float getFloatFromStartupConfig(std::string mapKeyIn);
	int getIntFromStartupConfig(std::string mapKeyIn);
	bool getBoolFromStartupConfig(std::string mapKeyIn);
	//osg::Vec3f getVec3fFromStartupConfig(std::string mapKeyIn);
	//osg::Vec4f getVec4fFromStartupConfig(std::string mapKeyIn);


	void writeToConfig(std::string mapKeyIn, std::string value);

	bool readConfigFile(const char * filePath);

	static void printDebugMessage(const char * message);

};