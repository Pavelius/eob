#include "crt.h"
#include "draw.h"
#include "win.h"

#pragma pack(push)
#pragma pack(1)
static struct video_8t {
	BITMAPINFO			bmp;
	unsigned char		bmp_pallette[256 * 4];
} video_descriptor;
#pragma pack(pop)

static int tokey(int vk) {
	switch(vk) {
	case VK_CONTROL: return Ctrl;
	case VK_MENU: return Alt;
	case VK_SHIFT: return Shift;
	case VK_LEFT: return KeyLeft;
	case VK_RIGHT: return KeyRight;
	case VK_UP: return KeyUp;
	case VK_DOWN: return KeyDown;
	case VK_PRIOR: return KeyPageUp;
	case VK_NEXT: return KeyPageDown;
	case VK_HOME: return KeyHome;
	case VK_END: return KeyEnd;
	case VK_BACK: return KeyBackspace;
	case VK_DELETE: return KeyDelete;
	case VK_RETURN: return KeyEnter;
	case VK_ESCAPE: return KeyEscape;
	case VK_SPACE: return KeySpace;
	case VK_TAB: return KeyTab;
	case VK_F1: return F1;
	case VK_F2: return F2;
	case VK_F3: return F3;
	case VK_F4: return F4;
	case VK_F5: return F5;
	case VK_F6: return F6;
	case VK_F7: return F7;
	case VK_F8: return F8;
	case VK_F9: return F9;
	case VK_F10: return F10;
	case VK_F11: return F11;
	case VK_F12: return F12;
	case VK_MULTIPLY: return Alpha + (unsigned)'*';
	case VK_DIVIDE: return Alpha + (unsigned)'/';
	case VK_ADD: return Alpha + (unsigned)'+';
	case VK_SUBTRACT: return Alpha + (unsigned)'-';
	case VK_OEM_COMMA: return Alpha + (unsigned)',';
	case VK_OEM_PERIOD: return Alpha + (unsigned)'.';
	default: return Alpha + vk;
	}
}

static int handle(HWND hwnd, MSG& msg) {
	switch(msg.message) {
	case WM_TIMER:
		if(msg.hwnd != hwnd)
			break;
		if(msg.wParam == InputTimer)
			return InputTimer;
		break;
	case WM_KEYDOWN:
		return tokey(msg.wParam);
	case WM_KEYUP:
		return InputKeyUp;
	case WM_CHAR:
		hot::param = msg.wParam;
		return InputSymbol;
	case WM_COMMAND:
		if(HIWORD(msg.wParam) == 0)
			return msg.lParam;
		break;
	}
	return 0;
}

static LRESULT CALLBACK WndProc(HWND hwnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_ERASEBKGND:
		if(draw::canvas) {
			RECT rc; GetClientRect(hwnd, &rc);
			video_descriptor.bmp.bmiHeader.biSize = sizeof(video_descriptor.bmp.bmiHeader);
			video_descriptor.bmp.bmiHeader.biWidth = draw::canvas->width;
			video_descriptor.bmp.bmiHeader.biHeight = -draw::canvas->height;
			video_descriptor.bmp.bmiHeader.biBitCount = draw::canvas->bpp;
			video_descriptor.bmp.bmiHeader.biPlanes = 1;
			if(draw::canvas->bpp == 8 && draw::palt)
				memcpy(video_descriptor.bmp_pallette, draw::palt, 4 * 256);
			if(rc.right != draw::canvas->width || rc.bottom != draw::canvas->height)
				StretchDIBits((void*)wParam,
					0, 0, rc.right, rc.bottom,
					0, 0, draw::canvas->width, draw::canvas->height,
					draw::canvas->bits, &video_descriptor.bmp, DIB_RGB_COLORS, SRCCOPY);
			else
				SetDIBitsToDevice((void*)wParam,
					0, 0, rc.right, rc.bottom,
					0, 0, 0, draw::canvas->height,
					draw::canvas->bits, &video_descriptor.bmp, DIB_RGB_COLORS);
		}
		return 1;
	case WM_CLOSE:
		PostQuitMessage(-1);
		return 0;
	}
	return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

static const char* register_class(const char* class_name) {
	WNDCLASS wc;
	if(!GetClassInfoA(GetModuleHandleA(0), class_name, &wc)) {
		memset(&wc, 0, sizeof(wc));
		wc.style = CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW; // Own DC For Window.
		wc.lpfnWndProc = WndProc;					// WndProc Handles Messages
		wc.hInstance = GetModuleHandleA(0);			// Set The Instance
		wc.hIcon = (void*)LoadIconA(wc.hInstance, (const char*)1); // WndProc Handles Messages
		wc.hCursor = (void*)LoadCursorA(0, (const char*)32512);
		wc.lpszClassName = class_name;				// Set The Class Name
		RegisterClassA(&wc);						// Attempt To Register The Window Class
	}
	return class_name;
}

static HWND	hwnd;

bool draw::create(int width, int height) {
	unsigned dwStyle = WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_VISIBLE; // Windows Style;
	RECT rc = {0, 0, width, height};
	AdjustWindowRectEx(&rc, dwStyle, 0, 0);
	hwnd = CreateWindowExA(0, register_class("CFaceWindow"), 0, dwStyle,
		32, 32, rc.right - rc.left, rc.bottom - rc.top, 0, 0, GetModuleHandleA(0), 0);
	return hwnd != 0;
}

void draw::rawinput() {
	MSG	msg;
	if(!hwnd)
		return;
	InvalidateRect(hwnd, 0, 1);
	while(GetMessageA(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
		hot::key = handle(hwnd, msg);
		if(hot::key == InputNoUpdate)
			continue;
		if(hot::key) {
			if(hot::key != MouseMove && hot::key >= (int)MouseLeft) {
				if(GetKeyState(VK_SHIFT) < 0)
					hot::key |= Shift;
				if(GetKeyState(VK_MENU) < 0)
					hot::key |= Alt;
				if(GetKeyState(VK_CONTROL) < 0)
					hot::key |= Ctrl;
			}
			return;
		}
	}
	hot::key = 0;
}

void draw::rawredraw() {
	MSG	msg;
	if(!hwnd)
		return;
	InvalidateRect(hwnd, 0, 1);
	while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
		handle(hwnd, msg);
	}
}

void draw::setcaption(const char* string) {
	SetWindowTextA(hwnd, string);
}

void draw::settimer(unsigned milleseconds) {
	if(milleseconds)
		SetTimer(hwnd, InputTimer, milleseconds, 0);
	else
		KillTimer(hwnd, InputTimer);
}