#include <windows.h>
#include <bits/stdc++.h>
#include <commctrl.h>
#include <commdlg.h>
#include <io.h>
#include <conio.h>
#include <tchar.h>
#include "main.h"
using namespace std;
HWND hwnd;
string codealltmp = "";
int wordsizepos = 4;
int wsizes[15] = {4,8,12,14,16,18,20,22,24,30,36,48,60,72,96};
string fontname = "Consolas";
bool fsaved=0, fopend=0, fcompiled=0;
bool programmeexiterrorstatusflag = 1;
unsigned long long variMsgCnt = 0;
HINSTANCE g_hInst;
char szFileName[MAX_PATH]="Untitled";
HWND g_hStatusBar, g_hToolBar;
bool hasstartopenfile = 0;
char commandline[MAX_PATH*10] = "";
string lasttimestr;
POINT cursorpoint;
BOOL runprocess(char szCommandLine[], int fwait, int fshow) {
	BOOL ret = system(szCommandLine);
	
	/*
	if (fshow && (!fwait)) {
		::ShellExecute(hwnd, "open", szCommandLine, NULL, NULL, SW_SHOWMAXIMIZED);
		return 0;
	}
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION  pi; 
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.wShowWindow = fshow?TRUE:FALSE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	MessageBox(0, szCommandLine, "Caption", 0);
	BOOL ret = ::CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	//if (fwait) {
		BOOL wret2 = ::WaitForSingleObject(pi.hProcess, INFINITE);
	//}
	char errorname[1000];
	sprintf(errorname, "ERROR:ret=%d; LastError=%d; WaitRet=%d", ret, GetLastError(), wret2);
	MessageBox(0, errorname, "", 0);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	*/
	return ret;
}


string output_time() {
	time_t rawtime;
   	time(&rawtime);   
   	char pblgtime[20];
	strftime(pblgtime, 20, "%Y-%m-%d %H-%M", localtime(&rawtime));
	string tttmps="";
	tttmps.insert(0, pblgtime);
	return tttmps;
}

BOOL LoadFile(HWND hEdit, LPSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF) {
			LPSTR pszFileText;
			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL) {
				DWORD dwRead;
				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL)) {
					pszFileText[dwFileSize] = 0; // Null terminator
					if(SetWindowText(hEdit, pszFileText)) {
						bSuccess = TRUE; // It worked!
						fopend=1;
						SendMessage(g_hStatusBar, SB_SETTEXT, 4, (LPARAM)szFileName); 
					}
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL SaveFile(HWND hEdit, LPSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD dwTextLength;
		dwTextLength = GetWindowTextLength(hEdit);
		if(dwTextLength > 0) {
			LPSTR pszText;
			pszText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
			if(pszText != NULL) {
				if(GetWindowText(hEdit, pszText, dwTextLength + 1)) {
					DWORD dwWritten;
					if(WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL)) {
						bSuccess = TRUE;
						fsaved=1;
					}
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL DoFileOpenSave(HWND hwnd, BOOL bSave) {
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	szFileName[0] = 0;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = (bSave ? "C++ Files (*.cpp; *.c++)\0*.cpp;*.c++\0C++ Header Files (*.hpp)\0*.hpp\0Pascal Files (*.pp)\0*.pp\0Windows命令脚本 (*.bat; *.cmd)\0*.bat;*.cmd\0All Files (*.*)\0*.*\0\0" : "C++ Files (*.cpp; *.c++; *.cxx)\0*.cpp;*.c++;*.cxx\0C++ Header Files (*.hpp)\0*.hpp\0Pascal Files (*.pp)\0*.pp\0Windows命令脚本 (*.bat; *.cmd)\0*.bat;*.com;*.cmd\0ClickIDE Temporary Compilation Logs\0*_compile_tmp.log\0All Files (*.*)\0*.*\0\0");
 	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "";

	if(bSave) {
		ofn.Flags = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
		if(GetSaveFileName(&ofn)) {
			if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
				MessageBox(hwnd, "Save file failed.\n(Or this is an empty file.)", "Error",MB_OK|MB_ICONEXCLAMATION);
				fsaved=0;
				return FALSE;
			}
		} else {
			return FALSE;
		}
	} else {
		ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
		if(GetOpenFileName(&ofn)) {
			if(!LoadFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
				MessageBox(hwnd, "Load of file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
				fopend=0;
				return FALSE;
			}
		} else {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL DoFileOpen(HWND hwnd, char rt[]) {
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	rt[0] = 0;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "配置设置(*.ini)\0*.ini\0\0";
	ofn.lpstrFile = rt;
	ofn.nMaxFile = MAX_PATH*4;
	ofn.lpstrDefExt = "";	
	ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	if(GetOpenFileName(&ofn)) {
		;
	} else {
		return FALSE;
	}
	return TRUE;
}

string getpasfn (char yufn[]) {
	string rtttmp = "";
	for (int i = 0; i < strlen(yufn) - 3; i++) {
		rtttmp += yufn[i];
	}
	return rtttmp;
}
string getcppfn (char yufn[]) {
	string rtttmp = "";
	for (int i = 0; i < strlen(yufn) - 4; i++) {
		rtttmp += yufn[i];
	}
	return rtttmp;
}

string i_to_str(int int_num) {
	string rt;
	char tmp[100];
	sprintf(tmp, "%d", int_num);
	rt.clear();
	rt+=tmp;
	return rt;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	HMENU hMenu, hFileMenu, hCompileMenu;
	ifstream wndfin;
	bool GHELPEXITFLAG = 0;
	string errreporti="";
	string titlestr01="Click 4.6 ";
	int errreportcnt = 0;
	char cmdbuf1[MAX_PATH+40];
	char cmdbuf2[MAX_PATH+40];
	char cmdbuf3[MAX_PATH+40];
	char cmdbuf4[MAX_PATH+40];
	char cmdbuf5[MAX_PATH+40];
	int iStatusWidths[] = {100, 230, 300, 320, -1};
	RECT rectStatus;
	bool isycl = 0;
	bool iszfc = 0;
	bool islfst = 1;
	bool nlycl = 0;
	bool issgzs = 0; /*//*/ //a
	bool ismtzs = 0;
	bool dontout = 0;
	RECT rctA; //定义一个RECT结构体，存储窗口的长宽高
	int wwidth = 1000, wheight = 600;
	ofstream fout;
	HFONT hFont;
	HFONT hFont_ln;
	FINDREPLACE repfindtag;
	char getallcodetmpstr[200000];
	int linecount = 0;
	string linenumtmptext;
	int cursorx=0, cursory=0;
	int cursorytmp=-1, cursorxtmp=-1;
	bool cursorxtmpset = 0;
	bool tosetcur = 0;
	/*4.8-- 
	if (tosetcur) {
		SetCaretPos((cursorpoint.x-6)/(wsizes[wordsizepos]/2.0), (cursorpoint.y-2)/wsizes[wordsizepos]);
		tosetcur=0;
	}
	--4.8*/ 
	switch(Message) {
		case WM_CREATE:
			GetWindowRect(hwnd,&rctA);//通过窗口句柄获得窗口的大小存储在rctA结构中
			wwidth = rctA.right - rctA.left;
			wheight = rctA.bottom - rctA.top;
			CreateWindow("EDIT", "",WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN|WS_BORDER,115, 30, wwidth-115/*CW_USEDEFAULT*/, wheight-120,hwnd, (HMENU)IDC_MAIN_TEXT, GetModuleHandle(NULL), NULL);
			CreateWindow("STATIC", "Welcome\nto\nClickIDE!\n\nVersion:\n4.6.0",WS_CHILD|WS_VISIBLE|WS_BORDER,0, 30, 60/*CW_USEDEFAULT*/, wheight-120,hwnd, (HMENU)IDC_LINE_NUM, GetModuleHandle(NULL), NULL);
			CreateWindow("STATIC", "1",WS_CHILD|WS_VISIBLE|WS_BORDER|SS_RIGHT,60, 30, 55/*CW_USEDEFAULT*/, wheight-120,hwnd, (HMENU)IDC_LINE_NUMT, GetModuleHandle(NULL), NULL);
			//CreateWindow("STATIC", "快捷功能：",WS_CHILD|WS_VISIBLE,60, wheight-112, 100, 19,hwnd, (HMENU)IDC_QUICKFUNC, GetModuleHandle(NULL), NULL);
			//CreateWindow("BUTTON", "Compile && Run as C++ File",WS_CHILD|WS_VISIBLE,180, wheight-114, 200, 23,hwnd, (HMENU)IDC_COMPRUN_C, GetModuleHandle(NULL), NULL);
			//CreateWindow("BUTTON", "Compile && Run as Pascal File",WS_CHILD|WS_VISIBLE,400, wheight-114, 200, 23,hwnd, (HMENU)IDC_COMPRUN_P, GetModuleHandle(NULL), NULL);
			//CreateWindow("BUTTON", "Save",WS_CHILD|WS_VISIBLE,620, wheight-114, 100, 23,hwnd, (HMENU)IDC_SAVE, GetModuleHandle(NULL), NULL);
			
			/*4.7*/hFont = CreateFont(wsizes[wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontname.c_str());//创建字体
			/*4.7*/hFont_ln = CreateFont(14,0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,"Consolas");//创建字体
			
			/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
			/*4.7*/SendDlgItemMessage(hwnd, IDC_LINE_NUMT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
			/*4.7*/SendDlgItemMessage(hwnd, IDC_LINE_NUM, WM_SETFONT,(WPARAM)hFont_ln/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
			///*4.7*/SendDlgItemMessage(hwnd, IDC_QUICKFUNC, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
			
			/*3.10: Statusbar*/
			g_hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL,
			WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
			hwnd, (HMENU)ID_STATUSBAR, g_hInst, NULL);
			
			SendMessage(g_hStatusBar, SB_SETPARTS, 5, (LPARAM)iStatusWidths);
			SendMessage(g_hStatusBar, SB_SETTEXT, 0, (LPARAM)"Click 4.6 IDE"); 
			SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
			SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "已编译" : "未编译")); 
			SendMessage(g_hStatusBar, SB_SETTEXT, 3, (LPARAM)""); 
			SendMessage(g_hStatusBar, SB_SETTEXT, 4, (LPARAM)szFileName); 
			/*--3.10*/
			
			/*4.7: ToolBar*/
			TBADDBITMAP tbab;
			TBBUTTON tbb[14];
			g_hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
			WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
			hwnd, (HMENU)ID_TOOLBAR, g_hInst, NULL);
			/*--4.7*/
			
			/*
			***
			*4.7 Tool Bar
			*Add Bitmap
			***
			*/
			SendMessage(g_hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
			
			tbab.hInst = HINST_COMMCTRL;
			tbab.nID = IDB_STD_SMALL_COLOR;
			SendMessage(g_hToolBar, TB_ADDBITMAP, 0, (LPARAM)&tbab);
			
			ZeroMemory(tbb, sizeof(tbb));
			
			tbb[0].iBitmap = STD_FILENEW;
			tbb[0].fsState = TBSTATE_ENABLED;
			tbb[0].fsStyle = TBSTYLE_BUTTON;
			tbb[0].idCommand = CM_FILE_NEW;
			
			tbb[1].iBitmap = STD_FILEOPEN;
			tbb[1].fsState = TBSTATE_ENABLED;
			tbb[1].fsStyle = TBSTYLE_BUTTON;
			tbb[1].idCommand = CM_FILE_OPEN;
			
			tbb[2].iBitmap = STD_FILESAVE;
			tbb[2].fsState = TBSTATE_ENABLED;
			tbb[2].fsStyle = TBSTYLE_BUTTON;
			tbb[2].idCommand = CM_FILE_SAVE;
			
			tbb[3].fsStyle = TBSTYLE_SEP;
			
			tbb[4].iBitmap = STD_CUT;
			tbb[4].fsState = TBSTATE_ENABLED;
			tbb[4].fsStyle = TBSTYLE_BUTTON;
			tbb[4].idCommand = CM_EDIT_CUT;
			
			tbb[5].iBitmap = STD_COPY;
			tbb[5].fsState = TBSTATE_ENABLED;
			tbb[5].fsStyle = TBSTYLE_BUTTON;
			tbb[5].idCommand = CM_EDIT_COPY;
			
			tbb[6].iBitmap = STD_PASTE;
			tbb[6].fsState = TBSTATE_ENABLED;
			tbb[6].fsStyle = TBSTYLE_BUTTON;
			tbb[6].idCommand = CM_EDIT_PASTE;
			
			tbb[7].fsStyle = TBSTYLE_SEP;
			
			tbb[8].iBitmap = STD_UNDO;
			tbb[8].fsState = TBSTATE_ENABLED;
			tbb[8].fsStyle = TBSTYLE_BUTTON;
			tbb[8].idCommand = CM_EDIT_UNDO;
			
			tbb[9].iBitmap = STD_FIND;
			tbb[9].fsState = TBSTATE_ENABLED;
			tbb[9].fsStyle = TBSTYLE_BUTTON;
			tbb[9].idCommand = CM_EDIT_FIND;
			
			tbb[10].fsStyle = TBSTYLE_SEP;
			
			tbb[11].iBitmap = STD_HELP;
			tbb[11].fsState = TBSTATE_ENABLED;
			tbb[11].fsStyle = TBSTYLE_BUTTON;
			tbb[11].idCommand = CM_ABOUT;
			
			tbb[12].fsStyle = TBSTYLE_SEP;
			
			tbb[13].iBitmap = STD_DELETE;
			tbb[13].fsState = TBSTATE_ENABLED;
			tbb[13].fsStyle = TBSTYLE_BUTTON;
			tbb[13].idCommand = CM_FILE_EXIT;
			
			SendMessage(g_hToolBar, TB_ADDBUTTONS, 14, (LPARAM)&tbb);
			/*
			*--4.7
			*/
			if (hasstartopenfile) {
				LoadFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), commandline);
				strcpy(szFileName, commandline);
				SendMessage(g_hStatusBar, SB_SETTEXT, 4, (LPARAM)szFileName); 
			}
			return 0;
			break;
		case WM_SIZE:
			RECT rectClient, rectStatus, rectTool;
			UINT uToolHeight, uStatusHeight, uClientAlreaHeight;
			
			GetWindowRect(hwnd,&rctA);//通过窗口句柄获得窗口的大小存储在rctA结构中
			wwidth = rctA.right - rctA.left;
			wheight = rctA.bottom - rctA.top;
			if(wParam != SIZE_MINIMIZED) {
				MoveWindow(GetDlgItem(hwnd, IDC_MAIN_TEXT), 115, 30, /*LOWORD(lParam)*/wwidth-115,/*HIWORD(lParam)*/wheight-120, TRUE);
				MoveWindow(GetDlgItem(hwnd, IDC_LINE_NUMT), 60, 30, /*LOWORD(lParam)*/55,/*HIWORD(lParam)*/wheight-120, TRUE);
				MoveWindow(GetDlgItem(hwnd, IDC_LINE_NUM), 0, 30, /*LOWORD(lParam)*/60,/*HIWORD(lParam)*/wheight-120, TRUE);
		    }
			SendMessage(g_hToolBar, TB_AUTOSIZE, 0, 0);
			SendMessage(g_hStatusBar, WM_SIZE, 0, 0);
			GetWindowRect(g_hStatusBar, &rectStatus);
			
			GetClientRect(hwnd, &rectClient);
			GetWindowRect(g_hStatusBar, &rectStatus);
			GetWindowRect(g_hToolBar, &rectTool);
			uToolHeight = rectTool.bottom - rectTool.top;
			uStatusHeight = rectStatus.bottom - rectStatus.top;
			uClientAlreaHeight = rectClient.bottom;
			
			break;
		case WM_SETFOCUS:
			SetFocus(GetDlgItem(hwnd, IDC_MAIN_TEXT));
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case CM_DT: {
					MessageBox(NULL, i_to_str(GetScrollPos(GetDlgItem(hwnd, IDC_MAIN_TEXT), SB_VERT)).c_str(), "", MB_OK);
					GetCaretPos(&cursorpoint);
					MessageBox(NULL, i_to_str(cursorpoint.x).c_str(), "", MB_OK);
					MessageBox(NULL, i_to_str(cursorpoint.y).c_str(), "", MB_OK);
					GetCaretPos(&cursorpoint);
					cursorx = (cursorpoint.x-6)/(wsizes[wordsizepos]/2.0)+1;
					cursory = (cursorpoint.y-2)/wsizes[wordsizepos]+1;
					MessageBox(NULL, i_to_str(cursorx).c_str(), "", MB_OK);
					MessageBox(NULL, i_to_str(cursory).c_str(), "", MB_OK);
					break;
				}
				case CM_FILE_OPEN:
					if (MessageBox (hwnd, " If you open a new file, the unsaved contents will be lost!\n Sure to continue?", "Warning!", MB_YESNO | MB_ICONWARNING) != IDYES) {
						break;
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Opening... ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Opening..."); 
					/*end:settitle*/ 
					if (!DoFileOpenSave(hwnd, FALSE)) {
						titlestr01="Click 4.6";
						SetWindowText (hwnd, titlestr01.c_str());
						SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "已编译" : "未编译")); 
						SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
						break;
					}
					fcompiled=0;
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "已编译" : "未编译")); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					/*end:settitle*/ 
					break;
				case CM_WLARGE: {
					if (wordsizepos >= 14) {
						MessageBox(hwnd, "已经是最大字体！", "", MB_OK);
						break;
					}
					/*4.7*/hFont = CreateFont(wsizes[++wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontname.c_str());//创建字体
					/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					/*4.7*/SendDlgItemMessage(hwnd, IDC_LINE_NUMT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					break;
				}
				case CM_WSMALL: {
					if (wordsizepos <= 0) {
						MessageBox(hwnd, "已经是最小字体！", "", MB_OK);
						break;
					}
					/*4.7*/hFont = CreateFont(wsizes[--wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontname.c_str());//创建字体
					/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					/*4.7*/SendDlgItemMessage(hwnd, IDC_LINE_NUMT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					break;
				}
				case CM_CFONT: {
					if (fontname == "Inconsolata") {
						fontname = "Consolas";
					} else {
						fontname = "Inconsolata";
					}
					/*4.7*/hFont = CreateFont(wsizes[wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontname.c_str());//创建字体
					/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					/*4.7*/SendDlgItemMessage(hwnd, IDC_LINE_NUMT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					break;
				}
				case CM_IMPORTSET: {
					char filenametoimport[MAX_PATH*10];
					if (!DoFileOpen(hwnd, filenametoimport)) {
						break;
					}
					wordsizepos  = GetPrivateProfileInt(TEXT("FONT"), TEXT("SIZE"), 5, filenametoimport);
					char fontnameini[100];
					GetPrivateProfileString(TEXT("FONT"), TEXT("NAME"), TEXT("Inconsolata"), fontnameini, 100, filenametoimport);
					/*4.7*/hFont = CreateFont(wsizes[wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontnameini);//创建字体
					/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					break;
				}
				case CM_FILE_SAVEAS:
					/*settitle*/ 
					titlestr01="Click 4.6 [ Saving... ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Saving..."); 
					/*end:settitle*/ 
					DoFileOpenSave(hwnd, TRUE);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					/*end:settitle*/ 
					break;
				case CM_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case CM_FILE_SAVE:
					/*settitle*/ 
					titlestr01="Click 4.6 [ Saving... ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Saving..."); 
					/*end:settitle*/ 
					if ((!fsaved && !fopend) || strcmp(szFileName, "Untitled") == 0) {
						if (!DoFileOpenSave(hwnd, TRUE)) {
							SetWindowText (hwnd, "Click 4.6");
							break;
						}
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.\n(Or this is an empty file.)", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_ABOUT:
					/*settitle*/ 
					SetWindowText (hwnd, "Click 4.6 [ About... ]");
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"About..."); 
					/*end:settitle*/ 
					MessageBox (hwnd, "Click IDE: 2020.4\nVersion: 4.6.5-Stable\nBy: 华育中学 Eric 倪天衡.\nGUI: Win32 API.\nIntroduction: Click is an light, open-source, convenient C++/Pascal IDE which based on MinGW and FPC.\nOnly for: Windows 7/8/8.1/10. You can contact us to get the XP Version.\nLicense: Apache License, Version 2.0\nTo learn more or get updates, please visit our official website: https://ericnth.cn/clickide/\nIf you meet some problems, please contact us or visit: Help->Get help..." , "About...", 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_RUN:
					if (fcompiled) {
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf2, "start \"Click4.6-Executing [%s.exe]\" /max %s.exe",getcppfn(szFileName).c_str(),getcppfn(szFileName).c_str());
						runprocess (cmdbuf2, 0, 1);
					} else {
						MessageBox (hwnd, "You haven't compiled this file yet (or have failed in it),\nPlease compile it first!", "Can't Run!", MB_OK | MB_ICONERROR);
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_RUNPAS:
					if (fcompiled) {
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
						sprintf (cmdbuf2, "start \"Click4.6-Executing [%s.exe]\" /max %s.exe",getpasfn(szFileName).c_str(),getpasfn(szFileName).c_str());
						runprocess (cmdbuf2, 0, 1);
					} else {
						MessageBox (hwnd, "You haven't compiled this file yet (or have failed in it),\nPlease compile it first!", "Can't Run!", MB_OK | MB_ICONERROR);
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_COMPILE:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Compiling..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						if (!DoFileOpenSave(hwnd, TRUE)) {
							break;
						}
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Compiling ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "g++.exe \"%s\" -o \"%s.exe\" 2> %s_compile_tmp.log",szFileName,getcppfn(szFileName).c_str(),szFileName);
					sprintf (cmdbuf2, "start \"Click4.6-Executing [%s.exe]\" /max %s.exe",getcppfn(szFileName).c_str(),getcppfn(szFileName).c_str());
					sprintf (cmdbuf3, "del \"%s.exe\"",getcppfn(szFileName).c_str());
					sprintf (cmdbuf4, "del \"%s_compile_tmp.log\"",szFileName);
					sprintf (cmdbuf5, "%s_compile_tmp.log",szFileName);
					runprocess (cmdbuf3, 1, 0);
					runprocess (cmdbuf1, 1, 0);
					wndfin.open (cmdbuf5);
					while (wndfin) {
						errreportcnt++;
						errreporti += wndfin.get();
					}
					wndfin.close();
					if (errreportcnt>1) {
						MessageBox (hwnd, errreporti.c_str(), "Click 4.6: Compile Error", MB_OK);
						break;
						fcompiled=0; 
					} else {
						fcompiled=1;
					}
					runprocess (cmdbuf4, 1, 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "已编译" : "未编译")); 
					/*end:settitle*/ 
					break;
				case CM_COMPILERUN:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Compiling..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						if (!DoFileOpenSave(hwnd, TRUE)) {
							break;
						}
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Compiling ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "g++ \"%s\" -o \"%s.exe\" 2> %s_compile_tmp.log",szFileName,getcppfn(szFileName).c_str(),szFileName);
					sprintf (cmdbuf2, "start \"Click4.6-Executing [%s.exe]\" /max %s.exe",getcppfn(szFileName).c_str(),getcppfn(szFileName).c_str());
					sprintf (cmdbuf3, "del \"%s.exe\"",getcppfn(szFileName).c_str());
					sprintf (cmdbuf4, "del \"%s_compile_tmp.log\"",szFileName);
					sprintf (cmdbuf5, "%s_compile_tmp.log",szFileName);
					runprocess (cmdbuf3, 1, 0);
					errreportcnt = 0;
					runprocess (cmdbuf1, 1, 0);
					wndfin.open (cmdbuf5);
					while (wndfin) {
						errreportcnt++;
						errreporti += wndfin.get();
					}
					wndfin.close();
					if (errreportcnt>1) {
						MessageBox (hwnd, errreporti.c_str(), "Click 4.6: Compile Error", MB_OK);
						fcompiled=0;
						break;
					} else {
						fcompiled=1;
					}
					runprocess (cmdbuf4, 1, 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					runprocess (cmdbuf2, 0, 1);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "已编译" : "未编译")); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					/*end:settitle*/ 
					break;
					
				case CM_COMPILPAS:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Compiling..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						if (!DoFileOpenSave(hwnd, TRUE)) {
							break;
						}
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Compiling ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "fpc.exe %s > %s_compile_tmp.log",szFileName,szFileName);
					sprintf (cmdbuf2, "start /max %s.exe",getpasfn(szFileName).c_str());
					sprintf (cmdbuf3, "del \"%s\"",getpasfn(szFileName).c_str());
					sprintf (cmdbuf4, "del \"%s_compile_tmp.log\"",szFileName);
					sprintf (cmdbuf5, "%s_compile_tmp.log",szFileName);
					//runprocess (cmdbuf3);
					runprocess (cmdbuf1, 1, 0);
					errreportcnt = 0;
					wndfin.open (cmdbuf5);
					while (wndfin) {
						errreportcnt++;
						errreporti += wndfin.get();
					}
					wndfin.close();
					if (errreportcnt>1) {
						MessageBox (NULL, errreporti.c_str(), "Click 4.6: Compile Message", MB_OK | MB_ICONINFORMATION);
						break;
						fcompiled=1;
					} else {
						fcompiled=1;
					}
					runprocess (cmdbuf4, 1, 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "已编译" : "未编译")); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_COMPILERUPAS:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Compiling..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						if (!DoFileOpenSave(hwnd, TRUE)) {
							break;
						}
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Compiling ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "fpc.exe %s > %s_compile_tmp.log",szFileName,szFileName);
					sprintf (cmdbuf2, "start /max %s.exe",getpasfn(szFileName).c_str());
					sprintf (cmdbuf3, "del \"%s\"",getpasfn(szFileName).c_str());
					sprintf (cmdbuf4, "del \"%s_compile_tmp.log\"",szFileName);
					sprintf (cmdbuf5, "%s_compile_tmp.log",szFileName);
					//runprocess (cmdbuf3);
					runprocess (cmdbuf1, 1, 0);
					wndfin.open (cmdbuf5);
					errreportcnt = 0;
					while (wndfin) {
						errreportcnt++;
						errreporti += wndfin.get();
					}
					wndfin.close();
					if (errreportcnt>1) {
						/*
						if (errreporti.find("Fatal:")&&errreporti.find("Error:")) {
							fcompiled=0;
							MessageBox (NULL, errreporti.c_str(), "Click 4.6: Compile Error", MB_OK | MB_ICONERROR);
							break;
						} else {
						*/
							fcompiled=1;
							MessageBox (NULL, errreporti.c_str(), "Click 4.6: Compile Message", MB_OK | MB_ICONINFORMATION);
						/*
						}
						*/
					} else {
						fcompiled=1;
					}
					runprocess (cmdbuf4, 1, 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					runprocess (cmdbuf2, 0, 1);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "已编译" : "未编译")); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_STARTCMD:
					runprocess ((char*)"start /max \"Click 4.6 [Command]\"", 0, 1);
					break;
				case CM_RUNBAT:
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running (Bat) ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running Bat..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.\n(Or this is an empty file.)", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					sprintf (cmdbuf2, "start \"Click4.6-Executing [%s]\" /max %s", szFileName, szFileName);
					runprocess (cmdbuf2, 0, 1);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_DEBUG: 
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Debugging..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					sprintf (cmdbuf2, "start /max \"Click4.6-Debugging [%s]\" gdb %s.exe", getcppfn(szFileName).c_str(), getcppfn(szFileName).c_str());
					runprocess (cmdbuf2, 0, 1);
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					break;
				case CM_VVARI:
					/*settitle*/ 
					titlestr01="Click 4.6 [ Viewing Variables... ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Viewing Variables..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "szFileName\t= %s\nfsaved         \t= %s\nfopened      \t= %s\nfcompiled   \t= %s\nCurrentTime\t= %s\nCurrentMessage\t= %d/%d\nMessageCount\t= %lld", szFileName, (fsaved ? "True" : "False"), (fopend ? "True" : "False"), (fcompiled ? "True" : "False"), output_time().c_str(), WM_COMMAND, CM_VVARI, variMsgCnt);
					MessageBox (hwnd, cmdbuf1, "Variables...", MB_OK | MB_ICONINFORMATION);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_GHELP:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Helps..."); 
					GHELPSTARTPLACE:
					
					switch (MessageBox (0, "在您使用该软件进行编译运行前，请确保您已经将您的g++编译器bin目录和fpc编译器的bin\\i386-win32\\目录添加到环境变量PATH。（环境变量设置方法：右击“此电脑”->属性，点击左侧“高级系统设置”，在“高级”标签下单击“环境变量(N)...”，双击“系统变量”中的PATH项进行编辑，在后面添加“XXX\\FPC\\2.2.2\\bin\\i386-win32\\”和“XXX\\MinGW\\bin\\”（将XXX替换为你的安装位置，不要加引号！），然后全部点击“确定”即可。", "Help 01", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "在您打开一个文件后，可以对它进行任何操作。我们并没有禁止类似打开一个C++文件后用\"Compile Pascal File...\"来进行编译等的操作（尽管这不对），因此您在使用编译/运行这些选项时，请务必确认是否选择了正确的语言！", "Help 02", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "由于作者能力有限以及本软件向C++的偏向性，部分Pascal程序可能无法正确编译/运行，请您谅解。您也可选择使用其他Pascal编译器（只要把它的目录添加到环境变量PATH，并将软件安装时自带的FPC目录从环境变量PATH中移除即可。", "Help 03", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "由于本软件开发时间较短，因此在使用过程中由以下限制：\n  1.仅用于Windows操作系统的部分支持Win32API的版本。\n  2.C++文件仅支持.cpp, .c++, .cxx后缀名，Pascal文件仅支持.pp后缀名，C++头文件仅支持.hpp后缀名，批处理文件仅支持.bat, .com, .cmd后缀名，请谅解。如您使用其他的后缀名（字符数量不符），可能导致编译运行失败。", "Help 04", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "在您想要直接运行/调试一个程序时，必须先保存。并且，如果您想要运行/调试当前您写的程序，请先编译，否则运行/调试的是您上一次编译后生成的程序。", "Help 05", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "若您的状态条被遮挡，且您想要查看，可以选择Help > Flush StatusBar进行刷新。", "Help 06", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "本文件夹内Click4.6.exe是可以单独使用的，即：您可以把这个exe文件复制到任意位置（甚至其他Windows电脑）均可使用。但您需要自己设置MinGW和FPC库，在本发布版中已经自带（即MinGW和FPC文件夹），但您仍然可以使用自己的库。只要将它添加到环境变量即可。", "Help 07", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "其次，若看到类似结尾为_compile_tmp.log的文件，是我们在编译过程中（可能会）生成的临时日志文件，您完全可以直接删除，对ClickIDE和其他软件的运行没有任何影响。", "Help 08", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "在您编译时，若是C++程序，仅当发生错误/警告时才会发出提示，否则直接编译完成；若是Pascal程序，任何情况下都会发出编译提示，所以请认真留意提示中是否存在例如\"Fatal\"或\"Error\"之类的字眼，如有，则表明编译出错，反之，则表明编译通过。", "Help 09", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "若有其他困难，问题，意见或者建议，请您一定要及时联系作者邮箱eric_ni2008@163.com进行咨询或投诉，以便我们今后把ClickIDE做得更加完善！", "Help 10", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					MessageBox (0, "没有更多提示了......", "Message", MB_OK | MB_ICONINFORMATION);
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					
					break;
				case CM_EDIT_UNDO:
					SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, EM_UNDO, 0, 0);
					break;
				case CM_EDIT_CUT:
					SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_CUT, 0, 0);
					break;
				case CM_EDIT_COPY:
					SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_COPY, 0, 0);
					break;
				case CM_EDIT_PASTE:
					SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_PASTE, 0, 0);
					break;
				case CM_EDIT_FIND:
					ZeroMemory(&repfindtag, sizeof(repfindtag));
					repfindtag.hwndOwner = hwnd;
					repfindtag.lpstrFindWhat = "\0";
					repfindtag.Flags = FR_DOWN|FR_FINDNEXT|FR_MATCHCASE;
					repfindtag.wFindWhatLen = MAX_PATH;
					repfindtag.wReplaceWithLen = MAX_PATH;
					//repfindtag.lpstrFindWhat = szFindWhat;
					repfindtag.lCustData = 0;
					repfindtag.lpfnHook = NULL;
					repfindtag.lStructSize = sizeof(repfindtag);
					FindText(&repfindtag);
					break;
				case CM_FLSTB:
					SendMessage(g_hStatusBar, SB_SETTEXT, 0, (LPARAM)"Click 4.6 IDE"); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "已编译" : "未编译")); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 3, (LPARAM)""); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 4, (LPARAM)szFileName); 
					break;
				case CM_GHTML: {
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"GeneratingHTML..."); 
					GetDlgItemText(hwnd, IDC_MAIN_TEXT, getallcodetmpstr, 200000);
					codealltmp.clear();
					codealltmp+=getallcodetmpstr;
					codealltmp+="\n     ";
					//MessageBox(hwnd, codealltmp.c_str(), "", MB_OK);
					char hlfilename[MAX_PATH*6];
					strcpy(hlfilename, szFileName);
					strcat(hlfilename, (char*)"_highlight.html");
					char titlefilename[MAX_PATH*6];
					strcpy(titlefilename, "Click4.6-");
					strcat(titlefilename, hlfilename);
					fout.open(hlfilename);
					fout << "<!DOCTYLE html>" << endl;
					fout << "<html><head><meta charset=\"utf-8\"/><title>";
					fout << titlefilename;
					fout << "</title><style>body{font-family:Consolas,Arial,Helvetica,sans-serif;}h3{font-style:italic}.operator{color:#ff2c0f;}.zfc{color:#0000ff;}.ycl{color:#80ab66}.zs{color:#0078d7;}.gjz{font-weight:bold;}.vari{font-weight:bold;color:#8e37a7;}</style></head>" << endl;
					fout << "<body><h3>Exported by ClickIDE 4.6</h3><p>";
					for (int i = 0; i < codealltmp.size(); i++) {
						if (dontout) {
							dontout = 0;
							continue;
						}
						char tch = codealltmp[i];
						if (tch == '\"') {
							islfst=0;
							if (codealltmp[i-1] == '\\') {
								fout << "quot;";continue;
							}
							if (!iszfc) {
								fout << "<span class=\"zfc\">&quot;";
								iszfc = true;continue;
							}
							if (iszfc) {
								fout << "&quot;</span>";
								iszfc = false;continue;
							}
						}
						if (tch == '#' && islfst) {
							islfst=0;
							isycl = 1;
							fout << "<span class=\"ycl\">#";continue; 
						}
						if (tch == '/' && codealltmp[i+1] == '*') {
							ismtzs = 1;
							fout << "<span class=\"zs\">/";continue; 
						}
						if (tch == '*' && codealltmp[i+1] == '/') {
							ismtzs = 0;dontout=1;
							fout << "*/</span>";continue; 
						}
						if (tch == '/' && codealltmp[i+1] == '/') {
							issgzs = 1;dontout=1;
							fout << "<span class=\"zs\">//";continue; 
						}
						if (tch == ' ') {
							fout << "&nbsp;";continue;
						}
						if (issgzs || ismtzs) {
							if (tch == '<') {fout << "&lt;";continue;}
							if (tch == '>') {fout << "&gt;";continue;}
							if (tch == '&') {fout << "&amp;";continue;}
							if (tch == '\"') {fout << "&quot;";continue;}
							if (tch == '\t') {fout << "&nbsp;&nbsp;&nbsp;&nbsp;";continue;}
							if (tch == ' ') {fout << "&nbsp;";continue;}
							if (tch == '\n') {
								if (isycl && !nlycl) {
									isycl = 0;
									fout << "</span>"; 
								}
								if (issgzs) {
									fout << "</span>";
									issgzs = 0;
								}
								islfst = 1;fout << "<br/>";continue;
							}
							fout << tch;
							continue;
						}
						if (tch == '<') {islfst=0;if (isycl || iszfc) {fout << "&lt";} else {fout << "<span class=\"operator\">&lt</span>";}continue;}
						if (tch == '>') {islfst=0;if (isycl || iszfc) {fout << "&gt";} else {fout << "<span class=\"operator\">&gt</span>";}continue;}
						if (tch == '&') {islfst=0;if (isycl || iszfc) {fout << "&amp";} else {fout << "<span class=\"operator\">&amp;</span>";}continue;}
						if (tch == '{') {islfst=0;if (isycl || iszfc) {fout << "{";} else {fout << "<span class=\"operator\">{</span>";}continue;}
						if (tch == '}') {islfst=0;if (isycl || iszfc) {fout << "}";} else {fout << "<span class=\"operator\">}</span>";}continue;}
						if (tch == '(') {islfst=0;if (isycl || iszfc) {fout << "(";} else {fout << "<span class=\"operator\">(</span>";}continue;}
						if (tch == ')') {islfst=0;if (isycl || iszfc) {fout << ")";} else {fout << "<span class=\"operator\">)</span>";}continue;}
						if (tch == ':') {islfst=0;if (isycl || iszfc) {fout << ":";} else {fout << "<span class=\"operator\">:</span>";}continue;}
						if (tch == ';') {islfst=0;if (isycl || iszfc) {fout << ";";} else {fout << "<span class=\"operator\">;</span>";}continue;}
						if (tch == '+') {islfst=0;if (isycl || iszfc) {fout << "+";} else {fout << "<span class=\"operator\">+</span>";}continue;}
						if (tch == '-') {islfst=0;if (isycl || iszfc) {fout << "-";} else {fout << "<span class=\"operator\">-</span>";}continue;}
						if (tch == '*') {islfst=0;if (isycl || iszfc) {fout << "*";} else {fout << "<span class=\"operator\">*</span>";}continue;}
						if (tch == '/') {islfst=0;if (isycl || iszfc) {fout << "/";} else {fout << "<span class=\"operator\">/</span>";}continue;}
						if (tch == '%') {islfst=0;if (isycl || iszfc) {fout << "%";} else {fout << "<span class=\"operator\">%</span>";}continue;}
						if (tch == '~') {islfst=0;if (isycl || iszfc) {fout << "~";} else {fout << "<span class=\"operator\">~</span>";}continue;}
						if (tch == '?') {islfst=0;if (isycl || iszfc) {fout << "?";} else {fout << "<span class=\"operator\">?</span>";}continue;}
						if (tch == '^') {islfst=0;if (isycl || iszfc) {fout << "^";} else {fout << "<span class=\"operator\">^</span>";}continue;}
						if (tch == '[') {islfst=0;if (isycl || iszfc) {fout << "[";} else {fout << "<span class=\"operator\">[</span>";}continue;}
						if (tch == ']') {islfst=0;if (isycl || iszfc) {fout << "]";} else {fout << "<span class=\"operator\">]</span>";}continue;}
						if (tch == '=') {islfst=0;if (isycl || iszfc) {fout << "=";} else {fout << "<span class=\"operator\">=</span>";}continue;}
						if (tch == ',') {islfst=0;if (isycl || iszfc) {fout << ",";} else {fout << "<span class=\"operator\">,</span>";}continue;}
						if (tch == '\n') {
							if (isycl && !nlycl) {
								isycl = 0;
								fout << "</span>"; 
							}
							if (issgzs) {
								fout << "</span>";
								issgzs = 0;
							}
							islfst = 1;
							fout << "<br/>";continue;
						}
						if (tch == '\\') {
							if (codealltmp[i+1] == '\n' && isycl) {
								nlycl = 1;
							}
						}
						if (tch == '\t') {
							fout << "&nbsp;&nbsp;&nbsp;&nbsp;";continue;
						}
						fout << tch;
						islfst = 0;
					}
					fout << "</p></body></html>" << endl;
					fout.close();
					Sleep(200);
					ShellExecute(NULL,TEXT("open"), hlfilename, TEXT(""),NULL,SW_SHOWNORMAL);
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					break;
				}
				
				case CM_ASTYLE: {
					MessageBox(hwnd, "本版本（4.6-Stable）不支持该功能。若想尝鲜，请联系作者获取内测版本。下一个正式版本（4.8-stable）将会包含此功能。\n", "Ah oh~", MB_OK);
					break;
					PostMessage(hwnd, WM_COMMAND, CM_FILE_SAVE, (LPARAM)"");
					char astylestr[MAX_PATH*6];
					sprintf(astylestr, "--recursive --style=bsd --convert-tabs --indent=spaces=4 --attach-closing-while --indent-switches --indent-namespaces --indent-continuation=4 --indent-preproc-block --indent-preproc-define --indent-preproc-cond --indent-col1-comments --pad-oper --pad-paren-in --unpad-paren --delete-empty-lines --align-pointer=name --align-reference=name --break-elseifs --add-braces >%s.astyle.log %s*", szFileName, szFileName);
					//system(astylestr)
					MessageBox(NULL, astylestr, "", MB_OK);
					ShellExecute(NULL,TEXT("open"), "AStyle.exe", TEXT(astylestr),NULL,SW_HIDE);
					sprintf(astylestr, "del %s.orig", szFileName);
					system(astylestr);
					
					break;
				}
				case CM_GITHUB: {
					ShellExecute(NULL,TEXT("open"), TEXT("https://github.com/EricNTH080103/ClickIDE"), TEXT(""),NULL,SW_SHOWNORMAL);
					break;
				}
				case CM_WEBSITE: {
					ShellExecute(NULL,TEXT("open"), TEXT("https://ericnth.cn/clickide/"), TEXT(""),NULL,SW_SHOWNORMAL);
					break;
				}
			}
			hMenu = GetMenu(hwnd);
			hFileMenu = GetSubMenu(hMenu, 0);
			hCompileMenu = GetSubMenu(hMenu, 1);
			EnableMenuItem(hFileMenu, CM_FILE_SAVE, MF_BYCOMMAND | (!(!fsaved && !fopend) || strcmp(szFileName, "Untitled") ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hCompileMenu, CM_RUN, MF_BYCOMMAND | ((fcompiled) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hCompileMenu, CM_RUNPAS, MF_BYCOMMAND | ((fcompiled) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hCompileMenu, CM_DEBUG, MF_BYCOMMAND | ((fcompiled) ? MF_ENABLED : MF_GRAYED));
			char tishitext[1024];
			GetDlgItemText(hwnd, IDC_MAIN_TEXT, getallcodetmpstr, 200000);
			codealltmp.clear();
			codealltmp+=getallcodetmpstr;
			linecount = 0;
			
			GetCaretPos(&cursorpoint);
			cursorx = (cursorpoint.x-6)/(wsizes[wordsizepos]/2.0);
			cursory = (cursorpoint.y-2)/wsizes[wordsizepos];
			cursorytmp = cursory;
			cursorxtmp = -1;
			for (int i = 0; i < codealltmp.size(); i++) {
				if (cursorytmp == 0 && cursorxtmpset == 0) {
					cursorxtmp = cursorx;
					cursorxtmpset = 1;
				}
				if(cursorxtmp == 0) {
					//if (i!=0){
					if (codealltmp[i] == '(') {
						/*
						if (cursorx==0&&cursory==0) {
							break;
						}*/
						tosetcur = 1;
						/*4.8-- 
						codealltmp.insert(i+1, ")");
						SetDlgItemText(hwnd, IDC_MAIN_TEXT, codealltmp.c_str());
						--4.8*/ 
						cursorxtmp = -1;
					}
					//MessageBox(NULL, "", "", 0);
					//}
				}
				if (codealltmp[i] == '\n') {
					linecount++;
					cursorytmp--;
				}
				if (codealltmp[i] == '\t') {
					cursorxtmp-=7;
				}
				cursorxtmp--;
			}
			cursorxtmp = -1;
			cursorytmp = -1;
			sprintf(tishitext, "Welcome\nto\nClickIDE!\n\nVersion:\n4.6.5\n\nWords:\n%d\nLines:\n%d\n\nFont size:%d", codealltmp.size(), linecount+1, wsizes[wordsizepos]);
			SetDlgItemText(hwnd, IDC_LINE_NUM, tishitext);
			
			for (int i = GetScrollPos(GetDlgItem(hwnd, IDC_MAIN_TEXT), SB_VERT); i <= linecount; i++) {
				/*
				//MessageBox(NULL, "", "", NULL);
				char tmpinsertlinenumstr[20] = "";
				char tmpinsertlinenumstr2[20];
				strcpy(tmpinsertlinenumstr2, i_to_str(i+1).c_str());
				for (int j = 0; j < 6-strlen(tmpinsertlinenumstr2); j++) {
					strcat(tmpinsertlinenumstr, " ");
				}
				strcat(tmpinsertlinenumstr, tmpinsertlinenumstr2);
				strcat(tmpinsertlinenumstr, "\n");*/
				linenumtmptext += i_to_str(i+1);
				linenumtmptext += "\n";
			}
			SetDlgItemText(hwnd, IDC_LINE_NUMT, linenumtmptext.c_str());
			//SetScrollPos(GetDlgItem(hwnd, IDC_MAIN_TEXT), SB_VERT, GetScrollPos(GetDlgItem(hwnd, IDC_MAIN_TEXT), SB_VERT), 1);
			break;
		/*
		case WM_CTLCOLOREDIT: {
      		HDC hdc = (HDC)wParam;
   			SetTextColor(hdc, RGB(0xFF, 0xFF, 0xFF));
   			SetBkColor((HDC)wParam,RGB(0x0, 0x0, 0x0));
   			return (long long int)((HBRUSH)GetStockObject(NULL_BRUSH));
   			break;
   		}*/
		case WM_CLOSE:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Exitting..."); 
					/*settitle*/ 
					titlestr01="Click 4.6 [ Exiting... ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
			if (MessageBox (hwnd, "Are you sure to quit? \nThings which are not saved will be lost!", "Exiting...", MB_OKCANCEL | MB_ICONQUESTION) != IDOK) {
				/*settitle*/ 
				titlestr01="Click 4.6 [ ";
				titlestr01+=szFileName;
				titlestr01+=" ]";
				SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
				SetWindowText (hwnd, titlestr01.c_str());
				/*end:settitle*/ 
				break;
			}
			programmeexiterrorstatusflag = 0;
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			/*
			if (programmeexiterrorstatusflag) {
				if (MessageBox (0, "Abnormal exit!\nDo you want to try to recover?", "Click 4.6", MB_YESNO | MB_ICONQUESTION) == IDYES) {
					break;
				}
			}
			*/
			DeleteObject(hFont);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow) {
	if (strcmp(lpCmdLine, "") != 0) {
		hasstartopenfile = 1;
		if (_access(lpCmdLine, W_OK) == -1) {
			MessageBox(NULL, "文件不存在或拒绝访问！", "Click 4.6", MB_OK);
			hasstartopenfile = 0;
		} else {
			strcpy(commandline, lpCmdLine);
		}
	}
	WNDCLASSEX wc;
	//HWND hwnd;
	MSG Msg;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = "MAINMENU";
	wc.lpszClassName = "WindowClass";
	wc.hIconSm       = LoadIcon(hInstance,"A"); /* A is name used by project icons */

	if(!RegisterClassEx(&wc)) {
		MessageBox(0,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
		return 0;
	}
	
	sprintf (szFileName, "%s", "Untitled"); 
	
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Click 4.6",WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,600,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL) {
		MessageBox(0, "Window Creation Failed!", "Error!",MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
		return 0;
	}

	ShowWindow(hwnd,1);
	UpdateWindow(hwnd);
	

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		variMsgCnt++;
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
