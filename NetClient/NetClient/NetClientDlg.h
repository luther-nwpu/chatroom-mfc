
// NetClientDlg.h: 头文件
//

#pragma once
#include <WinSock2.h>
#include "afxwin.h"

// CNetClientDlg 对话框
class CNetClientDlg : public CDialog
{
	// 构造
	public:
		unsigned __int64 clientn;
		unsigned __int64 clientd;
		unsigned __int64 servere;
		unsigned __int64 servern;
		unsigned char* clientkeys;
		unsigned char* serverkeys;
		CNetClientDlg(CWnd* pParent = NULL);	// 标准构造函数
		SOCKET connectClientSocket;
		SOCKET connectServerSocket;
		sockaddr_in connectServerAddr;
		sockaddr_in connectClientAddr;
		SOCKET udpRecvSocket;
		sockaddr_in udpRecvAddr;
		bool m_bTerminateThread;
		CWinThread* currentThread;
	#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_NETCLIENT_DIALOG };
	#endif
	protected:
		HICON m_hIcon;
		// 生成的消息映射函数
		virtual BOOL OnInitDialog();
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		DECLARE_MESSAGE_MAP()
	public:
		CString localClientPort;
		CString clientIPAddress;
		CListBox messagesList;
		CString udpRecvPortStr;
		CString connectClientAddressStr;
		CString connectClientPortStr;
		CString sendMessageStr;
		CString connectServerAddrStr;
		CString connectServerPortStr;
		LRESULT OnServerSocket(WPARAM wParam, LPARAM lParam);
		BOOL ConnectServer(LPCTSTR pszRemoteAddr, u_short nPort);
		afx_msg void OnBnClickedButtonConnetServer();
		afx_msg void OnBnClickedButtonSendMessage();
		afx_msg void OnBnClickedButtonStartUdpRecvPort();
		afx_msg void OnBnClickedButtonConnectUdpClient();
};
