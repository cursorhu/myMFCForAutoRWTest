
// myMFCDlg.h: 头文件
//
#pragma once


typedef struct TabRWParam
{
	CString SRCFolder_Path;
	CString DSTFolder_Path;
	UINT ThreadNum;
	UINT TestTimes;
}TabDialogRWTestParam;

typedef struct transParam
{
	LPVOID translpParam;
	int i;
}TransParam, *pTransParam;

#define MAX_THREAD_COUNT 6


// CmyMFCDlg 对话框
class CmyMFCDlg : public CDialogEx
{
// 构造
public:
	CmyMFCDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:


	CString m_src;
	CString m_dst;
	afx_msg void OnBnClickedButtonrun();
	afx_msg void OnEnChangeEditsrc();
	afx_msg void OnEnChangeEditdst();
	BOOL CmyMFCDlg::MakeDirectory(CString dd);
	BOOL CmyMFCDlg::ModeTestCopyFileFromSRCtoDST(CString SRC, CString DST, CString& StrResult);
	BOOL CmyMFCDlg::ModeTestCompareFilesBetweenSRCandDST(CString SRC, CString DST, CString& StrResult);
	afx_msg void OnBnClickedButtonsrc();
	afx_msg void OnBnClickedButtondst();
	CEdit ctrl_src;
	CEdit ctrl_dst;
	afx_msg void OnBnClickedButtondelete();
	afx_msg void OnBnClickedButtonadd();
	CListBox m_rwlist;
	afx_msg void OnLbnSelchangeListrwlist();
	TabDialogRWTestParam RWTestParamArray[MAX_THREAD_COUNT];
	int totalThreadCount = 0;
	void CmyMFCDlg::RefreshRWParam(TabDialogRWTestParam(&Array)[MAX_THREAD_COUNT], UINT CSel);
	CEdit ctrl_rwtime;
	DWORD rwtime;
	CString CmyMFCDlg::GetFolderPath(void);
	CString CmyMFCDlg::GetFilePath(void);
	void CmyMFCDlg::MsgWaitThreadExit(int thread_count, HANDLE(&hThreadArray)[]);
	void CmyMFCDlg::ShowLogInEditBox(CString str);
	void CmyMFCDlg::ShowLogInEditBox();
	BOOL CmyMFCDlg::CreateLogFile();
	void CmyMFCDlg::WriteLogFile(CString str);
	void CmyMFCDlg::CloseLogFile();
	CFile m_File;
	byte m_logCreated;
	CString strAppend;
	LRESULT CmyMFCDlg::OnMsg(WPARAM wp, LPARAM lp);
	void CmyMFCDlg::RunModeTestInstance();
	afx_msg void OnBnClickedButtonfwpath();
	BOOL DoUpdateFirmware(CString filename);
	void RecursiveFindFile(CString strPath);
	CString m_str_VendorID;
	CString m_str_FW_Path;
	CEdit m_ctrl_FW_Path;
	
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo2();
	CComboBox m_Comb_SlotID;
	CComboBox m_Comb_FWBin;
	
	CString m_str_bin_type;
	int m_SlotID;
	CProgressCtrl m_ProgressCtrl;
};

#define M_BUFSIZE 1024*1024