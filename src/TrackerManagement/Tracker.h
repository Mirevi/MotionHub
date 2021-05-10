#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Exception.h"
#include "MotionHubUtil/MMHmath.h"
#include "MotionHubUtil/ConfigManager.h"
#include "NetworkManagement/NetworkManager.h"


/*!
 * \class Tracker
 *
 * \brief Prototype class for implemetation - holds virtual methods
 *
 * \author Kester Evers, Eric Jansen and Manuel Zohlen
 */
class  Tracker
{

public:
	// old Property struct 
	/*struct NullProperty
	{
		enum class Type {
			INVALID,
			BOOL,
			INT,
			FLOAT,
			STRING
		};
		std::string name;

		NullProperty(std::string name) : name(name) {};

		virtual Type type() = 0 { return Type::INVALID; };
	};

	template<class T> struct Property : public NullProperty
	{
		T value;
			
		Property(std::string name, T value) : NullProperty(name), value(value) {};
		
		Type type() override {
			if (std::is_same_v<T, bool>) return Type::BOOL;
			else if (std::is_same_v<T, int>) return Type::INT;
			else if (std::is_same_v<T, float>) return Type::FLOAT;
			else if (std::is_same_v<T, std::string>) return Type::STRING;
			else return Type::INVALID;
		};
	};
	typedef NullProperty::Type PropertyType;*/

	struct NullProperty
	{
		std::string name;
		std::string type = "";

		NullProperty(std::string name) : name(name) {};

		template<class T> bool isType() { return (type == typeid(T).name()); }
	};

	template<class T> struct Property : public NullProperty
	{
		T value;

		Property(std::string name, T value) : NullProperty(name), value(value) { type = typeid(T).name(); }
	};
	/*!
	 * struct for containing tracker properties data
	 */
	struct Properties
	{
		/*!
		 * tracker ID
		 */
		int id = -1;
		/*!
		 * tracker name
		 */
		std::string name = "none";
		/*!
		 * offset of the trackers position
		 *
		 */
		Vector3f positionOffset = Vector3f(0, 0, 0);
		/*!
		 * offset of the trackers rotation
		 *
		 */
		Vector3f rotationOffset = Vector3f(0, 0, 0);
		/*!
		 * offset of the trackers scale
		 *
		 */
		Vector3f scaleOffset = Vector3f(1, 1, 1);
		/*!
		 * map containing additional properties. Properties have to be inserted in the Tracker's constructor
		 */
		std::map<std::string, NullProperty*> additionalProperties;

		~Properties() {
			if (!additionalProperties.empty()) {
				for (const auto& kv : additionalProperties) delete kv.second;
			}
		}
	};

	/*!
	 * default constructor
	 */
	Tracker();
	/*!
	 * default destructor
	*/
	virtual ~Tracker();


	bool valid = true;

	/*!
	 * starts Tracker in new temporary thread
	 */
	virtual void start();
	/*!
	 *  sets m_tracking to false
	 */
	virtual void stop();
	/*!
	 * disable tracker, so it doesn't track skeleton data during tracking loop
	 */
	virtual void disable();
	/*!
	 *  enables tracker, so it tracks skeleton data during tracking loop
	 */
	virtual void enable();
	/*!
	 * deletes all skeletons which are not in the scene anymore
	 */
	virtual void clean();
	/*!
	* checks if the tracker is enabled
	*/
	virtual bool isTracking();
	/*!
	* checks if the tracker is enabled
	*/
	virtual bool isEnabled();
	/*!
	 * checks if new skeleton date is available 
	 * \return true when new skeleton data is available
	 */
	virtual bool isDataAvailable();
	/*!
	 * resets the isDataAvailable to false
	 * call when you got the current skeleton data
	 */
	virtual void resetIsDataAvailable();
	/*!
	 *  checks if skeleton count has changed
	 * \return true when number of skeletons is different from last frame
	 */
	virtual bool hasSkeletonPoolChanged();

	/*!
	 * setter for m_hasSkeletonPoolChanged
	 *
	 * \param state
	 */
	virtual void setSkeletonPoolChanged(bool state);
	/*!
	* returns the number of skeletons  detected by this tracker
	*/
	virtual int getNumDetectedSkeletons();
	/*!
	 * getter for the trackers properties struct 
	 * \return the trackers Property struct
	 */
	virtual Properties* getProperties();
	/*!
	 * getter for the trackers skeleton pool
	 * \return the trackers skeleton pool
	 */

	 /*!
	  * getter for the trackers skeleton pool cache
	  * \return the trackers skeleton pool cache by value
	  */
	virtual std::map<int, Skeleton> getSkeletonPoolCache();

	virtual std::map<int, Skeleton> getSkeletonPool();

	/*!
	 * recalculates the update matrix
	 *
	 */
	//virtual void updateMatrix();

	/*!
	 * sets the position offset in the properties
	 *
	 * \param position position offset
	 */
	virtual void setPositionOffset(Vector3f position);

	/*!
	 * sets the rotation offset in the properties
	 *
	 * \param position rotation offset
	 */
	virtual void setRotationOffset(Vector3f rotation);

	/*!
	 * sets the scale offset in the properties
	 *
	 * \param position scale offset
	 */
	virtual void setScaleOffset(Vector3f scale);

	/*!
	 * applies the tracker's offset to a given Vector4f
	 */
	virtual Vector4f applyOffset(Vector4f pos);
	
	/*!
	* applies the tracker's offset to a given Quaternionf
	*/
	virtual Quaternionf applyOffset(Quaternionf rot);

	/*!
	 * sets the value of the property with given name. Returns false, if the type does not match or the name is invalid
	 */
	template<class T> bool setPropertyValue(std::string id, T value, bool enableTypeWarning = false) // Needs to be defined in the header, due to template
	{
		// check if property exists
		if (m_properties->additionalProperties.count(id) == 0) {
			Console::logError("Unable to set value of Property with ID " + id + ". The Property does not exist.");
			return false;
		}
		NullProperty* property = m_properties->additionalProperties.at(id);
		// check if property is of the correct type
		if (!property->isType<T>()) {
			Console::logError("Unable to set value of Property " + property->name + ". The Property is of type " + property->type + ", but the given value is of type " + typeid(T).name());;
			return false;
		}
		((Tracker::Property<T>*) property)->value = value;
		m_configManager->write<T>(id, value, getTrackerType(), getTrackerIdentifier(), enableTypeWarning);

		return true;
	}
	/*!
	 * copys the skeleton pool to it's cache
	 *
	 */
	virtual void cacheSkeletonData();

	/*!
	 * pointer to sendSkeletonDelegate() in main.cpp
	 *
	 */
	 //virtual void setSendSkeletonDelegate(void (*sendSkeletonDelegate)(std::map<int, Skeleton>* skeletonPool, int trackerID));

	/*!
	* returns a unique name to clearly identify the type of tracker
	*/
	virtual inline std::string getTrackerType() = 0;

	virtual float getTotalTime();

	virtual int getCurrFrameIdx();

	virtual int getFrameCount();

protected:
	/*!
	* returns a unique number as identifier in addition to the tracker type.
	*/
	virtual inline std::string getTrackerIdentifier(); //Only needs to be overridden, if it is possible to have multiple Trackers of the same type with different configurations
	/*!
	 * tries to read the tracker's offset from config. Returns false if the config does not contain one or more entries
	 */
	virtual bool readOffsetFromConfig();
	/*!
	* the trackers tracking state
	 */
	std::atomic<bool> m_isTracking = false;
	/*!
	 * the trackers enabling state
	 */
	std::atomic<bool> m_isEnabled = false;
	/*!
	 * the number of skeletons detected by this tracker
	 */
	int m_countDetectedSkeleton = 0;
	/*!
	 * the trackers property struct 
	 */
	Properties* m_properties;
	/*!
	 * matrix calculated with the offset Vectors
	 * 
	 */
	Matrix4f m_offsetMatrix;
	/*!
	 * rotation offset as Quaternion. Used internally to apply the offset to the tracking data
	 *
	 */
	Quaternionf m_rotationOffset = Quaternionf::Identity();


	/*!
	 * is true after one completed tracking cycle
	 */
	std::atomic<bool> m_isDataAvailable = false;
	/*!
	 * is true if skeleton was added or removed from pool
	 */
	std::atomic<bool> m_hasSkeletonPoolChanged = false;
	/*!
	 * thread for track() method
	 */
	std::thread* m_trackingThread;
	/*!
	 * pool containing all skeletons detected by this Tracker
	 */
	std::map<int, Skeleton> m_skeletonPool;

	std::map<int, Skeleton> m_skeletonPoolCache;

	/*!
	 * updade method used for tracker thread
	 */
	virtual void update();

	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	virtual void track();


	/*!
	 * tracks the refresh cycles of a tracker
	 *
	 */
	int m_trackingCycles = 0;

	/*!
	 * lock for save acces to skeleton pool
	 *
	 */
	std::mutex m_skeletonPoolLock;


	NetworkManager* m_networkManager = nullptr;

	ConfigManager* m_configManager = nullptr;

};
