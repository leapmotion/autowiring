#Our OpenSSL setup is non-standard, so set these variables up beforehand

find_path(OPENSSL_ROOT_DIR
  NAMES include/openssl/opensslconf.h
  PATH_SUFFIXES openssl
)

include(CreateImportTargetHelpers)

if(MSVC)
  find_library(LIB_EAY_DEBUG NAMES libeay32.lib HINTS ${OPENSSL_ROOT_DIR}/lib/debug)
  find_library(LIB_EAY_RELEASE NAMES libeay32.lib HINTS ${OPENSSL_ROOT_DIR}/lib/release)
  find_library(SSL_EAY_DEBUG NAMES ssleay32.lib HINTS ${OPENSSL_ROOT_DIR}/lib/debug)
  find_library(SSL_EAY_RELEASE NAMES ssleay32.lib HINTS ${OPENSSL_ROOT_DIR}/lib/release)
  mark_as_advanced(LIB_EAY_DEBUG LIB_EAY_RELEASE SSL_EAY_DEBUG SSL_EAY_RELEASE)

  include(${CMAKE_ROOT}/Modules/FindOpenSSL.cmake)

  #override the bad OPENSSL_LIBRARIES value
  include(SelectConfigurations)
  select_configurations(LIB_EAY LIBRARY)
  select_configurations(SSL_EAY LIBRARY)

  set(OPENSSL_LIBRARIES "${SSL_EAY_LIBRARY} ${LIB_EAY_LIBRARY}")
  set(OPENSSL_LIBRARY_DEBUG "${LIB_EAY_DEBUG};${SSL_EAY_DEBUG}")
  set(OPENSSL_LIBRARY_RELEASE "${LIB_EAY_RELEASE};${SSL_EAY_RELEASE}")
  generate_import_target(OPENSSL STATIC)

else()
  include(${CMAKE_ROOT}/Modules/FindOpenSSL.cmake)
  if(EXISTS "${OPENSSL_CRYPTO_LIBRARY}")
    set(OPENSSL_CRYPTO_FOUND TRUE)
  endif()
  if(EXISTS "${OPENSSL_SSL_LIBRARY}")
    set(OPENSSL_SSL_FOUND TRUE)
  endif()
  generate_import_target(OPENSSL_CRYPTO STATIC TARGET OPENSSL::Crypto)
  generate_import_target(OPENSSL_SSL STATIC TARGET OPENSSL::SSL)
  generate_import_target(OPENSSL INTERFACE)
  target_link_libraries(OPENSSL::OPENSSL INTERFACE OPENSSL::Crypto OPENSSL::SSL)
endif()


