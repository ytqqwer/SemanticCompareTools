#pragma once

#include "xWindow.h"

#include <map>

class sdReader;
class WordDetailWindow;

class SearchDictionaryWindow : public xWindow
{
public:
	friend class SettingModelWindow;

public:
	SearchDictionaryWindow(HINSTANCE);
	~SearchDictionaryWindow();
	
	virtual void registerClass() override;
	virtual void initWindow() override;
	virtual BOOL activeControls(bool) override;
	virtual LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

public:
	static WNDPROC oldSearchEditProc;	//原搜索编辑框处理过程
	static LRESULT CALLBACK searchEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void search();
	void openFiles();
	void resetFNAPOS();
	void resetDicName();
	void refreshComboBox();
	void refreshListView();
	
private:
	sdReader * reader;

	std::wstring szFilePath;

private:
	bool isOpenFile;

	std::vector<ListViewRow> listViewRows;

	std::vector<WordDetailWindow *> wordDetailWindowsPtr;

	std::vector<FileNameAndPartOfSpeech> FNAPOS;

private:
	HWND hSearchEdit;
	HWND hSearchButton;

	HWND hWordText;
	HWND hDicNameText;

	HWND hFileNameOrPartOfSpeechComboBox;
	HWND hDictionaryListView;
};
