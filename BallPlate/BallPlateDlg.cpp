
// BallPlateDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "BallPlate.h"
#include "BallPlateDlg.h"
#include "afxdialogex.h"
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

inline UINT64 CBallPlateDlg::GetCycleCount()
{
	__asm
	{
		_emit 0x0F;
		_emit 0x31;
	}
}

void CBallPlateDlg::GetCPUFrequency() 
{
	UINT64 start, end;

	start = GetCycleCount();
	end = GetCycleCount() - start;
	start = GetCycleCount();

	Sleep(1000);
	m_CPUFrequency = (double)(GetCycleCount() - start - end);
}


int CommandHandler(CString str, int error)
{
	if (!error) {
		return 0;
	}
	CString str1;
	str1.Format(" : %d", error);
	str1 = str + str1;
	AfxMessageBox(str1);

	return error;
}

void GrabCam(CBallPlateDlg* proc) {
	UINT64 t1, t2;
	t1 = proc->GetCycleCount();

	while (true) {
		proc->m_MilVision.ImageFrameGrab();
		proc->Array2Mat(proc->m_MilVision.m_pDataArray, proc->img_raw, 576, 768);

		if (proc->m_MyCamera.getBallPosition(proc->m_BallPos, proc->img_raw)) {
			//AfxMessageBox(TEXT("No Ball Detected!"));
			proc->img_raw.copyTo(proc->img_disp);
		}
		else {
			proc->m_BallXPosText.Format("%.1fmm", proc->m_BallPos.x);
			proc->m_BallYPosText.Format("%.1fmm", proc->m_BallPos.y);

			proc->m_MyCamera.drawPoint(proc->img_raw, proc->img_proc, proc->m_BallPos, cv::Scalar(0));
			proc->img_proc.copyTo(proc->img_disp);
		}
		t2 = proc->GetCycleCount();
		proc->m_TimeText.Format("%.1fms", (t2 - t1) * 1000.0 / proc->m_CPUFrequency);
		t1 = t2;
	}
}

void SaveData(CBallPlateDlg *proc) {

	time_t tim = time(NULL);
	tm LocTime;
	localtime_s(&LocTime, &tim);

	CString FileName;
	FileName.Format("../data/data_%d_%d_%d_%d_%d_%d.csv",
		LocTime.tm_year + 1900,
		LocTime.tm_mon + 1,
		LocTime.tm_mday,
		LocTime.tm_hour,
		LocTime.tm_min,
		LocTime.tm_sec);

	std::ofstream fout;
	fout.open(FileName, std::ios::out | std::ios::trunc);

	fout << "X_Kp," << proc->m_XCtrl.Kp << ",X_Ki," << proc->m_XCtrl.Ki << ",X_Kd," << proc->m_XCtrl.Kd << std::endl;
	fout << "Y_Kp," << proc->m_YCtrl.Kp << ",Y_Ki," << proc->m_YCtrl.Ki << ",Y_Kd," << proc->m_YCtrl.Kd << std::endl;
	fout << std::endl;

	UINT64 t3, t4;
	double XEnc, YEnc;

	t3 = proc->GetCycleCount();
	while(true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		GT_GetEncPos(1, &XEnc);
		GT_GetEncPos(2, &YEnc);

		t4 = proc->GetCycleCount();
		double time = (t4 - t3) * 1000 / proc->m_CPUFrequency;

		fout << time << "," << proc->m_BallPos.x << "," << proc->m_BallPos.y << "," << proc->m_XCtrl.Output << "," << proc->m_YCtrl.Output << "," << XEnc << "," << YEnc << std::endl;
	}
}

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
	, m_BallXPosText(_T(""))
	, m_BallYPosText(_T(""))
	, m_TimeText(_T(""))
	, m_XCtrlText(_T(""))
	, m_YCtrlText(_T(""))
{
	f_CamParams = _T("cameraParams.yaml");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_CPUFrequency = 3600000000;
	m_array = { 0 };
}

void CBallPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, m_Image);
	DDX_Text(pDX, IDC_BALL_POSX, m_BallXPosText);
	DDX_Text(pDX, IDC_BALL_POSY, m_BallYPosText);
	DDX_Text(pDX, IDC_TIME_TEXT, m_TimeText);
	DDX_Text(pDX, IDC_XCTRL, m_XCtrlText);
	DDX_Text(pDX, IDC_YCTRL, m_YCtrlText);
}

BEGIN_MESSAGE_MAP(CBallPlateDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GRAB_IMAGE, &CBallPlateDlg::OnBnClickedButtonGrabImage)
	ON_BN_CLICKED(IDC_BUTTON_CALI_IMAGE, &CBallPlateDlg::OnBnClickedButtonCaliImage)
	ON_BN_CLICKED(IDC_BUTTON_BALL_POS, &CBallPlateDlg::OnBnClickedButtonBallPos)
	ON_BN_CLICKED(IDC_BUTTON_CONTINUE, &CBallPlateDlg::OnBnClickedButtonContinue)
	ON_BN_CLICKED(IDC_BUTTON_CONTINUE_POSITION, &CBallPlateDlg::OnBnClickedButtonContinuePosition)
	ON_WM_TIMER()
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
	GetCPUFrequency();				//得到CPU频率，方便计算精确运行时间

	//img_raw = cv::imread("../picture/pic1.png", 0);
	//std::vector<cv::Vec3f> circles;
	//cv::HoughCircles(img_raw, circles, cv::HOUGH_GRADIENT, 1, 100, 100, 100, 0, 0);

	m_MilVision.InitialVision();

	//显示图像
	//m_MilVision.GrabContinuous(m_Image);

	double t1;
	t1 = GetCycleCount() * 1000.0 / m_CPUFrequency;

	m_XCtrl.InitParams(0.0, 5.0, 1, 1.0, t1, 0.03);
	m_YCtrl.InitParams(0.0, 5.0, 1, 1.0, t1, 0.03);


	int sRtn = 0;

	//sRtn = GT_Close();
	//if (CommandHandler("GT_Close", sRtn))
	//	return 0;

	sRtn = GT_Open();
	if (CommandHandler("GT_Open", sRtn))
		return 0;

	sRtn = GT_ClrSts(1, 8);

	sRtn = GT_Reset();
	if (CommandHandler("GT_Reset", sRtn))
		return 0;
	Sleep(500);

	sRtn = GT_LoadConfig("GTS800.cfg");
	if (CommandHandler("GT_LoadConfig", sRtn))
		return 0;

	sRtn = GT_ClrSts(1, 2);
	if (CommandHandler("GT_ClrSts", sRtn))
		return 0;

	TPid pid;

	sRtn = GT_GetPid(1, 1, &pid);
	if (CommandHandler("GT_GetPid", sRtn))
		return 0;

	pid.kp = 0.85;
	pid.ki = 0.00;
	pid.kd = 0.04;

	sRtn = GT_SetPid(1, 1, &pid);
	if (CommandHandler("GT_SetPid", sRtn))
		return 0;


	sRtn = GT_GetPid(2, 1, &pid);
	if (CommandHandler("GT_GetPid", sRtn))
		return 0;

	pid.kp = 0.85;
	pid.ki = 0.00;
	pid.kd = 0.04;

	sRtn = GT_SetPid(2, 1, &pid);
	if (CommandHandler("GT_SetPid", sRtn))
		return 0;


	sRtn = GT_AxisOn(1);
	if (CommandHandler("GT_AxisOn", sRtn))
		return 0;

	sRtn = GT_AxisOn(2);
	if (CommandHandler("GT_AxisOn", sRtn))
		return 0;



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
	UINT64 t1, t2;
	CString str;
	t1 = GetCycleCount();
	m_MyCamera.caliImage(img_raw);
	m_MyCamera.saveParams(f_CamParams);
	m_MyCamera.getMask(mask);

	img_disp = cv::Mat();
	img_raw.copyTo(img_disp, mask);

	t2 = GetCycleCount();
	str.Format(TEXT("use time: %fms"), (t2 - t1) * 1000.0 / m_CPUFrequency);
	MessageBox(str);

	OnPaint();
}


void CBallPlateDlg::OnBnClickedButtonBallPos()
{
	// TODO: 在此添加控件通知处理程序代码
	//UINT64 t1, t2;
	//CString str;
	//t1 = GetCycleCount();
	m_MyCamera.getBallPosition(m_BallPos, img_raw);
	m_MyCamera.drawPoint(img_raw, img_proc, m_BallPos, cv::Scalar(0));
	//t2 = GetCycleCount();
	//str.Format(TEXT("use time: %fms"), (t2 - t1) * 1000.0 / m_CPUFrequency);
	//MessageBox(str);

	//double x, y;
	//x = m_BallPos(0);
	//y = m_BallPos(1);
	//str.Format(TEXT("x = %.1fmm, y = %.1fmm"), x, y);
	//MessageBox(str);
	img_disp = cv::Mat();
	img_proc.copyTo(img_disp, mask);
	OnPaint();
}


void CBallPlateDlg::OnBnClickedButtonContinue()
{
	// TODO: 在此添加控件通知处理程序代码
	img_disp = cv::Mat();
	KillTimer(1);
	OnPaint();
	//m_MilVision.GrabContinuous(m_Image);

}


void CBallPlateDlg::OnBnClickedButtonContinuePosition()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MyCamera.readParams(f_CamParams);
	SetTimer(1, 60, NULL);

	std::thread thGrab;
	thGrab = std::thread(GrabCam, this);
	thGrab.detach();

	std::thread thSave;
	thSave = std::thread(SaveData, this);
	thSave.detach();
}


void CBallPlateDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UINT64 t;
	switch (nIDEvent) {
	case 1:
		t = GetCycleCount();
		m_XCtrl.ControlUpdate(m_BallPos.x, t * 1000.0 / m_CPUFrequency);
		m_YCtrl.ControlUpdate(m_BallPos.y, t * 1000.0 / m_CPUFrequency);

		short rtn;

		TCrdPrm crdPrm;
		memset(&crdPrm, 0, sizeof(crdPrm));
		crdPrm.dimension = 2;
		crdPrm.synVelMax = 20;
		crdPrm.synAccMax = 0.3;
		crdPrm.evenTime = 5;
		crdPrm.profile[0] = 1;
		crdPrm.profile[1] = 2;
		rtn = GT_SetCrdPrm(1, &crdPrm);
		rtn = GT_CrdClear(1, 0);

		double XEnc, YEnc;
		GT_GetEncPos(1, &XEnc);
		GT_GetEncPos(2, &YEnc);

		rtn = GT_LnXY(1, (long)(m_XCtrl.Output - XEnc), (long)(m_YCtrl.Output - YEnc), 20, 0.3, 0, 0);
		rtn = GT_CrdStart(1, 0);

		m_XCtrlText.Format("%.0f  %.0f", m_XCtrl.Output, XEnc);
		m_YCtrlText.Format("%.0f  %.0f", m_YCtrl.Output, YEnc);

		UpdateData(false);

		OnPaint();
		break;

	default:
		break;
	}
	

	CDialogEx::OnTimer(nIDEvent);
}
