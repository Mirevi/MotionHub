#pragma once

#include "Entity.h"

class Camera : public Entity
{

	public:
		Camera();
		~Camera();

		void addRotation(Vector3f rotation);
		void rotate();

	private:
		Vector3f m_rotation;

};