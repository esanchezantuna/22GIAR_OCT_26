cmake_minimum_required(VERSION 3.25)

project(CThreads LANGUAGES CXX)

include(cmake/projects.cmake)
set(PROJECTS_PATH ${CMAKE_CURRENT_LIST_DIR}/projects)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Config types" FORCE)

foreach(PROJECT ${PROJECT_LIST})
    message("Generating Project " ${PROJECT})

    file(MAKE_DIRECTORY ${PROJECTS_PATH}/${PROJECT})
    file(TOUCH ${PROJECTS_PATH}/${PROJECT}/main.cpp)

    add_executable(${PROJECT} ${PROJECTS_PATH}/${PROJECT}/main.cpp)
    target_compile_features(${PROJECT} PRIVATE cxx_std_20)
endforeach()