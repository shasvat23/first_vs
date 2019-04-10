
// GuiDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CGuiDlg dialog
class CGuiDlg : public CDialogEx
{
// Construction
public:
	CGuiDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
private:
	void beginConnectionToTcm(void);
	

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit1;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	afx_msg void OnEnChangeEdit1();

	void dispatchMsgFromTcm(char *buf, size_t sz);
};
