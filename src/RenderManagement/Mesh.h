#pragma once

#include <iostream>

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/qopenglshaderprogram.h>

#include "Entity.h"

/*!
 * \class Mesh
 *
 * \brief mesh base class
 *
 * \note derives from entity class
 *
 * \author Eric Jansen
 */
class Mesh : public Entity
{

	public:
		Mesh();
		~Mesh();

		/*!
		 * 
		 * binds mesh for drawing
		 * 
		 * \param view perspective matrix
		 */
		void bind(QMatrix4x4 matrix);
		/*!
		 * 
		 * release mesh after drawing
		 * 
		 */
		void release();
		int getFaceCount();

		bool isActive();
		void setActive(bool active);

		/*!
		 * 
		 * sets confidence shader color
		 * 
		 * \param color
		 */
		void setDiffuseColor(Vector3 color);

	protected:
		/*!
		 * 
		 * vertex data
		 * 
		 */
		QVector<GLfloat> m_vertexData;
		/*!
		 * 
		 * face count
		 * 
		 */
		int m_faceCount;

		/*!
		 * 
		 * vertex buffer object
		 * 
		 */
		QOpenGLBuffer m_vbo;
		/*!
		 * 
		 * shader texture
		 * 
		 */
		QOpenGLTexture* m_texture;
		/*!
		 * 
		 * shader
		 * 
		 */
		QOpenGLShaderProgram* m_shaderProgram;

		/*!
		 * 
		 * init: fill vbo with vertex data
		 * 
		 */
		void init();

		bool m_isActive = true;
		
		/*!
		 * 
		 * diffuse color of the shader
		 * 
		 */
		Vector3 m_diffuseColor = Vector3::one();

};