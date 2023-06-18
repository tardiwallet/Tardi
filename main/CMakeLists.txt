# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

# FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/main/*.*)

set(app_sources 
    main.cpp
    # DynamicQRCode.cpp
    Controller.cpp
    GUI.cpp
    NavButtons.cpp
    Button.cpp
    Window.cpp
    ScanQRWindow.cpp
    DisplayQRWindow.cpp
    ListRotatorWindow.cpp
    ButtonsWindow.cpp
    CodeScanner.cpp
    Camera.cpp
    qrcodegen.c
    # MyCborListener.cpp
    # examples/bc-crypto-base/test.c
    # examples/examples.cpp
    GpioKey.cpp
    EncoderEmulator.cpp
    # camera/WifiAP.c
    # camera/HttpCapture.c
)
# SimpleButtonsWindow.cpp


set(deps
lvgl
lvgl_esp32_drivers
esp32-camera
esp-code-scanner
)
#crypto

# idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include REQUIRES ${deps})
idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)

# target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_17)
   
add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
# add_definitions(-DLV_CONF_SKIP)
# add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
# target_compile_options(${COMPONENT_TARGET} PUBLIC
#     -DLV_LVGL_H_INCLUDE_SIMPLE
# )

