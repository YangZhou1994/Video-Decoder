add_library (video_decoder_jni SHARED video_decoder_jni.cpp)

set(CMAKE_C_CFLAGS "${CMAKE_C_CFLAGS} -fPIC")
set(CMAKE_CXX_CFLAGS "${CMAKE_CXX_CFLAGS} -fPIC")

target_include_directories (video_decoder_jni PUBLIC ${PROJECT_SOURCE_DIR}/inc)
target_include_directories (video_decoder_jni PUBLIC ${JAVA_HOME}/include)

if (MSVC)
	target_include_directories (video_decoder_jni PUBLIC ${JAVA_HOME}/include/windows)
else ()
	target_include_directories (video_decoder_jni PUBLIC ${JAVA_HOME}/include/linux)
endif ()

install (TARGETS video_decoder_jni DESTINATION bin/video_decoder)

