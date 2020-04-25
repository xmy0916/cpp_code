
// mfcKinectBodyCheckDlg.h: 头文件
//
#include "Kinect.h"
#include "afxwin.h"
#include<time.h>
#pragma once


// CmfcKinectBodyCheckDlg 对话框
class CmfcKinectBodyCheckDlg : public CDialogEx
{
// 构造
public:
	CmfcKinectBodyCheckDlg(CWnd* pParent = nullptr);	// 标准构造函数
	static CmfcKinectBodyCheckDlg *s_pDlg; //对象指针

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCKINECTBODYCHECK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	void OnTimer(UINT_PTR nIDEvent);
};
