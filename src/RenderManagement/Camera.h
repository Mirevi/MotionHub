#pragma once

#include "Entity.h"

class Camera : public Entity
{

	public:
		/*!
		 * 
		 * default constructor
		 * 
		 */
		Camera();
		~Camera();

		/*!
		 * 
		 * add rotation to camera based on previous rotation
		 * 
		 * \param rotation
		 */
		void addRotation(Vector3 rotation);
		/*!
		 * 
		 * rotate camera
		 * 
		 */
		void rotate();

	private:
		/*!
		 * 
		 * current rotation in euler
		 * 
		 */
		Vector3 m_rotation;

};