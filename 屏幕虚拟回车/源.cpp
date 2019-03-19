/*------------------------------------------------------------
   HELLOWIN.C -- Displays "Hello, Windows 98!" in client area
                 (c) Charles Petzold, 1998
  ------------------------------------------------------------*/

#include <windows.h>
#include <gdiplus.h> 
#include <stdio.h>
#include "resource.h"
  #pragma   data_seg("ifSingle")//自己定义了一个共享数据段   
      long   programCount=0;   
  #pragma   data_seg()   
  #pragma   comment(linker,"/section:ifSingle,rws")

int screenX=0 , screenY=0;
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
bool sendKey( int key );
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
HINSTANCE hinstance;
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{

	InterlockedIncrement(&programCount);

	if (programCount>1)
	{
          MessageBox (NULL, TEXT ("请不要多次运行本程序!"), 
                      TEXT ("Touch Enter"), MB_OK) ;
		  return 0;
	}


		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR            gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
     static TCHAR szAppName[] = TEXT ("Touch Enter") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;
	 hinstance = hInstance;
     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, MAKEINTRESOURCE(IDI_ICON3) );
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
				screenX = GetSystemMetrics ( SM_CXSCREEN );//获取屏幕分辨率
				screenY = GetSystemMetrics ( SM_CYSCREEN ); 



     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
     hwnd = CreateWindowEx (WS_EX_LAYERED|WS_EX_NOACTIVATE|WS_EX_TOPMOST,
						  szAppName,                  // window class name
                          TEXT ("Touch Enter"), // window caption
                          WS_POPUP,        // window style
						  0,
						  0,
						  800,
						  800,
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL) ;                     // creation parameters
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
	 GdiplusShutdown(gdiplusToken);
     return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     HDC         hdc ;
	 static HDC hdcMem , hdcMem2;
     PAINTSTRUCT ps ;
     RECT        rect ;
	 NOTIFYICONDATA ni;
	 Image * image;
	 Graphics * graphics;

     static HBITMAP hbitmap , hbitmap2;

	 wchar_t tempStr[20];

	 static POINT point={300 , 300 } , point2={ 0 , 0 };
	 static SIZE size={ 1024,576 };
	 static BLENDFUNCTION bf ={ AC_SRC_OVER , 0 , 255 , AC_SRC_ALPHA  };

	 BITMAPINFOHEADER bmih;
	 bmih.biSize = 40;
	 bmih.biWidth = 1024;
	 bmih.biHeight = 576;
	 bmih.biPlanes =1;
	 bmih.biBitCount =32;
	 bmih.biCompression =0;
	 bmih.biSizeImage =0;
	 bmih.biXPelsPerMeter=0;
	 bmih.biYPelsPerMeter=0;
	 bmih.biClrUsed =0;
	 bmih.biClrImportant =0;
	 static bool isLButtonDown=false;
	 static bool isMove = false;
	 HICON hicon;
	 static HMENU hMenu;
	 POINT tmpPoint;
	 INPUT input[1];
	 static int alpha=255;
	POINT tempPoint;
			HRSRC hrsrc;
     switch (message)
     {
	 case WM_TIMER:
		if ( wParam == 1)
		{

		 SetWindowPos( hwnd, HWND_TOPMOST , 0 , 0 ,800,600 ,SWP_NOMOVE|SWP_NOSIZE);	


		}
		return 0;

	 case WM_DISPLAYCHANGE:
				screenX = GetSystemMetrics ( SM_CXSCREEN );//获取屏幕分辨率
				screenY = GetSystemMetrics ( SM_CYSCREEN ); 
		SetWindowPos( hwnd, HWND_TOPMOST , screenX-100 , screenY/2 ,800,600 ,SWP_NOSIZE);	

	
		return 0;

	 case WM_USER +10:
		  if (lParam == WM_RBUTTONDOWN ||  lParam == WM_LBUTTONDOWN )
			{
				GetCursorPos(&tempPoint);
				SetForegroundWindow(hwnd); // 修正当用户按下ESCAPE 键或者在菜单之外单击鼠标时菜单不会消失的情况
				TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, tempPoint.x, tempPoint.y, 0, hwnd, NULL);
			}

		 return 0;
     case WM_CREATE:
				screenX = GetSystemMetrics ( SM_CXSCREEN );//获取屏幕分辨率
				screenY = GetSystemMetrics ( SM_CYSCREEN ); 
		 hMenu = LoadMenu(hinstance, MAKEINTRESOURCE(IDR_MENU1));
			hMenu = GetSubMenu(hMenu, 0);

			point.x= GetPrivateProfileInt(TEXT("保存退出时坐标信息"),TEXT("x"),screenX-100,TEXT(".\\setting.ini"));
			point.y= GetPrivateProfileInt(TEXT("保存退出时坐标信息"),TEXT("y"),screenY/2,TEXT(".\\setting.ini"));
			alpha =  GetPrivateProfileInt(TEXT("绘图信息：透明度"),TEXT("Alpha"),255,TEXT(".\\setting.ini"));
			bf.SourceConstantAlpha=alpha;

			if ( point.x > screenX)
			{
				point.x=screenX-100;

			}
			if ( point.y > screenY)
			{
				point.y=screenY-100;

			}
			ni.cbSize = sizeof(ni);
			ni.hWnd = hwnd;
			ni.uID = 123;
			ni.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			ni.uCallbackMessage = WM_USER+10;
			ni.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON3));
			
			Shell_NotifyIcon(NIM_ADD, &ni);

		 SetTimer( hwnd ,1 , 1000 , NULL);
		 hdc = GetDC (hwnd) ;
		 hdcMem = CreateCompatibleDC( hdc );
		// hdcMem2 = CreateCompatibleDC( hdc );
		// hbitmap2 = (HBITMAP)LoadImage( hinstance , TEXT("0000.bmp") ,IMAGE_BITMAP , 0 , 0 , LR_LOADFROMFILE); 
		 hbitmap = CreateDIBitmap( hdc , &bmih , 0 , NULL ,NULL , 0 );
		 hrsrc = FindResource(hinstance , MAKEINTRESOURCE(IDB_PNG1) , RT_BITMAP );


		 SelectObject( hdcMem , hbitmap );
		// SelectObject( hdcMem2 , hbitmap2 );
		 Ellipse( hdc , 0 , 0 , 48 ,48 );
		 graphics = new Graphics (  hdcMem );
		 image = new Image( TEXT("picture.png"));
		 graphics->DrawImage( image , 0 , 0 ,80,80);		 

		 SetWindowPos( hwnd, HWND_TOPMOST , 0 , 0 ,800,600 ,SWP_NOMOVE|SWP_NOSIZE);	
		 UpdateLayeredWindow( hwnd, NULL , &point  , &size , hdcMem , &point2 , NULL , &bf , ULW_ALPHA);
		 SetWindowPos( hwnd, HWND_TOPMOST , 0 , 0 ,800,600 ,SWP_NOMOVE|SWP_NOSIZE);	//将黄油的窗口置顶	 







		 delete graphics,image;
		 DeleteDC( hdc );
          return 0 ;


	 case WM_LBUTTONDOWN:
		PostMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0); 
		sendKey( VK_RETURN );
		return 0;  

	 case WM_RBUTTONUP:
		// GetWindowRect( hwnd , &rect );
		 GetCursorPos( &tmpPoint);


		 tmpPoint.x=(screenX/2-tmpPoint.x);
		 tmpPoint.y=(screenY/2-tmpPoint.y);


		 input[0].type=INPUT_MOUSE;
		 input[0].mi.dx = 65535/screenX*(screenX/2);
		 input[0].mi.dy = 65535/screenY*(screenY/2);

		 input[0].mi.mouseData = 0;
		 input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE;
		 input[0].mi.time=NULL;
		 input[0].mi.dwExtraInfo=GetMessageExtraInfo();



		 SendInput( 1 , input , sizeof(INPUT));

		 Sleep(10);
		 input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN;
		 SendInput( 1 , input , sizeof(INPUT));
		 Sleep(10);
		 input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP;
		 SendInput( 1 , input , sizeof(INPUT));
		 Sleep(10);
		/* mouse_event( MOUSEEVENTF_MOVE , tmpPoint.x   , tmpPoint.y , 0 , GetMessageExtraInfo());
		 Sleep(20);
		 mouse_event( MOUSEEVENTF_RIGHTDOWN , tmpPoint.x   , tmpPoint.y , 0 , GetMessageExtraInfo());
		 Sleep(20);
		 mouse_event( MOUSEEVENTF_RIGHTUP , tmpPoint.x   , tmpPoint.y , 0 , GetMessageExtraInfo());
		 Sleep(20);*/




		 return 0;
	 case WM_LBUTTONUP:  


		

		 return 0;
     case WM_DESTROY:
		ni.uID = 123;  // 修正点击Exit 菜单退出后图标仍在托盘区显示，
			ni.hWnd = hwnd;        //  要把鼠标在图标上面过一下才会消失。
		Shell_NotifyIcon(NIM_DELETE, &ni);


		  GetWindowRect( hwnd , &rect );
		  swprintf_s( tempStr , TEXT("%d") , rect.left );
                                                                                                                                                                                                                
		  WritePrivateProfileString(TEXT("保存退出时坐标信息"),TEXT("x"),tempStr,TEXT(".\\setting.ini"));
		  swprintf_s( tempStr , TEXT("%d") , rect.top );
		  WritePrivateProfileString(TEXT("保存退出时坐标信息"),TEXT("y"),tempStr,TEXT(".\\setting.ini"));
		  swprintf_s( tempStr , TEXT("%d") , alpha );
		  WritePrivateProfileString(TEXT("绘图信息：透明度"),TEXT("Alpha"),tempStr,TEXT(".\\setting.ini"));
		  PostQuitMessage (0) ;
          return 0 ;



	case WM_COMMAND:

		if (LOWORD(wParam)==ID_40001)  SendMessage( hwnd , WM_DESTROY , NULL, NULL);
		if (LOWORD(wParam)==ID_40004)  ShellExecute (NULL , TEXT("open") , TEXT(".\\setting.ini") ,NULL , NULL , SW_SHOWNORMAL);
		return 0;



     }

     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

bool sendKey( int key )
{

	keybd_event( key , MapVirtualKey(0x0D, MAPVK_VK_TO_VSC) , NULL , GetMessageExtraInfo());
	Sleep(20);
	keybd_event( key , MapVirtualKey(0x0D, MAPVK_VK_TO_VSC) , KEYEVENTF_KEYUP , GetMessageExtraInfo());
	Sleep(20);
	return true;
}