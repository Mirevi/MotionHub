#pragma once

#include "MotionHubUtil/Subject.h"
#include "OpenVRObserver.h"

/*!
 * \class OpenVRButtonSubject
 *
 * \brief Interface for the OpenVRButtonSubject
 */
class OpenVRButtonSubject : public Subject {

public:

    /*!
    * Destructor
    */
    ~OpenVRButtonSubject() {}

    /*!
    * getter for button state
    *
    * \return current button state
    */
    vr::EVRButtonId getButtonState() {
        return buttonState;
    }

    /*!
    * setter for button state
    *
    * \param button the button
    */
    void setButtonState(const vr::EVRButtonId button) {
        this->buttonState = button;
    }

    /*!
    * getter for device index
    *
    * \return current device index
    */
    vr::TrackedDeviceIndex_t getDeviceIndexState() {
        return deviceIndex;
    }

    /*!
    * setter for device index
    *
    * \param deviceIndex the device index
    */
    void setDeviceIndexState(const vr::TrackedDeviceIndex_t deviceIndex) {
        this->deviceIndex = deviceIndex;
    }

private:

    vr::EVRButtonId buttonState;

    vr::TrackedDeviceIndex_t deviceIndex;
};
