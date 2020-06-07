# MIREVI MotionHub

<p align="center">
  <img width="85%" src="doc/assets/images/headerLogo.jpg" />
</p>

[MIREVI](https://www.mirevi.de/) MotionHub (MMH) is a middleware for merging body tracking data from different systems into one coordinate space in real-time in order to combine and use their individual benefits.

MMH offers support for several body tracking systems and encompasses a game engine plug-in that connects the MMH with Unity by means of a standardized protocol. The plug-in allows for the usage of a single type of skeleton for any body tracking system and, therefore, facilitates the switch between different body tracking systems during app development significantly.

MotionHub is developed at the research lab [MIREVI](https://www.mirevi.de/) from the [University of Applied Sciences Düsseldorf](https://hs-duesseldorf.de/en) within the scope of the project [HIVE](https://mirevi.de/research/immersive-digital-technologies-for-a-healthy-life-through-exercise-and-well-being-hive-lab).

### Acknowledgements

The project [HIVE](https://mirevi.de/research/immersive-digital-technologies-for-a-healthy-life-through-exercise-and-well-being-hive-lab) is sponsored by the [German Federal Ministry of Education and Research](https://www.bmbf.de/en/index.html) (BMBF) under the project number **16SV8182**.

# Requirements

Currently MMH is only supported on Microsoft Windows operating systems.

- This version of MotionHub was tested on Microsoft **Windows 10 64 bit**.

### Minimum Computer Requirements

- Seventh Gen Intel i5 Processor
- NVidia GeForce GTX 1070
- 4 GB Memory
- 700 MB Storage

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

- MMH is developed with Microsoft **Visual Studio 2017**. *([CMake](https://cmake.org/) has only been tested with this IDE version.)*

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
