#
# Build CMakeFile for linked list

CMAKE_MINIMUM_REQUIRED(VERSION 3.11.0)
PROJECT(linked-list)

# Set to use C++11

ADD_DEFINITIONS(-std=c++11)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# External projects for parquet-format and thrift
INCLUDE(ExternalProject)
INCLUDE(GoogleTestCMakeLists)
INCLUDE(CheckIncludeFileCXX)

#INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

#ExternalProject_Get_Property(google-glog install_dir)
#include_directories (${install_dir}/include)

ADD_CUSTOM_COMMAND(
   OUTPUT ${SOURCE_DIR}/src/gtest-all.cc
   COMMAND "touch ${SOURCE_DIR}/src/gtest-all.cc"
)
ExternalProject_Get_Property(googletest SOURCE_DIR)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/googletest ${SOURCE_DIR}/googletest/include)

SET_SOURCE_FILES_PROPERTIES(${SOURCE_DIR}/googletest/src/gtest-all.cc PROPERTIES GENERATED 1)

CHECK_INCLUDE_FILE_CXX("tr1/tuple" SYSTEM_HAS_TR1_TUPLE)
IF(SYSTEM_HAS_TR1_TUPLE)
	set_property(TARGET parquet-file-test PROPERTY COMPILE_FLAGS
	"-DGTEST_USE_OWN_TR1_TUPLE=0")
ENDIF(SYSTEM_HAS_TR1_TUPLE)

ADD_EXECUTABLE(linked_list_test linked_list_test.cc mutex_wrapper.cc perf_measurement.cc ${SOURCE_DIR}/googletest/src/gtest-all.cc)
ADD_DEPENDENCIES(linked_list_test googletest)
set_property(TARGET linked_list_test PROPERTY DEPENDS googletest)

