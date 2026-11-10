#Include paths for deps
set(INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deps/stb)

#Common Dependency Files
set(DEPS_SRC_FILES_COMMON
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deps/stb/stb_image.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deps/stb/stb_image_write.h)

#Remove warnings for deps files
set_source_files_properties(
    ${DEPS_SRC_FILES_COMMON}
    PROPERTIES
    COMPILE_FLAGS "-W0"
)

source_group(Deps
    FILES ${DEPS_SRC_FILES_COMMON})