// #include "NetworkManager.h"
// #include "defines.h"

// NetworkManager::NetworkManager()
// {

	// poolSender.push_back(new OSCSender(BROADCAST, DEFAULT_PORT));

// }

// void NetworkManager::sendSkeletonPool(std::map<int, Skeleton>* skeletonPool)
// {
	// for (auto itSkeletonPool = skeletonPool->begin(); itSkeletonPool != skeletonPool->end(); itSkeletonPool++)
	// {
		// try
		// {
			// for (auto itSenderPool = poolSender.begin(); itSenderPool != poolSender.end(); itSenderPool++)
			// {

				// if ((*itSenderPool)->isActive())
				// {
					// (*itSenderPool)->sendSkeleton(&(itSkeletonPool->second), DEFAULT_URI);
				// }
			// }
		// }
		// catch (const std::exception&)
		// {
			// Console::logError("NetworkManager::sendSkeleton(): Skeleton could not be send!");
		// }
	// }
// }