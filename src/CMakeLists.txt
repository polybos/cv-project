include_directories( ${OpenCV_INCLUDE_DIRS} )

# simple opencv test
add_executable( opencv_test opencv_test.cpp )
target_link_libraries( opencv_test ${OpenCV_LIBS} )

# simple gui based on opencv highgui
add_executable( simple_gui simple_gui.cpp )
target_link_libraries( simple_gui ${OpenCV_LIBS} )
