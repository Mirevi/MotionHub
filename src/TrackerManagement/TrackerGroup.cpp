#include "TrackerGroup.h"

TrackerGroup::TrackerGroup()
{

}

TrackerGroup::TrackerGroup(int id)
{

	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "trackerGroup_" + std::to_string(id);

	//default is enabled
	m_properties->isEnabled = true;


	//set the offset values
	setPositionOffset(Vector3f(0.0f, 0.0f, 0.0f));
	setRotationOffset(Vector3f(0.0f, 0.0f, 0.0f));
	setScaleOffset(Vector3f(1.0f, 1.0f, 1.0f));

	m_idCam = -1;

}



void TrackerGroup::start()
{

}

void TrackerGroup::stop()
{

}

void TrackerGroup::destroy()
{

	//delete this;

}

void TrackerGroup::addTacker(Tracker* tracker)
{

	m_groupedTracker.push_back(tracker);

}

void TrackerGroup::removeTrackerAt(int position)
{

	m_groupedTracker.erase(m_groupedTracker.begin() + position);

}





void TrackerGroup::init()
{



}

void TrackerGroup::update()
{

}

void TrackerGroup::track()
{

}

void TrackerGroup::mergeSkeletons()
{

	//create new skeleton
	Skeleton* currSkeleton;

	//loop through al joints
	for (int i = 0; i < currSkeleton->m_joints.size(); i++)
	{

		//loop through all tracker to get joint data
		for (auto itTracker = m_groupedTracker.begin(); itTracker != m_groupedTracker.end(); itTracker++)
		{



		}

	}

}