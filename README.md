# STM32 development container
This is a development container for STM32 development boards with the following properties:
- base image: [Cpp Microsoft VS code development container](https://github.com/microsoft/vscode-dev-containers/tree/main/containers/cpp)
- GNU ARM cross toolchain
- CMake to build the project


## CubeMX configuration
- STM32CubeMX/IDE could be installed in the devcontainer  passing the display to the host (worked with an Ubuntu image, not Debian).
- However the GUI-tools are only used to generate the initial project setup (especially linker scripts), therefore it is installed on the host. The Arm toolchain is installed in the devcontainer.


## Doxygen
- [Reference example](https://www.cs.cmu.edu/~410/doc/doxygen.html)
- [Doxygen manual](https://www.doxygen.nl/manual/docblocks.html)


## Troubleshooting
- LIBUSB_ERROR_NO_DEVICE when trying OpenOCD with ST-Link. Remember to have correct udev rules active, see devops/scripts folder.


## Devcontainer
The devcontainer is based on the [C++ container from Microsoft](https://github.com/microsoft/vscode-dev-containers/tree/main/containers/cpp).

### License for devcontainer

Copyright (c) Microsoft Corporation. All rights reserved.

Licensed under the MIT License. See [LICENSE](https://github.com/microsoft/vscode-dev-containers/blob/main/LICENSE).
