#pragma once

#include <iostream>
#include "Vector3.h"

class Joint
{

private:
	std::string m_key;
	Vector3 m_position;
	Vector3 m_rotation;

public:
	Joint();
	Joint(std::string key);
};