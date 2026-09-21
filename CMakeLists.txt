cmake_minimum_required(VERSION 3.10)
project(pendulums)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
get_filename_component(TARGET "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
 
#SFML
find_package(SFML REQUIRED)
 
#TOML
find_package(tomlplusplus REQUIRED)

#BOOST
find_package(Boost REQUIRED)
 
add_executable(${TARGET} main.cpp
modes.cpp
)

target_compile_definitions(${TARGET} PRIVATE _USE_MATH_DEFINES)
 
target_link_libraries(${TARGET} PRIVATE sfml::sfml tomlplusplus::tomlplusplus Boost::headers)
 
add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.toml"
        "$<TARGET_FILE_DIR:${TARGET}>/config.toml"
)