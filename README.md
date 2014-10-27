video-server
============

Build
-----

 - OpenCV and POCO libraries are supposed to be installed
 - Out of source build is recommended
 - FFMPEG: on Windows I needed to copy libavcodec/x86/vp8dsp_init.o to libavcodec/x86/vp8dsp_init
 
TODO
----

 - proper multicore build (propagation to x264 and ffmpeg libraries)
