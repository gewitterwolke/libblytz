cmake_minimum_required(VERSION 2.8)

project( libblytz )

set(CMAKE_VERBOSE_MAKEFILE OFF)

FILE(GLOB blytz_src_files
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
list(REMOVE_ITEM blytz_src_files ${CMAKE_CURRENT_SOURCE_DIR}/blytz-test.cpp)

include(FindPNG)
if(NOT PNG_FOUND)
	message(SEND_ERROR "Couldn't find PNG library.")
endif(NOT PNG_FOUND)

include(FindCURL)
if(NOT CURL_FOUND)
	message(SEND_ERROR "Couldn't find curl library.")
endif(NOT CURL_FOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake/modules/")

include(FindQREncode)
if(NOT QRENCODE_FOUND)
	message(SEND_ERROR "Couldn't find qrencode library.")
endif(NOT QRENCODE_FOUND)

include(FindOpenSSL)
if(NOT OPENSSL_FOUND)
	message(SEND_ERROR "Couldn't find OpenSSL crypto library.")
endif(NOT OPENSSL_FOUND)

include_directories(. ${QRENCODE_INCLUDE_DIR})
add_library(blytz SHARED ${blytz_src_files})
target_link_libraries(blytz LINK_PUBLIC ${PNG_LIBRARIES} ${OPENSSL_LIBRARIES} ${QRENCODE_LIBRARY} ${CURL_LIBRARIES})

add_executable(blytz-test blytz-test.cpp)
IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	# compile gtest locally
	ADD_SUBDIRECTORY (gtest-1.7.0)
	include_directories(gtest-1.7.0/include gtest-1.7.0)

	enable_testing()
	target_link_libraries(blytz-test LINK_PUBLIC blytz gtest gtest_main)
ELSE()
	# use system library
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIR})
	target_link_libraries(blytz-test LINK_PUBLIC blytz ${GTEST_LIBRARIES})
ENDIF()
