// ClayMore - Immersive Mesh Modelling --- Copyright (c) 2014-2017 Philipp Ladwig, Jannik Fiedler, Jan Beutgen
#include "StringParser.h"

//ConfigReader::DebugLevel DEBUG_LEVEL = ConfigReader::DebugLevel::NO_OUTPUT;

StringParser::StringParser()
{
}

StringParser::~StringParser()
{
}

void StringParser::parseVec4f(std::string stringToParse, osg::Vec4f &targetVector)
{
	//std::cout << "stringToParse: " << stringToParse << std::endl;
	std::string delimiter = " ";

	size_t delimiterPos = 0;
	std::string stringPart;
	unsigned short i = 0;

	while ((delimiterPos = stringToParse.find(delimiter)) != std::string::npos)
	{
		//Get String from 0 to the next delimiter
		stringPart = stringToParse.substr(0, delimiterPos);

		switch (i)
		{
			case 0:
				targetVector.x() = parseFloat(stringPart);
				break;

			case 1:
				targetVector.y() = parseFloat(stringPart);
				break;

			case 2:
				targetVector.z() = parseFloat(stringPart);
				break;
		}

		i++;
		//Delete parsed string so the next part is at position 0
		stringToParse.erase(0, delimiterPos + delimiter.length());
	}
	targetVector.w() = parseFloat(stringToParse);

	//std::cout << "targetVector.x(): " << targetVector.x() << std::endl;
	//std::cout << "targetVector.y(): " << targetVector.y() << std::endl;
	//std::cout << "targetVector.z(): " << targetVector.z() << std::endl;
	//std::cout << "targetVector.w(): " << targetVector.w() << std::endl;

	//if (ConfigReader >= ConfigReader::DebugLevel::NOT_IN_LOOPS && i != 3)
	//	ConfigReader::printDebugMessage(" ### WARNING in StringParser::parseVec4f() - input string has too many or too few delminiter! Output vector might be invalid.");
}

void StringParser::parseVec3f(std::string stringToParse, osg::Vec3f &targetVector)
{
	std::string delimiter = " ";

	size_t delimiterPos = 0;
	std::string stringPart;
	unsigned short i = 0;

	while ((delimiterPos = stringToParse.find(delimiter)) != std::string::npos)
	{
		//Get String from 0 to the next delimiter
		stringPart = stringToParse.substr(0, delimiterPos);

		switch (i)
		{
		case 0:
			targetVector.x() = parseFloat(stringPart);
			break;

		case 1:
			targetVector.y() = parseFloat(stringPart);
			break;
		}

		i++;
		//Delete parsed string so the next part is at position 0
		stringToParse.erase(0, delimiterPos + delimiter.length());
	}
	targetVector.z() = parseFloat(stringToParse);

	//if (DEBUG_LEVEL >= ConfigReader::DebugLevel::NOT_IN_LOOPS && i != 2)
	//	ConfigReader::printDebugMessage(" ### WARNING in StringParser::parseVec3f() - input string has too many or too few delminiter! Output vector might be invalid.");
}

float StringParser::parseFloat(std::string &stringToParse)
{
	return ::atof(stringToParse.c_str());
}

float StringParser::parseInt(std::string &stringToParse)
{
	return ::atoi(stringToParse.c_str());
}