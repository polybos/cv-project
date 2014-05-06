cmake_minimum_required(VERSION 2.8)
project( cvproject )
find_package( OpenCV REQUIRED )
find_package( Boost COMPONENTS filesystem system )

add_subdirectory( src bin ) # put generated files of subdirectory 'src/' into a folder 'bin/'
