cmake_minimum_required (VERSION 2.8.11)

project (video_decoder)

option (BUILD_TEST "Whether to build test program." OFF)

set (JAVA_HOME $ENV{JAVA_HOME} CACHE PATH "Home directory of Java Divelooment Toolkit (JDK)." FORCE)
 
add_subdirectory (jni bin)

if (${BUILD_TEST})
	message (STATUS "Test enabled!")
	message (STATUS "You can disable test by adding -DBUILD_TEST=ON to your cmake command line.")
	set (TEST_VIDEO_PATH CACHE FILEPATH "File path of video for test.")
	add_subdirectory (test test_bin)
else ()
	message (STATUS "Test disabled!")
	message (STATUS "You can enable test by setting -DBUILD_TEST=OFF to your cmake command line.")
endif()

install (FILES ${PROJECT_SOURCE_DIR}/inc/video_decoder.hpp DESTINATION include/video_decoder)
install (FILES ${PROJECT_SOURCE_DIR}/inc/avio_context_manager.h DESTINATION include/video_decoder)
