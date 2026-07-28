// Repo: https://github.com/miri74804/kung-fu-chess
//
// Entry point: connects to the game server over WebSocket (the server owns
// the board/GameEngine now), then hands off to Game, which owns the
// network connection/input/rendering and runs the live loop until the
// window is closed.
#include <iostream>
#include <ixwebsocket/IXNetSystem.h>
#include <windows.h>
#include "Game.h"
#include "input/RoomDialog.h"

int main(int argc, char** argv) {
	// Without this, Windows silently rescales our window if display scaling
	// (e.g. 125%/150%, common on high-DPI screens) is on - the image then
	// gets stretched on screen and click coordinates stop lining up with
	// its actual pixels (an edge click can land in the next cell over).
	// This just tells Windows "I'll handle my own pixels" - unrelated to
	// Img/drawing rules, it's a one-time OS-level process setting, so it
	// belongs here rather than inside Game.
	SetProcessDPIAware();

	// Without this, typed non-ASCII text (e.g. a Hebrew username) comes back
	// from std::cin in the console's local codepage, not UTF-8 - Protocol's
	// JSON encoding requires valid UTF-8, so sending it as-is throws from a
	// background network thread with nothing to catch it there, crashing
	// the whole process (std::terminate -> abort()). This makes the console
	// itself hand back proper UTF-8 bytes instead.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	// Socket calls fail on Windows until WSAStartup has run - IXWebSocket
	// doesn't call it for you.
	ix::initNetSystem();

	std::string serverUrl = argc > 1 ? argv[1] : "ws://127.0.0.1:8080";

	// Shell-based login (per the course spec - no GUI login screen): just a
	// display name, no password/account behind it yet.
	std::cout << "Enter your username: ";
	std::string username;
	std::getline(std::cin, username);
	if (username.empty()) {
		username = "Player";
	}

	// Room selection is the one part of the "Home screen" that's an actual
	// window rather than a shell prompt - OpenCV's highgui (the game window
	// itself) has no text-input widget to type a room id into.
	RoomChoice roomChoice = ShowRoomDialog();
	if (roomChoice.action == RoomChoice::Action::Cancel) {
		ix::uninitNetSystem();
		return 0;
	}

	bool isCreate = roomChoice.action == RoomChoice::Action::Create;
	Game game(serverUrl, username, isCreate, roomChoice.roomId);
	int exitCode = game.run();

	ix::uninitNetSystem();
	return exitCode;
}
