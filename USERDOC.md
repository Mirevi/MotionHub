USERDOC

# Mirevi MotionHub User Documentation

![Organisations Logos][./assets/images/Logos.png]

Mirevi MotionHub (MMH) is a middleware for multiple body tracking systems and it unifies them in a single plug-in for the Unity game engine.

The software suite Mirevi MotionHub offers support for several body tracking systems and encompasses a game engine plug-in that connects the MMH with Unity by means of a standardized protocol. The plug-in allows for the usage of a single type of skeleton for any body tracking system and, therefore, facilitates the switch between different tracking systems during app development significantly.

Beyond that, MMH is capable of merging different tracking systems into one in order to combine and use their individual benefits.

*MotionHub is developed at the research labor [MIREVI](https://www.mirevi.de/) within the scope of the project [HIVE](https://tinyurl.com/y3ugxo3p).*

## Content

---

1. [Systems](#1-systems)
2. [Overview and Navigation](#2-overview-and-navigation)
3. [Adding and Removing Tracker](#3-adding-and-removing-tracker)
4. [Start and Stop Tracking](#4-start-and-stop-tracking)

## 1. Systems

---

| Supported         
| -------------
| Azure Kinect
| OptiTrack

| Planned
| -------------
| Perception Neuron Studio
| OpenPose

## 2. Overview and Navigation

---

![Main Window][./assets/images/img_mmhUi_v001_ej.png]

MMH consists of four panels. From left to bottom right:

| Name							 | Usage
| ------------------------------ | -------------
| tracker and skeleton hirarchy  | inspect tracker and detected skeletons
| render window					 | preview transformed tracking data
| tracker list					 | add / remove and start / stop tracker
| tracker property inspector	 | offset tracker

The render window camera can be rotated by holding the left mouse button down in the panel and pulling the mouse left or right.

## 3. Adding and Removing Tracker

---

![Tracker Controls 1][./assets/images/img_trackerList_v002_ej.png]

Add a new tracker by pressing the "Add"(1) button. A new popup will appear where you can select the type in a dropdown. Click "Create" to add the tracker to the list.

Remove a tracker by clicking on the name in the tracker list and click "Remove"(2). Trackers can only removed while the system is not tracking.

## 4. Start and Stop Tracking

---

![Tracker Controls 2][./assets/images/img_trackerList_v002_ej.png]

Start all trackers by pressing the play icon(3). This will start all tracker and the preview in the render window.

Stop all tacker by clicking the same button (now displayed as a stop icon)(3). This will stop all tracker.

## 5. Offset Tracker

---

![Tracker Property][./assets/images/img_trackerPropertyInspector_v001_ej.png]

Offset a tracker by selecting one in the tracker list and enter values in the position, rotation and scale fields. This will offset the trackers origin. All values are in meters.

**Version 1.0**
