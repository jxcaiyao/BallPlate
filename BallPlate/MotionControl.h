#pragma once

#include "Eigen/Dense"
#include "gts.h"
#include <math.h>

class MotionControl
{
public:
	double m_BallPosExpt;

	double Kp;
	double Ki;
	double Kd;
	double TLast;
	double TNow;
	double T;

	double Err;
	double ErrSum;
	double ErrLast;

	double P;
	double I;
	double D;

	double coffFilter;
	double Output;

	MotionControl();
	~MotionControl();

	void InitParams(double m_BallPosExpt, double Kp, double Ki, double Kd, double TNow, double coffFilter);
	void InitMotor(void);
	int CommandHandler(CString str, int error);
	void ControlUpdate(double m_BallPos, double TNow);
};

