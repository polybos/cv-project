include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( prototype01 
main.cpp
classification.cpp
classification.h
display.cpp
display.h
fileLoader.cpp
fileLoader.h
parameterEvaluator.cpp
parameterEvaluator.h
statistics.cpp
statistics.h
tracking.cpp
tracking.h
trackingreforming.cpp
trackingreforming.h )

target_link_libraries( prototype01 ${OpenCV_LIBS} )

add_custom_target( copy_haarcascades
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/haarcascades" "${CMAKE_CURRENT_BINARY_DIR}/haarcascades"
)

# copy haarcascade files
#
#list( APPEND cascade_files
#    "haarcascades/car.xml"
#    "haarcascades/human.xml"
#    "haarcascades/bicycle.xml" )
#
#foreach( cascade ${cascade_files} )
#  message( WARNING "Try to copy file ${CMAKE_CURRENT_SOURCE_DIR}/${cascade} to ${CMAKE_CURRENT_BINARY_DIR}/${cascade}" )
#  add_custom_command(
#    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${cascade}"
#    COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${cascade}" "${CMAKE_CURRENT_BINARY_DIR}/${cascade}"
#  )
#endforeach( cascade )
