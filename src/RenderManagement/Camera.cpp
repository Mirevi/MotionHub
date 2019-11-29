#include "Camera.h"

Camera::Camera()
{

	m_rotation = Vector3::zero();

}

Camera::~Camera()
{



}

void Camera::addRotation(Vector3 rotation)
{

	m_rotation = m_rotation + rotation;

}

void Camera::rotate()
{

	m_matrix.rotate(m_rotation.m_xyz.x, 1, 0, 0);
	m_matrix.rotate(m_rotation.m_xyz.y, 0, 1, 0);
	m_matrix.rotate(m_rotation.m_xyz.z, 0, 0, 1);

}