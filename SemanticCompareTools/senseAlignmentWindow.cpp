#include "stdafx.h"
#include "SenseAlignmentWindow.h"

#include "windowClassRegister.h"
#include "xWindowModule.h"

#include "IniFile.h"
#include "saReader.h"
#include "saRecorder.h"

extern xWindowModule winModule;

// ��̬���ݳ�Ա�������ඨ�� �� ���г�ʼ��
WNDPROC SenseAlignmentWindow::oldSearchEditProc = NULL;
static SenseAlignmentWindow* myWindow;

SenseAlignmentWindow::SenseAlignmentWindow(HINSTANCE instance) : xWindow(instance)
{
	wcscpy_s(className, L"SenseAlignmentWindow");
	wcscpy_s(title, L"����Ա�");

	reader = new saReader();
	recorder = new saRecorder();
	recorder->Init();
}

SenseAlignmentWindow::~SenseAlignmentWindow()
{
	delete reader;
	delete recorder;
}

void SenseAlignmentWindow::registerClass()
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = xWindowModule::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SEMANTICCOMPARETOOLS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = MAKEINTRESOURCEW(ID_MENU_SENSEALIGNMENT);
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	WindowClassRegister::getInstance()->registerClass(&wcex);
}

void SenseAlignmentWindow::initWindow()
{
	winModule.AddXWindowObject(nullptr, this);

	hWnd = CreateWindowW(className, title, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, 800, 450, nullptr, nullptr, hInstance, nullptr);

	//////////////////////////////////////////////////////////////////////
	std::vector<std::string> columnNames{
		u8"ID",
		u8"����",
		u8"�������",
		u8"ƴ��",
		u8"����",
		u8"��������",
		u8"ʾ��",
		u8"���ƶ�",	
		u8"gkb_����",
		u8"gkb_����",
		u8"gkb_ƴ��",
		u8"gkb_ͬ��",
		u8"gkb_����",
		u8"gkb_����",
		u8"ӳ���ϵ",
	};
	reader->setColumnNames(columnNames);
	reader->setWordColumnName(u8"����");

	//////////////////////////////////////////////////////////////////////
	//��ʼ�������༭��
	myWindow = this;
	hSearchEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
		65, 20, 180, 30, hWnd, (HMENU)ID_EDIT_SEARCH, hInstance, NULL);
	SenseAlignmentWindow::oldSearchEditProc =
		(WNDPROC)SetWindowLongPtr(hSearchEdit, GWLP_WNDPROC, (LONG_PTR)SenseAlignmentWindow::searchEditProc);

	//////////////////////////////////////////////////////////////////////
	//��ʼ��������ť
	hSearchButton = CreateWindow(_T("BUTTON"), _T("����"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		270, 20, 100, 30, hWnd, (HMENU)ID_BUTTON_SEARCH, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//��ʼ��������������б�
	hClassComboBox = CreateWindow(WC_COMBOBOX, _T(""), CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		430, 20, 100, 500, hWnd, (HMENU)ID_COMBOBOX_SEARCH, hInstance, NULL);

	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"r");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"v");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"d");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"p");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"c");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"q");

	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"n");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"o");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"m");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"e");

	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"δ");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"a");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"u");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"׺");
	// Send the CB_SETCURSEL message to display an initial item in the selection field 
	SendMessage(hClassComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//////////////////////////////////////////////////////////////////////
	//��ʼ���ı�	
	HWND wordStaticText = CreateWindow(_T("static"), _T("����"), WS_CHILD | WS_VISIBLE | SS_LEFT, 25, 20, 30, 30, hWnd,
		NULL, hInstance, NULL);
	HWND posStaticText = CreateWindow(_T("static"), _T("����"), WS_CHILD | WS_VISIBLE | SS_LEFT, 390, 20, 30, 30, hWnd,
		NULL, hInstance, NULL);

	HWND dic1StaticText = CreateWindow(_T("static"), _T("�ִ�����ʵ�"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 90, 250, 30, hWnd,
		NULL, hInstance, NULL);
	HWND dic2StaticText = CreateWindow(_T("static"), _T("GKB�ִ������﷨��Ϣ�ʵ�"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 200, 250, 30, hWnd,
		NULL, hInstance, NULL);

	hSimilarityStaticText = CreateWindow(_T("static"), _T("���ƶ�"), WS_CHILD | WS_VISIBLE | SS_LEFT, 560, 10, 90, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_SIMILARITY, hInstance, NULL);
	hSimilarityText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 10, 100, 30, hWnd,
		(HMENU)ID_TEXT_SIMILARITY, hInstance, NULL);

	hRelationshipStaticText = CreateWindow(_T("static"), _T("��Ӧ��ϵ"), WS_CHILD | WS_VISIBLE | SS_LEFT, 560, 40, 90, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_RELATIONSHIP, hInstance, NULL);
	hRelationshipText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 40, 100, 30, hWnd,
		(HMENU)ID_TEXT_RELATIONSHIP, hInstance, NULL);

	hNewRelationshipStaticText = CreateWindow(_T("static"), _T("�¶�Ӧ��ϵ"), WS_CHILD | WS_VISIBLE | SS_LEFT, 560, 70, 90, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_NEW_RELATIONSHIP, hInstance, NULL);
	hNewRelationshipText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 70, 100, 30, hWnd,
		(HMENU)ID_TEXT_NEW_RELATIONSHIP, hInstance, NULL);

	HFONT hFont = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");//��������
	SendMessage(hSearchButton, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hSearchEdit, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hClassComboBox, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ

	SendMessage(wordStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(posStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(dic1StaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(dic2StaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ

	SendMessage(hNewRelationshipStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hRelationshipStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hSimilarityStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hNewRelationshipText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hRelationshipText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hSimilarityText, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	
	{	
		/////////////////////////////////////////////////////////////////////
		//��ʼ���ʵ�1���б���ͼ
		hDictionaryListView_One = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
			30, 120, 600, 70, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_ONE, hInstance, NULL);

		WCHAR szText[256];     // Temporary buffer.
		LVCOLUMN lvc;
		// Initialize the LVCOLUMN structure.
		// The mask specifies that the format, width, text,
		// and subitem members of the structure are valid.
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		//Add the columns.

		lvc.iSubItem = 0;
		lvc.pszText = szText;
		lvc.cx = 60;
		wcscpy_s(szText, L"ID");
		if (ListView_InsertColumn(hDictionaryListView_One, 0, &lvc) == -1)
			return;

		lvc.iSubItem = 1;
		lvc.pszText = szText;
		lvc.cx = 60;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_One, 1, &lvc) == -1)
			return;

		lvc.iSubItem = 2;
		lvc.pszText = szText;
		lvc.cx = 90;
		wcscpy_s(szText, L"�������");
		if (ListView_InsertColumn(hDictionaryListView_One, 2, &lvc) == -1)
			return;

		lvc.iSubItem = 3;
		lvc.pszText = szText;
		lvc.cx = 75;
		wcscpy_s(szText, L"ƴ��");
		if (ListView_InsertColumn(hDictionaryListView_One, 3, &lvc) == -1)
			return;

		lvc.iSubItem = 4;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_One, 4, &lvc) == -1)
			return;

		lvc.iSubItem = 5;
		lvc.pszText = szText;
		lvc.cx = 130;
		wcscpy_s(szText, L"��������");
		if (ListView_InsertColumn(hDictionaryListView_One, 5, &lvc) == -1)
			return;

		lvc.iSubItem = 6;
		lvc.pszText = szText;
		lvc.cx = 130;
		wcscpy_s(szText, L"ʾ��");
		if (ListView_InsertColumn(hDictionaryListView_One, 6, &lvc) == -1)
			return;

		//////////////////////////////////////////////////////////////////////
		//��ʼ���ʵ�2���б���ͼ
		hDictionaryListView_Two = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
			30, 230, 600, 70, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_TWO, hInstance, NULL);
		
		lvc.iSubItem = 0;
		lvc.pszText = szText;
		lvc.cx = 60;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_Two, 0, &lvc) == -1)
			return;

		lvc.iSubItem = 1;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_Two, 1, &lvc) == -1)
			return;

		lvc.iSubItem = 2;
		lvc.pszText = szText;
		lvc.cx = 75;
		wcscpy_s(szText, L"ƴ��");
		if (ListView_InsertColumn(hDictionaryListView_Two, 2, &lvc) == -1)
			return;

		lvc.iSubItem = 3;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"ͬ��");
		if (ListView_InsertColumn(hDictionaryListView_Two, 3, &lvc) == -1)
			return;

		lvc.iSubItem = 4;
		lvc.pszText = szText;
		lvc.cx = 200;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_Two, 4, &lvc) == -1)
			return;

		lvc.iSubItem = 5;
		lvc.pszText = szText;
		lvc.cx = 200;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_Two, 5, &lvc) == -1)
			return;
	}

	//////////////////////////////////////////////////////////////////////
	//��ť
	hRelationEqualButton = CreateWindow(_T("BUTTON"), _T("����"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 120, 100, 30, hWnd, (HMENU)ID_BUTTON_EQUAL, hInstance, NULL);

	hRelationNotEqualButton = CreateWindow(_T("BUTTON"), _T("������"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 170, 100, 30, hWnd, (HMENU)ID_BUTTON_NOT_EQUAL, hInstance, NULL);

	hRelationBelongButton = CreateWindow(_T("BUTTON"), _T("����"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 220, 100, 30, hWnd, (HMENU)ID_BUTTON_BELONG, hInstance, NULL);

	hRelationUnsureButton = CreateWindow(_T("BUTTON"), _T("��ȷ��"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 270, 100, 30, hWnd, (HMENU)ID_BUTTON_UNSURE, hInstance, NULL);

	hPrevWordButton = CreateWindow(_T("BUTTON"), _T("��һ��"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		260, 330, 100, 30, hWnd, (HMENU)ID_BUTTON_PREV_WORD, hInstance, NULL);

	hNextWordButton = CreateWindow(_T("BUTTON"), _T("��һ��"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		380, 330, 100, 30, hWnd, (HMENU)ID_BUTTON_NEXT_WORD, hInstance, NULL);
	
	SendMessage(hRelationNotEqualButton, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hRelationEqualButton, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hRelationBelongButton, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hRelationUnsureButton, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hPrevWordButton, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hNextWordButton, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	
	//////////////////////////////////////////////////////////////////////
	// ����ĳЩ���ܣ�ֱ�������TRUEʹ�ã�FALSE��ֹ
	activeControls(FALSE);
}

BOOL SenseAlignmentWindow::activeControls(bool boolean)
{
	EnableWindow(hSearchButton, boolean);
	EnableWindow(hSearchEdit, boolean);
	EnableWindow(hDictionaryListView_One, boolean);
	EnableWindow(hDictionaryListView_Two, boolean);
	EnableWindow(hClassComboBox , boolean);

	EnableWindow(hRelationEqualButton, boolean);
	EnableWindow(hRelationNotEqualButton, boolean);
	EnableWindow(hRelationBelongButton, boolean);
	EnableWindow(hRelationUnsureButton, boolean);

	EnableWindow(hPrevWordButton, boolean);
	EnableWindow(hNextWordButton, boolean);

	return true;
}

void SenseAlignmentWindow::search()
{
	TCHAR buff[80] = _T("");
	GetWindowText(hSearchEdit, buff, 80);
	std::string& word = WTS_U8(buff);

	if (reader->findWord(word)) {
		refreshMainWindow();
	}
	else {
		MessageBox(hWnd, L"�ڵ�ǰ������δ�ҵ��ô��", L"��ʾ", MB_OK);
	}

}

void SenseAlignmentWindow::openFiles()
{
	//���� shell32.dll api   ��������ļ��жԻ��� 
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = GetForegroundWindow();//������  
	bInfo.lpszTitle = TEXT("��ѡ������Ӧ�����ڵ��ļ���");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI/*����һ���༭�� �û������ֶ���д·�� �Ի�����Ե�����С֮���..*/ |
		BIF_UAHINT/*��TIPS��ʾ*/ | BIF_NONEWFOLDERBUTTON /*�����½��ļ��а�ť*/;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL)//������ȷ����ť  
	{
		////////////////////////////////////////////////////////////////////
		// ��ȡĿ¼��Ϊ��֧�����ģ���Ҫ����ȡ���ַ�����TCHAR����תΪCHAR���룬ʹ�ô���ҳCP_ACP
		SHGetPathFromIDList(lpDlist, szPathName);

		char cPath[256];
		//��ȡ�ֽڳ���   
		int iLength = WideCharToMultiByte(CP_ACP, 0, szPathName, -1, NULL, 0, NULL, NULL);
		//��tcharֵ����_char    
		WideCharToMultiByte(CP_ACP, 0, szPathName, -1, cPath, iLength, NULL, NULL);

		////////////////////////////////////////////////////////////////////
		IniFile::getInstance()->SetFilePath(szPathName);
		
		////////////////////////////////////////////////////////////////////
		//����Ŀ¼�µ�xlsx�ļ�
		std::string searchPath = cPath;
		searchPath = searchPath + "\\*.xlsx";

		std::wstring wpath = szPathName;
		wpath = wpath + L"\\";

		_finddata_t fileDir;
		long lfDir;
		if ((lfDir = _findfirst(searchPath.c_str(), &fileDir)) == -1l) {
			MessageBox(hWnd, L"No xlsx file is found\n", L"��ʾ", MB_OK);
			return;
		}
		else {
			//�������֮ǰ�����������Ϣ
			reader->clear();

			do {
				//�ҵ���xlsx�ļ����Ƚ�charת��Ϊwchar���ٽ�wcharת��Ϊutf-8�����char
				TCHAR cache[256];
				iLength = MultiByteToWideChar(CP_ACP, 0, fileDir.name, strlen(fileDir.name) + 1, NULL, 0);
				MultiByteToWideChar(CP_ACP, 0, fileDir.name, strlen(fileDir.name) + 1, cache, iLength);

				std::wstring ws(cache);
				std::string utf8_str = WTS_U8(ws);

				reader->addXlsxFileName(utf8_str);

			} while (_findnext(lfDir, &fileDir) == 0);
		}
		_findclose(lfDir);
		
		////////////////////////////////////////////////////////////////////

		//����������ʽ���Լ�����Ĵ���Ӣ����ĸ
		std::wstring pattern_1to1{ L"\\w*\\dto\\d\\w*" };
		std::wstring pattern_1tom{ L"\\w*\\dto[a-zA-Z]\\w*" };
		std::wstring pattern_mto1{ L"\\w*[a-zA-Z]to\\d\\w*" };
		std::wstring pattern_mtom{ L"\\w*[a-zA-Z]to[a-zA-Z]\\w*" };
		std::wstring pattern_end{ L"\\w*.xlsx" };

		std::vector<std::wstring> pattern_part;
		pattern_part.push_back(pattern_1to1);
		pattern_part.push_back(pattern_1tom);
		pattern_part.push_back(pattern_mto1);
		pattern_part.push_back(pattern_mtom);

		for (auto& part : pattern_part) {
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_DAI), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_DONG), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_FU), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_JIE), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_LIAN), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_LIANG), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_MING), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_NI), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_SHU), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"̾" + pattern_end), WTS_U8(PART_OF_SPEECH_TAN), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"δ" + pattern_end), WTS_U8(PART_OF_SPEECH_WEI), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_XING), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_ZHU), WTS_U8(wpath));
			reader->loadXlsxFile(WTS_U8(part + L"׺" + pattern_end), WTS_U8(PART_OF_SPEECH_ZHUI), WTS_U8(wpath));
		}
	}

}

void SenseAlignmentWindow::next()
{
	if (reader->nextWord()) {
		refreshMainWindow();
	}
	else
		MessageBox(hWnd, L"�������һ�����", L"��ʾ", MB_OK);

}

void SenseAlignmentWindow::prev()
{
	if (reader->prevWord()) {
		refreshMainWindow();
	}
	else
		MessageBox(hWnd, L"���ǵ�һ�����", L"��ʾ", MB_OK);

}

void SenseAlignmentWindow::refreshMainWindow()
{
	// ���ListView�е������� 
	ListView_DeleteAllItems(hDictionaryListView_One);
	ListView_DeleteAllItems(hDictionaryListView_Two);

	SetWindowText(hNewRelationshipText, (LPWSTR)L"");	//��ӳ���ϵ
		
	int row = reader->getCurRowIndex();

	std::string str;
	std::wstring wstr;
	std::vector<std::string> datas;

	//////////////////////////////////////////////////////////////////////
	//�ʵ�1	
	LVITEM vitem;
	vitem.mask = LVIF_TEXT;
	vitem.iItem = 0;


	str = reader->getValueInColumnByRow(row, u8"ID");
	wstr = STW_U8(str);
	vitem.iSubItem = 0;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_InsertItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"����");
	wstr = STW_U8(str);
	vitem.iSubItem = 1;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"�������");
	wstr = STW_U8(str);
	vitem.iSubItem = 2;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);
	
	str = reader->getValueInColumnByRow(row, u8"ƴ��");
	wstr = STW_U8(str);
	vitem.iSubItem = 3;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"����");
	wstr = STW_U8(str);
	vitem.iSubItem = 4;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"��������");
	wstr = STW_U8(str);
	vitem.iSubItem = 5;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"ʾ��");
	wstr = STW_U8(str);
	vitem.iSubItem = 6;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);
	
	wstr = STW_U8(reader->getValueInColumnByRow(row, u8"���ƶ�"));
	vitem.iSubItem = 7;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	
	////////////////////////////////////////////////////////////////////////
	//�ʵ�2

	str = reader->getValueInColumnByRow(row, u8"gkb_����");
	wstr = STW_U8(str);
	vitem.iSubItem = 0;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_InsertItem(hDictionaryListView_Two, &vitem);//����insert��set
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_����");
	wstr = STW_U8(str);
	vitem.iSubItem = 1;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_ƴ��");
	wstr = STW_U8(str);
	vitem.iSubItem = 2;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_ͬ��");
	wstr = STW_U8(str);
	vitem.iSubItem = 3;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_����");
	wstr = STW_U8(str);
	vitem.iSubItem = 4;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_����");
	wstr = STW_U8(str);
	vitem.iSubItem = 5;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);
	
	////////////////////////////////////////////////////////////////////////
	wstr = STW_U8(reader->getValueInColumnByRow(row, u8"���ƶ�"));
	SetWindowText(hSimilarityText, (LPWSTR)wstr.c_str());		// ���ƶ�

	str = reader->getValueInColumnByRow(row, u8"ӳ���ϵ");
	wstr = STW_U8(str);
	//wstr = STW_U8(reader->getValueInColumnByRow(row, u8"ӳ���ϵ"));
	SetWindowText(hRelationshipText, (LPWSTR)wstr.c_str());		// ӳ���ϵ
	datas.push_back(str);

	////////////////////////////////////////////////////////////////////////
	if (recorder->findRecord(datas)) {
		wstr = STW_U8(recorder->getNewMappingRelation());
		SetWindowText(hNewRelationshipText, (LPWSTR)wstr.c_str());	//��ӳ���ϵ
	}
}

void SenseAlignmentWindow::resetPartOfSpeech()
{
	//��ȡ��ǰ�б���е�ѡ������
	int ItemIndex = SendMessage(hClassComboBox, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	//��ȡ��ǰ�б���е�ѡ��ֵ	
	TCHAR  part_of_speech[256];
	(TCHAR)SendMessage(hClassComboBox, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)part_of_speech);

	reader->setPartOfSpeech(WTS_U8(part_of_speech));
}

void SenseAlignmentWindow::recordMappingRelation(const std::string & relation)
{
	int row = reader->getCurRowIndex();

	std::vector<std::string> datas;

	datas.push_back(reader->getValueInColumnByRow(row, u8"ID"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"����"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"�������"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"ƴ��"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"����"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"��������"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"ʾ��"));

	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_����"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_����"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_ƴ��"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_ͬ��"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_����"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_����"));
	
	datas.push_back(reader->getValueInColumnByRow(row, u8"ӳ���ϵ"));
	datas.push_back(relation);

	if (recorder->findRecord(datas)) {
		recorder->updateMappingRelation(relation);
	}
	else {
		recorder->insertNewRecord(datas);
	}

	std::wstring wstr = STW_U8(relation);
	SetWindowText(hNewRelationshipText, (LPWSTR)wstr.c_str());
}

LRESULT SenseAlignmentWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		// ����wParam��λ: 
		switch (HIWORD(wParam))
		{
		case CBN_SELCHANGE:
		{
			if (reader->isExistingFile()) {
				resetPartOfSpeech();
				refreshMainWindow();
			}
		}
		break;
		default:
			//��������
			break;
		}

		// ����wParam��λ: 
		switch (LOWORD(wParam))
		{
		case ID_BUTTON_SEARCH:
		{		
			if (reader->isExistingFile()) {			
				search();
			}
		}
		break;
		case ID_BUTTON_PREV_WORD:
		{
			if (reader->isExistingFile()) {
				prev();
			}
		}
		break;
		case ID_BUTTON_NEXT_WORD:
		{
			if (reader->isExistingFile()) {
				next();
			}
		}
		break;

		case ID_BUTTON_EQUAL:
		{
			if (reader->isExistingFile()) {
				recordMappingRelation(u8"����");
			}
		}
		break;
		case ID_BUTTON_NOT_EQUAL:
		{
			if (reader->isExistingFile()) {
				recordMappingRelation(u8"������");
				
			}
		}
		break;
		case ID_BUTTON_BELONG:
		{
			if (reader->isExistingFile()) {
				recordMappingRelation(u8"����");
				
			}
		}
		break;
		case ID_BUTTON_UNSURE:
		{
			if (reader->isExistingFile()) {
				recordMappingRelation(u8"��ȷ��");
			}
		}
		break;

		case ID_MENUITEM_OPEN:
		{
			openFiles();
			if (reader->isExistingFile()) {
				activeControls(true);

				resetPartOfSpeech();
				refreshMainWindow();
			}
		}
		break;
		case ID_MENUITEM_SAVE:
		{
			recorder->save();
			MessageBox(hWnd, L"�ѱ��档", L"��ʾ", MB_OK);
		}
		break;
		case ID_MENUITEM_EXIT: {
			closeWindow();
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	}
	break;
	case WM_SIZE: {
		int nHeight = HIWORD(lParam);
		int nWidth = LOWORD(lParam);

		int heightInc = nHeight - 450;
		int widthInc = nWidth - 800;

		MoveWindow(hDictionaryListView_One, 25, 120, 600 + widthInc, 70, TRUE);
		MoveWindow(hDictionaryListView_Two, 25, 230, 600 + widthInc, 70, TRUE);

		MoveWindow(hRelationEqualButton, 650 + widthInc, 120, 100, 30, TRUE);
		MoveWindow(hRelationNotEqualButton, 650 + widthInc, 170, 100, 30, TRUE);
		MoveWindow(hRelationBelongButton, 650 + widthInc, 220, 100, 30, TRUE);
		MoveWindow(hRelationUnsureButton, 650 + widthInc, 270, 100, 30, TRUE);

		MoveWindow(hSimilarityStaticText, 560 + widthInc, 10, 90, 30, TRUE);
		MoveWindow(hSimilarityText, 650 + widthInc, 10, 100, 30, TRUE);
		MoveWindow(hRelationshipStaticText, 560 + widthInc, 40, 90, 30, TRUE);
		MoveWindow(hRelationshipText, 650 + widthInc, 40, 100, 30, TRUE);
		MoveWindow(hNewRelationshipStaticText, 560 + widthInc, 70, 90, 30, TRUE);
		MoveWindow(hNewRelationshipText, 650 + widthInc, 70, 100, 30, TRUE);

	}
	break;
	case WM_GETMINMAXINFO: {
		MINMAXINFO * pMinMaxInfo = (MINMAXINFO *)lParam;
		pMinMaxInfo->ptMinTrackSize.x = 810;
		pMinMaxInfo->ptMinTrackSize.y = 450;
		pMinMaxInfo->ptMaxTrackSize.x = 1600;
		pMinMaxInfo->ptMaxTrackSize.y = 450;
		return 0;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
		//
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY: {
		recorder->save();

		xWindowModule::postQuitMessage();
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK SenseAlignmentWindow::searchEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			//Do your stuff
			myWindow->search();
			
			break;  //or return 0; if you don't want to pass it further to def proc
					//If not your key, skip to default:
		}
	default:
		return CallWindowProc(SenseAlignmentWindow::oldSearchEditProc, hWnd, msg, wParam, lParam);
	}
	return 0;
}
