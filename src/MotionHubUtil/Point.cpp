#include "Point.h"

Point::Point() {

}

Point::Point(Vector3f position, Quaternionf rotation) :
	position(position),
	rotation(rotation),
	type(PointType::Undefined)
{

}

Point::Point(Vector3f position, Quaternionf rotation, PointType type) :
	position(position),
	rotation(rotation),
	type(type)
{

}

void Point::setJointName(Joint::JointNames jointName) {

	setCustomInt((int)jointName);
}

void Point::setCustomInt(int custom) {

	customInt = custom;
}

void Point::setCustomFloat(float custom) {

	customFloat = custom;
}

Vector3f Point::getPosition() {

	return position;
}

void Point::setPosition(Vector3f position) {

	this->position = position;
}

Quaternionf Point::getRotation() {

	return rotation;
}

void Point::setRotation(Quaternionf rotation) {

	this->rotation = rotation;
}

Point::PointType Point::getType() {

	return type;
}

void Point::setType(Point::PointType type) {

	this->type = type;
}

int Point::getCustomInt() {

	return customInt;
}

float Point::getCustomFloat() {

	return customFloat;
}
