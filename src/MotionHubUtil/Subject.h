#pragma once

#include "Observer.h"
#include <vector>

/*!
 * \class Subject
 *
 * \brief Interface for the Subject
 */
class Subject {

public:
    /*!
    * Destructor for subclasses
    */
    virtual ~Subject() {}

    /*!
    * Register an observer
    * 
    * \param observer the observer object to be registered
    */
    void registerObserver(Observer* observer) {

        observers.push_back(observer);
    }

    /*!
    * Unregister an observer
    * 
    * \param observer the observer object to be unregistered
    */
    void removeObserver(Observer* observer) {

        // Find the observer
        auto iterator = std::find(observers.begin(), observers.end(), observer);

        // Remove the observer if found
        if (iterator != observers.end()) {
            observers.erase(iterator);
        }
    }

    /*!
    * Notify all the registered observers when a change happens
    */
    void notifyObservers() {

        // Loop over all observers and notify them
        for (Observer* observer : observers) {
            observer->notify(this);
        }
    }

private:

    // vector for storing registered observers
    std::vector<Observer*> observers;
};
