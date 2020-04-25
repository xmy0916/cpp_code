
// mfctestDlg.h: 头文件
//
#include "Kinect.h"
#include "afxwin.h"
#include<time.h>
#pragma once


// CmfctestDlg 对话框
class CmfctestDlg : public CDialogEx
{
// 构造
public:
	CmfctestDlg(CWnd* pParent = nullptr);	// 标准构造函数
	static CmfctestDlg *s_pDlg; //对象指针
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCTEST_DIALOG };
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
	afx_msg void OnBnClickedButton2();
	void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStnClickedColorimage();
};
