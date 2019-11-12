#pragma once

static const char* CONFIG_PATH = "./data/config.xml";

static const char* LOGFILE = "log.txt";

static std::string toString(bool state)
{ 
	return state ? "true" : "false";

}

static std::string toString(int number)
{ 

	return std::to_string(number);

}