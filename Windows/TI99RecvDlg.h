//
// (C) 2007 Mike Brent aka Tursi aka HarmlessLion.com
// This software is provided AS-IS. No warranty
// express or implied is provided.
//
// This notice defines the entire license for this code.
// All rights not explicity granted here are reserved by the
// author.
//
// You may redistribute this software provided the original
// archive is UNCHANGED and a link back to my web page,
// http://harmlesslion.com, is provided as the author's site.
// It is acceptable to link directly to a subpage at harmlesslion.com
// provided that page offers a URL for that purpose
//
// Source code, if available, is provided for educational purposes
// only. You are welcome to read it, learn from it, mock
// it, and hack it up - for your own use only.
//
// Please contact me before distributing derived works or
// ports so that we may work out terms. I don't mind people
// using my code but it's been outright stolen before. In all
// cases the code must maintain credit to the original author(s).
//
// -COMMERCIAL USE- Contact me first. I didn't make
// any money off it - why should you? ;) If you just learned
// something from this, then go ahead. If you just pinched
// a routine or two, let me know, I'll probably just ask
// for credit. If you want to derive a commercial tool
// or use large portions, we need to talk. ;)
//
// If this, itself, is a derived work from someone else's code,
// then their original copyrights and licenses are left intact
// and in full force.
//
// http://harmlesslion.com - visit the web page for contact info
//
// TI99RecvDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_TI99RECVDLG_H__11B7E929_B830_4DC5_9217_E98BB918CB16__INCLUDED_)
#define AFX_TI99RECVDLG_H__11B7E929_B830_4DC5_9217_E98BB918CB16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTI99RecvDlg dialog

class CTI99RecvDlg : public CDialog
{
// Construction
public:
	HANDLE hReadyToWait;
	int nMode;
	int nCount;
	FILE *fp;
	LRESULT OnUser(WPARAM, LPARAM);
	void OpenPort(int nPort);
	int bufidx;
	CCriticalSection csLock;
	HANDLE hBufferReady;
	char buf[1024];
	HANDLE hDieEvent;
	void CommThread();
	CTI99RecvDlg(CWnd* pParent = NULL);	// standard constructor
	bool myWriteFile(IN HANDLE hFile, IN LPCVOID lpBuffer, IN DWORD nNumberOfBytesToWrite, OUT LPDWORD lpNumberOfBytesWritten, IN LPOVERLAPPED lpOverlapped);

// Dialog Data
	//{{AFX_DATA(CTI99RecvDlg)
	enum { IDD = IDD_TI99RECV_DIALOG };
	CStatic	m_Status;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTI99RecvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HANDLE hPort;
	char sec0[256];
	char *pBmp;
	int BmpMask;
	int RecvMode;

	// Generated message map functions
	//{{AFX_MSG(CTI99RecvDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	CComboBox ctrlPort;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TI99RECVDLG_H__11B7E929_B830_4DC5_9217_E98BB918CB16__INCLUDED_)
