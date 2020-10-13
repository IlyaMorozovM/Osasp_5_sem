// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <string>
#include <vector>
#include <atlstr.h>
#include <fstream>

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("r:1;c:1");

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int rowsCount = 1;
int colsCount = 1;
HDC hdc;

LPMINMAXINFO lpMMI;
RECT clRect;

int lfHeight;
HFONT hf;

std::vector<std::string> lines;

std::string tempstring;

std::vector<std::string>readLines(std::string filename) {

    std::ifstream hFile(filename);

    std::vector<std::string> lines;
    if (hFile.good()) {
        while (std::getline(hFile, tempstring))
            lines.push_back(tempstring);
    }

    else {
        lines.push_back("file not found");
    }

    return lines;
}

void drawGrid(int c, int r, HDC hdc, RECT clRect) {
    HFONT hfOld = (HFONT)SelectObject(hdc, hf);
    int w = (clRect.right - clRect.left);
    int h = (clRect.bottom - clRect.top);
    int stepC = w / c;  //|||
    int stepR = h / r;   //===
    TCHAR buf[2048];
    RECT rect;
    int aj = 0;
    for (int j = 0; j < r; j++) {
        rect.top = aj;
        rect.bottom = rect.top + stepR;
        int maxheight = stepR;
        for (int i = 0; i < c; i++) {
            rect.left = i * stepC;
            rect.right = rect.left + stepC;

            tempstring = lines.at((c * j + i) % lines.size());
            _tcscpy_s(buf, CA2T(tempstring.c_str()));
            rect.bottom = rect.top + maxheight;
            int temp = DrawTextEx(hdc, (LPWSTR)buf, _tcslen(buf), &rect, DT_CALCRECT | DT_WORDBREAK, 0);
            if (temp > maxheight)maxheight = temp;
            DrawTextEx(hdc, (LPWSTR)buf, _tcslen(buf), &rect, DT_WORDBREAK, 0);
        }
        aj += maxheight;
        MoveToEx(hdc, 0, aj, NULL);
        LineTo(hdc, clRect.right, aj);
        aj++;
        if (aj > clRect.bottom) { break; }
    }

    for (int i = 0; i <= w; i += stepC) {
        MoveToEx(hdc, i, 0, NULL);
        LineTo(hdc, i, clRect.bottom);
    }
    SelectObject(hdc, hfOld);
}

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    hInst = hInstance;

    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, Windows desktop!");

    switch (message)
    {
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_LEFT:
            if (colsCount > 1)
                colsCount -= 1;
            break;
        case VK_RIGHT:
            colsCount += 1;
            break;
        case VK_UP:
            if (rowsCount > 1)
                rowsCount -= 1;
            break;
        case VK_DOWN:
            rowsCount += 1;
            break;
        }
        TCHAR buf[300];
        _stprintf_s(buf, TEXT("r:%d;c:%d"), rowsCount, colsCount);
        SetWindowTextW(
            hWnd,
            buf
        );
        InvalidateRect(hWnd, 0, true);
        //UpdateWindow(hWnd);
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        //RECT clRect;
        GetClientRect(hWnd, &clRect);
        drawGrid(colsCount, rowsCount, hdc, clRect);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        DeleteObject(hf);
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        GetClientRect(hWnd, &clRect);
        hdc = GetDC(hWnd);

        lines = readLines("text.txt");

        lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, PROOF_QUALITY, 0, _T("Candara"));
        ReleaseDC(hWnd, hdc);
        break;
    case WM_GETMINMAXINFO:
        lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 300;
        lpMMI->ptMinTrackSize.y = 300;
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}