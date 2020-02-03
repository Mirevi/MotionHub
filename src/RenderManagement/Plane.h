#pragma once

#include "Mesh.h"

/*!
 * \class Plane
 *
 * \brief plane mesh
 *
 * \note derives from mesh class
 *
 * \author Eric Jansen
 */
class Plane : public Mesh
{

public:
	/*!
	 * standard constructor
	 *
	 * \param shader, texture, start position, start rotation
	 */
	Plane(QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3 position = Vector3::zero(), Vector3 scale = Vector3::one());
	~Plane();

};