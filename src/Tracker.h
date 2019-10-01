#include "Skeleton.h"
#include <list>

class Tracker
{
public:



private:
	StateTracker state;		std::list<Skeleton> skeletons;
	


	Tracker();
	void updateSkeletons();
};

enum StateTracker
{
	INIT,
	TRACKING,
	PAUSED
};