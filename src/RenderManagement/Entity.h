#pragma once

#include "MotionHubUtil/MMHmath.h"
#include <QtGui/qmatrix4x4.h>

class Entity
{

public:
	Entity();
	~Entity();

	QMatrix4x4* getMatrix();

	void translate(Vector3f position);
	void setPosition(Vector3f position);
	Vector3f getPosition();

	void rotate(Vector3f rotation);
	void setRotation(Quaternionf quaternion);
	Vector3f getRotation();

	void scale(Vector3f scalar);
	void scale(float uniformScalar);
	void setScale(Vector3f scalar);
	void setScale(float uniformScalar);
	Vector3f getScale();

protected:
	QMatrix4x4 m_matrix;

};