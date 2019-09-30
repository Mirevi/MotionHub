// ClayMore - Immersive Mesh Modelling --- Copyright (c) 2014-2017 Philipp Ladwig, Jannik Fiedler, Jan Beutgen
#pragma once
#pragma warning (disable : 4251)	//--> It's for surpressing the wanring about the need of a dll-interface for the STL map class...
									//... but can be ignored, because theoretically no "client" of configReader will ...
									//... ever use the member m_startupConfig. This means, there is no need for exporting Map to the dll.
									//Before january 2017, it was a static public STL Map, which is now a private member and can't directly accessed
									//by "clients"/"users" of ConfigReader. The warning could be some kind of minor bug in the compiler


//Doxygen Example for a class
/**
 * \class ConfigReader
 *
 * \brief This class reads the ClayMore XML configuration file located in /data
 *
 * \note TODO: Actually it would be useful, if there is a generic parent of 
 * this class (reads every xml and list all its tags) and a specific ClayMore class
 *
 * \author Philipp Ladwig
 *
 * \version $Revision: 1.0 $
 *
 * \date $Date: 2018/12/16 10:20 $
 *
 * Contact: philipp.ladwig@hs-duesseldorf.de	
 *
 * Created on: Before 2017
 *
 */

#include <map>
#include <string>

#include <osg/Array>

#include "tinyxml2.h"

using namespace tinyxml2;

class ConfigReader
{
	// #### CONSTRUCTOR & DESTRUCTOR ###############
public:
	ConfigReader();
	~ConfigReader();

	enum DebugLevel
	{
		NO_OUTPUT,
		NOT_IN_LOOPS,
		VERBOSE
	};

private:
	std::map<std::string, std::string> m_startupConfig;
	DebugLevel m_debugLevel;

public:
	DebugLevel getDebugLevel();
	//TODO: Remove Warning for not dll exported stl class ->look here: http://stackoverflow.com/questions/4145605/stdvector-needs-to-have-dll-interface-to-be-used-by-clients-of-class-xt-war
	std::string getStringFromStartupConfig(std::string mapKeyIn);
	float getFloatFromStartupConfig(std::string mapKeyIn);
	int getIntFromStartupConfig(std::string mapKeyIn);
	bool getBoolFromStartupConfig(std::string mapKeyIn);
	osg::Vec3f getVec3fFromStartupConfig(std::string mapKeyIn);
	osg::Vec4f getVec4fFromStartupConfig(std::string mapKeyIn);

	bool readConfigFile(const char * filePath);

	static void printDebugMessage(const char * message);

};