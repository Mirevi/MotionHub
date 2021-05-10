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

#ifndef METADATAGRAM_H
#define METADATAGRAM_H

#include "xs_datagram.h"

class MetaDatagram : public Datagram {
public:
	MetaDatagram();
	virtual ~MetaDatagram();

	virtual void printData() const override;

protected:
	virtual void deserializeData(Streamer &inputStreamer) override;

private:
	std::map<std::string, std::string> m_items;
	std::vector<std::string> split(std::string str, char delimiter);
	std::string MetaDatagram::join(std::vector<std::string> strList, char* delimiter);

	std::string itemData(const std::string &itemName) const;
	bool hasItem(const std::string &itemName) const;
	int itemCount() const;
};

#endif
