project(autowiring)
cmake_minimum_required(VERSION 2.8)

set(AUTOWIRING_MAJOR_VERSION 0)
set(AUTOWIRING_MINOR_VERSION 1)
set(AUTOWIRING_PATCH_VERSION 0)
set(AUTOWIRING_VERSION ${AUTOWIRING_MAJOR_VERSION}.${AUTOWIRING_MINOR_VERSION}.${AUTOWIRING_PATCH_VERSION})

set(INSTALL_INCLUDE_DIR src CACHE PATH "Path to Autowiring header files")

# Adds a precompiled header
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  if(MSVC)
    set_source_files_properties(${PrecompiledSource}
        PROPERTIES
        COMPILE_FLAGS "/Yc${PrecompiledHeader}"
        )
    foreach( src_file ${${SourcesVar}} )
        set_source_files_properties(
            ${src_file}
            PROPERTIES
            COMPILE_FLAGS "/Yu${PrecompiledHeader}"
            )
    endforeach( src_file ${${SourcesVar}} )
    list(APPEND ${SourcesVar} ${PrecompiledHeader} ${PrecompiledSource})
  endif(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

function(ADD_MSVC_DISABLED_FILES SourceGroupName SourcesVar ...)
  if(WIN32)
    set_source_files_properties( ${ARGN} PROPERTIES HEADER_FILE_ONLY TRUE )
    source_group(${SourceGroupName} FILES ${ARGN})
	set(${SourcesVar} ${${SourcesVar}} ${ARGN} PARENT_SCOPE)
  endif(WIN32)
endfunction(ADD_MSVC_DISABLED_FILES)

add_subdirectory(C++11)
add_subdirectory(src)
add_subdirectory(test)

configure_file(autowiring-config.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/autowiring-config.cmake" @ONLY)
configure_file(autowiring-configVersion.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/autowiring-configVersion.cmake" @ONLY)

# Install autowiring-config.cmake and autowiring-configVersion.cmake
install (FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/autowiring-config.cmake"
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/autowiring-configVersion.cmake"
  DESTINATION cmake
  COMPONENT dev
)

