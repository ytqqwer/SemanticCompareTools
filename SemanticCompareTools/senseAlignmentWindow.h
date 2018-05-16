#pragma once

#include "xWindow.h"

class saReader;
class saRecorder;

class SenseAlignmentWindow : public xWindow
{
public:
	SenseAlignmentWindow(HINSTANCE);
	~SenseAlignmentWindow();

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

	void next();
	void prev();

	void refreshMainWindow();
	void resetPartOfSpeech();

	void recordMappingRelation(const std::string&);

private:
	saReader * reader;
	saRecorder * recorder;

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
	HWND hRelationBelongButton;
	HWND hRelationUnsureButton;

	HWND hPrevWordButton;
	HWND hNextWordButton;

};




