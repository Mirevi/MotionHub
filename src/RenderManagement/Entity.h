#pragma once

#include "MotionHubUtil/Vector3.h"
#include <QtGui/qmatrix4x4.h>

class Entity
{

public:
	Entity();
	~Entity();

	void translate(Vector3 position);
	Vector3 getPosition();

	void rotate(Vector3 rotation);
	Vector3 getRotation();

	QMatrix4x4 getModelMatrix();

protected:
	QMatrix4x4 m_modelMatrix;

};