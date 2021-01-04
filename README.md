# MIREVI MotionHub

<p align="center">
  <img width="85%" src="doc/assets/images/headerLogo.jpg" />
</p>

[MIREVI](https://www.mirevi.de/) MotionHub (MMH) is a middleware for merging body tracking data from different systems into one coordinate space in real-time in order to combine and use their individual benefits.

MMH offers support for several body tracking systems and encompasses a [game engine plug-in](https://github.com/Mirevi/MotionHub/#Game-engine-plug-in) that connects the MMH with Unity by means of a standardized protocol. The plug-in allows for the usage of a single type of skeleton for any body tracking system and, therefore, facilitates the switch between different body tracking systems during app development significantly.

MotionHub is developed at the research lab [MIREVI](https://www.mirevi.de/) from the [University of Applied Sciences Düsseldorf](https://hs-duesseldorf.de/en) within the scope of the projects [HIVE](https://mirevi.de/research/immersive-digital-technologies-for-a-healthy-life-through-exercise-and-well-being-hive-lab) and [iKPT4.0](https://mirevi.de/research/ikpt4-0).

### Acknowledgements

This project is sponsored by: [German Federal Ministry of Education and Research](https://www.bmbf.de/en/index.html) (BMBF) under the project numbers **16SV8182** and **13FH022IX6**. Project names: [HIVE-Lab](https://mirevi.de/research/immersive-digital-technologies-for-a-healthy-life-through-exercise-and-well-being-hive-lab) (Health Immersive Virtual Environment Lab) and [Interactive body-near production technology 4.0](https://www.mirevi.de/research/ikpt4-0) (german: ’Interaktive körpernahe Produktionstechnik 4.0’ (iKPT4.0)).

# Requirements

Currently MMH is only supported on Microsoft Windows operating systems.

- This version of MotionHub was tested on Microsoft **Windows 10 64 bit**.

### Minimum Computer Requirements

- Seventh Gen Intel i5 Processor
- NVidia GeForce GTX 1070
- 4 GB Memory
- 5 GB Storage

# Documentation

The developer, API and user documentation including a class collaboration diagram can be found in the `doc` folder.

# Supported Body Tracking Systems

| Supported                                                       		  |
| ----------------------------------------------------------------------- |
| [Azure Kinect](https://azure.microsoft.com/en-gb/services/kinect-dk/)   |
| [OptiTrack](https://optitrack.com/)                                     |

| Planned                                                                             							|
| --------------------------------------------------------------------------------------------------------------|
| [The Captury (planned for ~Januar 2021)](https://thecaptury.com/)                                				|
| [XSens MVN Animate (only compatibel with MVN Pro License due to network streaming capabilities) (planned for ~Januar 2021)](https://www.xsens.com/products/mvn-animate)                    |
| [Perception Neuron 2 (planned for ~March 2021)](https://neuronmocap.com/content/product/32-neuron-edition-v2) |

# Installer (for users, not for developers)

If you want to install the MotionHub, please download the latest release:
[https://github.com/Mirevi/MotionHub/releases](https://github.com/Mirevi/MotionHub/releases)


# Building from source (for developers, not for users)

The [CMake](https://cmake.org/) system is used to generate project files and for downloading all required dependencies. Please use the `CMakeLists.txt` file for generating.

- MMH is developed with Microsoft **Visual Studio 2017** and **2019**. *([CMake](https://cmake.org/) has only been tested with these IDE versions.)*

1.  Download or clone MMH
2.  Download and install Azure Kinect [Sensor](https://docs.microsoft.com/en-us/azure/kinect-dk/sensor-sdk-download) and [Body Tracking](https://docs.microsoft.com/en-us/azure/kinect-dk/body-sdk-download) SDK
3.  In CMake, set source path to the MMH path
4.  Set binaries path to (MMH path)/build

5.  Click "configure" (click Yes, select your installed VS version and Finish). This will take a few minutes (and might FAIL).
6.  If CMake fails, because it couldn't find the Azure Kinect dependences (which you installed in step 2), paste the correct paths to K4A_ROOT (e.g. C:/Program Files/Azure Kinect SDK v1.4.0) and K4ABT_ROOT. Then again click "configure".
7.  If the configuration was successful, click "generate"

8. You can now click "Open Project" or open the Solution with VS (/MMH/build/MireviMotionHub.sln)
9. In the VS Solution Explorer, right click on the project "MireviMotionHub"->"Set as StartUp Project" and "MireviMotionHub"->"build"
10. After the code is compiled, you can push F5 (Local Windows Debugger) in VS or execute /MMH/build/bin/MireviMotionHub.exe to start the MotionHub

Please Note that we use Qt Framework for the UI. To build the project you need the [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123) and for editing Qt `.ui` files you need [Qt Designer](https://www.qt.io/download).

We recognized an Error, when using a N version of Windows, in which the .exe wouldn't start, because MF.dll was missing. To fix that, install the [Media Feature Pack for Windows 10 N](https://support.microsoft.com/en-us/help/4516397/media-feature-pack-for-windows-10-n-november-2019) and the Motionhub should work.

# Authors

- **[Philipp Ladwig](https://www.mirevi.de/team/philipp-ladwig)** - Project Manager and Developer
- **[Kester Evers](https://www.mirevi.de/team/kester-evers)** - Developer
- **[Eric Jansen](https://www.mirevi.de/team/eric-jansen)** - Developer
- **[Manuel Zohlen](https://www.mirevi.de/team/manuel-zohlen)** - Developer

# Build with

- [Qt](https://www.qt.io/)
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
- [OSC Pack](http://www.rossbencina.com/code/oscpack)
- [TinyXML](http://www.grinninglizard.com/tinyxml/)

# Game engine plug-in

Currently, we are supporting [Unity](https://unity.com/) with a plugIn, which you can integrate in your project. It receives the tracking data and animates a character.
You can download the project [here](https://github.com/Mirevi/MotionHub-Unity-Plugin).


# License

[License](https://github.com/Mirevi/MotionHub/blob/master/LICENSE.txt)


# Referencing MotionHub
When using MotionHub please reference:

```
@inproceedings{MotionHub:2020,
author = {Ladwig, Philipp and Evers, Kester and Jansen, Eric J. and Fischer, Ben and Nowottnik, David and Geiger, Christian},
title = {MotionHub: Middleware for Unification of Multiple Body Tracking Systems},
year = {2020},
publisher = {ACM},
url = {https://doi.org/10.1145/3401956.3404185},
booktitle = {Proceedings of the 7th International Conference on Movement and Computing},
}
```
