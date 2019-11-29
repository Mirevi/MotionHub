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

void Entity::translate(Vector3 position)
{

	m_matrix.translate(position.m_xyz.x, position.m_xyz.y, position.m_xyz.z);

}

Vector3 Entity::getPosition()
{

	return Vector3(m_matrix.column(3)[0], m_matrix.column(3)[1], m_matrix.column(3)[2]);

}

void Entity::rotate(Vector3 roation)
{

	m_matrix.rotate(roation.m_xyz.x, 1, 0, 0);
	m_matrix.rotate(roation.m_xyz.y, 0, 1, 0);
	m_matrix.rotate(roation.m_xyz.z, 0, 0, 1);

}

Vector3 Entity::getRotation()
{

	return Vector3(
					atan(m_matrix.column(0)[1] / m_matrix.column(0)[0]), 
					atan(m_matrix.column(1)[2] / m_matrix.column(2)[2]), 
					asin(-1 * m_matrix.column(0)[2])
				  );

}

QMatrix4x4* Entity::getMatrix()
{

	return &m_matrix;

}

Vector3 Entity::getScale()
{

	return Vector3(m_matrix.column(0)[0], m_matrix.column(1)[1], m_matrix.column(1)[1]);

}

void Entity::scale(Vector3 scale)
{

	m_matrix.scale(scale.m_xyz.x, scale.m_xyz.y, scale.m_xyz.z);

}

void Entity::scale(float uniformScale)
{

	m_matrix.scale(uniformScale);

}

void Entity::setPosition(Vector3 position)
{

	m_matrix.setColumn(3, QVector4D(position.m_xyz.x, position.m_xyz.y, position.m_xyz.z, m_matrix.column(3)[3]));

}

void Entity::setRotation(Vector4 quaternion)
{


}

void Entity::setScale(Vector3 scalar)
{

	m_matrix.setColumn(0, QVector4D(scalar.m_xyz.x, m_matrix.column(0)[1], m_matrix.column(0)[2], m_matrix.column(0)[3]));
	m_matrix.setColumn(1, QVector4D(m_matrix.column(1)[0], scalar.m_xyz.y, m_matrix.column(1)[2], m_matrix.column(1)[3]));
	m_matrix.setColumn(2, QVector4D(m_matrix.column(2)[0], m_matrix.column(2)[1], scalar.m_xyz.z, m_matrix.column(2)[3]));

}
void Entity::setScale(float uniformScalar)
{

	m_matrix.setColumn(0, QVector4D(uniformScalar, m_matrix.column(0)[1], m_matrix.column(0)[2], m_matrix.column(0)[3]));
	m_matrix.setColumn(1, QVector4D(m_matrix.column(1)[0], uniformScalar, m_matrix.column(1)[2], m_matrix.column(1)[3]));
	m_matrix.setColumn(2, QVector4D(m_matrix.column(2)[0], m_matrix.column(2)[1], uniformScalar, m_matrix.column(2)[3]));

}