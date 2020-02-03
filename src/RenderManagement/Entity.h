#pragma once

#include "MotionHubUtil/Vector3.h"
#include "MotionHubUtil/Vector4.h"
#include <QtGui/qmatrix4x4.h>

/*!
 * \class Entity
 *
 * \brief base class for all objects in openGl window
 *
 * \author Eric Jansen
 */
class Entity
{

public:
	/*!
	 * 
	 * default constructor
	 * 
	 */
	Entity();
	~Entity();

	/*!
	 * 
	 * getter for model matrix
	 * 
	 * \return 
	 */
	QMatrix4x4* getMatrix();

	/*!
	 * 
	 * translate position
	 * 
	 * \param position
	 */
	void translate(Vector3 position);
	/*!
	 * 
	 * set position
	 * 
	 * \param position
	 */
	void setPosition(Vector3 position);
	/*!
	 * 
	 * get position
	 * 
	 * \return 
	 */
	Vector3 getPosition();

	/*!
	 * 
	 * rotate entity
	 * 
	 * \param rotation
	 */
	void rotate(Vector3 rotation);
	/*!
	 * 
	 * set rotation
	 * 
	 * \param quaternion
	 */
	void setRotation(Vector4 quaternion);
	/*!
	 * 
	 * get rotation
	 * 
	 * \return 
	 */
	Vector3 getRotation();

	/*!
	 * 
	 * scale entity
	 * 
	 * \param scalar
	 */
	void scale(Vector3 scalar);
	/*!
	 * 
	 * scale uniform
	 * 
	 * \param uniformScalar
	 */
	void scale(float uniformScalar);
	/*!
	 * 
	 * set scale
	 * 
	 * \param scalar
	 */
	void setScale(Vector3 scalar);
	/*!
	 * 
	 * set scale uniform
	 * 
	 * \param uniformScalar
	 */
	void setScale(float uniformScalar);
	/*!
	 * 
	 * get scale
	 * 
	 * \return 
	 */
	Vector3 getScale();

protected:
	/*!
	 * 
	 * model matrix
	 * 
	 */
	QMatrix4x4 m_matrix;

};