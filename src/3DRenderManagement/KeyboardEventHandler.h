#pragma once

#include <osgGA/GUIEventHandler>

class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
	KeyboardEventHandler::KeyboardEventHandler(float* value);

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);

private:
	float* m_value;
};