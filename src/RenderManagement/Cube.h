#pragma once

#include "Mesh.h"
#include "MotionHubUtil/Console.h"

class Cube : public Mesh
{

public:
	/*!
	 * 
	 * default constructor
	 * 
	 */
	Cube();
	/*!
	 * 
	 * standard constructor
	 * 
	 * \param shaderProgram
	 * \param texture
	 * \param start position
	 * \param start scale
	 */
	Cube(QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3 position = Vector3::zero(), Vector3 scale = Vector3::one());
	~Cube();

private:
	void initVerts();

};