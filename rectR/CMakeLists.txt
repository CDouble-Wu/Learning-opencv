cmake_minimum_required(VERSION 2.8)
project( rectR )
find_package( OpenCV REQUIRED )
add_executable( rectR rectR.cpp)
target_link_libraries( rectR ${OpenCV_LIBS} )
