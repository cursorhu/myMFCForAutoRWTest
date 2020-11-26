
// myMFCDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "myMFC.h"
#include "myMFCDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <string>
#include <vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

//global data
HANDLE hThread[MAX_THREAD_COUNT];

#define WM_USER_MSG WM_USER + 1001

CCriticalSection criticalSec;

std::vector<CString> g_str_file_list;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CmyMFCDlg 对话框



CmyMFCDlg::CmyMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYMFC_DIALOG, pParent)
	, m_src(_T(""))
	, m_dst(_T(""))
	, rwtime(3)
	, m_str_VendorID(_T("0x1217"))
	, m_SlotID(1)
	, m_str_FW_Path(_T(""))
	, m_str_bin_type(_T("System"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmyMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_src, m_src);
	DDX_Text(pDX, IDC_EDIT_dst, m_dst);
	DDX_Control(pDX, IDC_EDIT_src, ctrl_src);
	DDX_Control(pDX, IDC_EDIT_dst, ctrl_dst);
	DDX_Control(pDX, IDC_LIST_rwlist, m_rwlist);
	DDX_Control(pDX, IDC_EDIT_time, ctrl_rwtime);
	DDX_Text(pDX, IDC_EDIT_time, rwtime);
	DDX_Text(pDX, IDC_EDIT_vid, m_str_VendorID);
	DDX_Text(pDX, IDC_EDIT4, m_str_FW_Path);
	DDX_Control(pDX, IDC_EDIT4, m_ctrl_FW_Path);
	DDX_Control(pDX, IDC_COMBO1, m_Comb_SlotID);
	DDX_Control(pDX, IDC_COMBO2, m_Comb_FWBin);
	DDX_CBString(pDX, IDC_COMBO2, m_str_bin_type);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
}

BEGIN_MESSAGE_MAP(CmyMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_run, &CmyMFCDlg::OnBnClickedButtonrun)
	ON_EN_CHANGE(IDC_EDIT_src, &CmyMFCDlg::OnEnChangeEditsrc)
	ON_EN_CHANGE(IDC_EDIT_dst, &CmyMFCDlg::OnEnChangeEditdst)
	ON_BN_CLICKED(IDC_BUTTON_src, &CmyMFCDlg::OnBnClickedButtonsrc)
	ON_BN_CLICKED(IDC_BUTTON_dst, &CmyMFCDlg::OnBnClickedButtondst)
	ON_BN_CLICKED(IDC_BUTTON_delete, &CmyMFCDlg::OnBnClickedButtondelete)
	ON_BN_CLICKED(IDC_BUTTON_add, &CmyMFCDlg::OnBnClickedButtonadd)
	ON_LBN_SELCHANGE(IDC_LIST_rwlist, &CmyMFCDlg::OnLbnSelchangeListrwlist)
	ON_MESSAGE(WM_USER_MSG, &CmyMFCDlg::OnMsg)
	ON_BN_CLICKED(IDC_BUTTON_fwpath, &CmyMFCDlg::OnBnClickedButtonfwpath)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CmyMFCDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CmyMFCDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CmyMFCDlg 消息处理程序

BOOL CmyMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_Comb_SlotID.AddString(_T("1"));
	m_Comb_SlotID.AddString(_T("2"));
	m_Comb_SlotID.AddString(_T("3"));
	m_Comb_SlotID.SetCurSel(0);
	m_SlotID = m_Comb_SlotID.GetCurSel() + 1;

	m_Comb_FWBin.AddString(_T("System"));
	m_Comb_FWBin.AddString(_T("Bootloader+Sys"));
	m_Comb_FWBin.AddString(_T("Bootloader"));
	m_Comb_FWBin.SetCurSel(0);

	if (0 == m_Comb_SlotID.GetCurSel() ||
		1 == m_Comb_SlotID.GetCurSel())
	{
		m_Comb_FWBin.EnableWindow(FALSE);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CmyMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmyMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmyMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CmyMFCDlg::OnEnChangeEditsrc()
{
	GetDlgItemText(IDC_EDIT_src, m_src);
}


void CmyMFCDlg::OnEnChangeEditdst()
{
	GetDlgItemText(IDC_EDIT_dst, m_dst);
}


CString CmyMFCDlg::GetFolderPath(void)
{
	CString strPath;
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	bInfo.lpszTitle = _T("Select Folder: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST lpDlist;					
	lpDlist = SHBrowseForFolder(&bInfo);	
	if (lpDlist != NULL)						
	{
		TCHAR chPath[255];					
		SHGetPathFromIDList(lpDlist, chPath);
		strPath = chPath;					
	}
	return strPath;
}

CString CmyMFCDlg::GetFilePath(void)
{
	CFileDialog mFileDlg(TRUE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR,
		_T("All Files(*.*)|*.*||"), AfxGetMainWnd());
	CString str(" ", 10000);
	mFileDlg.m_ofn.lpstrFile = str.GetBuffer(10000);
	mFileDlg.m_ofn.lpstrTitle = _T("Select File");
	str.ReleaseBuffer();
	mFileDlg.DoModal();
	POSITION mPos = mFileDlg.GetStartPosition();
	CFileStatus status;
	CString strPath;
	while (mPos != NULL)
	{
		strPath = mFileDlg.GetNextPathName(mPos);
		CFile::GetStatus(strPath, status);
	}
	return strPath;
}


void CmyMFCDlg::OnBnClickedButtonsrc()
{
	/*
	CString folderPath;
	folderPath = GetFolderPath();
	ctrl_src.SetWindowText(_T(""));
	ctrl_src.ReplaceSel(folderPath); //覆盖更新m_src成员， ctrl_src是src输入框的控件成员
	*/

	CString SrcPath;
	UINT ThreadCSelNum = m_rwlist.GetCurSel();
	SrcPath = GetFolderPath();
	ctrl_src.SetWindowText(SrcPath);
	RWTestParamArray[ThreadCSelNum].SRCFolder_Path = SrcPath;
}


void CmyMFCDlg::OnBnClickedButtondst()
{
	/*
	CString folderPath;
	folderPath = GetFolderPath();
	ctrl_dst.SetWindowText(_T(""));
	ctrl_dst.ReplaceSel(folderPath); //覆盖更新m_dst成员
	*/

	CString DstPath;
	UINT ThreadCSelNum = m_rwlist.GetCurSel();
	DstPath = GetFolderPath();
	ctrl_dst.SetWindowText(DstPath);
	RWTestParamArray[ThreadCSelNum].DSTFolder_Path = DstPath;
}

void CmyMFCDlg::OnBnClickedButtonadd()
{
	CString Threadtest = _T("TestThread");
	UINT ThreadCount = m_rwlist.GetCount();
	if (ThreadCount == 0)
	{
		m_rwlist.AddString(_T("TestThread1"));
	}
	else if (ThreadCount < MAX_THREAD_COUNT)
	{
		CString ThreadNum;
		ThreadNum.Format(_T("%d"), ThreadCount + 1);
		Threadtest = Threadtest + ThreadNum;
		m_rwlist.AddString(Threadtest);
	}
	else if (ThreadCount == MAX_THREAD_COUNT)
	{
		CString str;
		str.Format(_T("Only support %d threads at most!!"), MAX_THREAD_COUNT);
		MessageBox(str);
	}
	m_rwlist.SetCurSel(ThreadCount);
	if (ThreadCount < MAX_THREAD_COUNT)
		totalThreadCount++;
}


void CmyMFCDlg::OnBnClickedButtondelete()
{
	UINT ThreadCount = m_rwlist.GetCount();
	if (ThreadCount != 0)
	{
		m_rwlist.DeleteString(ThreadCount - 1);
		m_rwlist.SetCurSel(0);
	}
	if (ThreadCount > 0)
		totalThreadCount--;
}


void CmyMFCDlg::OnLbnSelchangeListrwlist()
{
	UpdateData(true); //update true: 从界面读入值到变量（使上次编辑生效）
	if (m_rwlist.GetCount() != 0)
	{
		UINT selectNum = m_rwlist.GetCurSel();
		RWTestParamArray[selectNum].ThreadNum = m_rwlist.GetCount();
		RefreshRWParam(RWTestParamArray, selectNum);

		//RWTestParamArray[selectNum].TestTimes = rwtime;
	}
}

void CmyMFCDlg::RefreshRWParam(TabDialogRWTestParam(&Array)[MAX_THREAD_COUNT], UINT CSel)
{
	ctrl_src.SetWindowText(Array[CSel].SRCFolder_Path);
	ctrl_dst.SetWindowText(Array[CSel].DSTFolder_Path);
	
	//CString str;
	//str.Format(_T("%d"), rwtime);
	//ctrl_rwtime.SetWindowText(str);
	UpdateData(false); //update false: 把变量写入到界面（实时显示）
}



unsigned int WINAPI DoThreadProc(void *threadTransParam)
{
	pTransParam pTrans = (pTransParam)threadTransParam;
	CString strResult;
	BOOL res = 0;

	CmyMFCDlg* pDlg = (CmyMFCDlg *)pTrans->translpParam;
	int thread_id = pTrans->i;


	for (int i = 0; i < (int)pDlg->rwtime; i++)
	{
		
		res = pDlg->ModeTestCopyFileFromSRCtoDST(pDlg->RWTestParamArray[thread_id].SRCFolder_Path, pDlg->RWTestParamArray[thread_id].DSTFolder_Path, strResult);
		if (!res)
		{
			//MessageBox(NULL, _T("copy file fail!"), _T("ERROR"), MB_OK);
			//strlog.Format(_T("CopyFileFromSRCtoDST fail: src: %s, dst: %s, test loop: %d \n"), pDlg->RWTestParamArray[thread_id].SRCFolder_Path, pDlg->RWTestParamArray[thread_id].DSTFolder_Path, pDlg->rwtime);
			//pDlg->ShowLogInEditBox(strlog);
		}
			

		res = pDlg->ModeTestCompareFilesBetweenSRCandDST(pDlg->RWTestParamArray[thread_id].SRCFolder_Path, pDlg->RWTestParamArray[thread_id].DSTFolder_Path, strResult);
		if (!res)
		{
			//MessageBox(NULL, _T("compare file mismatch!"), _T("ERROR"), MB_OK);
			//strlog.Format(_T("CompareFilesBetweenSRCandDST fail: src: %s, dst: %s, test loop: %d \n"), pDlg->RWTestParamArray[thread_id].SRCFolder_Path, pDlg->RWTestParamArray[thread_id].DSTFolder_Path, pDlg->rwtime);
			//pDlg->ShowLogInEditBox(strlog);
		}
		else
		{
			//MessageBox(NULL, _T("copy & compare file pass!"), _T("INFO"), MB_OK);
			//strlog.Format(_T("Thread %d Copy&Compare Pass: test loop: %d \n"), thread_id, pDlg->rwtime);
			//pDlg->ShowLogInEditBox(strlog);

			criticalSec.Lock();
			//pDlg->strAppend.Format(_T("Thread %d Copy&Compare Pass: test loop: %d \n"), thread_id+1, i+1);
			::PostMessage(pDlg->GetSafeHwnd(), WM_USER_MSG, WPARAM(thread_id + 1), LPARAM(i + 1));
			criticalSec.Unlock();
		}
			
	}

	return res;
}


void CmyMFCDlg::OnBnClickedButtonrun()
{
	//m_ProgressCtrl.SetRange32(0, 1000);//设置进度条的最小值和最大值

	//int nFirstStep = m_ProgressCtrl.SetStep(10);//设置步长，返回原来步长，默认是最大值的1%


	/*
	for (int x = 1; x <= 10; x++)

	{

		m_ProgressCtrl.SetPos(x * 10);//设置一个位置

		int a = m_ProgressCtrl.StepIt();//在原位置上增加一个步长距离，配合SetStep使用，先设置步长才能使用这个

		Sleep(100);

	}

	m_ProgressCtrl.OffsetPos(1000);//在原位置上增加400距离

	if (m_ProgressCtrl.GetPos() > 1000)//获取当前位置

	{

		//m_ProgressCtrl.SetPos(0);

	}*/
	//读入所有界面数据
	UpdateData(true);

	if ("" == m_str_FW_Path ||
		m_SlotID == 0 ||
		"" == m_str_VendorID)
	{
		MessageBox(_T("Please check VendorID , Slot ID and FW path!"), _T("INFO"), MB_OK);
		return;
	}
	
	m_ProgressCtrl.SetRange(0, 100);
	m_ProgressCtrl.SetStep(5);
	((CButton*)GetDlgItem(IDC_BUTTON_run))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_fwpath))->EnableWindow(FALSE);
	((CEdit*)GetDlgItem(IDC_EDIT4))->EnableWindow(FALSE);

	

	RecursiveFindFile(m_str_FW_Path);
	
	for (size_t i = 0; i < g_str_file_list.size(); i++)
	{
		m_ProgressCtrl.SetPos(0);
		if (DoUpdateFirmware(g_str_file_list[i]))
		{
			m_ProgressCtrl.OffsetPos(10);
			RunModeTestInstance();
		}
		else
		{
			strAppend.Format(_T("Update the %s to %d failed!!!"), g_str_file_list[i].GetBuffer(0), m_SlotID);
			ShowLogInEditBox();
			MessageBox(strAppend, _T("ERROR"), MB_OK);
			break;
		}
	}
	CloseLogFile();
	((CButton*)GetDlgItem(IDC_BUTTON_run))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_fwpath))->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT4))->EnableWindow(TRUE);

}

void CmyMFCDlg::RunModeTestInstance()
{
	CString strlog;
	strlog = _T("**************  Test Start! **************");
	this->ShowLogInEditBox(strlog);

	//线程内除了对象，还需要知道自己是哪个线程，因此打包this和ThreadCount
	pTransParam ThreadTransPArray[MAX_THREAD_COUNT];

	for (int i = 0; i < totalThreadCount; i++)
	{
		//m_src = RWTestParamArray[i].SRCFolder_Path;
		//m_dst = RWTestParamArray[i].DSTFolder_Path;

		ThreadTransPArray[i] = new(TransParam);
		ThreadTransPArray[i]->i = i;
		ThreadTransPArray[i]->translpParam = this;

		//CreateThread(ThreadTransPArray[i]);

		unsigned int rwThreadID;

		//hThread defined as global data
		hThread[i] = (HANDLE)_beginthreadex(
			NULL,
			0,
			DoThreadProc,
			ThreadTransPArray[i],
			0,
			&rwThreadID);

		if (hThread[i] == NULL)
			MessageBox(_T("CreateThread Fail!!"), MB_OK);
	}

	//wait all child threads return

	/*
	//主线程阻塞，不能处理消息
	DWORD dwWaitResult = WaitForMultipleObjects(
		totalThreadCount,
		hThread,
		TRUE,
		INFINITE);
	*/
	//主线程阻塞，但不阻塞消息
	int nWaitCount = totalThreadCount;
	int nExitThreadCount = 0;      //标记已经有几个线程退出了
	BOOL bWaitAll = FALSE;		//不等待所有线程完成，实时处理。如果TRUE, 会阻塞到所有线程完成
	DWORD result;
	MSG msg;

	while (TRUE)
	{
		/*该函数等待：多个线程的完成信号，或其他消息信号,有任意一种就返回
		*返回值为[WAIT_OBJECT_0, WAIT_OBJECT_0 + nWaitCount - 1]表示对应下标的线程已完成
		*返回值为WAIT_OBJECT_0 + nWaitCount表示有其他信号，如线程内发送的message
		*WAIT_OBJECT_0值为0
		*/
		result = MsgWaitForMultipleObjects(nWaitCount, hThread, bWaitAll, INFINITE, QS_ALLINPUT);

		if (result == WAIT_OBJECT_0 + nWaitCount) //表示收到消息
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //处理所有已入队的消息
			{
				TranslateMessage(&msg); //message translat and format, add into message queue
				DispatchMessage(&msg); //call message handler
			}
		}
		else if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + nWaitCount) //表示收到了线程结束信号
		{
			nExitThreadCount++;
			if (nExitThreadCount < totalThreadCount)
			{
				/*必须更新hThread，否则已退出的线程一直被检测到*/
				int nIndex = result - WAIT_OBJECT_0; //退出线程的index
				hThread[nIndex] = hThread[nWaitCount - 1]; //更新等待列表：hThread, 交换退出的成员和尾部成员
				hThread[nWaitCount - 1] = NULL;

				nWaitCount--; //更新要等待的线程数
				float fpos = (float)(((float)nExitThreadCount / (float)totalThreadCount)) * 100;
				m_ProgressCtrl.SetPos((int)fpos);
			}
			else
			{
				m_ProgressCtrl.SetPos(100);
				break; //等待的所有线程都已完成
			}

		}
	}

	//All threads returned

	strlog.Format(_T("All RW thread Pass: Total ThreadCount: %d, each thread RW times: %d "), totalThreadCount, this->rwtime);
	this->ShowLogInEditBox(strlog);

	strlog = _T("**************  Test finish! **************");
	this->ShowLogInEditBox(strlog);

	//MessageBox(_T("All copy & compare task finished!"), _T("INFO"), MB_OK);

	//release resource
	for (int i = 0; i < totalThreadCount; i++)
	{
		delete ThreadTransPArray[i];
		ThreadTransPArray[i] = NULL;
		CloseHandle(hThread[i]);
	}

	CloseLogFile();
}


LRESULT CmyMFCDlg::OnMsg(WPARAM wp, LPARAM lp)
{
	strAppend.Format(_T("Thread %d src:%s ---> des:%s, Copy&Compare Pass: test loop: %d \n"), wp, RWTestParamArray[wp-1].SRCFolder_Path, RWTestParamArray[wp-1].DSTFolder_Path, lp);
	ShowLogInEditBox();
	m_ProgressCtrl.StepIt();
	return 0;
}

/* call by message handler, for multiple child thread*/
void CmyMFCDlg::ShowLogInEditBox()
{
	CString str;
	UINT i;

	/*message 队列只在主线程内处理，无需加锁*/
	//criticalSec.Lock();
	//if (NoLog == FALSE)
		WriteLogFile(this->strAppend); //only write append str
	//criticalSec.Unlock();

	this->GetDlgItemText(IDC_EDIT_logbox, str);
	str += this->strAppend; //update old+append str
	str += "\r\n";

	this->SetDlgItemText(IDC_EDIT_logbox, str);

	i = ((CEdit*)GetDlgItem(IDC_EDIT_logbox))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT_logbox))->LineScroll(++i, 0);

}

/* call by main thread*/
void CmyMFCDlg::ShowLogInEditBox(CString strAppend)
{
	CString str;
	UINT i;

	//if (NoLog == FALSE)
		WriteLogFile(strAppend); //only write append str

	this->GetDlgItemText(IDC_EDIT_logbox, str);
	str += strAppend; //update old+append str
	str += "\r\n";

	this->SetDlgItemText(IDC_EDIT_logbox, str);

	i = ((CEdit*)GetDlgItem(IDC_EDIT_logbox))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT_logbox))->LineScroll(++i, 0);

}


BOOL CmyMFCDlg::CreateLogFile()
{
	CString strName;
	SYSTEMTIME st;

	GetLocalTime(&st);
	strName.Format(_T("UtilityLogFile_%4d-%d-%d_%d-%d-%d.log"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	if (!m_File.Open(strName, (CFile::modeCreate | CFile::modeReadWrite), 0))
	{
		::AfxMessageBox(_T("Create Utility Log File Error!!"));
		return FALSE;
	}

	m_logCreated = 1;
	return TRUE;
}

void CmyMFCDlg::WriteLogFile(CString str)
{
	BOOL CreateRes = TRUE;

	if (m_logCreated == 0)
		CreateRes = CreateLogFile();

	if (CreateRes)
	{
		str += _T("\r\n");
		int length = str.GetLength();
		length *= 2;
		m_File.Write(str, length);
		m_File.Flush();
	}
}


void CmyMFCDlg::CloseLogFile()
{
	if (m_logCreated == 1)
	{
		m_File.Close();
		m_logCreated = 0;
	}
		
}

/* copied from auto test tool */

BOOL CmyMFCDlg::MakeDirectory(CString dd)
{
	HANDLE fFile;
	WIN32_FIND_DATA fileinfo;
	CStringArray m_arr;
	BOOL tt;
	int x1 = 0;
	CString tem;


	fFile = FindFirstFile(dd, &fileinfo);

	if (fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		FindClose(fFile);
		return   FALSE;
	}

	m_arr.RemoveAll();

	for (x1 = 0; x1 < dd.GetLength(); x1++)
	{
		if (dd.GetAt(x1) != _T('\\'))
			tem += dd.GetAt(x1);
		else
		{
			m_arr.Add(tem);
			tem += "\\";
		}
		if (x1 == dd.GetLength() - 1)
			m_arr.Add(tem);
	}

	FindClose(fFile);

	for (x1 = 1; x1 < m_arr.GetSize(); x1++)
	{
		tem = m_arr.GetAt(x1);
		tt = CreateDirectory(tem, NULL);
		if (tt) SetFileAttributes(tem, FILE_ATTRIBUTE_NORMAL);

	}

	fFile = FindFirstFile(dd, &fileinfo);

	m_arr.RemoveAll();
	if (fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		FindClose(fFile);
		return FALSE;
	}
}


BOOL CmyMFCDlg::ModeTestCopyFileFromSRCtoDST(CString SRC, CString DST, CString& StrResult)
{

	CFileFind ff, ff_DST;
	CString SRCDir = SRC;                 //source folder path
	CString DSTDir = DST;
	UINT copyFileResult = 0;
	int i = 0;

	BOOL bmakedir = MakeDirectory(DSTDir);

	if (SRCDir.Right(1) != _T("\\"))
		SRCDir += _T("\\");
	SRCDir += _T("*.*");

	if (DSTDir.Right(1) != _T("\\"))
		DSTDir += _T("\\");


	SetLastError(0);
	CString DST_tmp = DSTDir + _T("*.*");
	BOOL res_DST = ff_DST.FindFile(DST_tmp);
	if (res_DST == 0)
	{
		StrResult.Format(_T("Access DST folder error, error code is %d. "), GetLastError());
	}
	BOOL res = ff.FindFile(SRCDir);

	while (res)
	{
		res = ff.FindNextFile();
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			CString DSTFildPath;
			CString SRCFilePath = ff.GetFilePath();
			DSTFildPath = DSTDir + ff.GetFileName();
			copyFileResult = CopyFile(ff.GetFilePath(), DSTFildPath, FALSE);

			Sleep(2000);

			if (copyFileResult == 0)
			{
				DWORD ErrCode = GetLastError();
				StrResult.Format(_T("CopyFile failed! The ErrCode is %d. "), ErrCode);

				for (i = 0; i < 10; i++)
				{
					copyFileResult = CopyFile(ff.GetFilePath(), DSTFildPath, FALSE);
					Sleep(2000);
					if (copyFileResult == 0)
					{
						ErrCode = GetLastError();
						StrResult.Format(_T("Retry CopyFile failed! The ErrCode is %d. "), ErrCode);
					}
					else
					{
						break;
					}
				}

				if (copyFileResult == 0)
				{
					ff.Close();
					return FALSE;
				}
			}
		}
		else if (ff.IsDirectory() && !ff.IsDots())
		{
			CString DSTFildPath;
			DSTFildPath = DSTDir + ff.GetFileName();
			copyFileResult = ModeTestCopyFileFromSRCtoDST(ff.GetFilePath(), DSTFildPath, StrResult);
			if (copyFileResult == 0)
				break;
		}
	}

	ff.Close();
	if (copyFileResult == 0)
		return FALSE;
	else
		return TRUE;

}



BOOL CmyMFCDlg::ModeTestCompareFilesBetweenSRCandDST(CString SRC, CString DST, CString& StrResult)
{
	CFileFind ff;
	CString SRCDir = SRC;
	CString DSTDir = DST;
	BOOL bRes = TRUE;
	HANDLE hSrcFile, hDstFile;
	DWORD dwSRCFile, dwDSTFile, dwCB;

	if (SRCDir.Right(1) != _T("\\"))
		SRCDir += _T("\\");
	SRCDir += _T("*.*");

	if (DSTDir.Right(1) != _T("\\"))
		DSTDir += _T("\\");
	hSrcFile = hDstFile = NULL;
	BYTE* pSrcBuffer = new BYTE[M_BUFSIZE];
	BYTE* pDstBuffer = new BYTE[M_BUFSIZE];
	memset(pSrcBuffer, 0, M_BUFSIZE);
	memset(pSrcBuffer, 0, M_BUFSIZE);

	BOOL res = ff.FindFile(SRCDir);

	while (res)
	{
		res = ff.FindNextFile();
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			CString DSTFilePath;
			DSTFilePath = DSTDir + ff.GetFileName();
			CString SRCFilePath = ff.GetFilePath();

			if (hSrcFile)
			{
				CloseHandle(hSrcFile);
				hSrcFile = NULL;
			}

			if (hDstFile)
			{
				CloseHandle(hDstFile);
				hSrcFile = NULL;
			}

			hSrcFile = CreateFile(SRCFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hSrcFile == INVALID_HANDLE_VALUE)
			{
				StrResult.Format(_T("\n Create Source file failed!! Error code = %d \n"), GetLastError());
				bRes = FALSE;
				break;
			}

			hDstFile = CreateFile(DSTFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hDstFile == INVALID_HANDLE_VALUE)
			{
				StrResult.Format(_T("\n Create Destination file failed!! Error code = %d \n"), GetLastError());
				bRes = FALSE;
				break;
			}

			LARGE_INTEGER SrcFileSize, DstFileSize;

			dwSRCFile = GetFileSizeEx(hSrcFile, &SrcFileSize);
			dwDSTFile = GetFileSizeEx(hDstFile, &DstFileSize);

			if (SrcFileSize.LowPart != DstFileSize.LowPart)
			{
				StrResult.Format(_T("\n Compare file is different!! Src Length = %d, Dest Length = %d \n"), SrcFileSize.LowPart, DstFileSize.LowPart);
				bRes = FALSE;
				break;
			}

			while (SrcFileSize.LowPart > 0)
			{
				BOOL bCmpResult;
				bCmpResult = ReadFile(hSrcFile, pSrcBuffer, M_BUFSIZE, &dwCB, NULL);
				if (bCmpResult == 0)
				{
					bRes = FALSE;
					break;
				}
				bCmpResult = ReadFile(hDstFile, pDstBuffer, M_BUFSIZE, &dwCB, NULL);
				if (bCmpResult == 0)
				{
					bRes = FALSE;
					break;
				}
				bCmpResult = memcmp(pSrcBuffer, pDstBuffer, dwCB);

				if (bCmpResult != 0)
				{
					bRes = FALSE;
					CString DiffByte;

					CString PostCmpErrorStr;
					CString SrcDumpData, DstDumpData;
					StrResult.Format(_T("\n Fatal_Error: Src Data from %d to %d.\n"), (DstFileSize.LowPart - SrcFileSize.LowPart), (DstFileSize.LowPart - SrcFileSize.LowPart + dwCB));

					PostCmpErrorStr = _T("SourceFilePath: ") + SRCFilePath + _T(" To \r\n") + _T("DstFilePath: ") + DSTFilePath;
					StrResult = PostCmpErrorStr + _T("  has compare error! \r\n");
					//HugoPostMessageAndShowSD1(PostCmpErrorStr,1);
					//HugoPostMessageAndShowSD2(PostCmpErrorStr,1);

					::MessageBox(
						NULL,
						(LPCWSTR)L"Compare error happened!!",
						(LPCWSTR)L"Fatal Error!",
						MB_OK
					);

					break;
				}
				SrcFileSize.LowPart -= dwCB;
			}

			if (bRes == FALSE)
				break;
			else
				ReadFile(hDstFile, pDstBuffer, 512, &dwCB, NULL);
		}
		else if (ff.IsDirectory() && !ff.IsDots())
		{
			CString DSTFildPath;
			DSTFildPath = DSTDir + ff.GetFileName();
			bRes = ModeTestCompareFilesBetweenSRCandDST(ff.GetFilePath(), DSTFildPath, StrResult);
			if (bRes == FALSE)
				break;
		}
	}

	if (hSrcFile)
	{
		CloseHandle(hSrcFile);
		hSrcFile = NULL;
	}

	if (hDstFile)
	{
		CloseHandle(hDstFile);
		hSrcFile = NULL;
	}

	if (bRes == FALSE)
	{
		delete[]pSrcBuffer;
		delete[]pDstBuffer;
		ff.Close();
		return FALSE;
	}
	else
	{
		delete[]pSrcBuffer;
		delete[]pDstBuffer;
		ff.Close();
		return TRUE;
	}
}



/* End of: copied from auto test tool */

BOOL CmyMFCDlg::DoUpdateFirmware(CString filename)
{
	//char strImage[MAX_PATH] = "FirwmareUpdateTool1.0.8.exe 0x1217 d:\\fw_update\\fw_sys_checksumed_9760.bin 1";
	//TCHAR szImageFile[MAX_PATH] = { 0 };
	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;

	CString strToolPath(szFilePath);
	strToolPath = strToolPath + _T("FirwmareUpdateTool.exe");
	CString strPath;
	strPath.Format(_T("%s %s %s %d"), strToolPath.GetBuffer(0), m_str_VendorID.GetBuffer(0), filename.GetBuffer(0), m_SlotID);
	
	strAppend = strPath;
	ShowLogInEditBox();

	if (!PathFileExists(strToolPath))
	{
		strAppend.Format(_T("The %s is not exist!"), strToolPath.GetBuffer(0));
		ShowLogInEditBox();
		MessageBox(strAppend, MB_OK);
		return FALSE;
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };//在产生子进程时，子进程的窗口相关信息
	PROCESS_INFORMATION pi;                  //子进程的ID/线程相关信息
	//memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	DWORD returnCode = -1;//用于保存子程进的返回值;

	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	BOOL bRet = CreateProcess(              //调用失败，返回0；调用成功返回非0；
		NULL,                               //一般都是空；（另一种批处理情况：此参数指定"cmd.exe",下一个命令行参数 "/c otherBatFile")
		strPath.GetBuffer(0),              //命令行参数         
		NULL,                               //_In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL,                               //_In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
		FALSE,                              //_In_        BOOL                  bInheritHandles,
		CREATE_NEW_CONSOLE,                 //新的进程使用新的窗口。
		NULL,                               //_In_opt_    LPVOID                lpEnvironment,
		NULL,                               //_In_opt_    LPCTSTR               lpCurrentDirectory,
		&si,                                //_In_        LPSTARTUPINFO         lpStartupInfo,
		&pi);                               //_Out_       LPPROCESS_INFORMATION lpProcessInformation

	if (bRet)
	{
		while (TRUE)
		{
			DWORD result;
			MSG msg;
			result = MsgWaitForMultipleObjects(1, &pi.hProcess, FALSE, INFINITE, QS_ALLINPUT);
			if (result == (WAIT_OBJECT_0))
			{
				//获取子进程的返回值
				GetExitCodeProcess(pi.hProcess, &returnCode);
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				break;
			}
			else
			{
				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
				DispatchMessage(&msg);
			}
		}
		strAppend.Format(_T("%s returnCode : %d "), strToolPath.GetBuffer(0), returnCode);
		ShowLogInEditBox();
	}
	else
	{
		strAppend.Format(_T("Start the %s failed!"), strToolPath.GetBuffer(0));
		ShowLogInEditBox();
		MessageBox(strAppend, MB_OK);
	}

	if (!returnCode)
	{
		return TRUE;
	}
	return FALSE;
}

void CmyMFCDlg::RecursiveFindFile(CString strPath)
{
	CFileFind ff;
	BOOL bFind = ff.FindFile(strPath + "\\*.*");
	CString strFilter;
	if (_T("Bootloader") == m_str_bin_type)
	{
		strFilter = _T("boot_flash");
	}
	else if (_T("Bootloader+Sys") == m_str_bin_type)
	{
		strFilter = _T("bootFwSys_");
	}
	else
	{
		strFilter = _T("fw_sys_");
	}
	
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (ff.IsDots() || ff.IsSystem() || ff.IsHidden())
			continue;

		if (ff.IsDirectory())
		{
			RecursiveFindFile(ff.GetFilePath());
		}
		else
		{
			CString strFileName = ff.GetFilePath();
			if ((-1 != strFileName.Find(_T(".bin"))) &&
				(-1 != strFileName.Find(strFilter)))
			{
				g_str_file_list.push_back(strFileName);
				strAppend = strFileName;
				ShowLogInEditBox();
			}
		}
	}
}


void CmyMFCDlg::OnBnClickedButtonfwpath()
{
	// TODO: 在此添加控件通知处理程序代码
	m_str_FW_Path = GetFolderPath();
	m_ctrl_FW_Path.SetWindowText(m_str_FW_Path);
}



void CmyMFCDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码

	if (0 == m_Comb_SlotID.GetCurSel() ||
		1 == m_Comb_SlotID.GetCurSel())
	{
		m_Comb_FWBin.SetCurSel(0);
		m_Comb_FWBin.EnableWindow(FALSE);
	}
	else
	{
		m_Comb_FWBin.SetCurSel(0);
		m_Comb_FWBin.EnableWindow(TRUE);
	}
	m_SlotID = m_Comb_SlotID.GetCurSel() + 1;
	CString msg;
	msg.Format(_T("m_SlotID : %d"), m_SlotID);
	MessageBox(msg, MB_OK);
}


void CmyMFCDlg::OnCbnSelchangeCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString msg;
	int select = m_Comb_FWBin.GetCurSel();                  //m_drive是combox控件的变量 
	m_Comb_FWBin.GetLBText(select, msg);
	MessageBox(msg, MB_OK);
}
