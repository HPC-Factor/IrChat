#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include "resource.h"

#define MENUBARHEIGHT 24

extern HWND hMainWnd;
extern HINSTANCE hInstance;
extern TCHAR *szTitle;

extern BYTE chKeyStroke;
extern HFONT hScreenFont;
extern int txtheight, txtwidth;
extern int rxrow, rxcol, txrow, txcol;
extern int maxrow, maxcol;
extern int nDispWidth, nDispHeight;
extern TCHAR *screen;

void UpdateTerminalScreen(HDC);
void UpdateTerminalLine(HDC hdc, int y);
void ClearTerminalScreen(HDC hdc);
void ScrollScreenUp(HDC hdc, int row1, int row2);
void OutCh(HDC hdc, int row, int col, TCHAR ch);

BOOL OpenComm(void);
void CloseComm(void);
void WriteComm(BYTE ch);

void ProcessRX(BYTE c);
void ProcessTX(BYTE c);
