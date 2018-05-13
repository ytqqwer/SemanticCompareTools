#pragma once

#include "common.h"

class xWindow
{
public:
	xWindow(HINSTANCE hInst);
	virtual ~xWindow();

public:
	// ��������  
	void create();
	void display();
	
	BOOL showWindow(int command);//SW_SHOW��SW_HIDE
	BOOL updateWindow();
	BOOL closeWindow();
	
	// ���ش��ڶ�����
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
	
	WCHAR title[MAX_LOADSTRING];                  // �������ı�
	WCHAR className[MAX_LOADSTRING];            // ����������

};