#MSVC pch macro.  Copied from http://pastebin.com/84dm5rXZ and modified by Walter Gray
macro(add_pch SourcesVar PrecompiledHeader PrecompiledSource)
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
    add_compile_options(/Yu)
  endif(MSVC)
endmacro(add_pch)