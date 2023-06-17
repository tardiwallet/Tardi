# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CONFIG_COMPILER_CXX_RTTI )

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(BitSigner)


idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)
# idf_build_set_property(COMPILE_OPTIONS "-frtti" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-DSPI_HOST_MAX=3" APPEND)

# macro(print_all_variables)
#     message(STATUS "print_all_variables------------------------------------------{")
#     get_cmake_property(_variableNames VARIABLES)
#     foreach (_variableName ${_variableNames})
#         message(STATUS "${_variableName}=${${_variableName}}")
#     endforeach()
#     message(STATUS "print_all_variables------------------------------------------}")
# endmacro()


# set(EXTRA_COMPONENT_DIRS ./components)
# set(COMPONENT_DIRS ${PROJECT_DIR}/components)
# set(COMPONENTS crypto esp-code-scanner esp32-camera lvgl lvgl_esp32_drivers)
add_compile_options(-fdiagnostics-color=always)

# component_compile_definitions("CONFIG_FREERTOS_HZ=1000")
# message(STATUS "message to display ${PROJECT_DIR}") 
# print_all_variables()
