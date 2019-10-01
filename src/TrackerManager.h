#include <list>
#include "Tracker.h"

class TrackerManager
{
public:


private:
	std::list<Tracker> tracker;




	Tracker CreateTracker(TypeTracker type);

};

enum TypeTracker
{
	AKTracker,
	XSTracker
};