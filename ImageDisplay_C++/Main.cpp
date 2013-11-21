//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Used to read in a standard RGB image and display it
// There are two copies that are displayed
// Left one is orginal, right one is after students perform operations on it
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************
#define STRICT
typedef void *PVOID;
typedef void * POINTER_64;// PVOID64;



// Include class files
#include "Image.h"
#include <Windows.h>
#include "basetsd.h"
#include <commdlg.h>
#include <mmreg.h>
#include "dxsdk_include/dxerr8.h"
#include "dxsdk_include/dsound.h"
#include "resource.h"
#include "CS576SoundUtil.h"
#include "DXUtil.h"

#define MAX_LOADSTRING 100

// UI TEST
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 30
#define IDC_QUERY_PLAY_BUTTON	1001
#define IDC_QUERY_PAUSE_BUTTON	1002
#define IDC_QUERY_STOP_BUTTON	1003
#define IDC_MATCH_PLAY_BUTTON	1004
#define IDC_MATCH_PAUSE_BUTTON	1005
#define IDC_MATCH_STOP_BUTTON	1006
#define ID_QUERY_TIMER 2001
#define ID_MATCH_TIMER 2002
#define ID_QUERY_AUDIO_TIMER 2003

// Global Variables:
MyImage			inImage, outImage;				// image objects
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
CSoundManager* g_pSoundManager = NULL;
CSound*        g_pSound = NULL;
BOOL           g_bBufferPaused;
TCHAR		   AudioPath[_MAX_PATH] = TEXT("");


// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);



// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Read in the image and its copy
	char ImagePath[_MAX_PATH];

	sscanf(lpCmdLine, "%s %s", &ImagePath, &AudioPath);
	inImage.setWidth(352);
	inImage.setHeight(288);
	if ( strstr(ImagePath, ".rgb" )==NULL )
	{ 
		AfxMessageBox( "Image has to be a '.rgb' file\nUsage - Image.exe image.rgb w h");
		//return FALSE;
	}
	else
	{
		inImage.setImagePath(ImagePath);
		if ( !inImage.ReadImage() )
		{ 
			AfxMessageBox( "Could not read image\nUsage - Image.exe image.rgb w h");
			//return FALSE;
		}
		else
			outImage = inImage;
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HRESULT OnPlaySound( HWND hDlg ) 
{
    HRESULT hr;

    BOOL bLooped = true;

    if( g_bBufferPaused )
    {
        // Play the buffer since it is currently paused
        DWORD dwFlags = bLooped ? DSBPLAY_LOOPING : 0L;
        if( FAILED( hr = g_pSound->Play( 0, dwFlags ) ) )
            return DXTRACE_ERR( TEXT("Play"), hr );

        // Update the UI controls to show the sound as playing
        g_bBufferPaused = FALSE;
        //EnablePlayUI( hDlg, FALSE );
    }
    else
    {
        if( g_pSound->IsSoundPlaying() )
        {
            // To pause, just stop the buffer, but don't reset the position
            if( g_pSound )
                g_pSound->Stop();

            g_bBufferPaused = TRUE;
            //SetDlgItemText( hDlg, IDC_PLAY, "Play" );
        }
        else
        {
            // The buffer is not playing, so play it again
            DWORD dwFlags = bLooped ? DSBPLAY_LOOPING : 0L;
            if( FAILED( hr = g_pSound->Play( 0, dwFlags ) ) )
                return DXTRACE_ERR( TEXT("Play"), hr );

            // Update the UI controls to show the sound as playing
            g_bBufferPaused = FALSE;
            //EnablePlayUI( hDlg, FALSE );
        }
    }

    return S_OK;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	HRESULT hr;

	switch (message) 
	{
		case WM_CREATE:
		{
			// Create a static IDirectSound in the CSound class.  
			// Set coop level to DSSCL_PRIORITY, and set primary buffer 
			// format to stereo, 22kHz and 16-bit output.
			g_pSoundManager = new CSoundManager();

			if( FAILED( hr = g_pSoundManager->Initialize( hWnd, DSSCL_PRIORITY, 1, 44100, 16 ) ) )	//  2, 22050, 16
			{
				DXTRACE_ERR( TEXT("Initialize"), hr );
				MessageBox( hWnd, "Error initializing DirectSound.  Sample will now exit.", 
									"DirectSound Sample", MB_OK | MB_ICONERROR );
				EndDialog( hWnd, IDABORT );
				return false;
			}

			g_bBufferPaused = FALSE;

			// Create a timer, so we can check for when the soundbuffer is stopped
			SetTimer( hWnd, ID_QUERY_AUDIO_TIMER, 250, NULL );

			if( g_pSound )
			{
				g_pSound->Stop();
				g_pSound->Reset();
			}

			SAFE_DELETE( g_pSound );

			// Load the wave file into a DirectSound buffer
			if( FAILED( hr = g_pSoundManager->Create( &g_pSound, AudioPath, 0, GUID_NULL ) ) )
			{
				// Not a critical failure, so just update the status
				DXTRACE_ERR_NOMSGBOX( TEXT("Create"), hr );
				//SetDlgItemText( hWnd, IDC_FILENAME, TEXT("Could not create sound buffer.") );
				return false; 
			}

			

			

			// Create the QUERY PLAY button
			HWND hWndQueryPlayButton=CreateWindowEx(NULL,
				"BUTTON",
				"PLAY",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				25,
				190 + inImage.getHeight(),
				BUTTON_WIDTH,
				BUTTON_HEIGHT,
				hWnd,
				(HMENU)IDC_QUERY_PLAY_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			
			// Create the QUERY PAUSE button
			HWND hWndQueryPauseButton=CreateWindowEx(NULL,
				"BUTTON",
				"PAUSE",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				25 + BUTTON_WIDTH + 26,
				190 + inImage.getHeight(),
				BUTTON_WIDTH,
				BUTTON_HEIGHT,
				hWnd,
				(HMENU)IDC_QUERY_PAUSE_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			// Create the QUERY STOP button
			HWND hWndQueryStopButton=CreateWindowEx(NULL,
				"BUTTON",
				"STOP",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				25 + (BUTTON_WIDTH + 26) * 2,
				190 + inImage.getHeight(),
				BUTTON_WIDTH,
				BUTTON_HEIGHT,
				hWnd,
				(HMENU)IDC_QUERY_STOP_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			// Create the MATCH PLAY button
			HWND hWndMatchPlayButton=CreateWindowEx(NULL,
				"BUTTON",
				"PLAY",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				outImage.getWidth()+55,
				190 + outImage.getHeight(),
				BUTTON_WIDTH,
				BUTTON_HEIGHT,
				hWnd,
				(HMENU)IDC_MATCH_PLAY_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			
			// Create the MATCH PAUSE button
			HWND hWndMatchPauseButton=CreateWindowEx(NULL,
				"BUTTON",
				"PAUSE",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				outImage.getWidth()+55 + BUTTON_WIDTH + 26,
				190 + outImage.getHeight(),
				BUTTON_WIDTH,
				BUTTON_HEIGHT,
				hWnd,
				(HMENU)IDC_MATCH_PAUSE_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			// Create the MATCH STOP button
			HWND hWndMatchStopButton=CreateWindowEx(NULL,
				"BUTTON",
				"STOP",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				outImage.getWidth()+55 + (BUTTON_WIDTH + 26) * 2,
				190 + outImage.getHeight(),
				BUTTON_WIDTH,
				BUTTON_HEIGHT,
				hWnd,
				(HMENU)IDC_MATCH_STOP_BUTTON,
				GetModuleHandle(NULL),
				NULL);
		}
		break;
		case WM_TIMER:
			switch(wParam) {
				case ID_QUERY_TIMER:
					inImage.Modify();
					break;
				case ID_MATCH_TIMER:
					outImage.Modify();
					break;
			}
			InvalidateRect(hWnd, &rt, false);
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
					break;
				case IDC_QUERY_PLAY_BUTTON:
					SetTimer(hWnd, ID_QUERY_TIMER, 32.5, NULL );
					// The 'play'/'pause' button was pressed
                    if( FAILED( hr = OnPlaySound( hWnd ) ) )
                    {
                        DXTRACE_ERR( TEXT("OnPlaySound"), hr );
                        MessageBox( hWnd, "Error playing DirectSound buffer. "
                                    "Sample will now exit.", "DirectSound Sample", 
                                    MB_OK | MB_ICONERROR );
                        EndDialog( hWnd, IDABORT );
                    }
					break;
				case IDC_QUERY_PAUSE_BUTTON:
					if( g_pSound )
                    {
                        g_pSound->Stop();
                        //g_pSound->Reset();
                    }
					KillTimer (hWnd, ID_QUERY_TIMER);
					break;
				case IDC_QUERY_STOP_BUTTON:
					KillTimer (hWnd, ID_QUERY_TIMER);
					inImage.setCurrentFrame(0);
					inImage.Modify();
					InvalidateRect(hWnd, &rt, false);
					break;
				case IDC_MATCH_PLAY_BUTTON:
					SetTimer(hWnd, ID_MATCH_TIMER, 40, NULL );
					break;
				case IDC_MATCH_PAUSE_BUTTON:
					KillTimer(hWnd, ID_MATCH_TIMER);
					break;
				case IDC_MATCH_STOP_BUTTON:
					KillTimer (hWnd, ID_MATCH_TIMER);
					outImage.setCurrentFrame(0);
					outImage.Modify();
					InvalidateRect(hWnd, &rt, false);
					break;
				case ID_MODIFY_IMAGE:
					outImage.Modify();
					InvalidateRect(hWnd, &rt, false);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				char text[1000];
				strcpy(text, "Original image (Left)  Image after modification (Right)\n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				strcpy(text, "\nUpdate program with your code to modify input image");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = inImage.getWidth();
				bmi.bmiHeader.biHeight = -inImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = inImage.getWidth()*inImage.getHeight();

				SetDIBitsToDevice(hdc,
								  25,175,inImage.getWidth(),inImage.getHeight(),
								  0,0,0,inImage.getHeight(),
								  inImage.getImageData(),&bmi,DIB_RGB_COLORS);

				SetDIBitsToDevice(hdc,
								  outImage.getWidth()+55,175,outImage.getWidth(),outImage.getHeight(),
								  0,0,0,outImage.getHeight(),
								  outImage.getImageData(),&bmi,DIB_RGB_COLORS);


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




// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


