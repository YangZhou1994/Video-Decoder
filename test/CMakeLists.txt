enable_testing ()

add_executable (video_decoder_test video_decoder_test.cpp )

target_include_directories (video_decoder_test PUBLIC ${PROJECT_SOURCE_DIR}/inc)

target_link_libraries (video_decoder_test avutil avformat avcodec swscale opencv_core opencv_highgui opencv_imgproc)

add_test (NAME video_decoder_test COMMAND video_decoder_test ${TEST_VIDEO_PATH})
add_test (echo echo 1)