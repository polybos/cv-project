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

# copy haarcascade files
list( APPEND cascade_files
    "haarcascades/car.xml"
    "haarcascades/human.xml"
    "haarcascades/bicycle.xml" )

foreach( cascade ${cascade_files} )
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${cascade}"
    COMMAND cmake -E copy
"${CMAKE_CURRENT_SOURCE_DIR}/${cascade}"
"${CMAKE_CURRENT_BINARY_DIR}/${cascade}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${cascade}"
  )
endforeach( cascade )
