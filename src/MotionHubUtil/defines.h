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
 * converts size_t to string
 *
 * \param number input int
 * \return int as string
 */
static std::string toString(size_t number)
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

/*!
* removes all instances of the given char from the string
* 
* \param _str input string
* \param _char the char that should be removed
* 
* \return final string without the char
* 
*/
static std::string removeChar(std::string _str, const char _char)
{
	std::string::iterator end_pos = std::remove(_str.begin(), _str.end(), _char);
	_str.erase(end_pos, _str.end());

	return _str;
}