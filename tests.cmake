include("standard/ParseVersion.cmake")

parse_version(V0 "0.0.1-alpha.1+build2223")
parse_version(V1 "1.0.0-beta.2")
parse_version(V2 "1.0.0+builtoforkevin")
parse_version(V3 "1.0.0")

function(dump_version NAME)
message("\
Version: ${${NAME}_VERSION}
  Major: ${${NAME}_VERSION_MAJOR}
  Minor: ${${NAME}_VERSION_MINOR}
  Patch: ${${NAME}_VERSION_PATCH}
  Prerelease: ${${NAME}_VERSION_PRERELEASE}
  Metadata: ${${NAME}_VERSION_METADATA}
")
endfunction()

dump_version(V0)
dump_version(V1)
dump_version(V2)
dump_version(V3)
