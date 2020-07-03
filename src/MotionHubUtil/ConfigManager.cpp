// ClayMore - Immersive Mesh Modelling --- Copyright (c) 2014-2017 Philipp Ladwig, Jannik Fiedler, Jan Beutgen
#include "ConfigManager.h"
#include "StringParser.h"

#include <iostream>

//Must be declared in .cpp since it is static


ConfigManager::ConfigManager()
{

}

ConfigManager::~ConfigManager()
{

}

ConfigManager::DebugLevel ConfigManager::getDebugLevel()
{
	return m_debugLevel;
}

std::string ConfigManager::getStringFromStartupConfig(std::string mapKeyIn)
{
	auto it = m_startupConfig.find(mapKeyIn);
	if (it == m_startupConfig.end())
	{
		std::cout << "WARNING MotionHub: No key found for " << mapKeyIn << " in m_startupConfig[mapKey] in ConfigReader" <<  std::endl;
	}
	return m_startupConfig[mapKeyIn];
}

float ConfigManager::getFloatFromStartupConfig(std::string mapKeyIn)
{
	return StringParser::parseFloat(m_startupConfig[mapKeyIn]);
}

int ConfigManager::getIntFromStartupConfig(std::string mapKeyIn)
{
	return StringParser::parseInt(m_startupConfig[mapKeyIn]);
}

bool ConfigManager::getBoolFromStartupConfig(std::string mapKeyIn)
{
	if (getStringFromStartupConfig(mapKeyIn).compare("true") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//osg::Vec3f ConfigReader::getVec3fFromStartupConfig(std::string mapKeyIn)
//{
//	osg::Vec3f output;
//	StringParser::parseVec3f(m_startupConfig[mapKeyIn], output);
//	return output;
//}
//
//osg::Vec4f ConfigReader::getVec4fFromStartupConfig(std::string mapKeyIn)
//{
//	osg::Vec4f output;
//	StringParser::parseVec4f(m_startupConfig[mapKeyIn], output);
//	return output;
//}

bool ConfigManager::readConfigFile(const char * filePath)
{
	//Read XML file
	int res = m_xmlFile.LoadFile(filePath);

	if (res != tinyxml2::XMLError::XML_SUCCESS)
	{
		std::string errorMessage = " ### ERROR in ConfigReader::readConfigFile() - ErrorCode: ";
		errorMessage.append(std::to_string(res));
		errorMessage.append(" from enum tinyxml2::XMLError.\n");
		if (res == 1)
			errorMessage.append("ErrorCode explaination: XML_NO_ATTRIBUTE");
		if (res == 2)
			errorMessage.append("ErrorCode explaination: XML_WRONG_ATTRIBUTE_TYPE");
		if (res == 3)
			errorMessage.append("ErrorCode explaination: XML_ERROR_FILE_NOT_FOUND -> no config.xml in data directory");
		if (res == 4)
			errorMessage.append("ErrorCode explaination: XML_ERROR_FILE_COULD_NOT_BE_OPENED");
		if (res == 5)
			errorMessage.append("ErrorCode explaination: XML_ERROR_FILE_READ_ERROR");
		if (res == 6)
			errorMessage.append("ErrorCode explaination: XML_ERROR_ELEMENT_MISMATCH");
		if (res == 7)
			errorMessage.append("ErrorCode explaination: XML_ERROR_PARSING_ELEMENT");
		if (res == 8)
			errorMessage.append("ErrorCode explaination: XML_ERROR_PARSING_ATTRIBUTE");
		if (res == 9)
			errorMessage.append("ErrorCode explaination: XML_ERROR_IDENTIFYING_TAG");
		if (res == 10)
			errorMessage.append("ErrorCode explaination: XML_ERROR_PARSING_TEXT");
		if (res == 11)
			errorMessage.append("ErrorCode explaination: XML_ERROR_PARSING_CDATA");
		if (res == 12)
			errorMessage.append("ErrorCode explaination: XML_ERROR_PARSING_COMMENT");
		if (res == 13)
			errorMessage.append("ErrorCode explaination: XML_ERROR_PARSING_DECLARATION");
		if (res == 14)
			errorMessage.append("ErrorCode explaination: XML_ERROR_PARSING_UNKNOWN");
		if (res == 15)
			errorMessage.append("ErrorCode explaination: XML_ERROR_EMPTY_DOCUMENT");
		if (res == 16)
			errorMessage.append("ErrorCode explaination: XML_ERROR_MISMATCHED_ELEMENT -> xml structure may be defective");
		if (res == 17)
			errorMessage.append("ErrorCode explaination: XML_ERROR_PARSING");
		if (res == 18)
			errorMessage.append("ErrorCode explaination: XML_CAN_NOT_CONVERT_TEXT");
		if (res == 19)
			errorMessage.append("ErrorCode explaination: XML_NO_TEXT_NODE");
		if (res == 20)
			errorMessage.append("ErrorCode explaination: XML_ERROR_COUNT");

		std::cerr << errorMessage << std::endl; // ### <--- Have a look in the console ###
		//throw(std::exception(errorMessage.c_str()));
		return false;
	}

	//Loop over all first level nodes
	tinyxml2::XMLElement const *rootElement = m_xmlFile.RootElement();

	for (tinyxml2::XMLElement const *firstLevelNodes = rootElement->FirstChildElement(); firstLevelNodes != NULL; firstLevelNodes = firstLevelNodes->NextSiblingElement())
	{
		//parse the startup config first level node
		if (strcmp(firstLevelNodes->Name(), "startup_config") == 0)
		{

			m_startUpConfigNode = firstLevelNodes;

			//Loop over all startup_config attributes
			for (tinyxml2::XMLElement const *secondLevelNodes = firstLevelNodes->FirstChildElement(); secondLevelNodes != NULL; secondLevelNodes = secondLevelNodes->NextSiblingElement())
			{
				//Save in seperate enum since this will be used during runtime

				if (strcmp(secondLevelNodes->Name(), "debug_level") == 0)
				{
					if (strcmp(secondLevelNodes->GetText(), "VERBOSE") == 0)
						m_debugLevel = VERBOSE;
					else if (strcmp(secondLevelNodes->GetText(), "NOT_IN_LOOPS") == 0)
						m_debugLevel = NOT_IN_LOOPS;
					else
						m_debugLevel = NO_OUTPUT;
				}
				else //Other config will be saved in a public static map, so everyone can access the config attribute they need..
					m_startupConfig.insert(std::pair<std::string, std::string>(secondLevelNodes->Name(), secondLevelNodes->GetText()));
			}
		}
	}
	return true;
}

void ConfigManager::printDebugMessage(const char * message)
{
	std::cout << message << std::endl;
}


void ConfigManager::writeToConfig(std::string mapKeyIn, std::string value)
{


	m_startupConfig[mapKeyIn] = value;

	tinyxml2::XMLElement* currElem = m_xmlFile.RootElement()->FirstChildElement("startup_config")->FirstChildElement(mapKeyIn.c_str())->ToElement();


	currElem->SetText(value.c_str());
	m_xmlFile.SaveFile(CONFIG_PATH);

}

void ConfigManager::createNewConfigFile()
{

	//create new Document
	tinyxml2::XMLDocument newXmlDoc;

	//create root node and add it to Doc
	tinyxml2::XMLNode* pRoot = newXmlDoc.NewElement("config");
	newXmlDoc.InsertFirstChild(pRoot);

	//create stratup node and add it to root
	tinyxml2::XMLNode* pStartup = newXmlDoc.NewElement("startup_config");
	pRoot->InsertFirstChild(pStartup);

	//create ipAddress Element to startup and set value to LOCALHOST
	tinyxml2::XMLElement* ipAddress = newXmlDoc.NewElement("ipAddress");
	ipAddress->SetText("127.0.0.1");
	pStartup->InsertEndChild(ipAddress);

	newXmlDoc.SaveFile(CONFIG_PATH);

	Console::log("ConfigManager::createNewConfigFile(): Created new config.xml");


}
