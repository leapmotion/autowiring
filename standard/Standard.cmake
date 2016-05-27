# Makes adding things a little easier
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Need these for the rest of these scripts to work properly
include(CTest)
include(CheckTypeSize)

# Omnibus include file that brings in all standard CMake scripts
include(CombinedInstaller)
include(DefaultValue)
include(GenerateVersion)
include(SetStandardOutputVariables)
include(InstallHeaders)
