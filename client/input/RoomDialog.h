#pragma once

#include <string>

// What the player picked in the Room dialog (see ShowRoomDialog): Create a
// brand-new room, Join an existing one by id, or Cancel out entirely.
struct RoomChoice {
	enum class Action { Create, Join, Cancel };
	Action action;
	std::string roomId; // only meaningful when action == Join
};

// Blocks until the player picks Create, Join, or Cancel in a small native
// Win32 window (a text box for the room id plus the three buttons) - shown
// once at startup, before the OpenCV game window exists. OpenCV's highgui
// has no text-input widget of its own, so this is a real WinAPI window
// rather than something drawn on the game canvas.
RoomChoice ShowRoomDialog();
