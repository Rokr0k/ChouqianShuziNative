#include <Windows.h>
#include "resource.h"

LPCWSTR lpszClassName = TEXT("ChouqianShuzi");
HINSTANCE g_inst;

HWND g_background, g_start, g_end, g_exceptions, g_selections, g_roll, g_change, g_result;
HWND g_start_static, g_end_static, g_exceptions_static;

int g_mode;

LPCWSTR g_digit[11] = { TEXT("零"), TEXT("一"), TEXT("二"), TEXT("三"), TEXT("四"), TEXT("五"), TEXT("六"), TEXT("七"), TEXT("八"), TEXT("九"), TEXT("十") };

unsigned long long GetTime();

unsigned long long g_timer;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
int validate();
void roll(HWND hwnd);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpszCommandLine, _In_ int nCmdShow)
{
	g_inst = hInstance;

	srand(GetTime());

	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszClassName;
	wc.lpfnWndProc = WindowProc;
	wc.style = 0;

	RegisterClass(&wc);

	HANDLE maShanZhengHandle = INVALID_HANDLE_VALUE;
	HRSRC maShanZhengRes = FindResource(hInstance, MAKEINTRESOURCE(IDF_MASHANZHENG), L"BINARY");
	if (maShanZhengRes)
	{
		HGLOBAL mem = LoadResource(hInstance, maShanZhengRes);
		if (mem != NULL)
		{
			void* data = LockResource(mem);
			size_t len = SizeofResource(hInstance, maShanZhengRes);
			DWORD nFonts = 0;
			maShanZhengHandle = AddFontMemResourceEx(data, len, NULL, &nFonts);
		}
	}
	HANDLE notoSansSCHandle = INVALID_HANDLE_VALUE;
	HRSRC notoSansSCRes = FindResource(hInstance, MAKEINTRESOURCE(IDF_NOTOSANSSC), L"BINARY");
	if (notoSansSCRes)
	{
		HGLOBAL mem = LoadResource(hInstance, notoSansSCRes);
		if (mem != NULL)
		{
			void* data = LockResource(mem);
			size_t len = SizeofResource(hInstance, notoSansSCRes);
			DWORD nFonts = 0;
			notoSansSCHandle = AddFontMemResourceEx(data, len, NULL, &nFonts);
		}
	}

	HWND hwnd = CreateWindow(lpszClassName, TEXT("抽签数字"), WS_OVERLAPPED | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_inst, NULL);
	ShowWindow(hwnd, nCmdShow);

	MSG msg;
	while (GetMessage(&msg, hwnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	RemoveFontMemResourceEx(maShanZhengHandle);
	RemoveFontMemResourceEx(notoSansSCHandle);

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		HFONT fTitle = CreateFont(50, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Noto Sans SC Regular"));
		HFONT fControl = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Noto Sans SC Regular"));
		HFONT fButton = CreateFont(25, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Noto Sans SC Regular"));
		HFONT fResult = CreateFont(200, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Ma Shan Zheng"));

		g_background = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, 0, 0, 500, 600, hwnd, NULL, g_inst, NULL);
		SendMessage(g_background, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadImage(g_inst, MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, 0, 0, 0));

		HWND temp = CreateWindow(TEXT("STATIC"), TEXT("韩国数码媒体高等学校"), WS_CHILD | WS_VISIBLE | SS_SIMPLE, 10, 10, 510, 50, hwnd, NULL, g_inst, NULL);
		SendMessage(temp, WM_SETFONT, (WPARAM)fTitle, TRUE);

		g_start_static = CreateWindow(TEXT("STATIC"), TEXT("最小"), WS_CHILD | WS_VISIBLE | SS_SIMPLE, 10, 70, 60, 30, hwnd, NULL, g_inst, NULL);
		SendMessage(g_start_static, WM_SETFONT, (WPARAM)fControl, TRUE);
		g_start = CreateWindow(TEXT("EDIT"), TEXT("1"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 60, 70, 60, 30, hwnd, NULL, g_inst, NULL);
		SendMessage(g_start, WM_SETFONT, (WPARAM)fControl, TRUE);

		g_end_static = CreateWindow(TEXT("STATIC"), TEXT("最大"), WS_CHILD | WS_VISIBLE | SS_SIMPLE, 130, 70, 60, 30, hwnd, NULL, g_inst, NULL);
		SendMessage(g_end_static, WM_SETFONT, (WPARAM)fControl, TRUE);
		g_end = CreateWindow(TEXT("EDIT"), TEXT("34"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 180, 70, 60, 30, hwnd, NULL, g_inst, NULL);
		SendMessage(g_end, WM_SETFONT, (WPARAM)fControl, TRUE);

		g_exceptions_static = CreateWindow(TEXT("STATIC"), TEXT("例外"), WS_CHILD | WS_VISIBLE | SS_SIMPLE, 250, 70, 60, 30, hwnd, NULL, g_inst, NULL);
		SendMessage(g_exceptions_static, WM_SETFONT, (WPARAM)fControl, TRUE);
		g_exceptions = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 300, 70, 200, 30, hwnd, NULL, g_inst, NULL);
		SendMessage(g_exceptions, WM_SETFONT, (WPARAM)fControl, TRUE);

		g_selections = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 10, 70, 490, 30, hwnd, NULL, g_inst, NULL);
		SendMessage(g_selections, WM_SETFONT, (WPARAM)fControl, TRUE);

		g_roll = CreateWindow(TEXT("BUTTON"), TEXT("抽签"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 510, 70, 55, 30, hwnd, (HMENU)1, g_inst, NULL);
		SendMessage(g_roll, WM_SETFONT, (WPARAM)fButton, TRUE);

		g_change = CreateWindow(TEXT("BUTTON"), TEXT("更改状态"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 110, 105, 30, hwnd, (HMENU)2, g_inst, NULL);
		SendMessage(g_change, WM_SETFONT, (WPARAM)fButton, TRUE);

		g_result = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE | SS_CENTER, 490, 310, 200, 600, hwnd, NULL, g_inst, NULL);
		SendMessage(g_result, WM_SETFONT, (WPARAM)fResult, TRUE);

		return 0;
	}
	case WM_COMMAND:
		switch (LOWORD(wp))
		{
		case 1:
		{
			int state = validate();
			switch(state)
			{
			case -1:
				MessageBox(hwnd, TEXT("内存分配失败"), TEXT("错误"), MB_ICONERROR | MB_OK);
				SetWindowText(g_result, NULL);
				break;
			case 0:
				EnableWindow(g_start, FALSE);
				EnableWindow(g_end, FALSE);
				EnableWindow(g_exceptions, FALSE);
				EnableWindow(g_selections, FALSE);
				EnableWindow(g_roll, FALSE);
				EnableWindow(g_change, FALSE);
				g_timer = GetTime() + 10000000;
				SetTimer(hwnd, 5521, 20, NULL);
				break;
			case 1:
				MessageBox(hwnd, TEXT("没有可用的数字"), TEXT("警告"), MB_ICONERROR | MB_OK);
				SetWindowText(g_result, NULL);
				if (!g_mode)
					SetWindowText(g_exceptions, NULL);
				break;
			case 2:
				MessageBox(hwnd, TEXT("有不可用的数字"), TEXT("警告"), MB_ICONERROR | MB_OK);
				SetWindowText(g_result, NULL);
				break;
			}
		}
			break;
		case 2:
			g_mode = !g_mode;
			if (g_mode)
			{
				ShowWindow(g_start_static, SW_HIDE);
				ShowWindow(g_start, SW_HIDE);
				ShowWindow(g_end_static, SW_HIDE);
				ShowWindow(g_end, SW_HIDE);
				ShowWindow(g_exceptions_static, SW_HIDE);
				ShowWindow(g_exceptions, SW_HIDE);
				ShowWindow(g_selections, SW_SHOW);
			}
			else
			{
				ShowWindow(g_start_static, SW_SHOW);
				ShowWindow(g_start, SW_SHOW);
				ShowWindow(g_end_static, SW_SHOW);
				ShowWindow(g_end, SW_SHOW);
				ShowWindow(g_exceptions_static, SW_SHOW);
				ShowWindow(g_exceptions, SW_SHOW);
				ShowWindow(g_selections, SW_HIDE);
			}
			break;
		}
		return 0;
	case WM_SIZE:
		MoveWindow(g_background, 0, 0, LOWORD(lp), HIWORD(lp), TRUE);
		MoveWindow(g_result, (LOWORD(lp) - 200) / 2, (HIWORD(lp) - 600) / 2, 200, 600, TRUE);
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdc = (HDC)wp;
		SetBkMode(hdc, TRANSPARENT);
		if ((HWND)lp == g_result)
		{
			SetTextColor(hdc, RGB(255, 255, 0));
			return (LRESULT)CreateSolidBrush(RGB(255, 0, 0));
		}
		else
		{
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
	}
	case WM_TIMER:
		if (g_timer > GetTime())
		{
			LPWSTR result = calloc(4, sizeof(TCHAR));
			if (result != NULL)
			{
				result[0] = g_digit[rand() % 11][0];
				result[1] = g_digit[rand() % 11][0];
				result[2] = g_digit[rand() % 11][0];
				SetWindowText(g_result, result);
				free(result);
			}
		}
		else
		{
			KillTimer(hwnd, 5521);
			EnableWindow(g_start, TRUE);
			EnableWindow(g_end, TRUE);
			EnableWindow(g_exceptions, TRUE);
			EnableWindow(g_selections, TRUE);
			EnableWindow(g_roll, TRUE);
			EnableWindow(g_change, TRUE);
			roll(hwnd);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
}

int validate()
{
	if (g_mode)
	{
		LPWSTR text = malloc(sizeof(TCHAR) * 1024);
		if (text == NULL)
			return -1;

		GetWindowText(g_selections, text, 1024);

		int* lpnSelections = malloc(sizeof(int) * 512);
		int nSelectionsLength = 0;

		int nbFlag = 0;
		TCHAR* buffer;
		if (lpnSelections != NULL)
		{
			buffer = NULL;
			LPWSTR token = wcstok_s(text, TEXT(" "), &buffer);
			while (token)
			{
				lpnSelections[nSelectionsLength++] = _wtoi(token);
				if (lpnSelections[nSelectionsLength - 1] < 0 || lpnSelections[nSelectionsLength - 1]>99)
				{
					nbFlag = 1;
					break;
				}
				token = wcstok_s(NULL, TEXT(" "), &buffer);
			}
		}

		free(text);

		if (lpnSelections == 0)
			return 1;
		else if (nbFlag)
			return 2;
		else
			return 0;
	}
	else
	{
		int nStart, nEnd;
		LPWSTR text = malloc(sizeof(TCHAR) * 1024);
		if (text == NULL)
			return -1;

		GetWindowText(g_start, text, 1024);
		nStart = _wtoi(text);

		GetWindowText(g_end, text, 1024);
		nEnd = _wtoi(text) + 1;

		int* lpbExceptions = calloc(nEnd - nStart, sizeof(int));

		if (lpbExceptions == NULL)
			return -1;

		GetWindowText(g_exceptions, text, 1024);
		TCHAR* buffer = NULL;
		LPWSTR token = wcstok_s(text, TEXT(" "), &buffer);
		while (token)
		{
			lpbExceptions[_wtoi(token) - nStart] = 1;
			token = wcstok_s(NULL, TEXT(" "), &buffer);
		}

		free(text);

		int flag = 1;
		for (int i = 0; i < nEnd - nStart; i++)
			if (!lpbExceptions[i])
				flag = 0;

		if (flag)
			return 1;
		else if (nStart < 0 || nStart > 99 || nEnd < 1 || nEnd > 100 || nStart > nEnd)
			return 2;
		else
			return 0;
	}
}

void roll(HWND hwnd)
{
	if (g_mode)
	{
		LPWSTR text = malloc(sizeof(TCHAR) * 1024);
		if (text == NULL)
		{
			MessageBox(hwnd, TEXT("内存分配失败"), TEXT("错误"), MB_ICONSTOP | MB_OK);
			SetWindowText(g_result, NULL);
			return;
		}

		GetWindowText(g_selections, text, 1024);

		int* lpnSelections = malloc(sizeof(int) * 512);
		int nSelectionsLength = 0;

		if (lpnSelections != NULL)
		{
			TCHAR* buffer = NULL;
			LPWSTR token = wcstok_s(text, TEXT(" "), &buffer);
			while (token)
			{
				lpnSelections[nSelectionsLength++] = _wtoi(token);
				if (lpnSelections[nSelectionsLength - 1] < 0 || lpnSelections[nSelectionsLength - 1]>99)
				{
					MessageBox(hwnd, TEXT("有不可用的数字"), TEXT("警告"), MB_ICONSTOP | MB_OK);
					SetWindowText(g_result, NULL);
					free(text);
					return;
				}
				token = wcstok_s(NULL, TEXT(" "), &buffer);
			}
		}
		if (nSelectionsLength == 0)
		{
			MessageBox(hwnd, TEXT("没有可用的数字"), TEXT("警告"), MB_ICONSTOP | MB_OK);
			SetWindowText(g_result, NULL);
			free(text);
			return;
		}

		int nIndex = rand() % nSelectionsLength;

		TCHAR lpszResult[10] = { 0 };

		if (lpnSelections[nIndex] <= 10)
		{
			wcscpy_s(lpszResult, 10, g_digit[lpnSelections[nIndex]]);
		}
		else
		{
			int ten = lpnSelections[nIndex] / 10 % 10;
			int one = lpnSelections[nIndex] % 10;
			if (ten > 1)
				wcscat_s(lpszResult, 10, g_digit[ten]);
			wcscat_s(lpszResult, 10, g_digit[10]);
			if (one)
				wcscat_s(lpszResult, 10, g_digit[one]);
		}
		SetWindowText(g_result, lpszResult);

		if (lpnSelections != NULL)
		{
			LPWSTR number = malloc(sizeof(TCHAR) * 10);
			if (number != NULL)
			{
				text[0] = TEXT('\0');
				for (int i = 0; i < nSelectionsLength; i++)
				{
					if (i != nIndex)
					{
						_itow_s(lpnSelections[i], number, 10, 10);
						if (wcslen(text) > 0)
							wcscat_s(text, 1024, TEXT(" "));
						wcscat_s(text, 1024, number);
					}
				}
				SetWindowText(g_selections, text);
				free(number);
			}

			free(lpnSelections);
		}

		free(text);
	}
	else
	{
		int nStart, nEnd;
		LPWSTR text = malloc(sizeof(TCHAR) * 1024);

		if (text == NULL)
		{
			MessageBox(hwnd, TEXT("内存分配失败"), TEXT("错误"), MB_ICONSTOP | MB_OK);
			SetWindowText(g_result, NULL);
			return;
		}

		GetWindowText(g_start, text, 1024);
		nStart = _wtoi(text);

		GetWindowText(g_end, text, 1024);
		nEnd = _wtoi(text) + 1;

		if (nStart < 0 || nStart>99 || nEnd < 1 || nEnd>100 || nStart > nEnd)
		{
			MessageBox(hwnd, TEXT("有不可用的数字"), TEXT("警告"), MB_ICONSTOP | MB_OK);
			SetWindowText(g_result, NULL);
			free(text);
			return;
		}

		int* lpbExceptions = calloc(nEnd - nStart, sizeof(int));

		if (lpbExceptions != NULL)
		{
			GetWindowText(g_exceptions, text, 1024);
			TCHAR* buffer = NULL;
			LPWSTR token = wcstok_s(text, TEXT(" "), &buffer);
			while (token)
			{
				lpbExceptions[_wtoi(token) - nStart] = 1;;
				token = wcstok_s(NULL, TEXT(" "), &buffer);
			}

			int flag = 1;
			for (int i = 0; i < nEnd - nStart; i++)
				if (!lpbExceptions[i])
					flag = 0;
			if (flag)
			{
				MessageBox(hwnd, TEXT("没有可用的数字"), TEXT("警告"), MB_ICONSTOP | MB_OK);
				SetWindowText(g_exceptions, NULL);
				SetWindowText(g_result, NULL);
				free(text);
				return;
			}
		}

		int nResult = rand() % (nEnd - nStart) + nStart;
		if (lpbExceptions != NULL)
			while (lpbExceptions[nResult - nStart])
				nResult = rand() % (nEnd - nStart) + nStart;

		TCHAR lpszResult[10] = { 0 };

		if (nResult <= 10)
		{
			wcscpy_s(lpszResult, 10, g_digit[nResult]);
		}
		else
		{
			int ten = nResult / 10 % 10;
			int one = nResult % 10;
			if (ten > 1)
				wcscat_s(lpszResult, 10, g_digit[ten]);
			wcscat_s(lpszResult, 10, g_digit[10]);
			if (one)
				wcscat_s(lpszResult, 10, g_digit[one]);
		}
		SetWindowText(g_result, lpszResult);

		if (lpbExceptions != NULL)
		{
			LPWSTR number = malloc(sizeof(TCHAR) * 1024);
			if (number != NULL)
			{
				_itow_s(nResult, number, 1024, 10);
				GetWindowText(g_exceptions, text, 1024);
				if (wcslen(text) > 0)
					wcscat_s(number, 1024, TEXT(" "));
				wcscat_s(number, 1024, text);
				SetWindowText(g_exceptions, number);
				free(number);
			}

			free(lpbExceptions);
		}

		free(text);
	}
}

unsigned long long GetTime()
{
	FILETIME file;
	GetSystemTimeAsFileTime(&file);
	ULARGE_INTEGER time;
	time.HighPart = file.dwHighDateTime;
	time.LowPart = file.dwLowDateTime;
	return time.QuadPart;
}