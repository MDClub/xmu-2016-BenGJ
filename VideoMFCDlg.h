
// VideoMFCDlg.h : header file
//

#pragma once

#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"


// CVideoMFCDlg dialog
class CVideoMFCDlg : public CDialogEx
{
// Construction
public:
	CVideoMFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VIDEOMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	IplImage* image;
//	CvCapture* capture;
	afx_msg void OnBnClickedButton1();
private:
//	void OnTimer(UINT nIDEvent);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton3();
};
