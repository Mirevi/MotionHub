// ClayMore - Immersive Mesh Modelling --- Copyright (c) 2014-2017 Philipp Ladwig, Jannik Fiedler, Jan Beutgen
#pragma once

#include "ConfigManager.h"
#include <string>

class StringParser
{
	// #### CONSTRUCTOR & DESTRUCTOR ###############
	public:

		StringParser();
		~StringParser();

		//These methods are slow in performance! Should not be used during runtime

		//String must look like this (4 floats delimited by spaces):  0.0 0.0 0.0 0.0
		//static void parseVec4f(std::string stringToParse, osg::Vec4f &targetVector);

		//String must look like this (3 floats delimited by spaces):  0.0 0.0 0.0
		//static void parseVec3f(std::string stringToParse, osg::Vec3f &targetVector);

		//String must look like this: x.x (e.g: 1.0)
		static float parseFloat(std::string &stringToParse);
		
		//String must look like this: x (e.g. 10)
		static float parseInt(std::string &stringToParse);
};