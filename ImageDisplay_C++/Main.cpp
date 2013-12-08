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
#include <string>
#include <vector>
#include <tuple>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <atlwin.h>

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
#define IDC_MATCH_SLIDER		1007
#define IDC_MATCH_LIST			1008
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
HANDLE queryTimer = NULL;
int queryTimerArg = 123;
BOOL	queryPlaying = false;

MyImage loadedVideos[3];

enum PlaybackActions { PLAY, PAUSE, STOP };
PlaybackActions playbackAction;
MyImage* videos[2];
BOOL videoPlaying[2] = {false, false};
HANDLE videoTimer[2] = {NULL, NULL};
HWND hWnd;
int videoIndex[2] = {0, 1};
HWND hwndTrack;
int scrubberPos = 0;
HWND hwndMatchList;
// Stores <name, percentage match> pair for each match video
std::vector<std::tuple<std::string, double, int> > matchDBList;
std::string currentSelectedMatchVideo;

struct sort_pred {
    bool operator()(const std::tuple<std::string,double, int> &left, const std::tuple<std::string,double, int> &right) {
        return std::get<1>(left) > std::get<1>(right);
    }
};

struct sort_avg_hue {
    bool operator()(const std::pair<int,double> &left, const std::pair<int,double> &right) {
        return left.second < right.second;
    }
};

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
VOID CALLBACK TimerRoutine(PVOID, BOOLEAN);
VOID PlaybackControl(int, PlaybackActions);
VOID QueryCompare();


// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	matchDBList.push_back(std::make_tuple("soccer1", 0, 0));
	matchDBList.push_back(std::make_tuple("soccer2", 0, 0));
	matchDBList.push_back(std::make_tuple("soccer3", 0, 0));
	matchDBList.push_back(std::make_tuple("soccer4", 0, 0));
	matchDBList.push_back(std::make_tuple("talk1", 0, 0));
	matchDBList.push_back(std::make_tuple("talk2", 0, 0));
	matchDBList.push_back(std::make_tuple("talk3", 0, 0));
	matchDBList.push_back(std::make_tuple("talk4", 0, 0));
	matchDBList.push_back(std::make_tuple("wreck1", 0, 0));
	matchDBList.push_back(std::make_tuple("wreck2", 0, 0));
	matchDBList.push_back(std::make_tuple("wreck3", 0, 0));
	matchDBList.push_back(std::make_tuple("wreck4", 0, 0));

	videos[0] = &inImage;
	videos[1] = &outImage;
	
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
		{
			outImage = inImage;
			QueryCompare();
		}
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
   //HWND hWnd;

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

			hwndMatchList = CreateWindowEx(
				0,
				WC_LISTBOX,
				"Match List",
				WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | LBS_HASSTRINGS,
				outImage.getWidth()+55, 10,
				200, 120,
				hWnd,
				(HMENU) IDC_MATCH_LIST,
				GetModuleHandle(NULL),
				NULL
			);
			
			SendMessage(hwndMatchList, LB_ADDSTRING, 0, (LPARAM)"Matched Videos:");

			std::stringstream ss;

			std::string fileName = std::get<0>(matchDBList[0]) + ".rgb";
			ss << std::fixed << std::setw(20) << std::left << fileName << std::setprecision(2) << 100*std::get<1>(matchDBList[0]) << "%";
			SendMessage(hwndMatchList, LB_ADDSTRING, 0, (LPARAM)ss.str().c_str());

			ss.clear();
			ss.str("");
			fileName = std::get<0>(matchDBList[1]) + ".rgb";
			ss << std::setw(20) << std::left << fileName << std::setprecision(2) << 100*std::get<1>(matchDBList[1]) << "%";
			SendMessage(hwndMatchList, LB_ADDSTRING, 0, (LPARAM)ss.str().c_str());

			ss.clear();
			ss.str("");
			fileName = std::get<0>(matchDBList[2]) + ".rgb";
			ss << std::setw(20) << std::left << fileName << std::setprecision(2) << 100*std::get<1>(matchDBList[2]) << "%";
			SendMessage(hwndMatchList, LB_ADDSTRING, 0, (LPARAM)ss.str().c_str());

			hwndTrack = CreateWindowEx( 
				0, // no extended styles 
				TRACKBAR_CLASS, // class name 
				"Trackbar Control", // title (caption) 
				WS_CHILD | WS_VISIBLE | 
				TBS_NOTICKS | TBS_BOTH, // style 
				outImage.getWidth()+55, 150, // position 
				352, 25, // size 
				hWnd, // parent window 
				(HMENU)IDC_MATCH_SLIDER, // control identifier 
				GetModuleHandle(NULL), // instance 
				NULL // no WM_CREATE parameter 
			);
			SendMessage(hwndTrack, TBM_SETRANGE, TRUE, MAKELONG(0, outImage.getNumFrames()-1));

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
		case WM_HSCROLL:
			scrubberPos = SendMessage(hwndTrack, TBM_GETPOS, 0, 0);
			outImage.setCurrentFrame(scrubberPos);
			outImage.Modify();
			InvalidateRect(hWnd, &rt, false);
			break;
		case WM_TIMER:
			/*switch(wParam) {
				case ID_QUERY_TIMER:
					inImage.Modify();
					break;
				case ID_MATCH_TIMER:
					outImage.Modify();
					break;
			}*/
			InvalidateRect(hWnd, &rt, false);
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_MATCH_LIST:
					if (wmEvent == LBN_SELCHANGE) {
						int sel = (int) SendMessage(hwndMatchList, LB_GETCURSEL, 0, 0);

						if (sel != 0) {
							if ((currentSelectedMatchVideo.compare(std::get<0>(matchDBList[sel-1])) != 0)){
								//int test = loadedVideos[0].getNumFrames();
								if(loadedVideos[sel-1].getNumFrames() == -1){
									std::stringstream ss;
									ss << "matchDB\\" << std::get<0>(matchDBList[sel-1]) << ".rgb";
									outImage.setImagePath(ss.str().c_str());
									if ( !outImage.ReadImage() )
									{ 
										AfxMessageBox( "Could not find video file.");
										//return FALSE;
									} else {
										currentSelectedMatchVideo = std::get<0>(matchDBList[sel-1]);
										loadedVideos[sel-1].CopyImage( outImage);
										SendMessage(hwndTrack, TBM_SETRANGE, TRUE, MAKELONG(0, outImage.getNumFrames()-1));
										SendMessage(hwndTrack, TBM_SETPOS, TRUE, std::get<2>(matchDBList[sel-1]));
										outImage.setCurrentFrame(std::get<2>(matchDBList[sel-1]));
										outImage.Modify();
									}
									InvalidateRect(hWnd, &rt, false);
								} else {
									// Already loaded
									outImage.CopyImage( loadedVideos[sel-1]);
									SendMessage(hwndTrack, TBM_SETRANGE, TRUE, MAKELONG(0, outImage.getNumFrames()-1));
									SendMessage(hwndTrack, TBM_SETPOS, TRUE, std::get<2>(matchDBList[sel-1]));
									outImage.setCurrentFrame(std::get<2>(matchDBList[sel-1]));
									outImage.Modify();
								}
							} else if ((currentSelectedMatchVideo.compare(std::get<0>(matchDBList[sel-1])) == 0)) {
								SendMessage(hwndTrack, TBM_SETPOS, TRUE, std::get<2>(matchDBList[sel-1]));
								outImage.setCurrentFrame(std::get<2>(matchDBList[sel-1]));
								outImage.Modify();
								InvalidateRect(hWnd, &rt, false);
							}
						} 
						CWindow myWindow;
						myWindow.Attach(hWnd);
						HWND hWndLeftFocus = myWindow.SetFocus();
					}
					break;
				case IDM_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
					break;
				case IDC_QUERY_PLAY_BUTTON:
					//SetTimer(hWnd, ID_QUERY_TIMER, 30, NULL );
					//if (!queryPlaying) {
					//	videoPlaying[0] = true;
					//	queryPlaying = true;
					//	CreateTimerQueueTimer(
					//		&videoTimer[0],
					//		NULL,
					//		(WAITORTIMERCALLBACK)TimerRoutine,
					//		0,
					//		0,
					//		40,
					//		WT_EXECUTEINTIMERTHREAD);
					//
					//	// The 'play'/'pause' button was pressed
					//	if( FAILED( hr = OnPlaySound( hWnd ) ) )
					//	{
					//		DXTRACE_ERR( TEXT("OnPlaySound"), hr );
					//		MessageBox( hWnd, "Error playing DirectSound buffer. "
					//					"Sample will now exit.", "DirectSound Sample", 
					//					MB_OK | MB_ICONERROR );
					//		EndDialog( hWnd, IDABORT );
					//	}
					//}
					PlaybackControl(0, PlaybackActions::PLAY);
					break;
				case IDC_QUERY_PAUSE_BUTTON:
					/*if (queryPlaying) {
						queryPlaying = false;
						if( g_pSound )
							g_pSound->Stop();
						KillTimer (hWnd, ID_QUERY_TIMER);

						DeleteTimerQueueTimer(NULL, videoTimer[0], NULL);
					}
					*/
					PlaybackControl(0, PlaybackActions::PAUSE);
					break;
				case IDC_QUERY_STOP_BUTTON:
					/*if (queryPlaying) {
						DeleteTimerQueueTimer(NULL, videoTimer[0], NULL);
					}
						if( g_pSound )
						{
							g_pSound->Stop();
							g_pSound->Reset();
						}
						KillTimer (hWnd, ID_QUERY_TIMER);
						inImage.setCurrentFrame(0);
						inImage.Modify();
						InvalidateRect(hWnd, &rt, false);
						queryPlaying = false;*/
					PlaybackControl(0, PlaybackActions::STOP);
					break;
				case IDC_MATCH_PLAY_BUTTON:
					PlaybackControl(1, PlaybackActions::PLAY);
					break;
				case IDC_MATCH_PAUSE_BUTTON:
					PlaybackControl(1, PlaybackActions::PAUSE);
					break;
				case IDC_MATCH_STOP_BUTTON:
					PlaybackControl(1, PlaybackActions::STOP);
					break;
				case ID_MODIFY_IMAGE:
					//outImage.Modify();
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
				/*char text[1000];
				strcpy(text, "Original image (Left)  Image after modification (Right)\n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				strcpy(text, "\nUpdate program with your code to modify input image");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);*/

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



VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	RECT rt;
	GetClientRect(hWnd, &rt);

	int index = *(int*)lpParam;

	if (videos[index]->getCurrentFrame() != (videos[index]->getNumFrames() - 1)) {
		videos[index]->Modify();
		videos[index]->setCurrentFrame(videos[index]->getCurrentFrame()+1);
		if (index == 1)
			SendMessage(hwndTrack, TBM_SETPOS, TRUE, videos[index]->getCurrentFrame());
	} else {
		PlaybackControl(index, PlaybackActions::STOP);
		
	}
    
	//InvalidateRect(*(HWND*)lpParam, &rt, false);
}



VOID PlaybackControl(int index, PlaybackActions action)
{
	RECT rt;
	HRESULT hr;
	GetClientRect(hWnd, &rt);
	
	switch(action) {
		case PLAY:
			if (!(videoPlaying[0] || videoPlaying[1])) {
				SetTimer(hWnd, ID_QUERY_TIMER, 30, NULL );
			}
			if (!videoPlaying[index]) {
				videoPlaying[index] = true;
				CreateTimerQueueTimer(
					&videoTimer[index],
					NULL,
					(WAITORTIMERCALLBACK)TimerRoutine,
					&videoIndex[index],
					0,
					40,
					WT_EXECUTEINTIMERTHREAD);
				
				if (index == 0){
					// The 'play'/'pause' button was pressed
					if( FAILED( hr = OnPlaySound( hWnd ) ) )
					{
						DXTRACE_ERR( TEXT("OnPlaySound"), hr );
						MessageBox( hWnd, "Error playing DirectSound buffer. "
									"Sample will now exit.", "DirectSound Sample", 
									MB_OK | MB_ICONERROR );
						EndDialog( hWnd, IDABORT );
					}
				}
			}
			break;
		case PAUSE:
			if (videoPlaying[index]) {
				
				if(index == 0){
					if( g_pSound )
						g_pSound->Stop();
				}

				if (!(videoPlaying[0] && videoPlaying[1])) {
					KillTimer (hWnd, ID_QUERY_TIMER);
				}
				videoPlaying[index] = false;
				DeleteTimerQueueTimer(NULL, videoTimer[index], NULL);
			}
			break;
		case STOP:
			if (videoPlaying[index]) {
				DeleteTimerQueueTimer(NULL, videoTimer[index], NULL);
			}

			if (index == 0) {
				if( g_pSound )
				{
					g_pSound->Stop();
					g_pSound->Reset();
				}
			}

			if (!(videoPlaying[0] && videoPlaying[1])) {
				KillTimer (hWnd, ID_QUERY_TIMER);
			}
			videos[index]->setCurrentFrame(0);
			videos[index]->Modify();
			if (index == 1)
				SendMessage(hwndTrack, TBM_SETPOS, TRUE, 0);
			InvalidateRect(hWnd, &rt, false);
		
			videoPlaying[index] = false;
			break;
	}
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



VOID QueryCompare()
{
	std::ifstream fin;
	std::stringstream ss;
	std::string line;
	int pixelsProcessed = 0;
	
	
	

	inImage.Analyze();
	for (int i=0 ; i<matchDBList.size() ; i++)
	{
		int BlackPixelAnalysis = -1;
		int	ColorAnalysis[SAT_INTERVALS][HUE_INTERVALS];
		double colorMatchSum = 0;
		int colorRowCount = -1;
		ss.str(std::string());
		ss.clear();
		ss << "matchDB\\" << std::get<0>(matchDBList[i]) << ".rgb.txt";
		//std::string test(ss.str());
		fin.open (ss.str().c_str(), std::ifstream::in);
		// Stages
		// 0 - Initial, Data
		// 1 - Frame by Frame
		// 2 - Color - Black, Hue, Sat
		// 3 - 
		int stage = 0;
		std::vector<double> matchAvgHue; // temporarily stores avg hue for currently processing match video
	
		if (fin)
		{
			while (std::getline(fin, line))
			{
				std::cout << line << std::endl;

				if (stage == 2)
				{
					colorRowCount++;
					int colorColCount = -1;
					int value;
					ss.clear();
					ss.str("");
					ss.str(line);
					while (ss >> value)
					{
						colorColCount++;
						ColorAnalysis[colorRowCount][colorColCount] = value;
						if (ss.peek() == ',')
							ss.ignore();
					}
				}

				if (stage == 1)
				{
					if (line.c_str()[0] == '#' && line.c_str()[1] == 'C') {
						std::getline(fin, line);
						ss.clear(); ss.str("");
						ss.str(line);
						ss >> BlackPixelAnalysis;
						stage = 2;
					} else
						matchAvgHue.push_back((double)atof(line.c_str()));
				}

				// Stage 0
				if (line.c_str()[0] == '#' && line.c_str()[1] == 'D')
				{
					std::getline(fin, line);
					ss.clear(); ss.str("");
					ss.str(line);
					ss >> pixelsProcessed;
					stage = 1;
					std::getline(fin, line); // hack to remove extra line that added extra row to avgHuePerFrame
				}
				
			}
		}
		fin.close();


		// Stores <int, double> pair corresponding to <which offset frame, match closeness>
		std::vector<std::pair<int, double>> offsetMatch;
		// Assumes match video longer than query video
		std::vector<double> queryAvgHue = inImage.getAvgHuePerFrame();
		for (int offset = 0 ; offset <= (matchAvgHue.size() - queryAvgHue.size()) ; offset++)
		{
			double dif = 0;
			for (int i = 0 ; i < queryAvgHue.size() ; i++)
			{
				dif += abs(queryAvgHue[i] - matchAvgHue[i+offset]);
			}
			offsetMatch.push_back(std::make_pair(offset, dif / (double)queryAvgHue.size()));
		}

		// offsetMatch now contains match % at each frame offset
		// sort it
		std::sort(offsetMatch.begin(), offsetMatch.end(), sort_avg_hue());
		// offsetMatch[0].first is the offset, offsetMatch[0].second is the "closeness" calculation

		// Color Analysis
		for (int row = 0; row < SAT_INTERVALS; row++){
			for (int col = 0; col < HUE_INTERVALS; col++){
				double qPercent = inImage.getColorAnalysisVal(row,col)/(double)((inImage.getWidth()/SUBSAMPLE_FACTOR)*(inImage.getHeight()/SUBSAMPLE_FACTOR)*inImage.getNumFrames());
				double mPercent = ColorAnalysis[row][col] / (double)pixelsProcessed;
				double dif = abs(qPercent - mPercent);
				colorMatchSum += (dif);
			}
		}

		// Black Pixel Analysis
		double qPercent = inImage.getBlackPixelAnalysis()/(double)((inImage.getWidth()/SUBSAMPLE_FACTOR)*(inImage.getHeight()/SUBSAMPLE_FACTOR)*inImage.getNumFrames());
		double mPercent = BlackPixelAnalysis / (double)pixelsProcessed;
		double blackMatch = abs(qPercent - mPercent);

		// TODO: integrate
		//double closeness = ((double)0.1 * blackMatch) + ((double)0.9 * ((colorMatchSum) / (double)(SAT_INTERVALS * HUE_INTERVALS)));
		double closeness = offsetMatch[0].second;
		std::get<1>(matchDBList[i]) = 1 - (closeness * (double)10);
		std::get<2>(matchDBList[i]) = offsetMatch[0].first;
	}

	std::sort(matchDBList.begin(), matchDBList.end(), sort_pred());

	/*char *lpBuffer = new char[256];
	strcpy_s(lpBuffer, 256, "test\0");
	SendMessage(hwndMatchList, LB_INSERTSTRING, 0, (LPARAM) lpBuffer);
	delete [] lpBuffer;*/

	//TCHAR* temp = (TCHAR*)matchDBList[0].first.c_str();
	//LPCWSTR pstr = (LPCWSTR)matchDBList[0].first.c_str(); //convert to LPCWSTR so that it could be converted to LPARAM on the next line 
	

	//SendMessageA(hwndMatchList, LB_ADDSTRING, 0, (LPARAM) (LPCSTR)matchDBList[0].first.c_str());
}

