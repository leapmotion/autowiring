## Utility for creating a combined installer for Windows
#
# This utility allows a WIX installer to be created with a single PACKAGE command,
# and allows both 32 and 64 variants of the installer to be installed on a system
# side-by-side, and includes both the Release and Debug versions of the configured
# package.
#
# On platforms that do not support multiple configurations, only one platform will
# be packaged.
#
# The result of invoking this function causes CPACK_XXX variables to be given
# values based on the passed arguments.  Operations that might need to consider
# these variables should be staged after the call to this routine.
#
# Usage;
# generate_wix_installer(
#   VENDOR Vendor            # The name of the library vendor
#   CONTACT Contact          # The E-mail address of the user point-of-contact
#   [NAME Name]              # The project name, if different from CMAKE_PROJECT_NAME
#   [VERSION Version]        # The project version, if different from PROJECT_VERSION
#   [GUID Guid]              # A GUID used to uniquely identify this library
#                            # Must be of the following form:
#                            #  {FFFFFFFF-0000-1111-2222-999999999999}
#                            # Defaults to a sha1 UUID generated from the major version number, vendor, project
#                            # name, and architecture with the CMake string command.
#   [LICENSE License]        # The path to the license file, defaults to LICENSE.txt
#   [README Readme]          # The path to the library description file, defaults to README.md
#   [CONFIGS Config1 [...]]  # Override the configurations to be packaged, defaults  to CMAKE_CONFIGURATION_TYPES
#   [WIXFILE WixFile]        # Overrides the location where the wxs template  file will be written.  Ignored on
#                            # platforms where wix isn't used.
# )
set(SELF ${CMAKE_CURRENT_LIST_DIR})
include(DefaultValue)
include(ParseVersion)

function(combined_installer)
  if(COMBINED_INSTALLER_DEBUG)
    message("Project name is ${CMAKE_PROJECT_NAME}")
  endif()

  set(options )
  set(oneValueArgs NAME VENDOR CONTACT VERSION GUID LICENSE README WIXFILE)
  set(multiValueArgs CONFIGS)

  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(NOT ARG_VENDOR)
    message(FATAL_ERROR "Vendor must be specified for the installer")
  endif()
  default_value(ARG_NAME ${CMAKE_PROJECT_NAME})

  parse_version(ARG ${${ARG_NAME}_VERSION})
  if(NOT ARG_VERSION)
    message(FATAL_ERROR "Could not determine the version number for this project, pass VERSION argument")
  endif()

  if(CMAKE_CONFIGURATION_TYPES)
    default_value(ARG_CONFIGS ${CMAKE_CONFIGURATION_TYPES})
  endif()
  default_value(ARG_WIXFILE "${CMAKE_BINARY_DIR}/CMakeFiles/standard_WixFile.wxs")
  default_value(ARG_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
  default_value(ARG_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

  string(TOLOWER ${ARG_NAME} ARG_NAME_LOWER)

  # Generate a GUID if the user didn't specifically request one
  if(NOT ARG_GUID)
    set(namespace_str "${ARG_NAME}##${ARG_VENDOR}##")

    # For 0. releases, we use the second digit as the indicator of an API-breaking change
    if(${ARG_VERSION_MAJOR} EQUAL 0)
      string(APPEND namespace_str ${ARG_VERSION_MINOR})
    else()
      string(APPEND namespace_str ${ARG_VERSION_MAJOR})
    endif()

    # 64-bit installations get a different upgrade GUID
    string(APPEND namespace_str ${CMAKE_SIZEOF_VOID_P})

    # There are also differences from version to version of visual studio
    string(APPEND namespace_str ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})

    # Now we generate the GUID
    string(
      UUID ARG_GUID
      NAMESPACE "20734ed8-cba2-4045-be14-2b3b3786eae2"
      NAME "${namespace_str}"
      TYPE SHA1
    )
  endif()

  if(WIN32)
    configure_file(${SELF}/WixFile.wxs.in ${ARG_WIXFILE})
  endif()

  # This is the upgrade GUID.  Part of the GUID is derived from the major version number.  Any time
  # the major version number is adjusted, the upgrade GUID changes.  This allows multiple versions
  # of the same product to be installed on a user's system at the same time, but also means that
  # manual uninstallation of older versions is required.
  #
  # For more information on the rationale for this process, see the discussion on semantic versioning
  # found at http://semver.org/
  if(COMBINED_INSTALLER_DEBUG)
    message("Upgrade GUID is ${ARG_GUID}")
  endif()
  set(CPACK_WIX_UPGRADE_GUID "{${ARG_GUID}}")

  # Need a custom wix installation template so that we update the CMake package registry correctly
  # Only really needed on Windows; Mac and Linux have pretty good default search behavior, so we
  # leave those alone.
  set(CPACK_WIX_TEMPLATE "${ARG_WIXFILE}")
  set(CPACK_MONOLITHIC_INSTALL ON)

  # Run the script that will grab the debug and release configurations and install them during packaging
  if(ARG_CONFIGS)
    set(CPACK_INSTALL_COMMANDS "")
    foreach(ONE_CONFIG IN LISTS ARG_CONFIGS)
      list(
        APPEND
        CPACK_INSTALL_COMMANDS
        "${CMAKE_COMMAND} --build \\\"${CMAKE_BINARY_DIR}\\\" --config ${ONE_CONFIG}"
        "${CMAKE_COMMAND} -DBUILD_TYPE=${ONE_CONFIG} -P \\\"${SELF}/cmake_package.cmake\\\""
      )
    endforeach()
  else()
    set(CPACK_INSTALL_COMMANDS
      "${CMAKE_COMMAND} --build \\\"${CMAKE_BINARY_DIR}\\\""
      "${CMAKE_COMMAND} -P \\\"${SELF}/cmake_package.cmake\\\""
    )
  endif()

  # Pick the generator in an appropriate way
  if(WIN32)
    set(CPACK_GENERATOR WIX ZIP)
  elseif(APPLE)
    # TODO: Add Bundle as a generator here
    set(CPACK_GENERATOR TGZ)
  else()
    set(CPACK_GENERATOR TGZ DEB)
  endif()

  # Packaging stuff, if an installer is being made instead
  set(CPACK_PACKAGE_VENDOR ${ARG_VENDOR})
  set(CPACK_PACKAGE_CONTACT ${ARG_CONTACT})
  set(CPACK_PACKAGE_DESCRIPTION_FILE ${ARG_README})
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${ARG_NAME})
  set(CPACK_RESOURCE_FILE_LICENSE ${ARG_LICENSE})
  set(CPACK_PACKAGE_VERSION_MAJOR "${ARG_VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${ARG_VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${ARG_VERSION_PATCH}")
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${ARG_NAME})
  set(CPACK_PACKAGE_INSTALL_DIRECTORY ${ARG_NAME})

  # Now we "call" cpack with the necessary variables
  include(CPack)
endfunction()
