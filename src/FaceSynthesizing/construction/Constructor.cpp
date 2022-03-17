#include "./Constructor.h"
#include <iostream>

namespace facesynthesizing::construction {

	void Constructor::construct() {
		if (this->constructionIsDone)
			throw ConstructionException("Cannot run construction twice!");

		this->validateDependencies();
		this->resolveDependencies();

		this->constructionIsDone = true;
	}
	void Constructor::validateDependencies(){}
	void Constructor::resolveDependencies(){}
}