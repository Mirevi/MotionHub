#pragma once

#include "Entity.h"

class Camera : public Entity
{

	public:
		Camera();
		~Camera();

		void addRotation(Vector3 rotation);
		void rotate();

	private:
		Vector3 m_rotation;

};