#pragma once

#include "MotionHubUtil/Vector3.h"
#include <QtGui/qmatrix4x4.h>

class Entity
{

public:
	Entity();
	~Entity();

	QMatrix4x4* getMatrix();

	void translate(Vector3 position);
	Vector3 getPosition();

	void rotate(Vector3 rotation);
	Vector3 getRotation();

	void scale(Vector3 scale);
	void scale(float uniformScale);
	Vector3 getScale();

protected:
	QMatrix4x4 m_matrix;

};