# Mirevi MotionHub

<p align="center">
  <img width="65%" src="doc/assets/images/orgLogos.png" />
</p>

Mirevi MotionHub (MMH) is a middleware for merging body tracking data from different systems into one coordinate space in real-time in order to combine and use their individual benefits.

MMH offers support for several body tracking systems and encompasses a game engine plug-in that connects the MMH with Unity by means of a standardized protocol. The plug-in allows for the usage of a single type of skeleton for any body tracking system and, therefore, facilitates the switch between different body tracking systems during app development significantly.

MotionHub is developed at the research lab [MIREVI](https://www.mirevi.de/) from the [University of Applied Sciences Düsseldorf](https://hs-duesseldorf.de/en) within the scope of the project [HIVE](https://tinyurl.com/y3ugxo3p).

### Acknowledgements

The project Hive is sponsored by the [German Federal Ministry of Education and Research](https://www.bmbf.de/en/index.html) (BMBF) under the project number 16SV8182.

# Requirements

At the moment the middleware is only supported on Microsoft Windows operation systems.

- This Version of MotionHub was tested on Microsoft **Windows 10 64 bit**.

### Minimum Computer Requirements

- Seventh Gen Intel i5 Processor
- NVidia GeForce GTX 1070
- 4 GB Memory
- 6 GB Storage

# Documentation

The user and developer documentation including a full class diagram can be found in the `doc` folder.

# Supported Systems

MMH currently supports the listed body tracking systems.

| Supported Systems |
| ----------------- |
| Azure Kinect      |
| OptiTrack         |

# Setup and Building

The CMake System is used to generate Build Files and download all dependencies required. Please use the `CMakeLists.txt` file for generating.

- MMH is developed with Microsoft **Visual Studio 2017**. CMake has only been tested with this IDE.

Please Note that we use the Framework Qt for the UI. To build the project you need the [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123) and for editing `.ui` files you need [Qt Designer](https://www.qt.io/download).

# Authors

- **Philipp Ladwig** - Project Manager and Developer
- **Kester Evers** - Developer
- **Eric Jansen** - Developer

# Build with

- [Qt](https://www.qt.io/)
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
- [OSC Pack](http://www.rossbencina.com/code/oscpack)

# License

Pending
