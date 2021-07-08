#pragma once

class Subject;

/*!
 * \class Observer
 *
 * \brief Interface for the Observer
 */
class Observer {

public:

    /*!
    * Destructor for subclasses
    */
    virtual ~Observer() {}

    /*!
     * update the state of this observer
     */
    virtual void notify(Subject* subject) = 0;
};