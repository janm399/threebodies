project(httpclientperf)
cmake_minimum_required(VERSION 3.1)

# This is a hack on a mac
set(OPENSSL_ROOT_DIR /usr/local/Cellar/openssl@1.1/*)
find_package(Boost COMPONENTS system REQUIRED)
find_package(OpenSSL REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp session.cpp session.h)
target_link_libraries( main ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
 