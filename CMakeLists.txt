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

# All project sources except the real main() (src/CTD.cpp), shared by the
# main game and every demo/ smoke test so new view/model/etc. files are
# automatically available everywhere without editing this file again.
file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
list(REMOVE_ITEM ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/CTD.cpp")
set(SHARED_SOURCES ${ALL_SOURCES})

function(add_kfc_executable target main_cpp)
	add_executable(${target} ${main_cpp} ${SHARED_SOURCES} ${HEADERS})
	target_include_directories(${target} PRIVATE src)
	link_opencv(${target})
endfunction()

# --- Main game executable ---
add_kfc_executable(${PROJECT_NAME} src/CTD.cpp)

# --- Standalone demos / smoke tests (not part of the game itself) ---
add_kfc_executable(ShowBoardDemo demo/show_board_demo.cpp)
add_kfc_executable(LoadPiecesDemo demo/load_pieces_demo.cpp)
