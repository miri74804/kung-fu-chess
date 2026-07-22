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

# --- Networking deps for the client/server split (fetched once, then cached
# inside the build directory - no need to vendor them in the repo). TLS/zlib
# are disabled: this is a LAN course server, plain ws:// only. ---
include(FetchContent)

set(USE_TLS OFF CACHE BOOL "" FORCE)
set(USE_ZLIB OFF CACHE BOOL "" FORCE)
FetchContent_Declare(ixwebsocket
	GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
	GIT_TAG v11.4.5
)
FetchContent_MakeAvailable(ixwebsocket)

FetchContent_Declare(json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# --- Core library: pure game logic (model/rules/notation/engine/realtime/io) -
# no OpenCV, no window, nothing view-specific. Shared by both the headless
# server and the graphical client, so game rules only ever live in one place. ---
file(GLOB_RECURSE CORE_SOURCES
	"src/model/*.cpp" "src/rules/*.cpp" "src/notation/*.cpp"
	"src/engine/*.cpp" "src/realtime/*.cpp" "src/io/*.cpp" "src/utils/*.cpp"
	"src/protocol/*.cpp"
)
file(GLOB_RECURSE CORE_HEADERS
	"src/model/*.h" "src/rules/*.h" "src/notation/*.h"
	"src/engine/*.h" "src/realtime/*.h" "src/io/*.h" "src/utils/*.h"
	"src/protocol/*.h"
)
add_library(KungFuChessCore STATIC ${CORE_SOURCES} ${CORE_HEADERS} src/Constants.h)
target_include_directories(KungFuChessCore PUBLIC src)
target_compile_definitions(KungFuChessCore PUBLIC PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(KungFuChessCore PUBLIC nlohmann_json::nlohmann_json)

# --- Client: the graphical executable (view/input/Game) - talks to the
# server over WebSocket instead of owning a GameEngine directly. ---
file(GLOB_RECURSE CLIENT_SOURCES "src/view/*.cpp" "src/input/*.cpp" "src/client/*.cpp")
file(GLOB_RECURSE CLIENT_HEADERS "src/view/*.h" "src/input/*.h" "src/client/*.h")
add_executable(${PROJECT_NAME} src/CTD.cpp src/Game.cpp src/Game.h ${CLIENT_SOURCES} ${CLIENT_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE KungFuChessCore ixwebsocket nlohmann_json::nlohmann_json)

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENCV_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE ${OPENCV_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE
	$<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world4100d.lib>
	$<$<NOT:$<CONFIG:Debug>>:${OPENCV_LIB_DIR}/opencv_world4100.lib>
)

# Copy the matching OpenCV DLL next to the client .exe after every build.
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<$<CONFIG:Debug>:${OPENCV_BIN_DIR}/opencv_world4100d.dll>
		$<$<NOT:$<CONFIG:Debug>>:${OPENCV_BIN_DIR}/opencv_world4100.dll>
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()

# --- Server: headless process - owns the GameEngine, speaks WebSocket to
# both clients. No OpenCV dependency at all. ---
file(GLOB_RECURSE SERVER_SOURCES "server/*.cpp")
file(GLOB_RECURSE SERVER_HEADERS "server/*.h")
add_executable(KungFuChessServer ${SERVER_SOURCES} ${SERVER_HEADERS})
target_link_libraries(KungFuChessServer PRIVATE KungFuChessCore ixwebsocket nlohmann_json::nlohmann_json)
