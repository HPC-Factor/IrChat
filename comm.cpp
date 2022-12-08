#include "IrChat.h"

#define MAXRXBUFFER 4096

static HANDLE hPort = INVALID_HANDLE_VALUE;
static int bStopThread, bCommEnabled;
static TCHAR szPort[6], sztmp[64];

static void FindIrCommPort(void)
{
  DWORD dwSize, dwData, dwType;
  HKEY hKey;

  *szPort = 0;

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("Drivers\\BuiltIn\\IrCOMM"),0,0,&hKey) == ERROR_SUCCESS)
  {
    dwSize = sizeof(dwData);

    if(RegQueryValueEx(hKey,TEXT("Index"),0,&dwType,(BYTE*)&dwData,&dwSize) == ERROR_SUCCESS)
      if(dwData < 10)
        wsprintf(szPort,TEXT("COM%d:"),dwData);

    RegCloseKey(hKey);
    wsprintf(sztmp,TEXT("Found IrComm port at [%s]"),szPort);
    MessageBox(hMainWnd,sztmp,szTitle,MB_OK);
    return;
  }

  lstrcpy(szPort,TEXT("COM3:"));
  MessageBox(hMainWnd,TEXT("IrComm port is not found."),szTitle,MB_OK);
}

static DWORD WINAPI ReadThread(LPVOID pvarg)
{
  BYTE byte;
  DWORD num;
  DWORD fdwCommMask;

  while(!bStopThread)
  {
    WaitCommEvent(hPort, &fdwCommMask, 0);

    if(fdwCommMask & EV_RXCHAR)
    {
      do
      {
        ReadFile(hPort, &byte, 1, &num, 0);

        if(num == 1)
          ProcessRX(byte);

      } while (num == 1);
    }
  }
  
  ExitThread(0);
  return 0;
}

void WriteComm(BYTE ch)
{
  DWORD n;

  WriteFile(hPort,&ch,1,&n,0);
}

BOOL OpenComm(void)
{
  DWORD ThreadID;
  DCB PortDCB;
  COMMTIMEOUTS CommTimeouts;
  DWORD fdwCommMask;
  BYTE chData;

  if(bCommEnabled)
    return 0;

  FindIrCommPort();
  hPort = CreateFile(szPort,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);

  if(hPort == INVALID_HANDLE_VALUE) 
  {
    MessageBox(hMainWnd,TEXT("Can not open IrComm port"),szTitle,MB_OK);
    return -1;
  }

  // Set the port.
  PortDCB.DCBlength = sizeof(DCB);
  GetCommState(hPort, &PortDCB);
  PortDCB.BaudRate = CBR_115200;
  PortDCB.fBinary = TRUE;
  PortDCB.fParity = TRUE;
  PortDCB.fOutxCtsFlow = FALSE;    // ignore possible hangups
  PortDCB.fOutxDsrFlow = FALSE;    // don't wait on the DSR line
  PortDCB.fDtrControl = DTR_CONTROL_DISABLE;
  PortDCB.fDsrSensitivity = FALSE;
  PortDCB.fTXContinueOnXoff = FALSE;
  PortDCB.fOutX = FALSE;           // no XON/XOFF control 
  PortDCB.fInX = FALSE;
  PortDCB.fErrorChar = FALSE;
  PortDCB.fNull = FALSE;
  PortDCB.fRtsControl = RTS_CONTROL_DISABLE;
  PortDCB.fAbortOnError = FALSE;
  PortDCB.ByteSize = 8; 
  PortDCB.Parity = NOPARITY;
  PortDCB.StopBits = ONESTOPBIT; 
  SetCommState(hPort, &PortDCB); 

  // Set comm timeouts (nonblocking read, and write waiting for completion).
  GetCommTimeouts(hPort, &CommTimeouts);
  CommTimeouts.ReadIntervalTimeout = 0;
  CommTimeouts.ReadTotalTimeoutMultiplier = 0;
  CommTimeouts.ReadTotalTimeoutConstant = 0;  
  CommTimeouts.WriteTotalTimeoutMultiplier = 0;
  CommTimeouts.WriteTotalTimeoutConstant = 0;  
  SetCommTimeouts(hPort, &CommTimeouts);
  EscapeCommFunction(hPort, SETIR);

  // set RX even mask
  SetCommMask(hPort, EV_RXCHAR);

  MessageBox(hMainWnd,TEXT("Ready?"),TEXT("Hand-snaking"),MB_OK);
  WriteFile(hPort,"X",1,&fdwCommMask,0);
  WaitCommEvent(hPort, &fdwCommMask, 0);

  if(fdwCommMask & EV_RXCHAR)
  {
    ReadFile(hPort, &chData, 1, &fdwCommMask, 0);
    WriteFile(hPort,"X",1,&fdwCommMask,0);
    bStopThread = FALSE;
    CloseHandle(CreateThread(0,0,ReadThread,0,0,&ThreadID));
    bCommEnabled = TRUE;

    return 0;
  }

  return -2;
}

void CloseComm(void)
{
  if(bCommEnabled)
  {
    bStopThread = TRUE;
    CloseHandle(hPort);
    Sleep(1000);
    bCommEnabled = FALSE;
  }
}
