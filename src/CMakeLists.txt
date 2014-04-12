include_directories( ${OpenCV_INCLUDE_DIRS} )

# simple opencv test
add_executable( opencv_test opencv_test.cpp )
add_executable( bg_sub bg_sub.cpp )
target_link_libraries( opencv_test ${OpenCV_LIBS} )
target_link_libraries( bg_sub ${OpenCV_LIBS} )


