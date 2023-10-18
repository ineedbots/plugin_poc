file(GLOB_RECURSE MyCPPSources src/*.c*)

include_directories(include)

IF (WIN32)
ELSE()
    link_libraries(dl)
ENDIF()
	

add_executable(main ${MyCPPSources})
