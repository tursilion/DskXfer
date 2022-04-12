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
// TI99RecvDlg.cpp : implementation file
//
// For some reason, this program crashes when using the release MFC library

#include "stdafx.h"
#include <process.h>
#include "TI99Recv.h"
#include "TI99RecvDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CommWrapper(void *pDlg) {
	((CTI99RecvDlg*)pDlg)->CommThread();
}

bool CTI99RecvDlg::myWriteFile(IN HANDLE hFile, IN LPCVOID lpBuffer, IN DWORD nNumberOfBytesToWrite, OUT LPDWORD lpNumberOfBytesWritten, IN LPOVERLAPPED lpOverlapped) {
	DWORD nCnt;
	char *pBuf=(char*)lpBuffer;

	*lpNumberOfBytesWritten=0;
	while (nNumberOfBytesToWrite) {
		if (!WriteFile(hFile, pBuf, 1, &nCnt, NULL)) {
			return false;
		}
		pBuf++;
		*lpNumberOfBytesWritten=(*lpNumberOfBytesWritten)+1;
		nNumberOfBytesToWrite--;
		Sleep(10);
	}
	SetEvent(hReadyToWait);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTI99RecvDlg dialog

CTI99RecvDlg::CTI99RecvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTI99RecvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTI99RecvDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTI99RecvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTI99RecvDlg)
	DDX_Control(pDX, IDC_STATUS, m_Status);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO1, ctrlPort);
}

BEGIN_MESSAGE_MAP(CTI99RecvDlg, CDialog)
	//{{AFX_MSG_MAP(CTI99RecvDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_CLOSE()
	ON_WM_CANCELMODE()
	ON_MESSAGE(WM_USER, OnUser)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON2, &CTI99RecvDlg::OnBnClickedButton2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTI99RecvDlg message handlers

void CTI99RecvDlg::OpenPort(int nPort) {
	CString csCom;
	bool bFirst=false;

	if (NULL != hPort) {
		CloseHandle(hPort);
		hPort=NULL;
	}
	if (nPort < 0) {
		nPort*=-1;
		bFirst=true;
	}

	csCom.Format("COM%d:", nPort);

	// open the serial port
	hPort=CreateFile(csCom, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (NULL == hPort) {
		SetWindowText("COM Port is not open");
		if (!bFirst) {
			csCom.Format("Failed to open COM%d:, %d.", nPort, GetLastError());
			AfxMessageBox(csCom);
		}
		return;
	}

	// Configure the serial port for 2400, 8N1
	DCB PortDCB;

	// Initialize the DCBlength member. 
	PortDCB.DCBlength = sizeof (DCB); 

	// Get the default port setting information.
	GetCommState (hPort, &PortDCB);

	// Change the DCB structure settings.
	PortDCB.BaudRate = CBR_2400;          // Current baud 
	PortDCB.fBinary = TRUE;               // Binary mode; no EOF check 
	PortDCB.fParity = FALSE;              // Enable parity checking 
	PortDCB.fOutxCtsFlow = FALSE;         // No CTS output flow control 
	PortDCB.fOutxDsrFlow = FALSE;         // No DSR output flow control 
	PortDCB.fDtrControl = DTR_CONTROL_ENABLE; 
										  // DTR flow control type 
	PortDCB.fDsrSensitivity = FALSE;      // DSR sensitivity 
	PortDCB.fTXContinueOnXoff = TRUE;     // XOFF continues Tx 
	PortDCB.fOutX = FALSE;                // No XON/XOFF out flow control 
	PortDCB.fInX = FALSE;                 // No XON/XOFF in flow control 
	PortDCB.fErrorChar = FALSE;           // Disable error replacement 
	PortDCB.fNull = FALSE;                // Disable null stripping 
	PortDCB.fRtsControl = RTS_CONTROL_ENABLE; 
										  // RTS flow control 
	PortDCB.fAbortOnError = FALSE;        // Do not abort reads/writes on 
										  // error
	PortDCB.ByteSize = 8;                 // Number of bits/byte, 4-8 
	PortDCB.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space 
	PortDCB.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2 

	// Configure the port according to the specifications of the DCB 
	// structure.
	if (!SetCommState (hPort, &PortDCB))
	{
		// Could not configure the serial port.
		SetWindowText("COM Port is not open");
		if (!bFirst) {
			AfxMessageBox("Unable to configure the serial port");
		}
		CloseHandle(hPort);
		hPort=NULL;
		return;
	}

	// Set timeouts
	COMMTIMEOUTS cto;
	cto.ReadIntervalTimeout=MAXDWORD;	// timeout immediately on read
	cto.ReadTotalTimeoutConstant=0;
	cto.ReadTotalTimeoutMultiplier=0;
	cto.WriteTotalTimeoutConstant=0;	// No timeout on write
	cto.WriteTotalTimeoutMultiplier=0;
	if (!SetCommTimeouts(hPort, &cto)) {
		SetWindowText("COM Port is not open");
		if (!bFirst) {
			AfxMessageBox("Failed to set port timeouts");
		}
		CloseHandle(hPort);
		hPort=NULL;
		return;
	}

	if (!SetCommMask(hPort, EV_RXCHAR|EV_ERR)) {
		SetWindowText("COM Port is not open");
		if (!bFirst) {
			AfxMessageBox("Failed to set port event mask.");
		}
		CloseHandle(hPort);
		hPort=NULL;
		return;
	}

	csCom.Format("COM%d: is open", nPort);
	SetWindowText(csCom);
}

BOOL CTI99RecvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	hPort=NULL;
	ctrlPort.SetCurSel(0);
	OpenPort(-99);	// negative means first try
//	myWriteFile(hPort, "Are you there?\012", strlen("Are you there?\012"), (DWORD*)&bufidx, NULL);

	CEdit *pWnd=(CEdit*)GetDlgItem(IDC_EDIT1);
	if (pWnd) {
		pWnd->SetWindowText("C:\\NEW\\TI\\");
		OnButton1();
	}

	bufidx=0;
	nMode=0;		// handshaking
	fp=NULL;
	nCount=0;
	RecvMode=0;	// Assume bitmap mode

	hDieEvent=CreateEvent(NULL, true, false, NULL);
	hReadyToWait=CreateEvent(NULL, false, true, NULL);

	if (hPort) {
		_beginthread(CommWrapper, 0, this);
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTI99RecvDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTI99RecvDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTI99RecvDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTI99RecvDlg::OnButton1() 
{
	CString csPath;
	CEdit *pWnd=(CEdit*)GetDlgItem(IDC_EDIT1);
	if (pWnd) {
		CFileDialog dlg(true, NULL, "(SELECT FOLDER)", 0, 0, this);
		if (IDOK == dlg.DoModal()) {
			csPath=dlg.GetPathName();
			if (csPath.Find('\\')) {
				// strip off the filename
				csPath=csPath.Left(csPath.ReverseFind('\\'));
			}
			if (!SetCurrentDirectory(csPath)) {
				AfxMessageBox("Unable to set save path!");
			} else {
				pWnd->SetWindowText(csPath);
			}
		}
	}
}

void CTI99RecvDlg::CommThread()
{
	DWORD dwType;
	HANDLE waiters[2];

	ResetEvent(hDieEvent);

	waiters[0]=hDieEvent;
	waiters[1]=hReadyToWait;

	while (1) {
		OutputDebugString("Waiting for outgoing data to complete.\n");
		dwType=WaitForMultipleObjects(2, waiters, false, INFINITE);
		if (WAIT_OBJECT_0 == dwType) {
			return;
		}

		OutputDebugString("Waiting for inbound data.\n");
		for (;;) {
			if (!WaitCommEvent(hPort, &dwType, NULL)) {
				if (WAIT_OBJECT_0 == WaitForSingleObject(hDieEvent, 0)) {
					return;		// exit silently
				}
				// else.. complain!?
			} else {
				if (dwType == EV_RXCHAR) {
//					OutputDebugString("Received character\n");

					DWORD dwRead=0;
					
					CSingleLock thelock(&csLock);
					thelock.Lock();
					if (!thelock.IsLocked()) {
						AfxMessageBox("Can't lock!");
					}

					if (ReadFile(hPort, &buf[bufidx], 1024-bufidx, &dwRead, NULL)) {
						if ((nMode == 0) && (bufidx == 0) && (buf[0]!='H')) {
							// ignore until the very first char is an H
						} else {
							bufidx+=dwRead;
							if ((buf[bufidx-1]=='\012') && (nMode != 2)) {
								PostMessage(WM_USER, 0, 0);
								break;
							}
							if ((bufidx>256) && (nMode==2)) {
								PostMessage(WM_USER, 0, 0);
								break;
							}
						}
					}
					
					thelock.Unlock();
				}
			}
		}
	}
}

void CTI99RecvDlg::OnClose() 
{
	SetEvent(hDieEvent);
	CloseHandle(hPort);		// Should cause the wait thread to error out

	CDialog::OnClose();
}

LRESULT CTI99RecvDlg::OnUser(WPARAM, LPARAM)
{
	char str[1024];
	CString cs;
	DWORD dwWrote;
	char check;
	int idx;

	if (bufidx == 0) { 
		return 0;
	}

	OutputDebugString("Got a complete packet.\n");

	CSingleLock thelock(&csLock);
	thelock.Lock();
	if (!thelock.IsLocked()) {
		AfxMessageBox("Can't lock!");
	}

	// We have a valid buffer
	switch (nMode) {
	case 0:		// HELLO
		buf[bufidx-1]='\0';
		sprintf_s(str, 1024, "Got: %s", buf);
		m_Status.SetWindowText(str);
		if ((strncmp(buf, "HELLO: ", 7))&&(strncmp(buf, "HELLP: ", 7))) {
			AfxMessageBox("Did not get clear HELLO! Reset TI End");
			myWriteFile(hPort, "FAILURE\012", 8, &dwWrote, NULL);
			nMode=0;
			break;
		}
		if (buf[4] == 'P') RecvMode=1;	// disk mode
		cs=&buf[7];	// strip the hello
		for (idx=0; idx<cs.GetLength(); idx++) {
			if (!isalnum(cs[idx])) cs.SetAt(idx, '_');	// map all punctuation to _
		}
		cs+=".DSK";
		if (fp) fclose(fp);
		fopen_s(&fp, cs, "rb");
		while (fp) {
			fclose(fp);
			cs=cs.Left(cs.GetLength()-4);
			cs+="x.DSK";
			fopen_s(&fp, cs, "rb");
		}
		fopen_s(&fp, cs, "wb");
		if (NULL == fp) {
			AfxMessageBox("Couldn't open file! Reset TI End");
			myWriteFile(hPort, "FAILURE\012", 8, &dwWrote, NULL);
			nMode=0;
		} else {
			if (!myWriteFile(hPort, "OK\012", 3, &dwWrote, NULL)) {
				AfxMessageBox("Failed to write OK! Reset TI End");
				nMode=0;
			} else {
				nMode++;
			}
		}
		break;

	case 1:		// SIZE
		buf[bufidx-1]='\0';
		sprintf_s(str, 1024, "Got: %s", buf);
		m_Status.SetWindowText(str);
		if (1 != sscanf_s(buf, "SIZE: %d", &nCount)) {
			AfxMessageBox("Got bad size string! Reset TI End");
			myWriteFile(hPort, "FAILURE\012", 8, &dwWrote, NULL);
			nMode=0;
			break;
		}
		if (nCount > 2880) {
			AfxMessageBox("Sector count is far too high! Reset TI End");
			myWriteFile(hPort, "FAILURE\012", 8, &dwWrote, NULL);
			nMode=0;
			break;
		}
		if (!myWriteFile(hPort, "READY\012", 6, &dwWrote, NULL)) {
			AfxMessageBox("Failed to write READY! Reset TI End");
			nMode=0;
		} else {
			nMode++;
			pBmp=NULL;
		}
		break;

	case 2:		// Received a buffer!
		check=0;
		for (idx=0; idx<256; idx++) {
			check+=buf[idx];
		}
		if (check != buf[256]) {
			m_Status.SetWindowText("Checksum does not match!");
			if (!myWriteFile(hPort, "N\012", 2, &dwWrote, NULL)) {
				AfxMessageBox("Failed to write Negative response! Reset TI End");
				nMode=0;
			}
			break;
		} else {
			if (pBmp == NULL) {
				// Must be sector 0, cache it and start reading the bitmap
				memcpy(sec0, buf, 256);
				pBmp=&sec0[0x38];
				BmpMask=0x01;
			}
			sprintf_s(str, 1024, "Sector OK, %d left", nCount-1);
			m_Status.SetWindowText(str);
			if (!myWriteFile(hPort, "Y\012", 2, &dwWrote, NULL)) {
				AfxMessageBox("Failed to write Positive response! Reset TI End");
				nMode=0;
				break;
			}
			do {
				if (!fwrite(buf, 256, 1, fp)) {
					AfxMessageBox("Failed to write to file! Reset TI End");
					myWriteFile(hPort, "FAILURE\012", 8, &dwWrote, NULL);
					fclose(fp);
					nMode=0;
					break;
				}

				nCount--;

				BmpMask<<=1;
				if (BmpMask > 255) {
					BmpMask=0x01;
					pBmp++;
				}

				memset(buf, 0xE5, 256);		// just to fill blank sectors (>E5 for unused by TI's spec)
			} while ((nCount>0) && ( (!((*pBmp)&BmpMask))&&(RecvMode==0) ) );

			if (nCount == 0) {
				m_Status.SetWindowText("Disk successfully received.");
				fclose(fp);
				nMode=0;
			}
		}
		break;
	}

	bufidx=0;
	thelock.Unlock();

	return 0;
}

void CTI99RecvDlg::OnBnClickedButton2()
{
	int nSel=ctrlPort.GetCurSel();

	if (nSel < 0) return;

	SetEvent(hDieEvent);
	OpenPort(nSel+1);
	if (hPort) {
		CWnd *pWnd=GetDlgItem(IDC_BUTTON2);
		if (NULL != pWnd) pWnd->EnableWindow(false);
		_beginthread(CommWrapper, 0, this);
	}
}
