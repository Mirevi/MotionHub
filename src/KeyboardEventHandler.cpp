#include "KeyboardEventHandler.h"
#include <iostream>
#include <osg/TexMat>

KeyboardEventHandler::KeyboardEventHandler(float* value) : m_value(value)
{

}


bool KeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
	{
		switch (ea.getKey())
		{
		case 'u':
		{
			std::cout << " U key pressed" << std::endl;
			*m_value += 1.1f;
		}
		return false;
		break;

		case 'i':
		{
			std::cout << " I key pressed" << std::endl;
			*m_value -= 1.1f;
		}
		return false;
		break;
		default:
			return false;
		}
	}
	default:
		return false;
	}
	return false;
}