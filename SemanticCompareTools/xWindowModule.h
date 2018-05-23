#pragma once
#include <vector>  
#include <map>  

class xWindow;

class xWindowModule
{
public:
	xWindowModule();
	~xWindowModule();

	void initInstance(HINSTANCE hInst);

	void changeWindow(xWindow* );
	void createModelWindow(xWindow* window , HWND parent);
	void createWindow(xWindow* window);

	void end();

	HINSTANCE getInstance();
	
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static void postQuitMessage();

	void* AddXWindowObject(HWND hWnd, void * pThis);
	void* GetXWindowObject(HWND hWnd);
	
public:
	xWindow* activeWindow;

private:
	HINSTANCE m_hInst;
		
	std::map<HWND, void*> m_MapObject;   // 保存映射关系的map  
	void* pTempThis;// 保存临时this的指针  
};
