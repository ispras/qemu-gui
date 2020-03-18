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

include_directories(src)

add_executable(qemu-gui src/main.cpp src/CreateVMForm.cpp src/QemuGUI.cpp
    src/RecordReplayTab.cpp src/VMSettingsForm.cpp src/QemuGUI.qrc
    src/GlobalConfig.cpp src/VMConfig.cpp src/QemuLauncher.cpp
    src/QMPInteraction.cpp src/TerminalSettingsForm.cpp
    src/PlatformInformationReader.cpp
    src/TerminalTab.cpp src/ConnectionSettingsForm.cpp src/QemuSocket.cpp
    src/ConsoleTab.cpp src/QemuRunOptions.cpp
    src/PlatformInfo.cpp src/VMPropertiesForm.cpp
    src/Device.cpp src/DeviceBus.cpp src/DeviceStorage.cpp
    src/DeviceFactory.cpp src/AddDeviceForm.cpp src/DeviceSystem.cpp
    src/DeviceUsb.cpp src/DeviceNetwork.cpp
    src/QemuImgLauncher.cpp
    src/CommandLineParameters.cpp
    src/DeviceForm.cpp)

target_link_libraries(qemu-gui Qt5::Widgets Qt5::Network)
target_compile_definitions(qemu-gui PUBLIC GUI)


add_executable(qemu-cli cli/main.cpp src/GlobalConfig.cpp src/VMConfig.cpp
    src/Device.cpp src/DeviceBus.cpp src/DeviceStorage.cpp src/DeviceSystem.cpp
    src/DeviceUsb.cpp src/DeviceNetwork.cpp
    src/CommandLineParameters.cpp src/DeviceFactory.cpp src/PlatformInfo.cpp)
target_link_libraries(qemu-cli Qt5::Core Qt5::Gui)



install(TARGETS qemu-gui
        RUNTIME DESTINATION bin)
install(TARGETS qemu-cli
        RUNTIME DESTINATION bin)
