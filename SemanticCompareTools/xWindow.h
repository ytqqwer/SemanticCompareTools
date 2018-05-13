#pragma once

#include "common.h"

class xWindow
{
public:
	xWindow(HINSTANCE hInst);
	virtual ~xWindow();

public:
	// 创建窗口  
	void create();
	void display();
	
	BOOL showWindow(int command);//SW_SHOW，SW_HIDE
	BOOL updateWindow();
	BOOL closeWindow();
	
	// 返回窗口对象句柄
	HWND getHandle();

	void messageLoop();

public:
	virtual void registerClass() = 0;
	virtual void initWindow() = 0;
	
	virtual LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

	virtual BOOL activeControls(bool) = 0;
protected:
	HINSTANCE hInstance;
	HWND hWnd;
	
	WCHAR title[MAX_LOADSTRING];                  // 标题栏文本
	WCHAR className[MAX_LOADSTRING];            // 主窗口类名

};