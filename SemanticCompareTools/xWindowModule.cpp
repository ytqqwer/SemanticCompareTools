#include "stdafx.h"
#include "xWindowModule.h"

#include "xWindow.h"

extern xWindowModule winModule;

xWindowModule::xWindowModule()
{
	activeWindow = nullptr;
}

xWindowModule::~xWindowModule()
{
}

void xWindowModule::initInstance(HINSTANCE hInst)
{
	m_hInst = hInst;
}

void xWindowModule::changeWindow(xWindow* window)
{
	if (activeWindow != nullptr) {
		activeWindow->closeWindow();
		delete activeWindow;
	}

	activeWindow = window;
	activeWindow->create();
	activeWindow->display();
	//activeWindow->messageLoop();
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void xWindowModule::createModelWindow(xWindow * window, HWND hParent)
{
	window->create();
	window->display();

	// disable掉父窗口，让其不再接受消息  
	HWND hParentWnd = hParent;
	while (hParentWnd != NULL)
	{
		EnableWindow(hParentWnd, FALSE);
		hParentWnd = GetParent(hParentWnd);
	}

	//window->messageLoop();
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 恢复父窗口enable状态  
	hParentWnd = hParent;
	while (hParentWnd != NULL)
	{
		EnableWindow(hParentWnd, TRUE);
		hParentWnd = GetParent(hParentWnd);
	}
	delete window;
}

void xWindowModule::createWindow(xWindow * window)
{
	window->create();
	window->display();

}

void xWindowModule::end()
{
	if (activeWindow != nullptr) {
		activeWindow->closeWindow();
		delete activeWindow;
	}
}

HINSTANCE xWindowModule::getInstance()
{
	return m_hInst;
}

LRESULT xWindowModule::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	void* p = winModule.GetXWindowObject(hWnd);

	if (p == nullptr)
	{
		p = winModule.AddXWindowObject(hWnd, nullptr);
	}

	xWindow* ptr = static_cast<xWindow *>(p);
	return ptr->processMessage(hWnd, message, wParam, lParam);
}

void xWindowModule::postQuitMessage()
{

	PostQuitMessage(0);
}

void* xWindowModule::AddXWindowObject(HWND hWnd, void* pThis)
{
	if (hWnd == nullptr)
	{
		pTempThis = pThis;
		return pTempThis;
	}
	else
	{
		m_MapObject.insert(std::make_pair(hWnd, pTempThis));
		void* p = pTempThis;
		pTempThis = nullptr;
		return p;
	}

}

void* xWindowModule::GetXWindowObject(HWND hWnd)
{
	std::map<HWND, void*>::iterator it = m_MapObject.find(hWnd);
	if (it != m_MapObject.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}
