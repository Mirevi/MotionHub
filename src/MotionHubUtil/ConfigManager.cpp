// ClayMore - Immersive Mesh Modelling --- Copyright (c) 2014-2017 Philipp Ladwig, Jannik Fiedler, Jan Beutgen
#include "ConfigManager.h"
#include "StringParser.h"

#include <iostream>

//Must be declared in .cpp since it is static


/*ConfigManager::ConfigManager()
{

	// load config file
	if (!readConfigFile(CONFIG_PATH))
	{

		Console::logError("MotionHub::MotionHub(): no config.xml found");

		createNewConfigFile();

		if (!readConfigFile(CONFIG_PATH))
		{

			Console::logError("MotionHub::MotionHub(): cannot create config.xml");

		}

	}

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
				{

					if (secondLevelNodes->GetText() != NULL)
					{

						m_startupConfig.insert(std::pair<std::string, std::string>(secondLevelNodes->Name(), secondLevelNodes->GetText()));

					}
				}
			}


		}
	}
	return true;
}

void ConfigManager::printDebugMessage(const char * message)
{
	std::cout << message << std::endl;
}


void ConfigManager::writeToConfig(std::string mapKeyIn, std::string value, std::string trackerType)
{


	m_startupConfig[mapKeyIn + trackerType] = value;

	tinyxml2::XMLElement* currElem = m_xmlFile.RootElement()->FirstChildElement("startup_config")->FirstChildElement((mapKeyIn + trackerType).c_str())->ToElement();


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

	//create recordPath Element to startup and set value to /data/
	tinyxml2::XMLElement* recordPath = newXmlDoc.NewElement("recordPath");
	recordPath->SetText("./data/");
	pStartup->InsertEndChild(recordPath);



	//create Azure offset Elements to startup and set value
	tinyxml2::XMLElement* xPosAzure = newXmlDoc.NewElement("xPosAzure");
	tinyxml2::XMLElement* yPosAzure = newXmlDoc.NewElement("yPosAzure");
	tinyxml2::XMLElement* zPosAzure = newXmlDoc.NewElement("zPosAzure");
	tinyxml2::XMLElement* xRotAzure = newXmlDoc.NewElement("xRotAzure");
	tinyxml2::XMLElement* yRotAzure = newXmlDoc.NewElement("yRotAzure");
	tinyxml2::XMLElement* zRotAzure = newXmlDoc.NewElement("zRotAzure");
	tinyxml2::XMLElement* xSclAzure = newXmlDoc.NewElement("xSclAzure");
	tinyxml2::XMLElement* ySclAzure = newXmlDoc.NewElement("ySclAzure");
	tinyxml2::XMLElement* zSclAzure = newXmlDoc.NewElement("zSclAzure");

	//create OptiTrack offset Elements to startup
	tinyxml2::XMLElement* xPosOptiTrack = newXmlDoc.NewElement("xPosOptiTrack");
	tinyxml2::XMLElement* yPosOptiTrack = newXmlDoc.NewElement("yPosOptiTrack");
	tinyxml2::XMLElement* zPosOptiTrack = newXmlDoc.NewElement("zPosOptiTrack");
	tinyxml2::XMLElement* xRotOptiTrack = newXmlDoc.NewElement("xRotOptiTrack");
	tinyxml2::XMLElement* yRotOptiTrack = newXmlDoc.NewElement("yRotOptiTrack");
	tinyxml2::XMLElement* zRotOptiTrack = newXmlDoc.NewElement("zRotOptiTrack");
	tinyxml2::XMLElement* xSclOptiTrack = newXmlDoc.NewElement("xSclOptiTrack");
	tinyxml2::XMLElement* ySclOptiTrack = newXmlDoc.NewElement("ySclOptiTrack");
	tinyxml2::XMLElement* zSclOptiTrack = newXmlDoc.NewElement("zSclOptiTrack");

	//set Azure values
	xPosAzure->SetText("0");
	yPosAzure->SetText("1.175");
	zPosAzure->SetText("2.2");
	xRotAzure->SetText("-5.0");
	yRotAzure->SetText("0");
	zRotAzure->SetText("0");
	xSclAzure->SetText("0.0010");
	ySclAzure->SetText("-0.0010");
	zSclAzure->SetText("-0.0010");

	//set OT values
	xPosOptiTrack->SetText("0");
	yPosOptiTrack->SetText("0.1");
	zPosOptiTrack->SetText("0");
	xRotOptiTrack->SetText("0");
	yRotOptiTrack->SetText("0");
	zRotOptiTrack->SetText("0");
	xSclOptiTrack->SetText("1");
	ySclOptiTrack->SetText("1");
	zSclOptiTrack->SetText("1");

	//add Azure to node
	pStartup->InsertEndChild(xPosAzure);
	pStartup->InsertEndChild(yPosAzure);
	pStartup->InsertEndChild(zPosAzure);
	pStartup->InsertEndChild(xRotAzure);
	pStartup->InsertEndChild(yRotAzure);
	pStartup->InsertEndChild(zRotAzure);
	pStartup->InsertEndChild(xSclAzure);
	pStartup->InsertEndChild(ySclAzure);
	pStartup->InsertEndChild(zSclAzure);

	//add OT to node
	pStartup->InsertEndChild(xPosOptiTrack);
	pStartup->InsertEndChild(yPosOptiTrack);
	pStartup->InsertEndChild(zPosOptiTrack);
	pStartup->InsertEndChild(xRotOptiTrack);
	pStartup->InsertEndChild(yRotOptiTrack);
	pStartup->InsertEndChild(zRotOptiTrack);
	pStartup->InsertEndChild(xSclOptiTrack);
	pStartup->InsertEndChild(ySclOptiTrack);
	pStartup->InsertEndChild(zSclOptiTrack);




	//save file at the CONFIG_PATH directory, throw Error if it fails
	if (newXmlDoc.SaveFile(CONFIG_PATH) == tinyxml2::XMLError::XML_SUCCESS)
	{

		Console::log("ConfigManager::createNewConfigFile(): Created new config.xml");


	}
	else
	{

		Console::logError("ConfigManager::createNewConfigFile(): ERROR saving file");

	}



}*/

bool ConfigManager::isLoaded()
{
	return (m_config.RootElement() != nullptr);
}

bool ConfigManager::loadConfig()
{
	if (isLoaded()) return true;

	//Read XML file
	tinyxml2::XMLError result = m_config.LoadFile(CONFIG_PATH);

	switch (result) {
	case tinyxml2::XMLError::XML_SUCCESS:
		return true;

	case tinyxml2::XMLError::XML_ERROR_FILE_NOT_FOUND:
		m_config.InsertFirstChild(m_config.NewElement("config"));
		return true;

	default:
		std::cerr << "ERROR: could not load config: " << m_config.ErrorIDToName(result) << std::endl;
		return false;
	}
}

bool ConfigManager::saveConfig()
{
	if (isLoaded()) return (m_config.SaveFile(CONFIG_PATH) == tinyxml2::XMLError::XML_SUCCESS);
	else return false;
}


tinyxml2::XMLElement* ConfigManager::findElement(std::string name, std::string identifier, tinyxml2::XMLElement* parent)
{
	if (!loadConfig() || name == "") return nullptr;

	if (parent == nullptr) parent = m_config.RootElement();
	tinyxml2::XMLElement* element = parent->FirstChildElement();

	while (element) {
		const char* elementName = element->Name();
		const char* elementId = element->Attribute("id");
		if (std::string(elementName) == name && ((elementId == nullptr && identifier == "") || (elementId != nullptr && std::string(elementId) == identifier))) break;
		element = element->NextSiblingElement();
	}
	return element;
}

tinyxml2::XMLElement* ConfigManager::findOrCreateElement(std::string name, std::string identifier, tinyxml2::XMLElement* parent)
{
	if (!loadConfig() || name == "") return nullptr;
	tinyxml2::XMLElement* element = findElement(name, identifier, parent);

	if (element == nullptr) {
		if (parent == nullptr) parent = m_config.RootElement();
		element = m_config.NewElement(name.c_str());
		if (identifier != "") element->SetAttribute("id", identifier.c_str());
		parent->InsertEndChild(element);
	}
	return element;
}

bool ConfigManager::exists(std::string parent, std::string identifier)
{
	tinyxml2::XMLElement* parentElement = findElement(parent, identifier);
	return parentElement != nullptr;
}

bool ConfigManager::exists(std::string name, std::string parent, std::string identifier)
{
	tinyxml2::XMLElement* parentElement = findElement(parent, identifier);
	if (parent != "" && parentElement == nullptr) return false;
	tinyxml2::XMLElement* element = findElement(name, "", parentElement);
	return element != nullptr;
}

bool ConfigManager::readString(std::string name, std::string& out, std::string parent, std::string identifier)
{
	if (!loadConfig()) return false;
	tinyxml2::XMLElement* parentElement = findElement(parent, identifier);
	if (parent != "" && parentElement == nullptr) return false;
	tinyxml2::XMLElement* element = findElement(name, "", parentElement);
	if (element != nullptr) {
		out = element->GetText();
		return true;
	}
	return false;
}

bool ConfigManager::readBool(std::string name, bool& out, std::string parent, std::string identifier)
{
	std::string outStr;
	if (readString(name, outStr, parent, identifier)) {
		out = (outStr == "true");
		return true;
	} else return false;
}

bool ConfigManager::readInt(std::string name, int& out, std::string parent, std::string identifier)
{
	std::string outStr;
	if (readString(name, outStr, parent, identifier)) {
		out = std::stoi(outStr);
		return true;
	} else return false;
}

bool ConfigManager::readFloat(std::string name, float& out, std::string parent, std::string identifier)
{
	std::string outStr;
	if (readString(name, outStr, parent, identifier)) {
		out = std::stof(outStr);
		return true;
	} else return false;
}

bool ConfigManager::readVec3f(std::string name, Vector3f& out, std::string parent, std::string identifier)
{
	std::string outStr;
	if (readString(name, outStr, parent, identifier)) {
		Vector3f v;
		try {
			size_t l = 0;
			size_t r = 0;
			for (int i = 0; i < 3; i++) {
				if (r == outStr.npos) return false;
				l = r;
				r = outStr.find(" ", l);
				v[i] = stof(outStr.substr(l, r - l));
				++r;
			}
		} catch (const std::invalid_argument& e) {
			return false;
		}
		//std::cout << std::to_string(v.x()) << ", " << std::to_string(v.y()) << ", " << std::to_string(v.z()) << std::endl;
		out = v;
		return true;
	} else return false;
}

bool ConfigManager::readVec4f(std::string name, Vector4f& out, std::string parent, std::string identifier)
{
	std::string outStr;
	if (readString(name, outStr, parent, identifier)) {
		Vector4f v;
		try {
			size_t l = 0;
			size_t r = 0;
			for (int i = 0; i < 4; i++) {
				if (r == outStr.npos) return false;
				l = r;
				r = outStr.find(" ", l);
				v[i] = stof(outStr.substr(l, r - l));
				++r;
			}
		} catch (const std::invalid_argument& e) {
			return false;
		}
		//std::cout << std::to_string(v.x()) << ", " << std::to_string(v.y()) << ", " << std::to_string(v.z()) << ", " << std::to_string(v.w()) << std::endl;
		out = v;
		return true;
	} else return false;
}



bool ConfigManager::writeString(std::string name, std::string value, std::string parent, std::string identifier)
{
	if (!loadConfig()) return false;
	tinyxml2::XMLElement* element = findOrCreateElement(name, "", findOrCreateElement(parent, identifier));
	if (element != nullptr) {
		element->SetText(value.c_str());
		return saveConfig();
	}
	return false;
}

bool ConfigManager::writeBool(std::string name, bool value, std::string parent, std::string identifier)
{
	if (value) return writeString(name, "true", parent, identifier);
	else return writeString(name, "false", parent, identifier);
}

bool ConfigManager::writeInt(std::string name, int value, std::string parent, std::string identifier)
{
	return writeString(name, std::to_string(value), parent, identifier);
}

bool ConfigManager::writeFloat(std::string name, float value, std::string parent, std::string identifier)
{
	return writeString(name, std::to_string(value), parent, identifier);
}

bool ConfigManager::writeVec3f(std::string name, Vector3f value, std::string parent, std::string identifier)
{
	return writeString(name, std::to_string(value.x()) + " " + std::to_string(value.y()) + " " + std::to_string(value.z()), parent, identifier);
}

bool ConfigManager::writeVec4f(std::string name, Vector4f value, std::string parent, std::string identifier)
{
	return writeString(name, std::to_string(value.x()) + " " + std::to_string(value.y()) + " " + std::to_string(value.z()) + " " + std::to_string(value.w()), parent, identifier);
}