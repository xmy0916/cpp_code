
// mfcKinectBodyCheckDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "mfcKinectBodyCheck.h"
#include "mfcKinectBodyCheckDlg.h"
#include "afxdialogex.h"

#include <Kinect.h>
#include "cv.h"
#include <opencv2/opencv.hpp>   //opencv头文件
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp> 
#include <kinect.h>

using namespace cv;
using namespace std;            //命名空间

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
CBodyBasics       myKinect;             //初始化一个全局Kinect 对象
CmfcKinectBodyCheckDlg * CmfcKinectBodyCheckDlg::s_pDlg = nullptr;

// CmfcKinectBodyCheckDlg 对话框



CmfcKinectBodyCheckDlg::CmfcKinectBodyCheckDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCKINECTBODYCHECK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmfcKinectBodyCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CmfcKinectBodyCheckDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()                     //定时器事件
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CmfcKinectBodyCheckDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CmfcKinectBodyCheckDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CmfcKinectBodyCheckDlg 消息处理程序

BOOL CmfcKinectBodyCheckDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	s_pDlg = this;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmfcKinectBodyCheckDlg::OnPaint()
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
HCURSOR CmfcKinectBodyCheckDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CmfcKinectBodyCheckDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CmfcKinectBodyCheckDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	HRESULT hr = myKinect.InitializeDefaultSensor();        //初始化默认Kinect

	if (SUCCEEDED(hr))
	{
		myKinect.Update();                              //刷新骨骼和深度数据 

	}
	SetTimer(35, 20, NULL);             //定时器
}

void CmfcKinectBodyCheckDlg::OnTimer(UINT_PTR nIDEvent)     //定时器处理
{
	switch (nIDEvent)
	{
	case 35:
		myKinect.Update();
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}