#pragma once

#include "xWindow.h"


class WordDetailWindow : public xWindow
{
public:
	WordDetailWindow(HINSTANCE);
	~WordDetailWindow();
	
	virtual void registerClass() override;
	virtual void initWindow() override;
	virtual BOOL activeControls(bool) override;
	virtual LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

public:
	void setData(std::vector<std::pair<std::string, std::string>>&);
	void pushData(std::pair<std::string, std::string>&);

private:
	TEXTMETRIC  tm;
	int cxChar, cxCaps, cyChar;

	std::vector<std::pair<std::string, std::string>> datas;
};

