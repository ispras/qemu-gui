cmake_minimum_required(VERSION 3.1.0)

project(qemu-gui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Network)

include_directories(gui device)

add_executable(qemu-gui gui/main.cpp gui/CreateVMForm.cpp gui/QemuGUI.cpp
    gui/RecordReplayTab.cpp gui/VMSettingsForm.cpp gui/QemuGUI.qrc
    gui/GlobalConfig.cpp gui/VMConfig.cpp gui/QemuLauncher.cpp
    gui/QMPInteraction.cpp gui/TerminalSettingsForm.cpp
    gui/PlatformInformationReader.cpp
    gui/TerminalTab.cpp gui/ConnectionSettingsForm.cpp gui/QemuSocket.cpp
    gui/ConsoleTab.cpp gui/QemuRunOptions.cpp
    gui/PlatformInfo.cpp gui/VMPropertiesForm.cpp
    gui/AddDeviceForm.cpp
    device/Device.cpp device/DeviceBus.cpp device/DeviceStorage.cpp
    device/DeviceFactory.cpp device/DeviceSystem.cpp
    device/DeviceUsb.cpp device/DeviceNetwork.cpp
    gui/QemuImgLauncher.cpp
    gui/CommandLineParameters.cpp
    gui/DeviceForm.cpp)

target_link_libraries(qemu-gui Qt5::Widgets Qt5::Network)
target_compile_definitions(qemu-gui PUBLIC GUI)


add_executable(qemu-cli cli/main.cpp gui/GlobalConfig.cpp gui/VMConfig.cpp
    device/Device.cpp device/DeviceBus.cpp device/DeviceStorage.cpp device/DeviceSystem.cpp
    device/DeviceUsb.cpp device/DeviceNetwork.cpp device/DeviceFactory.cpp
    gui/CommandLineParameters.cpp gui/PlatformInfo.cpp)
target_link_libraries(qemu-cli Qt5::Core Qt5::Gui)



install(TARGETS qemu-gui
        RUNTIME DESTINATION bin)
install(TARGETS qemu-cli
        RUNTIME DESTINATION bin)
