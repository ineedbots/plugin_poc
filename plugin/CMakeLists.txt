file(GLOB_RECURSE MyCPPSources src/*.c*)

include_directories(include)
include_directories(../main/include)

add_library(myplugin SHARED ${MyCPPSources})

IF (WIN32)
    set_target_properties(myplugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins)
ENDIF()
