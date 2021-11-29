cmake_minimum_required(VERSION 3.20)
project(xml_ruban)

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-g")
add_definitions(-w)
set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src src/_utils ${CMAKE_CURRENT_SOURCE_DIR})
#https://stackoverflow.com/questions/3201154/automatically-add-all-files-in-a-folder-to-a-target-using-cmake
file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")
message(${CPP_FILES})
#https://stackoverflow.com/questions/15550777/how-do-i-exclude-a-single-file-from-a-cmake-fileglob-pattern/15551302
list(FILTER CPP_FILES EXCLUDE REGEX ".*cmake-build.*")
add_executable(RubanXml ${CPP_FILES} src/entities/XmlElement.cpp)
#target_include_directories(RubanXml PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
#add_library(xml_ruban library.cpp)

#add_executable(RubanXml main.cpp library.cpp)