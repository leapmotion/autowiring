# primary function of this macro: strip symbols at link time on Mac and Linux
# secondary function: avoid MSVC warnings in Debug by specifying /nodefaultlib
# tertiary function: avoid MSVC warnings about combining /incremental with /ltcg
function(target_strip Target)
  if(MSVC)
    if(CMAKE_CXX_FLAGS_RELEASE)
      set_target_properties(${Target} PROPERTIES LINK_FLAGS_DEBUG "/INCREMENTAL:NO /NODEFAULTLIB:MSVCRT")
    else()
      set_target_properties(${Target} PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:MSVCRT")
    endif()
  else()
    set_target_properties(${Target} PROPERTIES LINK_FLAGS_RELEASE 
		"-Xlinker -unexported_symbol -Xlinker \"*\" -Xlinker -dead_strip -Xlinker -dead_strip_dylibs")
  endif()
endfunction()
