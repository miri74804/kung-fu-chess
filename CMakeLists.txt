cmake_minimum_required(VERSION 3.16)
project(KungFuChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- OpenCV (prebuilt Windows package, installed outside the repo) ---
# Override with: cmake -DOPENCV_ROOT=<path to opencv>\build ..
set(OPENCV_ROOT "C:/opencv/opencv/build" CACHE PATH "Path to OpenCV's build directory")
set(OPENCV_INCLUDE_DIR "${OPENCV_ROOT}/include")
set(OPENCV_LIB_DIR "${OPENCV_ROOT}/x64/vc16/lib")
set(OPENCV_BIN_DIR "${OPENCV_ROOT}/x64/vc16/bin")

# Applies the shared OpenCV include/link/DLL-copy setup to any target.
function(link_opencv target)
	target_include_directories(${target} PRIVATE ${OPENCV_INCLUDE_DIR})
	target_link_directories(${target} PRIVATE ${OPENCV_LIB_DIR})
	target_link_libraries(${target}
		$<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world4100d.lib>
		$<$<NOT:$<CONFIG:Debug>>:${OPENCV_LIB_DIR}/opencv_world4100.lib>
	)
	target_compile_definitions(${target} PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
	if(WIN32)
		add_custom_command(TARGET ${target} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<$<CONFIG:Debug>:${OPENCV_BIN_DIR}/opencv_world4100d.dll>
			$<$<NOT:$<CONFIG:Debug>>:${OPENCV_BIN_DIR}/opencv_world4100.dll>
			$<TARGET_FILE_DIR:${target}>)
	endif()
endfunction()

# --- Main game executable ---
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE src)
link_opencv(${PROJECT_NAME})

# --- Small standalone demo: loads assets/board.png and shows it in a window ---
add_executable(ShowBoardDemo demo/show_board_demo.cpp src/view/Img.cpp src/view/Img.h)
target_include_directories(ShowBoardDemo PRIVATE src)
link_opencv(ShowBoardDemo)
