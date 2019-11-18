#include "Entity.h"

Entity::Entity()
{

	m_modelMatrix.setToIdentity();

}

Entity::~Entity()
{



}

void Entity::translate(Vector3 position)
{

	m_modelMatrix.translate(position.m_xyz.x, position.m_xyz.y, position.m_xyz.z);

}

Vector3 Entity::getPosition()
{

	return Vector3(m_modelMatrix.column(3)[0], m_modelMatrix.column(3)[1], m_modelMatrix.column(3)[2]);

}

void Entity::rotate(Vector3 roation)
{

	m_modelMatrix.rotate(roation.m_xyz.x, 1, 0, 0);
	m_modelMatrix.rotate(roation.m_xyz.y, 0, 1, 0);
	m_modelMatrix.rotate(roation.m_xyz.z, 0, 0, 1);

}

Vector3 Entity::getRotation()
{

	return Vector3(
					atan(m_modelMatrix.column(0)[1] / m_modelMatrix.column(0)[0]), 
					atan(m_modelMatrix.column(1)[2] / m_modelMatrix.column(2)[2]), 
					asin(-1 * m_modelMatrix.column(0)[2])
				  );

}

QMatrix4x4 Entity::getModelMatrix()
{

	return m_modelMatrix;

}