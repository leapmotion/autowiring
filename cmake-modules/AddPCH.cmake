#MSVC pch macro.  Copied from http://pastebin.com/84dm5rXZ and modified by Walter Gray
macro(add_pch TargetName PrecompiledHeader PrecompiledSource)
  if(MSVC)
    set_source_files_properties(
      ${PrecompiledSource}
      PROPERTIES
      COMPILE_FLAGS "/Yc${PrecompiledHeader}"
    )
    target_sources(${TargetName} PRIVATE ${PrecompiledHeader} ${PrecompiledSource})
    set_property(TARGET ${TargetName} APPEND PROPERTY COMPILE_FLAGS "/Yu")
  endif(MSVC)
endmacro(add_pch)
