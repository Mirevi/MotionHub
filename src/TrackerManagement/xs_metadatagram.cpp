/*! \file
	\section FileCopyright Copyright Notice
	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or
	distribute this software, either in source code form or as a compiled
	binary, for any purpose, commercial or non-commercial, and by any
	means.

	In jurisdictions that recognize copyright laws, the author or authors
	of this software dedicate any and all copyright interest in the
	software to the public domain. We make this dedication for the benefit
	of the public at large and to the detriment of our heirs and
	successors. We intend this dedication to be an overt act of
	relinquishment in perpetuity of all present and future rights to this
	software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#include "xs_metadatagram.h"

/*! \class MetaDatagram
	Meta data (type 0x12)
	This packet contains some meta-data about the character. This is in a tagged format, each tag is formatted as “tagname:tagdata” and each tagline is terminated by a newline. Each value is a string that can be interpreted in its own way.
*/

/*! Construct a meta data datagram */
MetaDatagram::MetaDatagram()
	: Datagram()
{
	setType(SPMetaMoreMeta);
	setDataCount(1);
}

/*! Destructor */
MetaDatagram::~MetaDatagram()
{
}

/*! Return the item data for \a itemName

	The items are stored in a std::map, which means one has to check whether items are available before using
	this function.
	\sa hasItem addItem
*/
std::string MetaDatagram::itemData(const std::string &itemName) const
{
	if(hasItem(itemName))
	{
		//element found;
		return m_items.find(itemName)->second;
	}
	return std::string();
}

/*! Return whether the datagram has the item identified by \a itemName
	\sa itemData addItem
*/
bool MetaDatagram::hasItem(const std::string &itemName) const
{
	return m_items.find(itemName) != m_items.end();
}

/*! Deserialize the data */
void MetaDatagram::deserializeData(Streamer &inputStreamer)
{
	Streamer* streamer = &inputStreamer;

	int stringSize = 0;
	streamer->read(stringSize);

	std::string in;
	streamer->read(in, stringSize);

	std::map<std::string, std::string> out;

	// split string
	std::vector<std::string> input = split(in, '\n');

	for (int i=0; i < input.size(); i++)
	{
		std::string &item = input.at(i);
		std::vector<std::string> keyvalue = split(item, ':');
		std::string key, value;

		if (keyvalue.size() == 2)
		{
			// expected
			key = keyvalue[0];
			value = keyvalue[1];
		}
		else if (keyvalue.size() > 2)
		{
			// apparently someone put some colons in there
			// how nice of us to take this into account
			key = keyvalue[0];
			keyvalue.erase(keyvalue.begin(), keyvalue.begin());
			value = join(keyvalue,":");
		}
		else
			continue; // report error?

		out.insert(std::pair<std::string,std::string>(key, value));
	}
	m_items = out;
}

/*! Return the amount of items in this datagram

	\note This does not return the same as dataCount, which returns the value stored in the header of the datagram (1 string), but
	the amount of data items that are stored in the string.
*/
int MetaDatagram::itemCount() const
{
	return m_items.size();
}


/*! Print Data datagram in a formatted way
*/
void MetaDatagram::printData() const
{
	if (itemCount())
	{
		// name: contains the name as displayed in MVN Studio
		if (hasItem("name"))
			std::cout << "Suit Name: " << itemData("name") << std::endl;

		// color: contains the color of the character as used in MVN Studio, the format is hex RRGGBB
		if (hasItem("color"))
			std::cout << "Suit color: " << "#" + itemData("color") << std::endl;

		// xmid: contains the BodyPack/Awinda-station ID as shown in MVN Studio
		if (hasItem("xmid"))
		{
			int did = std::stoi(itemData("xmid"),nullptr,16);
			std::cout << "Device ID: " <<   std::to_string(did) << std::endl;
		}
	}
}

//  Splits the string into substrings wherever sep occurs, and returns the list of those strings.
std::vector<std::string> MetaDatagram::split(std::string str, char delimiter)
{
	std::vector<std::string> internal;
	std::stringstream ss(str); // Turn the string into a stream.
	std::string tok;

	while(getline(ss, tok, delimiter))
		internal.push_back(tok);

	return internal;
}


// Joins all the string list's strings into a single string with each element separated by the given separator
std::string MetaDatagram::join(std::vector<std::string> strList, char* delimiter)
{
	std::string res;
	for (int i = 0; strList.size(); i++)
	{
		res.append(strList.at(i));
		res.append(delimiter);
	}

	return res;
}
