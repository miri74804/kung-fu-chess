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

int main(int argc, char** argv) {
	// Without this, Windows silently rescales our window if display scaling
	// (e.g. 125%/150%, common on high-DPI screens) is on - the image then
	// gets stretched on screen and click coordinates stop lining up with
	// its actual pixels (an edge click can land in the next cell over).
	// This just tells Windows "I'll handle my own pixels" - unrelated to
	// Img/drawing rules, it's a one-time OS-level process setting, so it
	// belongs here rather than inside Game.
	SetProcessDPIAware();

	// Socket calls fail on Windows until WSAStartup has run - IXWebSocket
	// doesn't call it for you.
	ix::initNetSystem();

	std::string serverUrl = argc > 1 ? argv[1] : "ws://127.0.0.1:8080";

	Game game(serverUrl);
	int exitCode = game.run();

	ix::uninitNetSystem();
	return exitCode;
}
