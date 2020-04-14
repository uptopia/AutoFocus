#cmake need this line宣告要求的 cmake 最低版本
cmake_minimum_required(VERSION 2.8)

#Define project name
project( Test )

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 設定編譯模式
#set( CMAKE_BUILD_TYPE "Debug" )

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
#新增OPENCV庫
#指定OpenCV版本，程式碼如下
#find_package(OpenCV 3.3 REQUIRED)
#如果不需要指定OpenCV版本，程式碼如下
find_package(OpenCV REQUIRED)


# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
#message(STATUS "OpenCV library status:")
#message(STATUS "    version: ${OpenCV_VERSION}")
#message(STATUS "    libraries: ${OpenCV_LIBS}")
#message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

#MESSAGE("OpenCV_DIR = " ${OpenCV_DIR})
#MESSAGE("OpenCV_INCLUDE_DIRS = " ${OpenCV_INCLUDE_DIRS})
#MESSAGE("OpenCV_LIBS = " ${OpenCV_LIBS})

#新增OpenCV標頭檔案
# Add OpenCV headers location to your include paths
INCLUDE_DIRECTORIES(
    include
    ${OpenCV_INCLUDE_DIRS}
    /usr/include/flycapture
    )


# 新增一個可執行程式
# 語法：add_executable( 程式名 原始碼檔案 ）
# Declare the executable target built from your sources
add_executable( ${PROJECT_NAME}  test.cpp )


# 將庫檔案連結到可執行程式上
# Link your application with OpenCV libraries
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} /usr/lib/libflycapture.so)   
