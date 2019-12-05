// lab2.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lab2.h"
#include <Commdlg.h>


#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
RECT rcClient;
int col, row, cellCount;
int font;
int Pdx = 0, Pdy = 0;
unsigned long bytes;
WCHAR* FileText = (WCHAR*)calloc(1024, sizeof(WCHAR*));
WCHAR **cellsText = nullptr;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int StrToInt(TCHAR* s)
{
	int temp = 0; // число
	int i = 0;
	int sign = 0; // знак числа 0- положительное, 1 - отрицательное
	if (s[i] == '-')
	{
		sign = 1;
		i++;
	}
	while (s[i] >= 0x30 && s[i] <= 0x39)
	{
		temp = temp + (s[i] & 0x0F);
		temp = temp * 10;
		i++;
	}
	temp = temp / 10;
	if (sign == 1)
		temp = -temp;
	return(temp);
}

WCHAR* ResultStr(WCHAR* str, unsigned long* length)
{
	unsigned long strLen = 0;
	WCHAR* result = (WCHAR*)calloc(*length, sizeof(WCHAR*));
	for (int i = 0; i < *length; i++)
	{
		if (str[i] != L'\n' && str[i] != L' ' && str[i] != L'\r')
		{
			if (result != nullptr)
			{
				result[strLen] = str[i];
			}		
			strLen++;
		}	
	}
	*length = strLen;
	return result;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 600, 200, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int len;
	TCHAR str[20];
	static HWND hBtnDrawTable;
	static HWND hBtnSelectFile;
	static HWND hEdtCol;
	static HWND hEdtRow;
	static HWND Col;
	static HWND Row;
	static HWND FileName;
    switch (message)
    {
	case WM_CREATE:
		{
			hBtnDrawTable = CreateWindow(_T("button"), _T("Таблица"), WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 100, 100, 30, hWnd, 0, hInst, NULL);
			ShowWindow(hBtnDrawTable, SW_SHOWNORMAL);

			hBtnSelectFile = CreateWindow(_T("button"), _T("Файл..."), WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 65, 100, 30, hWnd, 0, hInst, NULL);
			ShowWindow(hBtnSelectFile, SW_SHOWNORMAL);

			Col = CreateWindow(_T("static"), _T("Колонки"), WS_CHILD | WS_VISIBLE, 10, 10, 60, 20, hWnd, 0, hInst, NULL);
			ShowWindow(hBtnSelectFile, SW_SHOWNORMAL);

			Row = CreateWindow(_T("static"), _T("Строки"), WS_CHILD | WS_VISIBLE, 10, 40, 60, 20, hWnd, 0, hInst, NULL);
			ShowWindow(hBtnSelectFile, SW_SHOWNORMAL);

			FileName = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE, 120, 65, 450, 30, hWnd, 0, hInst, NULL);
			ShowWindow(hBtnSelectFile, SW_SHOWNORMAL);

			hEdtCol = CreateWindow(_T("edit"), _T("5"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 70, 10, 60, 20, hWnd, 0, hInst, NULL);
			ShowWindow(hEdtCol, SW_SHOWNORMAL);

			hEdtRow = CreateWindow(_T("edit"), _T("4"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 70, 40, 60, 20, hWnd, 0, hInst, NULL);
			ShowWindow(hEdtRow, SW_SHOWNORMAL);
		}
		break;
    case WM_COMMAND:
        {
			if (lParam == (LPARAM)hBtnDrawTable)
			{
				len = GetWindowText(hEdtCol, str, 20);
				col = StrToInt(str);
				len = GetWindowText(hEdtRow, str, 20);
				row = StrToInt(str);

				WNDCLASSEXW wcex;

				wcex.cbSize = sizeof(WNDCLASSEX);

				wcex.style = CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc = ChildProc;
				wcex.cbClsExtra = 0;
				wcex.cbWndExtra = 0;
				wcex.hInstance = hInst;
				wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LAB2));
				wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
				wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
				wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB2);
				wcex.lpszClassName = _T("ChildWClass");
				wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

				RegisterClassExW(&wcex);
				HWND child = CreateWindowW(_T("ChildWClass"), _T("Таблица"), WS_OVERLAPPEDWINDOW, 10, 10, col * 100 + 10, row * 50 + 10, hWnd, nullptr, hInst, nullptr);
				ShowWindow(child, SW_NORMAL);
				UpdateWindow(child);
			}

			if (lParam == (LPARAM)hBtnSelectFile)
			{
				OPENFILENAME ofn;
				HANDLE hf;
				WCHAR szFile[260] = {0};
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0");
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetOpenFileName(&ofn) == TRUE)
				{
					hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
						0, (LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
						(HANDLE)NULL);
					if (!ReadFile(hf, FileText, 1024, &bytes, nullptr))
					{
						//error
					}
					SetWindowText(FileName, ofn.lpstrFile);
					FileText = ResultStr(FileText, &bytes);
					CloseHandle(hf);
				}
			}

            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_GETMINMAXINFO:
	{
		GetClientRect(hWnd, &rcClient);
		MINMAXINFO *mmi = (MINMAXINFO *)lParam;
		mmi->ptMinTrackSize.x = 300;
		mmi->ptMinTrackSize.y = 200;
	}
	break;
	case WM_CREATE:
	{
		int textPos = 1;
		
		cellCount = ((bytes - 2) % (row * col * 2) == 0) ? (bytes - 2) / (row * col * 2) : (bytes - 2) / (row * col * 2) + 1;
		cellsText = (WCHAR **)calloc(row * col, sizeof(WCHAR*));
		for (int i = 0; i < row * col; i++)
		{
			if (cellsText != nullptr)
			{
				cellsText[i] = (WCHAR*)calloc(cellCount + 1, sizeof(WCHAR));
			}				
			for (int j = 0; j < cellCount; j++)
			{
				if (FileText[textPos] != 0)
				{
					cellsText[i][j] = FileText[textPos];				
					textPos++;
				}
			}
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		SIZE sz;
		WCHAR *tmp = (WCHAR *)calloc(1024, sizeof(WCHAR));
		WCHAR *tmpline = (WCHAR *)calloc(1024, sizeof(WCHAR));
		RECT outrect;

		GetClientRect(hWnd, &rcClient);
	
		int width = rcClient.right - rcClient.left - 10;
		int height = rcClient.bottom - rcClient.top - 10;
		int dx = width / col;
		int dy = height / row;
		if (font == 0) font = dy - 10;
		if (Pdx == 0) Pdx = dx;
		if (Pdy == 0) Pdy = dy;

		HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		HDC winDC = BeginPaint(hWnd, &ps);
		SelectObject(winDC, pen);
		
		MoveToEx(winDC, 10, 10, nullptr);

		HFONT hFont = CreateFont(font, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("Arial"));
		SelectObject(winDC, hFont);		

		int cellInd = 0;		
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				outrect.top = 10 + (i * dy);
				outrect.left = 10 + (j * dx);
				outrect.right = outrect.left + dx - 10;
				outrect.bottom = outrect.top + dy - 10;

				int substr = 0;
				int subStrInd = 0;
				int lineCount = 1;
				GetTextExtentPoint32(winDC, cellsText[cellInd], cellCount, &sz);
				if (sz.cx > dx - 20)
				{
					for (int k = 0; k < cellCount; k++)
					{											
						GetTextExtentPoint32(winDC, tmp, substr, &sz);
						if (sz.cx > dx - 20)
						{
							tmp[subStrInd] = L'\n';
							substr = 0;
							subStrInd++;
							lineCount++;
						}
						tmp[subStrInd] = cellsText[cellInd][k];
						substr++;
						subStrInd++;
					}
					GetTextExtentPoint32(winDC, tmp, substr, &sz);
					if (sz.cy * lineCount > dy)
					{
						GetTextExtentPoint32(winDC, cellsText[cellInd], cellCount, &sz);
						while (sz.cx > dx - 20)
						{
							if (font - 2 > 0) {
								font -= 2;
							}
							HFONT hFont = CreateFont(font, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("TimesNewRoman"));
							SelectObject(winDC, hFont);
							GetTextExtentPoint32(winDC, cellsText[cellInd], cellCount, &sz);
						}
						DrawText(winDC, cellsText[cellInd], cellCount, &outrect, DT_LEFT);
					}
					else
					{
						DrawText(winDC, tmp, subStrInd, &outrect, DT_LEFT);
					}
				}
				else
				{
					GetTextExtentPoint32(winDC, cellsText[cellInd], cellCount, &sz);
					if (Pdx < dx || Pdy < dy)
					{						
						while (sz.cx < dx - 20)
						{
							font += 2;
							HFONT hFont = CreateFont(font, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("Arial"));
							SelectObject(winDC, hFont);
							GetTextExtentPoint32(winDC, cellsText[cellInd], cellCount, &sz);
						}
						DrawText(winDC, cellsText[cellInd], cellCount, &outrect, DT_LEFT);
					}
					else DrawText(winDC, cellsText[cellInd], cellCount, &outrect, DT_LEFT);
				}		
				memset(tmp, 0, 1024);
				cellInd++;
			}
		}

		Pdx = dx;
		Pdy = dy;

		MoveToEx(winDC, 5, 5, nullptr);
		for (int i = 0; i <= row; i++)
		{
			LineTo(winDC, width, 5 + (i * dy));
			MoveToEx(winDC, 5, 5 + (i * dy) + dy, nullptr);
		}
		MoveToEx(winDC, 5, 5, nullptr);
		for (int i = 0; i <= col; i++)
		{
			LineTo(winDC, 5 + (i * dx), height);
			MoveToEx(winDC, 5 + (i * dx) + dx, 5, nullptr);
		}		
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
