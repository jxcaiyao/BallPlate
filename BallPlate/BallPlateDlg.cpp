﻿
// BallPlateDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "BallPlate.h"
#include "BallPlateDlg.h"
#include "afxdialogex.h"
#include "gts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CBallPlateDlg 对话框



CBallPlateDlg::CBallPlateDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BALLPLATE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBallPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, m_Image);
}

BEGIN_MESSAGE_MAP(CBallPlateDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GRAB_IMAGE, &CBallPlateDlg::OnBnClickedButtonGrabImage)
	ON_BN_CLICKED(IDC_BUTTON_CALI_IMAGE, &CBallPlateDlg::OnBnClickedButtonCaliImage)
	ON_BN_CLICKED(IDC_BUTTON_BALL_POS, &CBallPlateDlg::OnBnClickedButtonBallPos)
	ON_BN_CLICKED(IDC_BUTTON_CONTINUE, &CBallPlateDlg::OnBnClickedButtonContinue)
END_MESSAGE_MAP()


// CBallPlateDlg 消息处理程序

BOOL CBallPlateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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
	m_Image.SetWindowPos(NULL, 0, 0, 768, 576, SWP_NOMOVE);

	//img_raw = cv::imread("../picture/pic1.png", 0);
	//std::vector<cv::Vec3f> circles;
	//cv::HoughCircles(img_raw, circles, cv::HOUGH_GRADIENT, 1, 100, 100, 100, 0, 0);

	m_MilVision.InitialVision();

	//显示图像
	m_MilVision.GrabContinuous(m_Image);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


int CBallPlateDlg::Array2Mat(BYTE* array, cv::Mat& img, int row, int col) {

	img = cv::Mat(row, col, CV_8UC1, (BYTE*)array);

	return 0;
}

int CBallPlateDlg::Mat2CImage(cv::Mat* mat, CImage& img) {
	if (!mat || mat->empty())
		return -1;
	int nBPP = mat->channels() * 8;
	img.Create(mat->cols, mat->rows, nBPP);
	if (nBPP == 8)
	{
		static RGBQUAD pRGB[256];
		for (int i = 0; i < 256; i++)
			pRGB[i].rgbBlue = pRGB[i].rgbGreen = pRGB[i].rgbRed = i;
		img.SetColorTable(0, 256, pRGB);
	}
	uchar* psrc = mat->data;
	uchar* pdst = (uchar*)img.GetBits();
	int imgPitch = img.GetPitch();
	for (int y = 0; y < mat->rows; y++)
	{
		memcpy(pdst, psrc, mat->cols * mat->channels());//mat->step is incorrect for those images created by roi (sub-images!)
		psrc += mat->step;
		pdst += imgPitch;
	}

	return 0;
}

void CBallPlateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBallPlateDlg::OnPaint()
{
	CImage cImage;
	if (Mat2CImage(&img_disp, cImage)) {
		//MessageBox("Mat2CImage Wrong!");
	}
	else {
		HDC hDc = m_Image.GetDC()->m_hDC;
		CRect rect;
		m_Image.GetWindowRect(&rect);
		cImage.Draw(hDc, 0, 0, img_disp.cols, img_disp.rows, 0, 0, img_disp.cols, img_disp.rows);
	}

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
HCURSOR CBallPlateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBallPlateDlg::OnBnClickedButtonGrabImage()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MilVision.ImageFrameGrab();
	Array2Mat(m_MilVision.m_pDataArray, img_raw, 576, 768);
	img_disp = img_raw.clone();
	OnPaint();
}


void CBallPlateDlg::OnBnClickedButtonCaliImage()
{
	// TODO: 在此添加控件通知处理程序代码

	//初始化标定
	//cv::namedWindow("123");
	//cv::imshow("123", img_raw);
	//cv::waitKey(0);
	m_MyCamera.caliImage(img_raw);
}


void CBallPlateDlg::OnBnClickedButtonBallPos()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MyCamera.getBallPosition(m_BallPos, img_raw);
	m_MyCamera.drawPoint(img_raw, img_proc, m_BallPos, cv::Scalar(0));
	img_disp = img_proc.clone();
	OnPaint();
}


void CBallPlateDlg::OnBnClickedButtonContinue()
{
	// TODO: 在此添加控件通知处理程序代码
	img_disp = cv::Mat();
	OnPaint();
	m_MilVision.GrabContinuous(m_Image);

}
