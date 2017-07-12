#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <tuple>
#include <algorithm>

#include "tinyxml2.h"

#include <commctrl.h>
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

HFONT g_pMainFont;

HINSTANCE gInst; // Global Handel to instance 

bool InitRegisterClass(HINSTANCE hinst);
bool MainForm(HINSTANCE hinst);
bool initModifyMenu(HINSTANCE hinst);

LRESULT CALLBACK MainWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK initModifyForm(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

static TCHAR szWindowClass[] = TEXT("ASS2Class");
static TCHAR szTitle[] = TEXT("assignment 2 programming");

int ScreenW = 0, ScreenH = 0;

int windowH = 640, windowW = 655;

HWND WindowForm; // Window Form 

HWND TeamName, AddTeamIndividual;

HWND IndividulaName, IndividualTeam, AddIndividualBTN;

HWND EventName, EventType, AddEventBTN;

HWND TeamOrIndividual, ListOfTeamsOrIndividuals, Event, AddToBTN;

HWND ListView, EventSelection, hwModifyMenu;

HWND AddScore, minScore;

HWND modifyform;

HWND ListOfIndividualsOrTeams, TypeOfEdit,
ModifyUserBTN, //ListOfTeamz, //IndividualsTeam
EventIndividuallb, EventIndividualList;
HWND IndividualsOrTeam;

std::vector<std::tuple<std::string, std::string >> Events; //Event Name, // Event Type
std::vector<std::tuple<std::string, std::string >> Individuals; //Individual Name, // Team name

std::vector<std::tuple<std::string, int >> Teams; //Name, // Slots left

std::vector<std::tuple<std::string, std::string, bool, int >> inEvents; //Name (Individual or team), // Event Name, // IsTeam (True) //, Score

//std::vector<std::tuple<std::string, std::string, int >> Individual; //Individual Name, // Team name, // 

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

//Send this in reverse order :P 
bool AddItemToRankBoard(std::string name, int rank, int score)
{
	
	const int MAX_SIZE = 256;
	wchar_t szBuffer[MAX_SIZE];

	std::wstring temps;

	LVITEM LvItem; // Defines the item to insert to randboard

	memset(&LvItem, 0, sizeof(LvItem)); // Resets it to NULL / Zero base

	LvItem.mask = LVIF_TEXT; // Only allow for text input
	LvItem.cchTextMax = MAX_SIZE; // Max size 256
	LvItem.iItem = 0; // Item definition 

	LvItem.iSubItem = 0;
	//convert
	temps = s2ws(std::to_string(rank));
	wcscpy(szBuffer, temps.c_str());
	LvItem.pszText = szBuffer;
	SendMessage(ListView, LVM_INSERTITEM, 1, (LPARAM)&LvItem); // send message to rankboard to insert item

	LvItem.iSubItem = 1; // col 2
	//Convert 
	temps = s2ws(name);
	wcscpy(szBuffer, temps.c_str());
	LvItem.pszText = szBuffer;
	SendMessage(ListView, LVM_SETITEM, 2, (LPARAM)&LvItem); // send message to rankboard to set item

	LvItem.iSubItem = 2; // col 3
	//Convert
	temps = s2ws(std::to_string(score));
	wcscpy(szBuffer, temps.c_str());
	LvItem.pszText = szBuffer;
	SendMessage(ListView, LVM_SETITEM, 3, (LPARAM)&LvItem); // send message to rankboard to set item

	return true;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hPrevInst, char* cmdArgs, int cmdShow)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// Get screen width and height
	ScreenH = GetSystemMetrics(SM_CYSCREEN);
	ScreenW = GetSystemMetrics(SM_CXSCREEN);

	gInst = hinst;

	HDC hdcScreen = GetDC(HWND_DESKTOP);

	g_pMainFont = CreateFont(-MulDiv(14, GetDeviceCaps(hdcScreen, LOGPIXELSY), 72),
		0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("Tahoma")); // Created font Tahoma size 14PX

	ReleaseDC(HWND_DESKTOP, hdcScreen); // Let go of  the screen handel

	if (!InitRegisterClass(hinst) || !MainForm(hinst) || !initModifyMenu(hinst))
		MessageBox(NULL, TEXT("Failed to initilize GUI"), NULL, NULL);


	Teams.push_back(std::make_tuple("None", 20)); // Team name, Slots left

	std::string tempstr;
	std::wstring temp;

#pragma region Initilize Event Type

	SendMessage(EventType, CB_RESETCONTENT, 0, 0);

	tempstr = "Team";
	temp = std::wstring(tempstr.begin(), tempstr.end());
	SendMessage(EventType, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

	tempstr = "Individual";
	temp = std::wstring(tempstr.begin(), tempstr.end());
	SendMessage(EventType, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

#pragma endregion

#pragma region Initilize Modify User Type

	SendMessage(TypeOfEdit, CB_RESETCONTENT, 0, 0);

	tempstr = "Team";
	temp = std::wstring(tempstr.begin(), tempstr.end());
	SendMessage(TypeOfEdit, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

	tempstr = "Individual";
	temp = std::wstring(tempstr.begin(), tempstr.end());
	SendMessage(TypeOfEdit, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

#pragma region Hide all
	
	ShowWindow(ModifyUserBTN, SW_HIDE);

	ShowWindow(AddScore, SW_HIDE);
	ShowWindow(minScore, SW_HIDE); 

	//ShowWindow(IndividualsTeam, SW_HIDE);
	//ShowWindow(ListOfTeamz, SW_HIDE);

	ShowWindow(EventIndividuallb, SW_HIDE);
	ShowWindow(EventIndividualList, SW_HIDE);

	ShowWindow(IndividualsOrTeam, SW_HIDE);
	ShowWindow(ListOfIndividualsOrTeams, SW_HIDE);

#pragma endregion

#pragma endregion


	ShowWindow(WindowForm, SW_SHOW); // Make sure to make the window vidable
	UpdateWindow(WindowForm); // Update it. 
	
							  /*
	SetWindowLongPtr(modifyform, GWL_WNDPROC, (LONG)&initModifyForm);
	ShowWindow(modifyform, SW_SHOW);
	SetFocus(modifyform);
	UpdateWindow(modifyform);
	*/

	MSG msg; 
	while (GetMessage(&msg, NULL, 0, 0)) // Wait for input for form, keeps thread open so the form will not close. 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

/*
Cpoied for me loader, Registers a new instace for a window. Can be simplified but this is good practise for long term / multiple windows. 
*/
bool InitRegisterClass(HINSTANCE hinst)
{
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hIconSm = NULL;
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hInstance = gInst;
	wcx.lpfnWndProc = MainWindow;
	wcx.lpszClassName = szWindowClass;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.lpszMenuName = 0;

	if (!RegisterClassEx(&wcx))
		return false;
	else
		return true;
}

/*
Form Design, This will control its order of appearance.  
*/
bool MainForm(HINSTANCE hinst)
{
	WindowForm = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		(ScreenW /2) - (windowW / 2), (ScreenH / 2) - (windowH / 2),
		windowW, windowH,
		NULL,
		NULL,
		hinst,
		NULL);

	if (!WindowForm)
		return false;

#pragma region Add Team

	HWND StaticLabel = CreateWindow(
		WC_STATIC, TEXT("Team Name:"),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 10, 120, 18, 
		WindowForm, NULL, gInst, NULL);

	if (!StaticLabel)
		return false;

	TeamName = CreateWindow(
			WC_EDIT, NULL,
			ES_LEFT | WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL,
			135, 10, 175, 18,
			WindowForm, NULL, gInst, NULL);

	if (!TeamName)
		return false;

	AddTeamIndividual = CreateWindow(
		WC_BUTTON, TEXT("Add Team"),
		ES_LEFT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 33, 300, 25,
		WindowForm, NULL, gInst, NULL);

	if (!TeamName)
		return false;

#pragma endregion

#pragma region Add Individual

	HWND lbIndividualName = CreateWindow(
		WC_STATIC, TEXT("Individual Name:"),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 73, 120, 18,
		WindowForm, NULL, gInst, NULL);

	if (!lbIndividualName)
		return false;

	IndividulaName = CreateWindow(
		WC_EDIT, NULL,
		ES_LEFT | WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL,
		135, 73, 175, 18,
		WindowForm, NULL, gInst, NULL);

	if (!IndividulaName)
		return false;

	HWND lbTeam = CreateWindow(
		WC_STATIC, TEXT("Team:"),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 96, 120, 18,
		WindowForm, NULL, gInst, NULL);

	if (!lbTeam)
		return false;

	IndividualTeam = CreateWindow(
		WC_COMBOBOX, 
		NULL,
		CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_VISIBLE,
		135, 96, 175, 18,
		WindowForm, NULL, gInst, NULL);

	if (!IndividualTeam)
		return false;

	AddIndividualBTN = CreateWindow(
		WC_BUTTON, TEXT("Add Individual"),
		ES_LEFT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 129, 300, 25,
		WindowForm, NULL, gInst, NULL);

	if (!AddIndividualBTN)
		return false;

#pragma endregion

#pragma region Add Event
	//HWND EventName, EventType, AddEventBTN;

	HWND LbStaticLabel = CreateWindow(
		WC_STATIC, TEXT("Event Name:"),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10 + 300 + 20, 10, 120, 18,
		WindowForm, NULL, gInst, NULL);

	if (!LbStaticLabel)
		return false;

	EventName = CreateWindow(
		WC_EDIT, NULL,
		ES_LEFT | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		10 + 300 + 20 + 120 + 5, 10, 175, 18,
		WindowForm, NULL, gInst, NULL);

	if (!EventName)
		return false;

	HWND LbStaticLabel2 = CreateWindow(
		WC_STATIC, TEXT("Event Type:"),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10 + 300 + 20, 10 + 18 + 5, 120, 18,
		WindowForm, NULL, gInst, NULL);

	if (!LbStaticLabel2)
		return false;

	EventType = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_VISIBLE,
		10 + 300 + 20 + 120 + 5, 10 + 18 + 5, 175, 18,
		WindowForm, NULL, gInst, NULL);

	if (!EventType)
		return false;

	AddEventBTN = CreateWindow(
		WC_BUTTON, TEXT("Add Event"),
		ES_LEFT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10 + 300 + 20, 10 + 18 + 5 + 25 + 5, 300, 25,
		WindowForm, NULL, gInst, NULL);

	if (!AddEventBTN)
		return false;


#pragma endregion

	hwModifyMenu = CreateWindow(
		WC_BUTTON, TEXT("Modify Events (Team/Individual)"),
		ES_LEFT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10 + 300 + 20, 10 + 18 + 5 + 25 + 5 + 50, 300, 25,
		WindowForm, NULL, gInst, NULL);

	if (!hwModifyMenu)
		return false;

	HWND lbEventSelection = CreateWindow(
		WC_STATIC, TEXT("Event Type:"),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10 + 300 + 20, 129 + 3 + 25 + 10, 120, 18,
		WindowForm, NULL, gInst, NULL);

	if (!lbEventSelection)
		return false;

	EventSelection = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_VISIBLE,
		10 + 300 + 20 + 120 + 5, 129 + 3 + 25 + 10, 175, 18,
		WindowForm, NULL, gInst, NULL);

	if (!EventSelection)
		return false;

#pragma region sadhjikldbasjkbdjkasbjdkabsjkdbakshjbdkhjblni

#pragma endregion

	AddScore = CreateWindow(
		WC_BUTTON, TEXT("+1 Score"),
		ES_LEFT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10 + 300 + 20, 129 + 3 + 25 + 10 + 100, 145, 25,
		WindowForm, NULL, gInst, NULL);

	if (!AddScore)
		return false;

	minScore = CreateWindow(
		WC_BUTTON, TEXT("-1 Score"),
		ES_LEFT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10 + 300 + 20 + 145 + 10, 129 + 3 + 25 + 10 + 100, 145, 25,
		WindowForm, NULL, gInst, NULL);

	if (!minScore)
		return false;

#pragma region Creates the Rankboard

	ListView = CreateWindowEx(0L,
		WC_LISTVIEW,                // list view class
		L"",                         // no default text
		WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT |
		LVS_EDITLABELS | WS_EX_CLIENTEDGE | LBS_MULTICOLUMN,
		10, 129 + 3 + 25 + 10,
		300, 400,
		WindowForm,
		NULL,
		gInst,
		NULL);

	if (!ListView)
		return false;


	LVCOLUMN lvc;

	memset(&lvc, 0, sizeof(lvc)); // Reset Coluom

	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvc.cx = 100;
	lvc.pszText = TEXT("Rank");

	SendMessage(ListView, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc); // Insert/Show the coloum
	lvc.pszText = TEXT("Name");
	SendMessage(ListView, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc); // Insert/Show the coloum
	lvc.pszText = TEXT("Score");
	SendMessage(ListView, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc); // Insert/Show the coloum
	
#pragma endregion

	return true;
}

/*
Windows controller, This can take multiple response, Alter the design colors so on. 
*/
LRESULT CALLBACK MainWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
	switch (msg)
	{
	case WM_INITDIALOG:
		
		break;
	case WM_CREATE:
		break;
	case WM_COMMAND:
		//Add Team
		if ((HWND)lparam == AddTeamIndividual && HIWORD(wparam) == BN_CLICKED)
		{
			wchar_t szTeamName[256];
			GetWindowText(TeamName, szTeamName, 256);

			if (!wcslen(szTeamName))
			{
				MessageBox(NULL, L"Team Name cannot be empty", NULL, NULL);
			}
			else {
				std::wstring temp(szTeamName);
				std::string TeamSTR(temp.begin(), temp.end());

				if (Teams.size() == 0)
				{
					Teams.push_back(std::make_tuple(TeamSTR, 5)); // Team name, Slots left
				}
				else {
					if (Teams.size() > 4)
					{
						MessageBox(NULL, L"Too many teams", NULL, NULL);
					}
					else {
						bool add = false;

						for (int i = 0; i < Teams.size(); i++)
						{
							if (std::get<0>(Teams[i]) == TeamSTR)
							{
								MessageBox(NULL, L"Team Name allready exsists", NULL, NULL);
								add = false;
								break;
							}
							else if (std::get<0>(Teams[i]) != TeamSTR) {
								add = true;
								continue;
							}
						}

						if (add)
						{
							Teams.push_back(std::make_tuple(TeamSTR, 5)); // Team name, Slots left
							add = false;
						}
					}
				}
				std::cout << Teams.size() << std::endl;
				
				SendMessage(IndividualTeam, CB_RESETCONTENT, 0, 0);

				for (int i = 0; i < Teams.size(); i++)
				{
					std::wstring temp = std::wstring(std::get<0>(Teams[i]).begin(), std::get<0>(Teams[i]).end());
					SendMessage(IndividualTeam, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

				}

			}


		}

		//Add Event
		if ((HWND)lparam == AddEventBTN && HIWORD(wparam) == BN_CLICKED)
		{
			wchar_t szEventName[256], szEventType[256];
			GetWindowText(EventName, szEventName, 256);
			GetWindowText(EventType, szEventType, 256);

			if (!wcslen(szEventName) || !wcslen(szEventType))
			{
				MessageBox(NULL, L"Event name or Event Type cannot be empty", NULL, NULL);
			}
			else {
				std::wstring temp(szEventName);
				std::wstring temp2(szEventType);
				std::string strEventName(temp.begin(), temp.end());
				std::string strEventType(temp2.begin(), temp2.end());

				if (Events.size() == 0)
				{
					Events.push_back(std::make_tuple(strEventName, strEventType)); // Event name and event type
				}
				else {
					bool add = false;

					for (int i = 0; i < Events.size(); i++)
					{
						if (std::get<0>(Events[i]) == strEventName && std::get<1>(Events[i]) == strEventType)
						{
							MessageBox(NULL, L"This event allready exsists", NULL, NULL);
							add = false;
							break;
						}
						else {
							add = true;
							continue;
						}
					}

					if (add)
					{
						Events.push_back(std::make_tuple(strEventName, strEventType)); // Event name and event type
						add = false;
					}
				}

				SendMessage(EventSelection, CB_RESETCONTENT, 0, 0);

				for (int i = 0; i < Events.size(); i++)
				{
					std::string tempFormat = std::get<0>(Events[i]) + " (" + std::get<1>(Events[i]) + ")";
					std::wstring temp = std::wstring(tempFormat.begin(), tempFormat.end());
					SendMessage(EventSelection, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

				}

			}


		}

		//Add Individual
		if ((HWND)lparam == AddIndividualBTN && HIWORD(wparam) == BN_CLICKED)
		{
			wchar_t szIndvName[256], szIndivTeam[256];
			GetWindowText(IndividulaName, szIndvName, 256);
			GetWindowText(IndividualTeam, szIndivTeam, 256);

			if (!wcslen(szIndvName) || !wcslen(szIndivTeam))
			{
				MessageBox(NULL, L"Event name or Event Type cannot be empty", NULL, NULL);
			}
			else {
				std::wstring temp(szIndvName);
				std::wstring temp2(szIndivTeam);
				std::string strIndvName(temp.begin(), temp.end());
				std::string strIndivTeam(temp2.begin(), temp2.end());

				bool add = false;
				if (Individuals.size() == 0)
				{ 
					add = true;
				}
				else {

					for (int i = 0; i < Individuals.size(); i++)
					{
						if (std::get<0>(Individuals[i]) == strIndvName || (std::get<0>(Individuals[i]) == strIndvName && std::get<1>(Individuals[i]) == strIndivTeam))
						{
							MessageBox(NULL, L"This person allready exsists", NULL, NULL);
							add = false;
							break;
						}
						else {
							add = true;
							continue;
						}
					}

					
				}

				for (int i = 0; i < Teams.size(); i++)
				{
					if (std::get<0>(Teams[i]) == strIndivTeam)
					{
						int slots = std::get<1>(Teams[i]);
						std::cout << strIndivTeam << "  :  " << slots << std::endl;
						if (slots == 0)
						{
							MessageBox(NULL, L"No more slots", NULL, NULL);
							add = false;
							break;
						}
						else {
							slots -= 1;
							std::get<1>(Teams[i]) = slots;
							add = true;
							continue;
						}
					}
				}

				if (add)
				{
					Individuals.push_back(std::make_tuple(strIndvName, strIndivTeam));
					add = false;
				}

				/*
				SendMessage(EventSelection, CB_RESETCONTENT, 0, 0);

				for (int i = 0; i < Events.size(); i++)
				{
					std::string tempFormat = std::get<0>(Events[i]) + " (" + std::get<1>(Events[i]) + ")";
					std::wstring temp = std::wstring(tempFormat.begin(), tempFormat.end());
					SendMessage(EventSelection, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

				}
				*/

			}


		}

		//Modify Individual / Team
		if ((HWND)lparam == hwModifyMenu && HIWORD(wparam) == BN_CLICKED)
		{
			SetWindowLongPtr(modifyform, GWL_WNDPROC, (LONG)&initModifyForm);
			ShowWindow(modifyform, SW_SHOW);
			SetFocus(modifyform);
			UpdateWindow(modifyform);
			std::cout << "Opened form" << std::endl;
		}

		if ((HWND)lparam == EventSelection && HIWORD(wparam) == CBN_SELCHANGE)
		{
			wchar_t szEventName[256];
			GetWindowText(EventSelection, szEventName, 256);

			std::wstring temp(szEventName);
			std::string eventname(temp.begin(), temp.end());

			if (!wcslen(szEventName))
			{
				MessageBox(NULL, L"Event name or Event Type cannot be empty", NULL, NULL);
			}
			else {
				SendMessage(ListView, LVM_DELETEALLITEMS, 0, 0);

				for (int i = 0; i < inEvents.size(); i++)
				{
					std::string tempEventname = std::get<1>(inEvents[i]);
					//Name (Individual or team), // Event Name, // IsTeam (True) //, Score

					if (eventname.find(tempEventname) != std::string::npos)
					{
						AddItemToRankBoard(std::get<0>(inEvents[i]), 0, std::get<3>(inEvents[i])); 

					}

				/*	if (tempEventname.find(eventname) != std::string::npos && tempEventname.find(std::get<3>(inEvents[i]) ? "Team" : "Individual") != std::string::npos)
					{
						AddItemToRankBoard(std::get<0>(inEvents[i]), 0, std::get<3>(inEvents[i]));
					}*/
				}

				//MessageBox(NULL, L"Changed Event", NULL, NULL);
			}
			ShowWindow(AddScore, SW_SHOW);
			ShowWindow(minScore, SW_SHOW);
		}

		if ((HWND)lparam == ListView && HIWORD(wparam) == LBN_SELCHANGE)
		{


		}

		//Add Score 
		if ((HWND)lparam == AddScore && HIWORD(wparam) == BN_CLICKED)
		{

			// line number to highlight (zero-based, so this example will select the third line down)
			int nLine = 2;

			ListView_SetExtendedListViewStyle(ListView, LVS_EX_FULLROWSELECT);
			ListView_SetItemState(ListView, -1, 0, LVIS_SELECTED); // deselect all
			ListView_SetItemState(ListView, 1, LVIS_SELECTED, LVIS_SELECTED); // select line nLine

			int selectedItem = ListView_GetItem(ListView,0 );

			std::cout << "Selected Index: " << selectedItem << std::endl;
		}

		if ((HWND)lparam == minScore && HIWORD(wparam) == BN_CLICKED)
		{

		}


		break;
	case WM_PAINT:
		break;
	case WM_SETFOCUS:
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_CTLCOLORSTATIC:
		break;
	case WM_CTLCOLOREDIT:
		break;
	case WM_CTLCOLORBTN:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);;

}

bool initModifyMenu(HINSTANCE hinst)
{
	int wndwidth = 500;
	int wndheight = 300;

	modifyform = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		(ScreenW / 2) - (windowW / 2), (ScreenH / 2) - (windowH / 2),
		wndwidth, wndheight,
		NULL,
		NULL,
		hinst,
		NULL);

	if (!modifyform)
		return false;

	HWND TypeOfEditLB = CreateWindow(
		WC_STATIC, TEXT("Type: "),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 10, 120, 18,
		modifyform, NULL, gInst, NULL);

	if (!TypeOfEditLB)
		return false;

	TypeOfEdit = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_VISIBLE,
		135, 10, 175, 18,
		modifyform, NULL, gInst, NULL);

	if (!TypeOfEdit)
		return false;

	IndividualsOrTeam = CreateWindow( // List Individuals Or Team
		WC_STATIC, TEXT("Individual: "),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 38, 120, 18,
		modifyform, NULL, gInst, NULL);

	if (!IndividualsOrTeam)
		return false;

	ListOfIndividualsOrTeams = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_VISIBLE,
		135, 38, 175, 18,
		modifyform, NULL, gInst, NULL);

	if (!ListOfIndividualsOrTeams)
		return false;

	//Only Show when Type = Individual
	/*IndividualsTeam = CreateWindow( 
		WC_STATIC, TEXT("Team: "),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 66, 120, 18,
		modifyform, NULL, gInst, NULL);

	if (!IndividualsTeam)
		return false;

	ListOfTeamz = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_VISIBLE,
		135, 66, 175, 18,
		modifyform, NULL, gInst, NULL);

	if (!ListOfTeamz)
		return false;*/

	EventIndividuallb = CreateWindow(
		WC_STATIC, TEXT("Event: "),
		ES_RIGHT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 66, 120, 18,
		modifyform, NULL, gInst, NULL);

	if (!EventIndividuallb)
		return false;

	EventIndividualList = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL | WS_VISIBLE,
		135, 66, 175, 18,
		modifyform, NULL, gInst, NULL);

	if (!EventIndividualList)
		return false;

	ModifyUserBTN = CreateWindow(
		WC_BUTTON, TEXT("Modify User"),
		ES_LEFT | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		10, 66 + 18 + 10, 300, 25,
		modifyform, NULL, gInst, NULL);

	if (!ModifyUserBTN)
		return false;

	return true;
}

LRESULT CALLBACK initModifyForm(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CTLCOLORSTATIC:
		
		break;
	case WM_CREATE:
		break;
	case WM_COMMAND:

		if ((HWND)lparam == TypeOfEdit && HIWORD(wparam) == CBN_SELCHANGE)
		{
			wchar_t szTypeOfEdit[256];
			GetWindowText(TypeOfEdit, szTypeOfEdit, 256);

			if (!wcslen(szTypeOfEdit))
			{
				MessageBox(NULL, L"Type Of edit cannot be blank", NULL, NULL);
			}
			else {
				std::wstring temp(szTypeOfEdit);
				std::string TypeOdEdit(temp.begin(), temp.end());

				std::string tempstr;
				std::wstring temp2;

				if (TypeOdEdit == "Individual")
				{
					ShowWindow(ModifyUserBTN, SW_SHOW);

					

					ShowWindow(IndividualsOrTeam, SW_SHOW); // Individual label 

					tempstr = "Individual";
					temp2 = std::wstring(tempstr.begin(), tempstr.end());
					SendMessage(IndividualsOrTeam, WM_SETTEXT, NULL, (LPARAM)temp2.c_str());

					ShowWindow(ListOfIndividualsOrTeams, SW_SHOW); // Load all individuals here in to this

					SendMessage(ListOfIndividualsOrTeams, CB_RESETCONTENT, 0, 0);

					for (int i = 0; i < Individuals.size(); i++)
					{
						std::wstring temp = std::wstring(std::get<0>(Individuals[i]).begin(), std::get<0>(Individuals[i]).end());
						SendMessage(ListOfIndividualsOrTeams, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

					}

					/*ShowWindow(IndividualsTeam, SW_SHOW); // Change this label to Individual
					ShowWindow(ListOfTeamz, SW_SHOW); // Load individuals in to this. 

					for (int i = 0; i < Teams.size(); i++)
					{
						std::wstring temp = std::wstring(std::get<0>(Teams[i]).begin(), std::get<0>(Teams[i]).end());
						SendMessage(ListOfTeamz, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

					}*/



					ShowWindow(EventIndividuallb, SW_SHOW); // Event labels
					ShowWindow(EventIndividualList, SW_SHOW); // Load Events in to this

					for (int i = 0; i < Events.size(); i++)
					{
						std::wstring temp = std::wstring(std::get<0>(Events[i]).begin(), std::get<0>(Events[i]).end());
						SendMessage(EventIndividualList, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

					}

				} 
				else if (TypeOdEdit == "Team")
				{

					ShowWindow(IndividualsOrTeam, SW_SHOW); // Team label 

					tempstr = "Team";
					temp2 = std::wstring(tempstr.begin(), tempstr.end());
					SendMessage(IndividualsOrTeam, WM_SETTEXT, NULL, (LPARAM)temp2.c_str());

					ShowWindow(ListOfIndividualsOrTeams, SW_SHOW); // Load all teams in to this

					SendMessage(ListOfIndividualsOrTeams, CB_RESETCONTENT, 0, 0);

					for (int i = 0; i < Teams.size(); i++)
					{
						std::wstring temp = std::wstring(std::get<0>(Teams[i]).begin(), std::get<0>(Teams[i]).end());
						SendMessage(ListOfIndividualsOrTeams, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

					}

					//ShowWindow(IndividualsTeam, SW_HIDE); // Change this label to Individual
					//ShowWindow(ListOfTeamz, SW_HIDE); // Load individuals in to this. 

					ShowWindow(EventIndividuallb, SW_SHOW); // Event labels
					ShowWindow(EventIndividualList, SW_SHOW); // Load Events in to this

					for (int i = 0; i < Events.size(); i++)
					{
						std::wstring temp = std::wstring(std::get<0>(Events[i]).begin(), std::get<0>(Events[i]).end());
						SendMessage(EventIndividualList, CB_ADDSTRING, 0, (LPARAM)((LPCWSTR)temp.c_str()));

					}

					ShowWindow(ModifyUserBTN, SW_SHOW);

				}
				else {
					ShowWindow(ModifyUserBTN, SW_HIDE);

					//ShowWindow(IndividualsTeam, SW_HIDE);
					//ShowWindow(ListOfTeamz, SW_HIDE);

					ShowWindow(EventIndividuallb, SW_HIDE);
					ShowWindow(EventIndividualList, SW_HIDE);

					ShowWindow(IndividualsOrTeam, SW_HIDE);
					ShowWindow(ListOfIndividualsOrTeams, SW_HIDE);
				}

			}
		}

		if ((HWND)lparam == ModifyUserBTN && HIWORD(wparam) == BN_CLICKED)
		{
			wchar_t szTypeOfEdit[256];
			GetWindowText(TypeOfEdit, szTypeOfEdit, 256);

			if (!wcslen(szTypeOfEdit))
			{
				MessageBox(NULL, L"Type Of edit cannot be blank", NULL, NULL);
			}
			else {
				std::wstring temp(szTypeOfEdit);
				std::string TypeOdEdit(temp.begin(), temp.end());
				
				//IndividualsOrTeam - Event Type
				//ListOfIndividualsOrTeams - Individual Name
				//ListOfTeamz - Selected Team
				//EventIndividualList - Event selected

				wchar_t szIndividualName[256];
				//wchar_t szTeam[256];
				wchar_t szEvent[256];
				GetWindowText(ListOfIndividualsOrTeams, szIndividualName, 256);
				//GetWindowText(ListOfTeamz, szTeam, 256);
				GetWindowText(EventIndividualList, szEvent, 256);

				std::wstring tpindivname(szIndividualName);
				//std::wstring tpteam(szTeam);
				std::wstring tpevent(szEvent);

				std::string strName(tpindivname.begin(), tpindivname.end());
				//std::string strTeam(tpteam.begin(), tpteam.end());
				std::string StrEvent(tpevent.begin(), tpevent.end());

				if (!wcslen(szIndividualName) || !wcslen(szEvent)) //|| !wcslen(szTeam)
				{
					MessageBox(NULL, L"Must Select all fields", NULL, NULL);
				}
				else {
					if (TypeOdEdit == "Individual")
					{
						if (inEvents.size() == 0)
						{
							inEvents.push_back(std::make_tuple(strName, StrEvent, true, 0));
						}
						else {
							bool add = false;

							for (int i = 0; i < inEvents.size(); i++)
							{
								if (std::get<0>(inEvents[i]) == strName && std::get<1>(inEvents[i]) == StrEvent)
								{
									MessageBox(NULL, L"This individual is allready in this event", NULL, NULL);
									add = false;
									break;
								}
								else if (std::get<0>(inEvents[i]) != strName && std::get<1>(inEvents[i]) != StrEvent) {
									add = true;
									continue;
								}
							}

							if (add)
							{
								inEvents.push_back(std::make_tuple(strName, StrEvent, true, 0));
								add = false;
							}

						}

					} else if (TypeOdEdit == "Team") {

					if (inEvents.size() == 0)
					{
						inEvents.push_back(std::make_tuple(strName, StrEvent, true, 0));
					}
					else {
						bool add = false;

						for (int i = 0; i < inEvents.size(); i++)
						{
							if (std::get<0>(inEvents[i]) == strName && std::get<1>(inEvents[i]) == StrEvent)
							{
								MessageBox(NULL, L"This individual is allready in this event", NULL, NULL);
								add = false;
								break;
							}
							else if (std::get<0>(inEvents[i]) != strName && std::get<1>(inEvents[i]) != StrEvent) {
								add = true;
								continue;
							}
						}

						if (add)
						{
							inEvents.push_back(std::make_tuple(strName, StrEvent, true, 0));
							add = false;
						}

					}
					}
				
				}
			}
		}

		break;
	case WM_PAINT:
		break;
	case WM_SETFOCUS:
		UpdateWindow(modifyform);
		break;
	case WM_LBUTTONDOWN:
		
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_CLOSE:
		SetWindowLongPtr(WindowForm, GWL_WNDPROC, (LONG)&MainWindow);
		ShowWindow(WindowForm, SW_SHOW);
		SetFocus(WindowForm);
		UpdateWindow(WindowForm);
		ShowWindow(modifyform, SW_HIDE);
		//PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);;
}
