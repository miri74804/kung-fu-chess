#include "RoomDialog.h"
#include <windows.h>

namespace {
	constexpr int ID_EDIT = 101;
	constexpr int ID_CREATE = 102;
	constexpr int ID_JOIN = 103;
	constexpr int ID_CANCEL = 104;

	// Lives on ShowRoomDialog's stack for the lifetime of its message loop -
	// stashed in the window's GWLP_USERDATA so the WndProc (which only gets
	// an HWND) can reach the edit control and report back which button, if
	// any, has been pressed yet.
	struct DialogContext {
		HWND hEdit = nullptr;
		bool done = false;
		RoomChoice result{ RoomChoice::Action::Cancel, "" };
	};

	// Trims stray leading/trailing whitespace (easy to pick up copying a
	// room id by hand) - the edit control itself is ES_UPPERCASE, so case
	// mismatches can't happen, only whitespace can.
	std::string getEditText(HWND hEdit) {
		char buffer[64] = {};
		GetWindowTextA(hEdit, buffer, sizeof(buffer));
		std::string text(buffer);

		size_t start = text.find_first_not_of(" \t\r\n");
		if (start == std::string::npos) {
			return "";
		}
		size_t end = text.find_last_not_of(" \t\r\n");
		return text.substr(start, end - start + 1);
	}

	LRESULT CALLBACK RoomDialogWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		DialogContext* ctx = reinterpret_cast<DialogContext*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if (message == WM_COMMAND) {
			if (ctx == nullptr) {
				return 0;
			}
			int id = LOWORD(wParam);
			if (id == ID_CREATE) {
				ctx->result = { RoomChoice::Action::Create, "" };
				ctx->done = true;
			}
			else if (id == ID_JOIN) {
				ctx->result = { RoomChoice::Action::Join, getEditText(ctx->hEdit) };
				ctx->done = true;
			}
			else if (id == ID_CANCEL) {
				ctx->result = { RoomChoice::Action::Cancel, "" };
				ctx->done = true;
			}
			return 0;
		}
		if (message == WM_CLOSE) {
			// Don't destroy here - the message loop below owns that, once it
			// notices ctx->done and stops pumping messages.
			if (ctx != nullptr) {
				ctx->result = { RoomChoice::Action::Cancel, "" };
				ctx->done = true;
			}
			return 0;
		}

		return DefWindowProcA(hwnd, message, wParam, lParam);
	}
}

RoomChoice ShowRoomDialog() {
	const char* className = "KungFuChessRoomDialog";
	HINSTANCE instance = GetModuleHandle(nullptr);

	static bool registered = false;
	if (!registered) {
		WNDCLASSA wc{};
		wc.lpfnWndProc = RoomDialogWndProc;
		wc.hInstance = instance;
		wc.lpszClassName = className;
		wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		RegisterClassA(&wc);
		registered = true;
	}

	HWND hwnd = CreateWindowExA(WS_EX_DLGMODALFRAME, className, "Room",
		WS_POPUP | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 170,
		nullptr, nullptr, instance, nullptr);

	DialogContext ctx;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&ctx));

	CreateWindowA("STATIC", "Room ID (only needed to Join):",
		WS_CHILD | WS_VISIBLE, 15, 15, 260, 20, hwnd, nullptr, instance, nullptr);
	// ES_UPPERCASE so whatever's typed always matches the server's
	// uppercase-only generated room ids, regardless of Caps Lock/Shift.
	ctx.hEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE,
		15, 40, 260, 24, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_EDIT)), instance, nullptr);

	CreateWindowA("BUTTON", "Create", WS_CHILD | WS_VISIBLE,
		15, 85, 80, 30, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_CREATE)), instance, nullptr);
	CreateWindowA("BUTTON", "Join", WS_CHILD | WS_VISIBLE,
		105, 85, 80, 30, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_JOIN)), instance, nullptr);
	CreateWindowA("BUTTON", "Cancel", WS_CHILD | WS_VISIBLE,
		195, 85, 80, 30, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_CANCEL)), instance, nullptr);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg;
	while (!ctx.done && GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hwnd);
	return ctx.result;
}
