# MIREVI MotionHub User Documentation

<p align="center">
  <img width="85%" src="assets/images/headerLogo.jpg" />
</p>

[MIREVI](https://www.mirevi.de/) MotionHub (MMH) is a middleware for merging body tracking data from different systems into one coordinate space in real-time in order to combine and use their individual benefits.

MMH offers support for several body tracking systems and encompasses a game engine plug-in that connects the MMH with Unity by means of a standardized protocol. The plug-in allows for the usage of a single type of skeleton for any body tracking system and, therefore, facilitates the switch between different body tracking systems during app development significantly.

MotionHub is developed at the research lab [MIREVI](https://www.mirevi.de/) from the [University of Applied Sciences Düsseldorf](https://hs-duesseldorf.de/en) within the scope of the project [HIVE](https://mirevi.de/research/immersive-digital-technologies-for-a-healthy-life-through-exercise-and-well-being-hive-lab).

### Acknowledgements

The project [HIVE](https://mirevi.de/research/immersive-digital-technologies-for-a-healthy-life-through-exercise-and-well-being-hive-lab) is sponsored by the [German Federal Ministry of Education and Research](https://www.bmbf.de/en/index.html) (BMBF) under the project number **16SV8182**.

# Content

1. [Supported Systems](#1-supported-systems)
2. [MotionHub](#2-motionhub)  
  2.1 [Overview and Navigation](#21-overview-and-navigation)  
  2.2 [Adding and Removing Tracker](#22-adding-and-removing-tracker)  
  2.3 [Start and Stop Tracking](#23-start-and-stop-tracking)  
  2.4 [Offset Tracker](#24-offset-tracker)
3. [Setup Body Tracking Systems](#3-setup-body-tracking-systems)  
  3.1 [Azure Kinect](#31-azure-kinect)  
  3.2 [OptiTrack](#32-optitrack)

# 1. Supported Systems

MMH currently supports the listed body tracking systems.

| Supported Systems                                                       |
| ----------------------------------------------------------------------- |
| [Azure Kinect](https://azure.microsoft.com/en-gb/services/kinect-dk/)   |
| [OptiTrack](https://optitrack.com/)                                     |

# 2. MotionHub

## 2.1 Overview and Navigation

<p align="center">
  <img src="assets/images/img_mmhUi_v001_ej.png" />
</p>

MMH consists of four panels. From left to bottom right:

| Name							             | Usage
| ------------------------------ | --------------------------------------
| tracker and skeleton hierarchy | inspect tracker and detected skeletons
| render window					         | preview transformed tracking data
| tracker list					         | add / remove and start / stop tracker
| tracker property inspector	   | offset tracker

The render window camera can be rotated by holding the left mouse button down in the panel and pulling the mouse left or right.

## 2.2 Adding and Removing Tracker

<img src="assets/images/img_trackerList_v002_ej.png" />

Add a new tracker by pressing the **"Add"**(1) button. A new popup will appear where you can select the type in a dropdown. Click **"Create"** to add the tracker to the list.

Remove a tracker by clicking on the name in the tracker list and click **"Remove"**(2). Trackers can only removed while the system is not tracking.

## 2.3 Start and Stop Tracking

<img src="assets/images/img_trackerList_v002_ej.png" />

Start all trackers by pressing the play icon(3). This will start all tracker and the preview in the render window.

Stop all tacker by clicking the same button (now displayed as a stop icon)(3). This will stop all tracker.

## 2.4 Offset Tracker

<img src="assets/images/img_trackerPropertyInspector_v001_ej.png" />

Offset a tracker by selecting one in the tracker list and enter values in the position, rotation and scale fields. This will offset the trackers origin. All values are in meters.

# 3. Setup Body Tracking Systems

## 3.1 Azure Kinect

1. Follow the official [Quick Start Guide](https://docs.microsoft.com/en-us/azure/kinect-dk/set-up-azure-kinect-dk).
2. Add the Tracker in MMH (see `2.2 Adding and Removing Tracker`).

## 3.2 OptiTrack

1. Follow the official [Quick Start Guide](https://v22.wiki.optitrack.com/index.php?title=Quick_Start_Guide:_Getting_Started).
2. Configure the [Motive](https://optitrack.com/products/motive/) Software for data streaming.
- **At the moment MMH only supports Motive version x.x!**

[//]: # "Image Motive Settings"

3. Add the Tracker in MMH (see `2.2 Adding and Removing Tracker`).
