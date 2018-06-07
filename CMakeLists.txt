cmake_minimum_required(VERSION 3.1.0)

project(qemu-gui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Network)

add_executable(qemu-gui src/main.cpp src/CreateVMForm.cpp src/QemuGUI.cpp src/RecordReplayTab.cpp src/VMSettingsForm.cpp //
src/QemuGUI.qrc src/GlobalConfig.cpp src/VMConfig.cpp src/QemuLauncher.cpp src/QMPInteraction.cpp)

# Use the Widgets module from Qt 5.
target_link_libraries(qemu-gui Qt5::Widgets)
target_link_libraries(qemu-gui Qt5::Network)
