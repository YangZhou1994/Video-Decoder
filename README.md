# Video Decoder

[![AUR](https://img.shields.io/aur/license/yaourt.svg?maxAge=2592000)](LICENSE)

Video Decoder is a tool for decoding video stored in memory.

It depends on FFMPEG. To build this tool, you should have libavcodec-dev, libavformat-dev, libavutil-dev and libswscale-dev installed on your machine:

```Shell
sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev
```

Also to test this tool, you should have libcv-dev:

```Shell
sudo apt-get install libopencv-dev
```

The video decoder is a header-only tool. To use it in your code, include the [video_decoder.hpp](inc/video_decoder.hpp) is enough.

To use it in Java, JNI wrapper building is supported with CMake. To build the JNI shared library, run in your console under the project directory:

```Shell
cmake -DBUILD_TEST=OFF .
make -j 4
sudo make install
```

If you want to enable test, run:

```Shell
cmake -DBUILD_TEST=ON [-DTEST_VIDEO_PATH=/path/to/your/test/video] .
make -j 4
cd test_bin
ctest
``` 