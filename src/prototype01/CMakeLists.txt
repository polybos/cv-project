include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( prototype01 
main.cpp
classification.cpp
classification.h
display.cpp
display.h
fileLoader.cpp
fileLoader.h
tracking.cpp
tracking.h )

target_link_libraries( prototype01 ${OpenCV_LIBS} )
