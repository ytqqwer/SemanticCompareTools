#include "stdafx.h"
#include "xWindow.h"

xWindow::xWindow(HINSTANCE hInst) : hInstance(hInst)
{
}

xWindow::~xWindow()
{
}

void xWindow::create()
{
	registerClass();
	initWindow();
}

void xWindow::display()
{
	showWindow(SW_SHOW);
	updateWindow();
}

HWND xWindow::getHandle()
{
	return hWnd;
}

BOOL xWindow::showWindow(int command)
{
	return ::ShowWindow(hWnd, command);
}

BOOL xWindow::updateWindow()
{
	return ::UpdateWindow(hWnd);
}

BOOL xWindow::closeWindow()
{
	//SendMessage(hWnd,WM_CLOSE,0,0);

	if (!::DestroyWindow(hWnd))
		return FALSE;

	//hWnd = NULL;
	return TRUE;
}
