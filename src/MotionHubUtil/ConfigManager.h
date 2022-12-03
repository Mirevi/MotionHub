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
#include "MMHmath.h"

/*class MotionHubUtil_DLL_import_export ConfigManager
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


	void writeToConfig(std::string mapKeyIn, std::string value, std::string trackerType = "");




	void createNewConfigFile();

	bool readConfigFile(const char * filePath);

	static void printDebugMessage(const char * message);






};*/

class MotionHubUtil_DLL_import_export ConfigManager {
private:
	tinyxml2::XMLDocument m_config;

	inline bool isLoaded();
	bool loadConfig();
	bool saveConfig();

	tinyxml2::XMLElement* findElement(std::string name, std::string identifier = "", tinyxml2::XMLElement* parent = nullptr);
	tinyxml2::XMLElement* findOrCreateElement(std::string name, std::string identifier = "", tinyxml2::XMLElement* parent = nullptr);

public:
	void refresh();

	bool exists(std::string parent, std::string identifier);
	bool exists(std::string name, std::string parent, std::string identifier);

	bool readString(std::string name, std::string& out, std::string parent = "", std::string identifier = "");
	bool readBool(std::string name, bool& out, std::string parent = "", std::string identifier = "");
	bool readInt(std::string name, int& out, std::string parent = "", std::string identifier = "");
	bool readFloat(std::string name, float& out, std::string parent = "", std::string identifier = "");
	bool readVec3f(std::string name, Vector3f& out, std::string parent = "", std::string identifier = "");
	bool readVec4f(std::string name, Vector4f& out, std::string parent = "", std::string identifier = "");

	bool writeString(std::string name, std::string value, std::string parent = "", std::string identifier = "");
	bool writeBool(std::string name, bool value, std::string parent = "", std::string identifier = "");
	bool writeInt(std::string name, int value, std::string parent = "", std::string identifier = "");
	bool writeFloat(std::string name, float value, std::string parent = "", std::string identifier = "");
	bool writeVec3f(std::string name, Vector3f value, std::string parent = "", std::string identifier = "");
	bool writeVec4f(std::string name, Vector4f value, std::string parent = "", std::string identifier = "");

	template<class T> bool write(std::string name, T value, std::string parent = "", std::string identifier = "", bool enableTypeWarning = false) // Needs to be defined in the header, due to template
	{
		if (std::is_same_v<T, std::string>) return writeString(name, *((std::string*) &value), parent, identifier);
		else if (std::is_same_v<T, bool>) return writeBool(name, *((bool*) &value), parent, identifier);
		else if (std::is_same_v<T, int>) return writeInt(name, *((int*) &value), parent, identifier);
		else if (std::is_same_v<T, float>) return writeFloat(name, *((float*) &value), parent, identifier);
		else if (std::is_same_v<T, Vector3f>) return writeVec3f(name, *((Vector3f*) &value), parent, identifier);
		else if (std::is_same_v<T, Vector4f>) return writeVec4f(name, *((Vector4f*) &value), parent, identifier);
		if (enableTypeWarning) std::cerr << "ERROR: could not write to config: The given type is not supported" << std::endl;
		return false;
	}
};