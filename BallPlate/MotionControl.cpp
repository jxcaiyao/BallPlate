#include "pch.h"
#include "MotionControl.h"

MotionControl::MotionControl()
{
}

MotionControl::~MotionControl()
{
}

void MotionControl::InitParams(double m_BallPosExpt, double Kp, double Ki, double Kd, double TNow)
{
	this->m_BallPosExpt = m_BallPosExpt;
	this->Kp = Kp;
	this->Ki = Ki;
	this->Kd = Kd;
	this->TLast = TNow;

	this->Err = 0;
	this->ErrLast = 0;
	this->ErrSum = 0;
}

void MotionControl::InitMotor(void)
{
	int sRtn = 0;
	
	sRtn = GT_Close();
	if (CommandHandler("GT_Close", sRtn))
		return;

	sRtn = GT_Open();
	if (CommandHandler("GT_Open", sRtn))
		return;

	sRtn = GT_Reset();
	if (CommandHandler("GT_Reset", sRtn))
		return;
	Sleep(500);

	sRtn = GT_LoadConfig("GTS800.cfg");
	if (CommandHandler("GT_LoadConfig", sRtn))
		return;

	sRtn = GT_ClrSts(1, 2);
	if (CommandHandler("GT_ClrSts", sRtn))
		return;

	TPid pid;

	sRtn = GT_GetPid(1, 1, &pid);
	if (CommandHandler("GT_GetPid", sRtn))
		return;

	pid.kp = 0.85;
	pid.ki = 0.00;
	pid.kd = 0.04;

	sRtn = GT_SetPid(1, 1, &pid);
	if (CommandHandler("GT_SetPid", sRtn))
		return;


	sRtn = GT_GetPid(2, 1, &pid);
	if (CommandHandler("GT_GetPid", sRtn))
		return;

	pid.kp = 0.85;
	pid.ki = 0.00;
	pid.kd = 0.04;

	sRtn = GT_SetPid(2, 1, &pid);
	if (CommandHandler("GT_SetPid", sRtn))
		return;
}

int MotionControl::CommandHandler(CString str, int error)
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

void MotionControl::ControlUpdate(double m_BallPos, double TNow)
{
	this->TLast = this->TNow;
	this->TNow = TNow;
	this->T = this->TNow - this->TLast;

	if (T > 200) {
		Output = 0;
		return;
	}

	ErrLast = Err;
	Err = m_BallPosExpt - m_BallPos;
	ErrSum += Err;

	P = Kp * Err;
	I = Ki * T * ErrSum;
	D = Kd / T * (Err - ErrLast);

	I = min(fabs(I), 1500) * (I > 0 ? 1 : -1) * (Err > 10 ? 0 : 1);

	Output = P + I + D;
	Output = min(fabs(Output), 1500) * (Output > 0 ? 1 : -1);
}