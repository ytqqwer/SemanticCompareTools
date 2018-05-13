#pragma once

#include "xWindow.h"

class SelectFunctionWindow : public xWindow
{
public:
	SelectFunctionWindow(HINSTANCE);
	~SelectFunctionWindow();
	
	virtual void registerClass() override;
	virtual void initWindow() override;
	virtual LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual BOOL activeControls(bool) override;

private:
	HWND senseAlignmentHandle;
	HWND checkSimilarityHandle;
	HWND searchDictionaryHandle;
	HWND confirmButtonHandle;	
};

