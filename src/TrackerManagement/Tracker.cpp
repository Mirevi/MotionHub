#include "Tracker.h"

Tracker::Tracker()
{
	m_properties = new Properties();
}

Tracker::~Tracker()
{
	delete m_properties;
}

#pragma region controls/util

void Tracker::start()
{
	//isTracking is true, so update loop will continue tracking
	m_isTracking = true;

	//start new thread for update loop
	m_trackingThread = new std::thread(&Tracker::update, this);

	SetPriorityClass(m_trackingThread, HIGH_PRIORITY_CLASS);
	SetThreadPriority(m_trackingThread, THREAD_PRIORITY_ABOVE_NORMAL);
}

void Tracker::stop()
{
	//sets isTracking to false, so update loop retruns after current frame
	m_isTracking = false;

	//wait for tracking thread to terminate, then dispose of thread object
	if (m_trackingThread->joinable()) m_trackingThread->join();
	delete m_trackingThread;
}

void Tracker::disable()
{
	//sets isEnabled to false, so the trackingThread doesn't check for new data
	m_isEnabled = false;

	Console::log("Tracker::disable(): Disabled tracker with id = " + std::to_string(m_properties->id));
}

void Tracker::enable()
{
	//sets isEnabled to true, so the trackingThread checks for new data
	m_isEnabled = true;

	Console::log("Tracker::enable(): Enabled tracker with id = " + std::to_string(m_properties->id));
}

void Tracker::clean()
{
	//reset number of skeletons
	m_countDetectedSkeleton = 0;

	m_skeletonPoolLock.lock();
	//clear skeleton pool
	m_skeletonPool.clear();

	m_skeletonPoolLock.unlock();

}

bool Tracker::isTracking()
{
	return m_isTracking;
}

bool Tracker::isEnabled()
{
	return m_isEnabled;
}

void Tracker::cacheSkeletonData()
{

	//lock the skeleton pool and copy it to the cache
	m_skeletonPoolLock.lock();

	m_skeletonPoolCache = m_skeletonPool;

	m_skeletonPoolLock.unlock();

}

std::string Tracker::getTrackerType()
{
	return "";
}

std::string Tracker::getTrackerIdentifier()
{
	return "";
}



#pragma endregion

#pragma region getter/setter

bool Tracker::isDataAvailable()
{

	return m_isDataAvailable;

}

void Tracker::resetIsDataAvailable()
{

	m_isDataAvailable = false;

}

bool Tracker::hasSkeletonPoolChanged()
{

	//resets when it's true
	if (m_hasSkeletonPoolChanged)
	{

		m_hasSkeletonPoolChanged = false;
		return true;

	}
	else
	{

		return false;

	}
}

void Tracker::setSkeletonPoolChanged(bool state)
{

	m_hasSkeletonPoolChanged = state;

}

int Tracker::getNumDetectedSkeletons()
{
	return m_countDetectedSkeleton;
}

Tracker::Properties* Tracker::getProperties()
{

	return m_properties;

}

std::map<int, Skeleton> Tracker::getSkeletonPoolCache()
{

	//lock skeleton pool for the case, that cacheSkeletonData() is called while this method reads from the cache
	m_skeletonPoolLock.lock();

	//copy cache to local copy, so we can unlock the skeleton pool befor return
	std::map<int, Skeleton> skeletonPoolCacheCopy = m_skeletonPoolCache;


	m_skeletonPoolLock.unlock();

	//Console::log("Tracker::getSkeletonPoolCache(): pool size= " + toString(skeletonPoolCacheCopy.size()));


	return skeletonPoolCacheCopy;

}

std::map<int, Skeleton> Tracker::getSkeletonPool()
{

	//lock skeleton pool for the case, that cacheSkeletonData() is called while this method reads from the cache
	m_skeletonPoolLock.lock();

	//copy cache to local copy, so we can unlock the skeleton pool befor return
	std::map<int, Skeleton> skeletonPoolCacheCopy = m_skeletonPool;


	m_skeletonPoolLock.unlock();

	//Console::log("Tracker::getSkeletonPoolCache(): pool size= " + toString(skeletonPoolCacheCopy.size()));


	return skeletonPoolCacheCopy;

}

PointCollection Tracker::getPointCollection()
{
	// lock point collection bevore accessing point collection
	m_pointCollectionLock.lock();

	// local copy, so we can unlock the skeleton pool befor return
	PointCollection pointCollectionCopy = m_pointCollection;

	// unlock point collection
	m_pointCollectionLock.unlock();

	return pointCollectionCopy;
}

float Tracker::getTotalTime()
{
	return -1.0;
}

int Tracker::getCurrFrameIdx()
{
	return -1;
}

int Tracker::getFrameCount()
{
	return -1;
}

#pragma endregion

#pragma region tracker_Offset_handling

/*void Tracker::updateMatrix()
{
	//create new Matrix and set it to be identity
	m_offsetMatrix = transformMatrix(m_properties->positionOffset, m_properties->rotationOffset, m_properties->scaleOffset);
}*/

void Tracker::setPositionOffset(Vector3f position)
{
	m_properties->positionOffset = position;
	m_configManager->writeVec3f("position", position, getTrackerType(), getTrackerIdentifier());
	//updateMatrix();
}

void Tracker::setRotationOffset(Vector3f rotation)
{
	m_properties->rotationOffset = rotation;
	m_rotationOffset = AngleAxisf(rotation.y() * M_PI / 180, Vector3f::UnitY()) * AngleAxisf(rotation.x() * M_PI / 180, Vector3f::UnitX()) * AngleAxisf(rotation.z() * M_PI / 180, Vector3f::UnitZ());
	m_configManager->writeVec3f("rotation", rotation, getTrackerType(), getTrackerIdentifier());
	//updateMatrix();
}

void Tracker::setScaleOffset(Vector3f scale)
{
	m_properties->scaleOffset = scale;
	m_configManager->writeVec3f("scale", scale, getTrackerType(), getTrackerIdentifier());
	//updateMatrix();
}

Vector4f Tracker::applyOffset(Vector4f pos)
{
	pos.head<3>() = m_rotationOffset * (pos.head<3>()).cwiseProduct(m_properties->scaleOffset) + m_properties->positionOffset;
	return pos;
}

Quaternionf Tracker::applyOffset(Quaternionf rot)
{
	if (m_properties->scaleOffset.x() < 0) rot.x() *= -1;
	if (m_properties->scaleOffset.y() < 0) rot.y() *= -1;
	if (m_properties->scaleOffset.z() < 0) rot.z() *= -1;
	return m_rotationOffset * rot;
}

bool Tracker::readOffsetFromConfig()
{
	//std::cout << getTrackerType() << ", " << getTrackerIdentifier() << std::endl;
	return	m_configManager->readVec3f("position", m_properties->positionOffset, getTrackerType(), getTrackerIdentifier()) &&
			m_configManager->readVec3f("rotation", m_properties->rotationOffset, getTrackerType(), getTrackerIdentifier()) &&
			m_configManager->readVec3f("scale", m_properties->scaleOffset, getTrackerType(), getTrackerIdentifier());
}

#pragma endregion

#pragma region protected_methods


void printFPS() {
	static std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now();
	static int fps; fps++;

	if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - oldTime) >= std::chrono::seconds{ 1 }) {
		oldTime = std::chrono::high_resolution_clock::now();

		Console::log("FPS: " + std::to_string(fps));
		fps = 0;
	}
}

void Tracker::update()
{
	// track while tracking is true
	while (m_isTracking)
	{
		//Console::log("Tracker::update()");
		// if no new data is procressed
		if (!m_isDataAvailable && m_isEnabled) {
			using namespace std::chrono;

			using fps = duration<int64_t, std::ratio<1, 240>>;

			auto start = high_resolution_clock::now();
			auto nextFrame = (start + fps{1}) - start;

			// get new data
			track();

			// send Skeleton Pool to NetworkManager
			if (m_networkManager != nullptr) {

				//send Skeleton Pool to NetworkManager
				m_networkManager->sendSkeletonPool(&m_skeletonPool, m_properties->id);

				// send Point Pool to NetworkManager
				m_networkManager->sendPointPool(&m_pointCollection, m_properties->id);
			}
			
			//printFPS();

			if(shouldSleep) {
				while((high_resolution_clock::now() - start) < nextFrame) {
					// Do nothing...
				}

				//std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::milliseconds(1));
			}

			/*
			auto end = std::chrono::high_resolution_clock::now();
			auto timing = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			Console::logWarning(std::to_string(timing.count()));
			*/
		}
	}
	//clean skeleton pool after tracking
	clean();
}

void Tracker::track()
{
	Console::log("Tracker::track()");

	m_isDataAvailable = true;
}

#pragma endregion
