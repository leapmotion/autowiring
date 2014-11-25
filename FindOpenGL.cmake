#.rst:
# FindOpenGL
# ----------
#
# Total refactor of the cmake-supplied FindOpenGL.cmake module to be
# more in-line with the generate_import_target command.

set(_OpenGL_REQUIRED_VARS "")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  list(APPEND _OpenGL_REQUIRED_VARS OpenGL_LIBRARY)

  #opengl32.lib lives in the windows sdk, which means we can't use find_library since windows
  #doesn't add the wdk directory to any path variable that I could find.
  set(OpenGL_LIBRARY opengl32.lib CACHE STRING "OpenGL library for win32")
  set(OpenGL_INTERFACE_LIBS glu32.lib CACHE STRING "GLU library for win32")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # Mac
  list(APPEND _OpenGL_REQUIRED_VARS OpenGL_INCLUDE_DIR)
  find_path(OpenGL_INCLUDE_DIR OpenGL/gl.h DOC "Include for OpenGL on OSX")
  set(OpenGL_INTERFACE_LIBS "-framework OpenGL;-framework AGL")
else()
  # The first line below is to make sure that the proper headers
  # are used on a Linux machine with the NVidia drivers installed.
  # They replace Mesa with NVidia's own library but normally do not
  # install headers and that causes the linking to
  # fail since the compiler finds the Mesa headers but NVidia's library.
  # Make sure the NVIDIA directory comes BEFORE the others.
  #  - Atanas Georgiev <atanas@cs.columbia.edu>

  list(APPEND _OpenGL_REQUIRED_VARS OpenGL_INCLUDE_DIR)
  list(APPEND _OpenGL_REQUIRED_VARS OpenGL_LIBRARY)
  find_path(
    OpenGL_INCLUDE_DIR
    NAMES
      GL/gl.h
    PATHS
      /usr/share/doc/NVIDIA_GLX-1.0/include
      /usr/openwin/share/include
      /opt/graphics/OpenGL/include
      /usr/X11R6/include
  )

  find_library(
    OpenGL_LIBRARY
    NAMES
      GL
      MesaGL
    PATHS
      /opt/graphics/OpenGL/lib
      /usr/openwin/lib
      /usr/shlib /usr/X11R6/lib
  )
  find_library(
    OpenGL_INTERFACE_LIBS
    NAMES
      GLU
      MesaGLU
    PATHS
      ${OpenGL_gl_LIBRARY}
      /opt/graphics/OpenGL/lib
      /usr/openwin/lib
      /usr/shlib /usr/X11R6/lib
  )

  # # On Unix OpenGL most certainly always requires X11.
  # # Feel free to tighten up these conditions if you don't
  # # think this is always true.

  # if (OpenGL_gl_LIBRARY)
  #   if(NOT X11_FOUND)
  #     include(${CMAKE_CURRENT_LIST_DIR}/FindX11.cmake)
  #   endif()
  #   if (X11_FOUND)
  #     set (OPENGL_LIBRARIES ${X11_LIBRARIES})
  #   endif ()
  # endif ()
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(OpenGL REQUIRED_VARS ${_OpenGL_REQUIRED_VARS})
unset(_OpenGL_REQUIRED_VARS)

mark_as_advanced(
  OpenGL_LIBRARY
  OpenGL_INCLUDE_DIR
)

include(CreateImportTargetHelpers)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  generate_import_target(OpenGL INTERFACE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   #technically not true, but we don't have to do anything with the dll so...
  generate_import_target(OpenGL STATIC)
else()
  generate_import_target(OpenGL SHARED)
endif()
