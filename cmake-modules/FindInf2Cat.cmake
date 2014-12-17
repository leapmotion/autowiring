# - Try to find Inf2Cat
# Once done this will define
#  Inf2Cat_FOUND - System has Inf2Cat
#  Inf2Cat_BIN - The path to Inf2Cat

set(wdkregpath80 "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot]")
get_filename_component(wdkpath80 ${wdkregpath80} ABSOLUTE)

set(wdkregpath81 "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot81]")
get_filename_component(wdkpath81 ${wdkregpath81} ABSOLUTE)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
 set(WDK_ARCH "x64")
else()
 set(WDK_ARCH "x86")
endif()

# Directly locate inf2cat:
find_program(
  Inf2Cat_BIN Inf2Cat
  PATHS ${wdkpath81} ${wdkpath80} ${WDK_ROOT_ALTERNATE}
  PATH_SUFFIXES bin/${WDK_ARCH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Inf2Cat DEFAULT_MSG Inf2Cat_BIN)


