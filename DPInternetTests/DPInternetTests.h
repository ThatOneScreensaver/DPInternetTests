#pragma once

#include "resource.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32")
HIMAGELIST ImageList;
HWND Toolbar;
TBADDBITMAP Bitmap;
const char ToolbarButtonTxt[] = {
    "Copy Log",
};

class HUD{

public:
    static HWND CreateToolbar(HINSTANCE hInst, HWND hDlg, const int numButtons) {
        
        const int buttonsToAdd = numButtons;
        TBBUTTON Buttons[2];

        //
        // Create toolbar window
        //

        Toolbar = CreateWindowExA(0, TOOLBARCLASSNAMEA, "DPInternetTools", WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_LIST | TBSTYLE_FLAT, 0, 0, 0, 0, hDlg, (HMENU)MainToolbar, hInst, 0);
        SendMessageA(Toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
        
        //
        // Choose which bitmap set to use
        //

        Bitmap.hInst = HINST_COMMCTRL;
        Bitmap.nID = IDB_STD_SMALL_COLOR;
        SendMessageA(Toolbar, TB_ADDBITMAP, 0, (LPARAM)&Bitmap);

        memset(&Buttons, 0, sizeof(TBBUTTON));

        //
        // ---------------------------- Buttons
        //

        // ------ Separator
        Buttons[0].fsState = TBSTATE_ENABLED;
        Buttons[0].fsStyle = BTNS_SEP;

        // ------ Copy Log
        Buttons[1].iBitmap = STD_COPY;
        Buttons[1].fsState = TBSTATE_ENABLED;
        Buttons[1].fsStyle = TBSTYLE_BUTTON;
        Buttons[1].idCommand = CopyLog;
        Buttons[1].iString = (INT_PTR)L"Copy Log to Clipboard";

        //
        // Send/Create buttons
        //

        SendMessageA(Toolbar, TB_SETMAXTEXTROWS, 0, 0);
        SendMessageA(Toolbar, TB_AUTOSIZE, 0, 0);
        SendMessageA(Toolbar, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_MIXEDBUTTONS);
        SendMessageA(Toolbar, TB_ADDBUTTONS, sizeof(Buttons)/sizeof(TBBUTTON), (LPARAM)&Buttons);

        return Toolbar;
    }

};