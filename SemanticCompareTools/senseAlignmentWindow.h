#pragma once

#include "xWindow.h"

class senseAlignmentWindow : public xWindow
{
public:
	senseAlignmentWindow(HINSTANCE);
	~senseAlignmentWindow();

	virtual void registerClass() override;
	virtual void initWindow() override;
	virtual LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual BOOL activeControls(bool) override;
public:
	static WNDPROC oldSearchEditProc;//原搜索编辑框处理过程
	static LRESULT CALLBACK searchEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:

	HWND hSearchEdit;
	HWND hSearchButton;
	HWND hClassComboBox;
	
	HWND hSimilarityStaticText;
	HWND hSimilarityText;
	HWND hRelationshipStaticText;
	HWND hRelationshipText;
	HWND hNewRelationshipStaticText;
	HWND hNewRelationshipText;

	HWND hDictionaryListView_One;
	HWND hDictionaryListView_Two;

	HWND hRelationEqualButton; 
	HWND hRelationNotEqualButton;
	HWND hRelationUnsureButton;
	HWND hRelationBelongButton;

	HWND hDetailButton;
	HWND hNextWordButton;

};




