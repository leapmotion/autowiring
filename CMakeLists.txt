cmake_minimum_required(VERSION 2.8)

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

add_subdirectory(src)
add_subdirectory(test)
