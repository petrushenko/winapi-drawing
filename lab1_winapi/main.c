#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>

#define IDT_TIMER1 1
#define IDM_RECTANGLE 1
#define ID_RECTANGLE 1
#define IDM_ELLIPSE 2
#define ID_ELLIPSE 2
#define IDM_BITMAP 3
#define ID_BITMAP 3
#define IDM_TIMER_START 4
#define MOVEMENT_STEP 5
#define RECTANGLE_WIDTH 100
#define RECTANGLE_HEIGHT 50
#define SPRITES_COLOR RGB(255, 127, 128);
#define BITMAP_FNAME L"cat.bmp"

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void PaintSprite(HWND);
void MoveLeft();
void MoveRight();
void MoveUp();
void MoveDown();
void MoveOnMouseMove(HWND, WPARAM, LPARAM);
void MoveOnKeyDown(WPARAM);
void MoveOnMouseWheel(WPARAM);
void MoveOnTimer(HWND hWnd);
void AddMenu(HWND);
void BordersCheck(int, int, int, int, int, int);

//GLOBAL VARS
HMENU hMenu;
HMENU hSpriteSubmenu;
HMENU hTimerSubmenu;
INT X = 10;
INT Y = 10;
USHORT IsTimer = 0;
SHORT DirX = MOVEMENT_STEP; 
SHORT DirY = MOVEMENT_STEP;
USHORT CurrentSprite = ID_RECTANGLE;
USHORT IsBitmap = 0;
HBITMAP hBitmap;
BITMAP Bitmap;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	WNDCLASSEX wcex = { 0 };
	HWND hWnd; 
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MainWindow";
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow(L"MainWindow", L"LAB1",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	RECT wndRect;
	switch (message)
	{
	case WM_CREATE:	
		hBitmap = (HBITMAP)LoadImage(NULL, BITMAP_FNAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
		AddMenu(hWnd);
		break;
	case WM_PAINT:
		PaintSprite(hWnd);
		break;	
	case WM_LBUTTONDOWN:
		MoveOnMouseMove(hWnd, wParam, lParam);
		GetClientRect(hWnd, &wndRect);
		InvalidateRect(hWnd, &wndRect, TRUE);
		break;
	case WM_MOUSEMOVE:
		MoveOnMouseMove(hWnd, wParam, lParam);
		GetClientRect(hWnd, &wndRect);
		InvalidateRect(hWnd, &wndRect, TRUE);
		break;
	case WM_KEYDOWN:
		MoveOnKeyDown(wParam);
		GetClientRect(hWnd, &wndRect);
		InvalidateRect(hWnd, &wndRect, TRUE);
		break;
	case WM_MOUSEWHEEL:
		MoveOnMouseWheel(wParam);
		GetClientRect(hWnd, &wndRect);
		InvalidateRect(hWnd, &wndRect, TRUE);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDM_RECTANGLE:
			IsBitmap = 0;
			CurrentSprite = ID_RECTANGLE;
			break;
		case IDM_ELLIPSE:
			IsBitmap = 0;
			CurrentSprite = ID_ELLIPSE;
			break;
		case IDM_BITMAP:
			IsBitmap = 1;
			CurrentSprite = ID_BITMAP;
			break;
		case IDM_TIMER_START:
			if (IsTimer)
			{
				IsTimer = 0;
				KillTimer(hWnd, IDT_TIMER1);
			}
			else
			{
				IsTimer = 1;
				SetTimer(hWnd, IDT_TIMER1, USER_TIMER_MINIMUM, (TIMERPROC)NULL);
			}
			break;
		default:
			break;
		}
	
	case WM_TIMER:
		if (wParam == IDT_TIMER1)
		{
			MoveOnTimer(hWnd);
			GetClientRect(hWnd, &wndRect);
			InvalidateRect(hWnd, &wndRect, TRUE);
		}		
		break;		
	case WM_DESTROY:
		KillTimer(hWnd, IDT_TIMER1);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void MoveRight()
{
	X += MOVEMENT_STEP;
}

void MoveLeft()
{
	X -= MOVEMENT_STEP;
}

void MoveUp()
{
	Y -= MOVEMENT_STEP;
}

void MoveDown()
{
	Y += MOVEMENT_STEP;
}

void PaintSprite(HWND hWnd)
{
	PAINTSTRUCT ps = { 0 };
	HDC winDC = BeginPaint(hWnd, &ps);
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	if (IsBitmap)
	{
		HDC memDC = CreateCompatibleDC(winDC);

		HBITMAP oldBitmap = SelectObject(memDC, hBitmap);
		TransparentBlt(winDC, X, Y, Bitmap.bmWidth, Bitmap.bmHeight, memDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, RGB(255, 255, 255));
		DeleteDC(memDC);
	}
	else
	{
		COLORREF color = SPRITES_COLOR;
		HPEN pen = CreatePen(PS_DASH, 3, color);
		HPEN oldPen = SelectObject(winDC, pen);
		HBRUSH brush = CreateSolidBrush(color);
		HBRUSH oldBrush = SelectObject(winDC, brush);
		switch (CurrentSprite)
		{
		case ID_RECTANGLE:
			Rectangle(winDC, X, Y, RECTANGLE_WIDTH + X, RECTANGLE_HEIGHT + Y);
			break;
		case ID_ELLIPSE:
			Ellipse(winDC, X, Y, RECTANGLE_WIDTH + X, RECTANGLE_HEIGHT + Y);
			break;
		default:
			break;
		}		
		SelectObject(winDC, oldPen);
		DeleteObject(pen);
		SelectObject(winDC, oldBrush);
		DeleteObject(brush);
	}
	EndPaint(hWnd, &ps);
}

void MoveOnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam == MK_LBUTTON)
	{

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		if (IsBitmap) 
		{
			x -= Bitmap.bmWidth / 2;
			y -= Bitmap.bmHeight / 2;
		}
		else
		{
			x -= RECTANGLE_WIDTH / 2;
			y -= RECTANGLE_HEIGHT / 2;
		}

		X = x;
		Y = y;
	}
}

void MoveOnKeyDown(WPARAM wParam)
{
	switch (wParam) {
	case VK_RIGHT:
		MoveRight();
		break;
	case VK_LEFT:
		MoveLeft();
		break;
	case VK_DOWN:
		MoveDown();
		break;
	case VK_UP:
		MoveUp();
		break;
	}
}

void MoveOnMouseWheel(WPARAM wParam)
{
	if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) 
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) 
		{
			MoveRight();
		}
		else 
		{
			MoveLeft();
		}
	}
	else {
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) 
		{
			MoveDown();
		}
		else 
		{
			MoveUp();
		}
	}
}

void AddMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	hSpriteSubmenu = CreatePopupMenu();
	hTimerSubmenu = CreatePopupMenu();

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSpriteSubmenu, L"Sprite");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hTimerSubmenu, L"Timer");
	AppendMenu(hSpriteSubmenu, MF_STRING, IDM_RECTANGLE, L"Rectangle");
	AppendMenu(hSpriteSubmenu, MF_STRING, IDM_ELLIPSE, L"Ellipse");
	AppendMenu(hSpriteSubmenu, MF_STRING, IDM_BITMAP, L"Bitmap");
	AppendMenu(hTimerSubmenu, MF_STRING, IDM_TIMER_START, L"Start/Stop");
	SetMenu(hWnd, hMenu);
}

void MoveOnTimer(HWND hWnd)
{
	X += DirX;
	Y += DirY;

	RECT wndRect;
	GetClientRect(hWnd, &wndRect);
	if (IsBitmap)
	{
		BordersCheck(X, Y, Bitmap.bmWidth, Bitmap.bmHeight, wndRect.right, wndRect.bottom);
	}
	else
	{
		BordersCheck(X, Y, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, wndRect.right, wndRect.bottom);
	}
}

void BordersCheck(int X, int Y, int rectWidth, int rectHeight, int maxWidth, int maxHeight)
{
	if (X < 0)
	{
		X = 0;
		DirX = MOVEMENT_STEP;
	}

	if (X + rectWidth > maxWidth)
	{
		X = maxWidth - rectWidth;
		DirX = -MOVEMENT_STEP;
	}

	if (Y < 0)
	{
		Y = 0;
		DirY = MOVEMENT_STEP;
	}

	if (Y + rectHeight > maxHeight)
	{
		Y = maxHeight - rectHeight;
		DirY = -MOVEMENT_STEP;
	}
}