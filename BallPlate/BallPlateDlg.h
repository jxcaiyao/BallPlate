
// BallPlateDlg.h: 头文件
//

#pragma once
#include <iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "Eigen/Dense"
#include "MilVision.h"
#include "MyView.h"
#include "MyCamera.h"
#include "MotionControl.h"
#include "gts.h"

// CBallPlateDlg 对话框
class CBallPlateDlg : public CDialogEx
{
// 构造
public:
	CBallPlateDlg(CWnd* pParent = nullptr);	// 标准构造函数
	int Array2Mat(BYTE* array, cv::Mat& img, int width, int height);
	int Mat2CImage(cv::Mat* mat, CImage& img);

	inline UINT64 GetCycleCount();
	void GetCPUFrequency();
	void SaveData(void);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BALLPLATE_DIALOG };
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
private:
	double m_CPUFrequency;

	CStatic m_Image;			//图像显示控件
	CMilVision m_MilVision;		//用于连接相机驱动板
	CMyView m_MyView;			//暂无用处
	MyCamera m_MyCamera;		//用于标定与坐标计算
	MotionControl m_XCtrl;
	MotionControl m_YCtrl;
	std::ofstream fout;

	BYTE* m_array;				//测试用数组
	cv::Mat mask;
	cv::Mat img_raw;			//原始的图像
	cv::Mat img_proc;			//处理的图像
	cv::Mat img_disp;			//显示的图像
	Eigen::Vector2d m_BallPos;	//小球世界坐标

public:
	afx_msg void OnBnClickedButtonGrabImage();	//截图
	afx_msg void OnBnClickedButtonCaliImage();	//标定
	afx_msg void OnBnClickedButtonBallPos();
	afx_msg void OnBnClickedButtonContinue();
	afx_msg void OnBnClickedButtonContinuePosition();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CString m_BallXPosText;
	CString m_BallYPosText;
	CString m_TimeText;
	CString m_XCtrlText;
	CString m_YCtrlText;
};
