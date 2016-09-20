add_library (video_decoder_jni SHARED video_decoder_jni.cpp)

target_include_directories (video_decoder_jni PUBLIC ${PROJECT_SOURCE_DIR}/inc)
target_include_directories (video_decoder_jni PUBLIC ${JAVA_HOME}/include)

if (MSVC)
	target_include_directories (video_decoder_jni PUBLIC ${JAVA_HOME}/include/windows)
else ()
	target_include_directories (video_decoder_jni PUBLIC ${JAVA_HOME}/include/linux)
endif ()

install (TARGETS video_decoder_jni DESTINATION bin/video_decoder)

