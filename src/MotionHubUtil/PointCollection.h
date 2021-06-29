#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include <MotionHubUtil/Point.h>
#include <unordered_map>

/*!
 * \class Points
 *
 * \brief List for multiple Point objects
 *
 * \author David Nowottnik
 */
class MotionHubUtil_DLL_import_export PointCollection {

public:

	/*!
	 * Adds a point to the collection with a specific type
	 * 
	 * \param id the point id
	 * \param type the point type
	 */
	void addPoint(int id, Point::PointType type);

	/*!
	 * Adds a point to the collection with a specific type
	 *
	 * \param id the point id
	 * \param type the point
	 */
	void addPoint(int id, Point point);

	/*!
	 * Sets the position & rotation of a point
	 *
	 * \param id the point id
	 * \param position the point position
	 * \param rotation the point rotation
	 */
	void updatePoint(int id, Vector3f position, Quaternionf rotation);

	/*!
	 * Sets the type of a point
	 *
	 * \param id the point id
	 * \param type the point type
	 */
	void updatePoint(int id, Point::PointType type);

	/*!
	 * Sets the joint of a point
	 *
	 * \param id the point id
	 * \param jointName the joint
	 */
	void updatePoint(int id, Joint::JointNames jointName);

	/*!
	 * Checks if the point id exsists in the collection
	 *
	 * \param id the point id
	 * \return true if the point exists, false if not
	 */
	bool pointExists(int id);

	/*!
	 * Clears all points from collection 
	 */
	void clear();

public:

	/*!
	 * List of all Points
	 */
	std::unordered_map<int, Point> points;
};
