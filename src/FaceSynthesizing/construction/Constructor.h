#pragma once

#include <exception>
#include <string>

namespace facesynthesizing::construction {
	class ConstructionException: std::exception {
	public:
		explicit ConstructionException(std::string message) {
			this->message = message;
		};
		const char* what() const throw () {
			return message.c_str();
		}
	private:
		std::string message;
	};

	class Constructor {
	public:
		void construct();
	protected:
		virtual void validateDependencies();
		virtual void resolveDependencies();
	
		bool constructionIsDone = false;
	};
}