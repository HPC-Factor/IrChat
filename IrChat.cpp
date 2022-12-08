#include "IrChat.h"

HWND hMainWnd;
HINSTANCE hInstance;
TCHAR *szTitle = TEXT("IrChat 1.0");

BYTE chKeyStroke;
HFONT hScreenFont;
int txtheight, txtwidth;
int rxrow, rxcol, txrow, txcol;
int maxrow, maxcol;
int nDispWidth, nDispHeight;
TCHAR *screen;

static HWND hCmdBar;
static int rxmaxrow;

void ProcessRX(BYTE c)
{
  HDC hdc = GetDC(hMainWnd);
  HFONT holdfont = (HFONT)SelectObject(hdc,hScreenFont);

  switch( c )
  {
    case 0x08:      //BS
      if( rxcol > 1 )
        if(rxcol>0)
          OutCh( hdc, rxrow, --rxcol, (TCHAR)' ' );
      break;

    case 0x0D:
      rxcol=0;
      rxrow++;
      break;

    default:
      OutCh(hdc,rxrow,rxcol++,c);
  }

  if( rxcol >= maxcol )
  {
    rxcol = 0;
    rxrow++;
  }

  if( rxrow >= rxmaxrow )
  {
    ScrollScreenUp( hdc, 0, rxmaxrow );
    rxrow--;
    rxcol=0;
  }

  SelectObject(hdc,holdfont);
  ReleaseDC(hMainWnd,hdc);
}

void ProcessTX(BYTE c)
{
  HDC hdc = GetDC(hMainWnd);
  HFONT holdfont = (HFONT)SelectObject(hdc,hScreenFont);
  WriteComm( c );

  switch( c )
  {
    case 0x08:      //BS
      if( txcol > 0 )
        OutCh( hdc, txrow+rxmaxrow, --txcol, (TCHAR)' ');
      break;

    case 0x0d:      //enter
      txcol=0;
      txrow++;
      break;

    default:
      OutCh( hdc, txrow+rxmaxrow, txcol++, (TCHAR)c);
  }

  if( txcol >= maxcol )
  {
    txcol=0;
    txrow++;
  }

  if( txrow >= rxmaxrow )
  {
    ScrollScreenUp( hdc, rxmaxrow, maxrow );
    txrow--;
    txcol = 0;
  }

  SelectObject(hdc,holdfont);
  ReleaseDC(hMainWnd,hdc);
}

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  HDC hDC;
  PAINTSTRUCT  ps;
  RECT rc;

  switch(message)
  {
  case WM_PAINT:
    hDC = BeginPaint(hWnd, &ps);
    UpdateTerminalScreen(hDC);
    rc.left = 0;
    rc.right = nDispWidth;
    rc.top = (rxmaxrow*txtheight)+MENUBARHEIGHT+1;
    rc.bottom = rc.top+1;
    FillRect(hDC,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));
    EndPaint(hWnd, &ps);
    break;

  case WM_CHAR:
    ProcessTX((BYTE)wParam);
    break;

  case WM_SIZE:
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_SETFOCUS:
    SetFocus(hMainWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam))
    {
    case ID_CONNECT:
      OpenComm();
      break;

    case ID_DISCONNECT:
      CloseComm();
      break;

    case ID_FILE_EXIT:
      DestroyWindow(hWnd);
      break;
    }
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

static BOOL initialize(int nCmdShow)
{   
  WNDCLASS wc;
  LOGFONT lf;
  TEXTMETRIC tm;
  HDC hdc;
  HFONT hfont;
  RECT rc;

  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = MainWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = 0;
  wc.hCursor = 0; 
  wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = 0;
  wc.lpszClassName = TEXT("NgIrChat");

  if (!RegisterClass(&wc)) 
    return FALSE;
  
  // Create main window and client area size.
  if (!(hMainWnd=CreateWindowEx(0, TEXT("NgIrChat"), szTitle,
    WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 
    0, 0, hInstance, 0)))
    return FALSE;

  lf.lfHeight = 14;
  lf.lfWidth = 0;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;
  lf.lfWeight = 400;
  lf.lfItalic = 0;
  lf.lfUnderline = 0;
  lf.lfStrikeOut = 0;
  lf.lfCharSet = ANSI_CHARSET;
  lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
  lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lf.lfQuality = DEFAULT_QUALITY;
  lf.lfPitchAndFamily = FIXED_PITCH;
  lf.lfFaceName[0] = 0;
  hScreenFont = CreateFontIndirect(&lf);
  hdc = GetDC(0);
  hfont = (HFONT)SelectObject(hdc,hScreenFont);
  GetTextMetrics(hdc,&tm);
  SelectObject(hdc,hfont);
  GetClientRect(hMainWnd,&rc);
  nDispWidth = rc.right;
  nDispHeight = rc.bottom;
  txtheight = tm.tmHeight;
  txtwidth = tm.tmMaxCharWidth;
  rxrow = txrow = txcol = rxcol = 0;
  maxrow = (nDispHeight-MENUBARHEIGHT) / txtheight;
  maxcol = nDispWidth / txtwidth;
  rxmaxrow = maxrow / 2;
  screen = (TCHAR*)malloc(sizeof(TCHAR)*maxrow*maxcol);
  ClearTerminalScreen(NULL);

  // Create command bar.
  hCmdBar = CommandBar_Create(hInstance, hMainWnd, 1);
  CommandBar_InsertMenubar(hCmdBar, hInstance, ID_MAIN_MENU, 0);
  CommandBar_AddAdornments(hCmdBar, 0, ID_FILE_EXIT);

  // Display windows and return.
  ShowWindow(hMainWnd, nCmdShow);
  UpdateWindow(hMainWnd);
  SetFocus(hMainWnd);

  return TRUE;
}

//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPWSTR pszCmdLine, int nCmdShow)
{
  MSG msg;

  hInstance = hInst;

  if(!initialize(nCmdShow))
  {
    MessageBox(0, TEXT("Failed to initialize."), szTitle, MB_OK);
    return FALSE;
  }

  while(GetMessage(&msg, 0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if(hScreenFont)
    DeleteObject((HGDIOBJ)hScreenFont);

  if(screen)
    free(screen);

  CloseComm();
  return TRUE;
}
