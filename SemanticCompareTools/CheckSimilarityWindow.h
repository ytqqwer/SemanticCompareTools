#pragma once

#include "xWindow.h"

class csReader;
class csRecorder;

class CheckSimilarityWindow : public xWindow
{
public:
	CheckSimilarityWindow(HINSTANCE);
	~CheckSimilarityWindow();

	virtual void registerClass() override;
	virtual void initWindow() override;
	virtual LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual BOOL activeControls(bool) override;

public:
	static WNDPROC oldSearchEditProc;//原搜索编辑框处理过程
	static LRESULT CALLBACK searchEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void search();
	void openFiles();
	void loadFiles();

	void next();
	void prev();

	void setCheckState();
	void check();

	void refreshMainWindow();
	void resetPartOfSpeech();
	void refreshListView();
	
	void insertAllRecord();

private:
	csReader * reader;
	csRecorder * recorder;

private:
	int rowOfWord;
	int numberOfItem;

	std::wstring szFilePath;

private:
	HWND hSearchEdit;
	HWND hSearchButton;
	HWND hClassComboBox;

	HWND hWordText;
	HWND hPosText;

	HWND hDicText_One;
	HWND hDicText_Two;

	HWND hDictionaryListView_One;
	HWND hDictionaryListView_Two;

	HWND hCheckButton;
	HWND hNextWordButton;
	HWND hPrevWordButton;
};
