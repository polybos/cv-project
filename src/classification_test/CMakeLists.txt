include_directories( ${OpenCV_INCLUDE_DIRS} )

# simple gui based on opencv highgui
add_executable( classification classification.cpp )
target_link_libraries( classification ${OpenCV_LIBS} )


if( Boost_FOUND ) # following needs boost ...

  include_directories( ${Boost_INCLUDE_DIR} )

  # flann
  add_executable( flann flann.cpp )
  target_link_libraries( flann ${OpenCV_LIBS} ${Boost_LIBRARIES} )

  # surf
  add_executable( surf surf.cpp )
  target_link_libraries( surf ${OpenCV_LIBS} ${Boost_LIBRARIES} )  

else( Boost_FOUND )
  message( WARNING "flann.cpp and surf.cpp needs BOOST" )
endif( Boost_FOUND )
