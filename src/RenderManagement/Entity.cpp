#include "Entity.h"

#include <iostream>

Entity::Entity()
{

	// init entity and set model matrix to identity matrix
	m_matrix.setToIdentity();

}

Entity::~Entity()
{



}

void Entity::translate(Vector3f position)
{

	m_matrix.translate(position.x(), position.y(), position.z());

}

Vector3f Entity::getPosition()
{

	return Vector3f(m_matrix.column(3)[0], m_matrix.column(3)[1], m_matrix.column(3)[2]);

}

void Entity::rotate(Vector3f roation)
{

	m_matrix.rotate(roation.x(), 1, 0, 0);
	m_matrix.rotate(roation.y(), 0, 1, 0);
	m_matrix.rotate(roation.z(), 0, 0, 1);

}

Vector3f Entity::getRotation()
{

	return Vector3f(
					atan(m_matrix.column(0)[1] / m_matrix.column(0)[0]), 
					atan(m_matrix.column(1)[2] / m_matrix.column(2)[2]), 
					asin(-1 * m_matrix.column(0)[2])
				  );

}

QMatrix4x4* Entity::getMatrix()
{

	return &m_matrix;

}

Vector3f Entity::getScale()
{

	return Vector3f(m_matrix.column(0)[0], m_matrix.column(1)[1], m_matrix.column(1)[1]);

}

void Entity::scale(Vector3f scale)
{

	m_matrix.scale(scale.x(), scale.y(), scale.z());

}

void Entity::scale(float uniformScale)
{

	m_matrix.scale(uniformScale);

}

void Entity::setPosition(Vector3f position)
{

	m_matrix.setColumn(3, QVector4D(position.x(), position.y(), position.z(), m_matrix.column(3)[3]));

}

void Entity::setRotation(Quaternionf quaternion)
{


}

void Entity::setScale(Vector3f scalar)
{

	m_matrix.setColumn(0, QVector4D(scalar.x(), m_matrix.column(0)[1], m_matrix.column(0)[2], m_matrix.column(0)[3]));
	m_matrix.setColumn(1, QVector4D(m_matrix.column(1)[0], scalar.y(), m_matrix.column(1)[2], m_matrix.column(1)[3]));
	m_matrix.setColumn(2, QVector4D(m_matrix.column(2)[0], m_matrix.column(2)[1], scalar.z(), m_matrix.column(2)[3]));

}
void Entity::setScale(float uniformScalar)
{

	m_matrix.setColumn(0, QVector4D(uniformScalar, m_matrix.column(0)[1], m_matrix.column(0)[2], m_matrix.column(0)[3]));
	m_matrix.setColumn(1, QVector4D(m_matrix.column(1)[0], uniformScalar, m_matrix.column(1)[2], m_matrix.column(1)[3]));
	m_matrix.setColumn(2, QVector4D(m_matrix.column(2)[0], m_matrix.column(2)[1], uniformScalar, m_matrix.column(2)[3]));

}