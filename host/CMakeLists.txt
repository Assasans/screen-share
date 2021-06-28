project(ScreenShare-Host VERSION 0.1.0)

add_executable(
	ScreenShare-Host

	src/main.cxx

	src/Frame.cxx

	src/compression/JPEG.cxx
	src/compression/PNG.cxx
	src/compression/Raw.cxx
	src/compression/RawDiff.cxx

	src/network/Server.cxx
)

set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake_modules")

find_package(X11 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(DevIL REQUIRED)
find_package(TurboJPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(Boost 1.76.0 COMPONENTS program_options REQUIRED)

include_directories(${X11_INCLUDE_DIR} ${X11_xcb_xfixes_INCLUDE_PATH} ${OpenCV_INCLUDE_DIRS} ${IL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TurboJPEG_INCLUDE_DIRS} ${PNG_INCLUDE_DIR})

link_directories(${X11_LIBRARIES})

target_link_libraries(ScreenShare-Host ${X11_LIBRARIES} ${X11_Xfixes_LIB} ${OpenCV_LIBS} ${IL_LIBRARIES} ${Boost_LIBRARIES} ${TurboJPEG_LIBRARIES} ${PNG_LIBRARY})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
