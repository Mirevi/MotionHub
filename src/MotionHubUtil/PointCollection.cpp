#include "PointCollection.h"

void PointCollection::addPoint(int id, Point::PointType type) {
	
	// Create Point from values
	Point point = Point(Vector3f::Zero(), Quaternionf::Identity(), type);

	addPoint(id, point);
}

void PointCollection::addPoint(int id, Point point) {

	points[id] = point;
}

void PointCollection::updatePoint(int id, Vector3f position, Quaternionf rotation) {

	Point &point = points[id];

	point.setPosition(position);
	point.setRotation(rotation);
}

void PointCollection::updatePoint(int id, Point::PointType type) {

	Point& point = points[id];

	point.setType(type);
}

void PointCollection::updatePoint(int id, Joint::JointNames jointName) {

	Point& point = points[id];

	point.setJointName(jointName);
}


bool PointCollection::pointExists(int id) {

	return points.find(id) != points.end();
}

void PointCollection::clear() {

	points.clear();
}