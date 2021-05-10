#pragma once

#include <exception>
#include <string>

class Exception : virtual public std::exception {

public:

	/*!
	 * Constructor
	 *
	 * \param description The error message
	 */
	explicit
		Exception(const std::string& description) :
		description(description)
	{}


	/*!
	 * Destructor.
	 *
	 * Virtual to allow subclassing.
	 */
	virtual ~Exception() = default;

	/*!
	 * Returns a pointer to the (constant) error description.
	 *
	 * \return A pointer to a const char*. The underlying memory is in possession of the Except object.
	 * Callers must not attempt to free the memory.
	 */
	virtual const char* what() const throw () {
		return description.c_str();
	}

protected:

	std::string description;
};

