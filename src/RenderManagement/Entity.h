#pragma once

#include "MotionHubUtil/Vector3.h"
#include "MotionHubUtil/Vector4.h"
#include <QtGui/qmatrix4x4.h>

class Entity
{

public:
	Entity();
	~Entity();

	QMatrix4x4* getMatrix();

	void translate(Vector3 position);
	void setPosition(Vector3 position);
	Vector3 getPosition();

	void rotate(Vector3 rotation);
	void setRotation(Vector4 quaternion);
	Vector3 getRotation();

	void scale(Vector3 scalar);
	void scale(float uniformScalar);
	void setScale(Vector3 scalar);
	void setScale(float uniformScalar);
	Vector3 getScale();

protected:
	QMatrix4x4 m_matrix;

};