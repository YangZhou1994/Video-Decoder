enable_testing ()

add_executable(video_decoder_test EXCLUDE_FROM_ALL video_decoder_test.cpp)

include_directories (video_decoder_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries (video_decoder_test avutil avformat avcodec swscale opencv_core opencv_highgui opencv_imgproc)

add_test (NAME video_decoder_test COMMAND video_decoder_test ${TEST_VIDEO_PATH})
