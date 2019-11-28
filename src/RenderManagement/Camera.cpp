#include "Camera.h"

Camera::Camera()
{

	m_rotation = Vector3f(0, 0, 0);

}

Camera::~Camera()
{



}

void Camera::addRotation(Vector3f rotation)
{

	m_rotation = m_rotation + rotation;

}

void Camera::rotate()
{

	m_matrix.rotate(m_rotation.x(), 1, 0, 0);
	m_matrix.rotate(m_rotation.y(), 0, 1, 0);
	m_matrix.rotate(m_rotation.z(), 0, 0, 1);

}