#include "IrChat.h"

void UpdateTerminalLine(HDC hdc, int y)
{
  int n = (y<(maxrow/2)) ? 0 : 2;
  n = (y*txtheight)+MENUBARHEIGHT+n;
  ExtTextOut(hdc,0,n,0,NULL,screen+y*maxcol,maxcol,NULL);
}

void UpdateTerminalScreen(HDC hdc)
{
  int r;

  for(r=0; r<maxrow; r++)
    UpdateTerminalLine(hdc,r);
}

void ClearTerminalScreen(HDC hdc)
{
  int r, c;

  for(r=0; r<maxrow; r++)
    for(c=0; c<maxcol; c++)
      screen[r*maxcol+c] = ' ';

  if(hdc)
    UpdateTerminalScreen(hdc);
}

void ScrollScreenUp(HDC hdc, int row1, int row2)
{
  int r;

  row2--;

  for(r=row1; r<row2; r++)
  {
    memcpy(&screen[r*maxcol],&screen[(r+1)*maxcol],maxcol*sizeof(TCHAR));
    UpdateTerminalLine(hdc,r);
  }

  for(r=0; r<maxcol; r++)
    screen[row2*maxcol+r] = (TCHAR)' ';

  UpdateTerminalLine(hdc,row2);
}

void OutCh(HDC hdc, int row, int col, TCHAR ch)
{
  screen[row*maxcol+col] = ch;
  UpdateTerminalLine(hdc,row);
}
