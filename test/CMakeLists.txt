#---------------------------------------------------------------------------
#CMake minimum required version
cmake_minimum_required (VERSION 2.8)
#---------------------------------------------------------------------------
#look up dir's file, and add to var
aux_source_directory(. DIR_TESTS)
#---------------------------------------------------------------------------
#add include path
#include_directories(${LIBRARY_OUTPUT_PATH_INC})
#---------------------------------------------------------------------------
#add lib path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../depend/base/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../depend/json/lib)
#---------------------------------------------------------------------------
#generate test
add_executable(test_codec test_codec.cc ../src/codec.cc ../src/http_config.cc
    ../src/request_message.cc fake_buffer.cc fake_tcp_conn.cc fake_inetaddr.cc)
#---------------------------------------------------------------------------
#add link lib
target_link_libraries(test_codec base json)
#---------------------------------------------------------------------------
#install target 
#install(TARGETS json_test DESTINATION ${EXECUTABLE_OUTPUT_PATH})
#---------------------------------------------------------------------------
#add test
add_test(test_run ${EXECUTABLE_OUTPUT_PATH}/${TESTNAME})
#---------------------------------------------------------------------------
