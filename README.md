# MIREVI MotionHub

<p align="center">
  <img width="85%" src="doc/assets/images/headerLogo.jpg" />
</p>

[MIREVI](https://www.mirevi.de/) MotionHub (MMH) is a middleware for merging body tracking data from different systems into one coordinate space in real-time in order to combine and use their individual benefits.

MMH offers support for several body tracking systems and encompasses a game engine plug-in that connects the MMH with Unity by means of a standardized protocol. The plug-in allows for the usage of a single type of skeleton for any body tracking system and, therefore, facilitates the switch between different body tracking systems during app development significantly.

MotionHub is developed at the research lab [MIREVI](https://www.mirevi.de/) from the [University of Applied Sciences Düsseldorf](https://hs-duesseldorf.de/en) within the scope of the project [HIVE](https://mirevi.de/research/immersive-digital-technologies-for-a-healthy-life-through-exercise-and-well-being-hive-lab).

### Acknowledgements

This project is sponsored by: [German Federal Ministry of Education and Research](https://www.bmbf.de/en/index.html) (BMBF) under the project numbers **16SV8182** and **13FH022IX6**. Project names: [HIVE-Lab](https://mirevi.de/research/immersive-digital-technologies-for-a-healthy-life-through-exercise-and-well-being-hive-lab) (Health Immersive Virtual Environment Lab) and [Interactive body-near production technology 4.0](https://www.mirevi.de/research/ikpt4-0) (german: ’Interaktive körpernahe Produktionstechnik 4.0’ (iKPT4.0)).

# Requirements

Currently MMH is only supported on Microsoft Windows operating systems.

- This version of MotionHub was tested on Microsoft **Windows 10 64 bit**.

### Minimum Computer Requirements

- Seventh Gen Intel i5 Processor
- NVidia GeForce GTX 1070
- 4 GB Memory
- 4 GB Storage

# Documentation

The developer, API and user documentation including a class collaboration diagram can be found in the `doc` folder.

# Supported Systems

MMH currently supports the listed body tracking systems.

| Supported Systems                                                       |
| ----------------------------------------------------------------------- |
| [Azure Kinect](https://azure.microsoft.com/en-gb/services/kinect-dk/)   |
| [OptiTrack](https://optitrack.com/)                                     |

# Setup and Building

The [CMake](https://cmake.org/) system is used to generate project files and for downloading all required dependencies. Please use the `CMakeLists.txt` file for generating.

- MMH is developed with Microsoft **Visual Studio 2017** and **2019**. *([CMake](https://cmake.org/) has only been tested with this IDE versions.)*

1.  Download or clone MMH
2.  In CMake, set source to the MMH path
3.  Set binaries to (MMH path)/build
4.  Click "configure" (click Yes, select your installed VS version and Finish). This will take a few minutes (and is expected to FAIL) so go on with 6 and 7.

5.  Download [Azure Kinect Sensor SDK](https://docs.microsoft.com/en-us/azure/kinect-dk/sensor-sdk-download)
6.  Download [Azure Kinect Body Tracking SDK[ (https://docs.microsoft.com/en-us/azure/kinect-dk/body-sdk-download)
7.  Install both and remember the install paths!
8.  Meanwhile, the configuration process in CMake should have failed, because it couldn't find Azure Kinect dependencies. 

9.  In CMake, set the flags "advanced" and "grouped", you should see all dependencies listed, including K4A (Sensor SDK) and K4ABT (Body Tracking SDK)
10. Under K4A set K4A_INCLUDE_DIR to the include folders path (something like this in your freshly installed directory: /Azure Kinect SDK v1.4.0/sdk/include)
11. Repeat for K4A_LIBRARIES_DIR (/Azure Kinect SDK v1.4.0/sdk/windows-desktop/amd64/release/lib).
12. Repeat both for K4ABT (/AzureKinectBodyTrackingSDKv1.0.1/sdk/include) and (/AzureKinectBodyTrackingSDKv1.0.1/sdk/windows-desktop/amd64/release/lib)
13. Again, click "Configure" (This should be successful), then click "Generate".

14. You can now click "Open Project" or open the Solution with VS (/MMH/build/MireviMotionHub.sln)
15. In the VS Solution Explorer, right click on the project "MireviMotionHub"->"Set as StartUp Project" and "MireviMotionHub"->"build"
16. After the code is compiled, you can push F5 (Local Windows Debugger) in VS or execute /MMH/build/bin/MireviMotionHub.exe to start the MotionHub

Please Note that we use Qt Framework for the UI. To build the project you need the [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123) and for editing Qt `.ui` files you need [Qt Designer](https://www.qt.io/download).

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

# License

Pending
