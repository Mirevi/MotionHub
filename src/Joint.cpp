#include "Joint.h"

Joint::Joint()
{



}

Joint::Joint(std::string key)
{



}

Joint::Joint(Vector3 position, Vector4 rotation)
{
	m_position = position;
	m_rotation = rotation;
}