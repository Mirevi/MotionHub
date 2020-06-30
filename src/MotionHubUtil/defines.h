#pragma once

static const char* CONFIG_PATH = "./data/config.xml";

static const char* LOGFILE = "log.txt";



/*!
 * converts bool to string
 * 
 * \param state input bool
 * \return bool as string
 */
static std::string toString(bool state)
{ 
	return state ? "true" : "false";

}

/*!
 * converts integer to string
 * 
 * \param number input int
 * \return int as string
 */
static std::string toString(int number)
{ 

	return std::to_string(number);

}

/*!
 * converts float to string
 *
 * \param number input float
 * \return int as string
 */
static std::string toString(float number)
{

	return std::to_string(number);

}