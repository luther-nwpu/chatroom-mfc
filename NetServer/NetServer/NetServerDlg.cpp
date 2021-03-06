
// NetServerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "NetServer.h"
#include "NetServerDlg.h"
#include "afxdialogex.h"
#include <WinSock2.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 忽略4996警告
#pragma warning(disable : 4996)

// 定义网络事件通知消息
#define WM_SOCKET WM_USER + 1	

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


// CNetServerDlg 对话框



CNetServerDlg::CNetServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NETSERVER_DIALOG, pParent)
	, serverPortStr(_T(""))
	, iPAddressStr(_T(""))
{
	serverPortStr = "";
	iPAddressStr = "";
	recieveSocket = INVALID_SOCKET;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER_PORT, serverPortStr);
	DDX_Control(pDX, IDC_LIST2, clientkeys);
	DDX_Text(pDX, IDC_STATIC_IPADDRESS, iPAddressStr);
}

BEGIN_MESSAGE_MAP(CNetServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_SERVER, &CNetServerDlg::OnBnClickedButtonStartServer)
	ON_EN_CHANGE(IDC_EDIT_SERVER_PORT, &CNetServerDlg::OnEnChangeEditServerPort)
	ON_MESSAGE(WM_SOCKET, OnSocket)
END_MESSAGE_MAP()


// CNetServerDlg 消息处理程序

BOOL CNetServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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
	clientcount = 0;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	::WSAStartup(sockVersion, &wsaData);
	// 下面是取得本地IP地址的过程，将它显示在状态栏的第一个分栏中
	// 取得本机名称	
	char szHost[256];
	::gethostname(szHost, 256);
	// 通过本机名称取得地址信息
	HOSTENT* pHost = gethostbyname(szHost);
	if (pHost != NULL)
	{
		// 得到第一个IP地址
		in_addr *addr = (in_addr*) *(pHost->h_addr_list);
		// 显示给用户
		iPAddressStr = inet_ntoa(addr[0]);
		UpdateData(FALSE);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNetServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNetServerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNetServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CNetServerDlg::OnBnClickedButtonStartServer()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (recieveSocket == INVALID_SOCKET ) // 开启服务 
	{
		// 取得端口号
		int nPort = atoi((LPCSTR)serverPortStr.GetBuffer(0));
		if (nPort < 1 || nPort > 65535) {
			MessageBox("Port Error!");
			return;
		}
		// 创建监听套接字，使它进入监听状态
		if (!CreateAndListen(nPort)) {
			MessageBox("Start Port Error");
			return;
		}
		// 设置相关子窗口控件状态
		GetDlgItem(IDC_BUTTON_START_SERVER)->SetWindowText("Stop Server");
		GetDlgItem(IDC_EDIT_SERVER_PORT)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else 
	{
		// 关闭所有连接
		CloseAllSocket();
		// 设置相关子窗口控件状态
		GetDlgItem(IDC_BUTTON_START_SERVER)->SetWindowText("Start Server");
		GetDlgItem(IDC_EDIT_SERVER_PORT)->EnableWindow(TRUE);
		UpdateData(FALSE);
	}
}

BOOL CNetServerDlg::CloseAllSocket() {
	if (recieveSocket != INVALID_SOCKET) {
		::closesocket(recieveSocket);
		recieveSocket = INVALID_SOCKET;
	}
	// 关闭所有客户的连接
	for (int i = 0; i < clientcount; i++) {
		::closesocket(client[i]);
	}
	clientcount = 0;
	return TRUE;
}
BOOL CNetServerDlg::CreateAndListen(int nPort) {
	if (recieveSocket == INVALID_SOCKET) {
		::closesocket(recieveSocket);
	}
	// 创建套接字
	recieveSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (recieveSocket == INVALID_SOCKET) {
		return FALSE;
	}
	// 填写要关联的本地地址
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);
	sin.sin_addr.s_addr = INADDR_ANY;
	// 绑定端口
	if (::bind(recieveSocket, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR) {
		return FALSE;
	}

	// 设置socket为窗口通知消息类型
	::WSAAsyncSelect(recieveSocket, m_hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
	// 进入监听模式
	::listen(recieveSocket, 5);

	return TRUE;

}

void CNetServerDlg::OnEnChangeEditServerPort()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

LRESULT CNetServerDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
	// 取得有事件发生的套接字句柄
	SOCKET s = wParam;
	// 查看是否出错
	if (WSAGETSELECTERROR(lParam)) 
	{
		RemoveClient(s);
		::closesocket(s);
		return FALSE;
	}
	// 处理发生的事件
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:		// 监听中的套接字检测到有连接进入
		{
			if (clientcount < MAX_SOCKET) {
				// 接受连接请求，新的套接字client是新连接的套接字
				SOCKET client = ::accept(s, NULL, NULL);
				// 设置新的套接字为窗口通知消息类型
				int i = ::WSAAsyncSelect(client, m_hWnd, 
					WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
				AddClient(client);
			}
			else
			{
				MessageBox("connect Server Too More");
			}
		}
		break;
	case FD_CLOSE:		// 检测到套接字对应的连接被关闭。
		{
			RemoveClient(s);
			::closesocket(s);
		}
		break;
	case FD_READ:		// 套接字接收到对方发送过来的数据包
		{
			// 取得对方的IP地址和端口号（使用getpeername函数）
			// Peer对方的地址信息
			sockaddr_in sockAddr;
			memset(&sockAddr, 0, sizeof(sockAddr));
			int nSockAddrLen = sizeof(sockAddr);
			::getpeername(s, (SOCKADDR*)&sockAddr, &nSockAddrLen);
			// 转化为主机字节顺序
			int nPeerPort = ::ntohs(sockAddr.sin_port);
			// 转化为字符串IP
			CString sPeerIP = ::inet_ntoa(sockAddr.sin_addr);

			// 取得对方的主机名称
			// 取得网络字节顺序的IP值
			DWORD dwIP = ::inet_addr(sPeerIP);
			// 获取主机名称，主义其中第一个参数的转化
			hostent* pHost = ::gethostbyaddr((LPSTR)&dwIP, 4, AF_INET);
			char szHostName[256];
			strncpy(szHostName, pHost->h_name, 256);

			// 接受真正的网络数据
			char szText[1024] = { 0 };
			::recv(s, szText, 1024, 0);
			// 显示给用户
			CString szstr = CString(szText);
			if (szstr.Find(".") == -1) {
				clientkeys.InsertString(0, sPeerIP + ":" + CString(szText));
				map<CString, CString>::iterator iter;

				iter = messagesMap.find(sPeerIP);
				messagesMap.insert(pair<CString, CString>((sPeerIP), CString(szText)));
				for (iter = messagesMap.begin(); iter != messagesMap.end(); iter++) {
				}
				CString keysStr = "";
				if (::send(s, keysStr, keysStr.GetLength(), 0) != -1) {
				}
			}
			else {
				CString content = messagesMap.find(CString(szText))->second;
				if (::send(s, content, content.GetLength(), 0) != -1) {
				}
			}
			UpdateData(FALSE);
		}

		break;
	default:
		break;
	}
	return FALSE;
}
BOOL CNetServerDlg::AddClient(SOCKET s) {
	if (clientcount < MAX_SOCKET) {
		// 添加新的成员
		client[clientcount++] = s;
		return TRUE;
	}
	return FALSE;
}

void CNetServerDlg::RemoveClient(SOCKET s) {

	BOOL bFind = FALSE;
	int i;
	for (i = 0; i < clientcount; i++) {
		if (client[i] == s) {
			bFind = TRUE;
			break;
		}
	}
	// 如果找到就将此成员从列表中移除
	if (bFind) {
		clientcount--;
		// 将此成员后面的成员都向前移动一个单位
		for (int j = i; j < clientcount; j++) {
			client[j] = client[j + i];
		}
	}
}