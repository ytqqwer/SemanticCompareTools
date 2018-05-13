#pragma once

#include "xWindow.h"

class SettingModelWindow : public xWindow
{
public:
	SettingModelWindow(HINSTANCE);
	SettingModelWindow(HINSTANCE,HWND);
	~SettingModelWindow();
	
	void setParentWindowPtr(xWindow*);

public:
	virtual void registerClass() override;
	virtual void initWindow() override;
	virtual BOOL activeControls(bool) override;
	virtual LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	
	void save();//保存选择的结果至文件

private:
	std::vector<FileNameAndColumnNames> FNACN;
	xWindow* parentWindowPtr;

private:
	HWND hParent;

	HWND hSaveButton;
	HWND hCancelButton;
	HWND hFileNameListView;

	HWND hPosText;

	HWND hDicNameText;
	HWND hDicNameEdit;
};
