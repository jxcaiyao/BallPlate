#include "pch.h"
#include <iostream>
#include "MyCamera.h"

MyCamera::MyCamera() {};
MyCamera::~MyCamera() {};

int MyCamera::caliImage(cv::Mat img_raw) {

	short rtn;
	cv::Mat img_proc = img_raw.clone();
	cv::Mat img_disp;

	//得到大致圆盘区域
	double proc_area[3];
	rtn = getProcArea(img_proc, proc_area);
	if (rtn) {
		std::cout << "Bad parameters for getProcArea!" << std::endl;
		return -1;
	}
	this->center = cv::Point(cvRound(proc_area[0]), cvRound(proc_area[1]));
	this->radius = cvRound(proc_area[2]);

	//截取圆盘图像
	cv::Mat img_clp;
	this->mask = cv::Mat::zeros(img_proc.size(), CV_8UC1);
	cv::circle(this->mask, this->center, this->radius, cv::Scalar(255), -1);
	img_proc.copyTo(img_clp, this->mask);

	//图像二值化
	//cv::threshold(img_clp, img_clp, 200, 255, cv::THRESH_BINARY);

	//求取标定点坐标
	std::vector<cv::Vec3f> circles_cali;
	rtn = getCaliPnt(img_clp, circles_cali, 30, 200, 10, 0, this->radius / 30);
	if (rtn) {
		std::cout << "Bad parameters for getCaliPnt!" << std::endl;
		return -1;
	}
	//img_disp = img_clp.clone();
	//for (int i = 0; i < circles_cali.size();i++) {
	//	cv::Point p_center = cv::Point(cvRound(circles_cali[i][0]), cvRound(circles_cali[i][1]));
	//	cv::circle(img_disp, p_center, 5, cv::Scalar(127), 3);
	//}
	//cv::imshow("Proc Image", img_disp);
	//cv::waitKey(0);

	//预估变换矩阵将实际标定点转为预估标定点在图像中的位置
	double k = this->radius / 140.0 / sqrt(2);

	trans << k, k, center.x,
			 k,-k, center.y,
			 0, 0, 1;

	Eigen::MatrixXd pnt_exp(3, 9);
	pnt_exp <<	-120,	-60,	0,		-60,	0,		60,		0,		60,		120,
				0,		60,		120,	-60,	0,		60,		-120,	-60,	0,
				1,		1,		1,		1,		1,		1,		1,		1,		1;

	Eigen::MatrixXd pnt_tru(3, circles_cali.size());
	for (int i = 0; i < (int)circles_cali.size();i++) {
		pnt_tru(0, i) = (double)circles_cali[i][0];
		pnt_tru(1, i) = (double)circles_cali[i][1];
		pnt_tru(2, i) = 1;
	}

	//匹配特征点，除去距离过大的点
	Eigen::MatrixXd pnt_tru_match, pnt_exp_match;
	pnt_tru_match = pnt_tru;
	pnt_exp_match = pnt_exp;

	rtn = findNearest(pnt_tru_match, pnt_exp_match);
	if (rtn) {
		std::cout << "findNearest false!" << std::endl;
		return -1;
	}

	if (pnt_tru_match.cols() < 6) {
		std::cout << "Not enough match point!" << std::endl;
		return -1;
	}

	//img_disp = img_clp.clone();
	//cv::Point pt1, pt2;
	//for (int i = 0;i < pnt_tru_match.cols();i++) {
	//	pt1.x = cvRound(pnt_tru_match(0, i));
	//	pt1.y = cvRound(pnt_tru_match(1, i));
	//	pt2.x = cvRound(pnt_exp_match(0, i));
	//	pt2.y = cvRound(pnt_exp_match(1, i));

	//	cv::line(img_disp, pt1, pt2, cv::Scalar(127), 2);
	//}
	//cv::imshow("Proc Image", img_disp);
	//cv::waitKey(0);

	//std::cout << pnt_tru_match << std::endl;

	//求取精确变换矩阵
	rtn = getBestTrans(pnt_tru_match, pnt_exp_match);
	if (rtn) {
		std::cout << "getBestTrans false!" << std::endl;
		return -1;
	}
	//cv::Point center_acu;
	//center_acu.x = cvRound(this->trans(0, 2));
	//center_acu.y = cvRound(this->trans(1, 2));
	//img_disp = img_clp.clone();
	//cv::circle(img_disp, center_acu, 3, cv::Scalar(127), 3);
	//cv::imshow("Proc Image", img_disp);
	//cv::waitKey(0);

	this->center.x = cvRound(this->trans(0, 2));
	this->center.y = cvRound(this->trans(1, 2));
	this->radius = cvRound((this->trans.block(0, 0, 2, 2) * Eigen::Vector2d(150, 0)).norm());
	this->mask = cv::Mat::zeros(img_proc.size(), CV_8UC1);
	cv::circle(this->mask, this->center, this->radius, cv::Scalar(255), -1);

	//cv::Mat img_clp_acu;
	//img_proc.copyTo(img_clp_acu, mask);
	//cv::imshow("Proc Image", img_clp_acu);
	//cv::waitKey(0);
	
	return 0;
}

int MyCamera::getBallPosition(Eigen::Vector2d &ball_pos, cv::Mat img_raw) {

	cv::Mat img_clp;
	img_raw.copyTo(img_clp, this->mask);

	//求取小球相对圆盘坐标
	std::vector<cv::Vec3f> circle_ball;
	if (getBallPnt(img_clp, circle_ball, 100, 200, 50, this->radius / 10, this->radius / 7)) {
		std::cout << "No ball detected!" << std::endl;
		return 0;
	}
	Eigen::Vector3d ball_vec1, ball_vec2;
	ball_vec1(0) = circle_ball[0][0];
	ball_vec1(1) = circle_ball[0][1];
	ball_vec1(2) = 1;

	//cv::Mat img_disp = img_raw.clone();
	//cv::circle(img_disp, cv::Point(ball_vec1(0), ball_vec1(1)), 2, cv::Scalar(0), 3);
	//cv::imshow("Proc Image", img_disp);
	//cv::waitKey(0);

	ball_vec2 = trans.inverse() * ball_vec1;

	ball_pos(0) = ball_vec2(0);
	ball_pos(1) = ball_vec2(1);

	return 0;
}

int MyCamera::drawPoint(cv::Mat img_raw, cv::Mat &img_disp, Eigen::Vector2d p_world, cv::Scalar color, int radius, int thickness) {
	img_disp = img_raw.clone();
	cv::Point center;
	center.x = cvRound(this->trans(0, 0) * p_world(0) + this->trans(0, 1)*p_world(1) + this->trans(0, 2));
	center.y = cvRound(this->trans(1, 0) * p_world(0) + this->trans(1, 1)*p_world(1) + this->trans(1, 2));
	cv::circle(img_disp, center, radius, color, thickness);

	return 0;
}

int MyCamera::getMask(cv::Mat& mask) {
	mask = this->mask.clone();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MyCamera::getProcArea(cv::Mat img_raw, double proc_area[3], double mindist, double param1, double param2, double minradius, double maxradius) {

	int k = 10;
	cv::Mat img_proc = img_raw.clone();
	cv::resize(img_proc, img_proc, cv::Size(img_proc.cols / k, img_proc.rows / k));

	std::vector<cv::Vec3f> circles;
	while (true) {
		cv::HoughCircles(img_proc, circles, cv::HOUGH_GRADIENT, 1, mindist, param1, param2 / k, (int)(minradius / k), (int)(maxradius / k));

		if (circles.size() == 1) {
			break;
		}
		else if (circles.size() == 0) {
			param2 /= 1.1;
			printf("No circle detected! param2 = %.1f\n", param2);
		}
		else {
			param2 *= 1.05;
			printf("Too many circles detected! param2 = %.1f\n", param2);
		}
		if (param2 <= 5 || param2 >= 300) {
			return -1;
		}
	}

	proc_area[0] = circles[0][0] * k;
	proc_area[1] = circles[0][1] * k;
	proc_area[2] = circles[0][2] * k;

	return 0;
}

int MyCamera::getCaliPnt(cv::Mat img_clp, std::vector<cv::Vec3f> &circles, double mindist, double param1, double param2, int minRadius, int maxRadius) {

	int minCircles = 6;
	int maxCircles = 9;

	cv::Mat img_proc;
	cv::GaussianBlur(img_clp, img_proc, cv::Size(3, 3), 0, 0);

	int maxiter = 20;
	int times = 0;
	while (true) {
		cv::GaussianBlur(img_proc, img_proc, cv::Size(3, 3), 0, 0);
		cv::HoughCircles(img_proc, circles, cv::HOUGH_GRADIENT, 1, mindist, param1, param2, minRadius, maxRadius);

		if ((int)circles.size() >= minCircles && (int)circles.size() <= maxCircles) {
			break;
		}
		else if ((int)circles.size() < minCircles) {
			param2 /= 1.1;
			printf("Not enough point detected! param2 = %.1f\n", param2);
		}
		else if ((int)circles.size() > maxCircles) {
			param2 *= 1.05;
			printf("Too many points detected! param2 = %.1f\n", param2);
		}
		if (param2 <= 3 || param2 >= 200) {
			return -1;
		}
		times++;
		if (times > maxiter) {
			printf("getCaliPnt max iteration!\n");
			return -1;
		}
	}

	return 0;
}

int MyCamera::getBallPnt(cv::Mat img_clp, std::vector<cv::Vec3f> &ball_pos, double mindist, double param1, double param2, int minRadius, int maxRadius) {

	int k = 10;
	cv::Mat img_proc = img_clp.clone();
	cv::resize(img_proc, img_proc, cv::Size(img_clp.cols / k, img_clp.rows / k));
	cv::threshold(img_proc, img_proc, 180, 255, cv::THRESH_BINARY);

	int sum = 0;
	int times = 0;
	int maxiter = 5;
	while (true) {
		cv::HoughCircles(img_proc, ball_pos, cv::HOUGH_GRADIENT, 1, mindist / k, param1, param2 / k, minRadius / k, maxRadius / k);
		if (ball_pos.size() == 0) {
			printf("HoughCircles not find ball!\n");
			param2 /= 1.2;
			printf("Too many ball found! param2:%.1f\n", param2);
			//return -1;
		}
		else if (ball_pos.size() == 1) {
			ball_pos[0][0] *= k;
			ball_pos[0][1] *= k;
			ball_pos[0][2] = (float)(1.5*k*ball_pos[0][2] + k);
			break;
		}
		else {
			param2 *= 1.1;
			printf("Too many ball found! param2:%.1f\n", param2);
		}
		if (++times > maxiter) {
			printf("No Ball found!\n");
			return -1;
		}
	}

	//cv::Mat img_disp = img_clp.clone();
	//cv::circle(img_disp, cv::Point(ball_pos[0][0], ball_pos[0][1]), 2, cv::Scalar(0), 2);
	//cv::imshow("Proc Image", img_disp);
	//cv::waitKey(0);

	int x, y, width, height;
	x = cvRound(ball_pos[0][0] - ball_pos[0][2]);
	if (x < 0)
		x = 0;
	else if (x >= img_clp.cols)
		x = img_clp.cols - 1;

	y = cvRound(ball_pos[0][1] - ball_pos[0][2]);
	if (y < 0)
		y = 0;
	else if (y >= img_clp.rows)
		y = img_clp.rows - 1;

	width = cvRound(2 * ball_pos[0][2]);
	if (x + width < 0)
		width = 0;
	else if (x + width > img_clp.cols)
		width = img_clp.cols - 1 - x;

	height = cvRound(2 * ball_pos[0][2]);
	if (y + height < 0)
		height = 0;
	else if (y + height > img_clp.rows)
		height = img_clp.rows - 1 - y;

	cv::Rect rect(x, y, width, height);

	cv::Mat img_ballclp;
	img_ballclp = img_clp(rect);
	cv::threshold(img_ballclp, img_ballclp, 160, 255, cv::THRESH_BINARY);

	//cv::imshow("Proc Image", img_ballclp);
	//cv::waitKey(0);

	std::vector<cv::Vec3f> temp_pos;
	temp_pos.push_back(cv::Vec3f(0, 0, 0));
	sum = 0;
	for (int i = 0;i < img_ballclp.cols; i++) {
		for (int j = 0;j < img_ballclp.rows;j++) {
			if (img_ballclp.at<uchar>(j, i) == 255) {
				temp_pos[0][0] += i;
				temp_pos[0][1] += j;
				sum++;
			}
		}
	}
	temp_pos[0][0] /= sum;
	temp_pos[0][1] /= sum;

	ball_pos[0][0] = x + temp_pos[0][0];
	ball_pos[0][1] = y + temp_pos[0][1];
	ball_pos[0][2] = temp_pos[0][2];

	return 0;
}

void MyCamera::removeColumn(Eigen::MatrixXd& matrix, unsigned int colToRemove)
{
	unsigned int numRows = (unsigned int)matrix.rows();
	unsigned int numCols = (unsigned int)matrix.cols() - 1;

	if (colToRemove < numCols)
		matrix.block(0, colToRemove, numRows, numCols - colToRemove) = matrix.block(0, colToRemove + 1, numRows, numCols - colToRemove);

	matrix.conservativeResize(numRows, numCols);
}

int MyCamera::findNearest(Eigen::MatrixXd &p_set1, Eigen::MatrixXd &p_set2, double thres) {

	Eigen::MatrixXd p_mat1 = Eigen::MatrixXd::Zero(p_set1.rows(), p_set1.cols());
	Eigen::MatrixXd p_mat2 = Eigen::MatrixXd::Zero(p_set2.rows(), p_set2.cols());

	int match_num = 0;

	double dist, mindist;
	unsigned int minindex;
	for (int i = 0; i < p_set1.cols(); ) {
		mindist = (p_set1.col(i) - this->trans * p_set2.col(0)).norm();
		minindex = 0;
		for (int j = 1;j < p_set2.cols(); j++) {

			dist = (p_set1.col(i) - this->trans * p_set2.col(j)).norm();
			if (dist < mindist) {
				mindist = dist;
				minindex = j;
			}
		}
		if (mindist < thres) {
			p_mat1.col(match_num) = p_set1.col(i);
			p_mat2.col(match_num) = p_set2.col(minindex);
			match_num++;

			removeColumn(p_set1, i);
			removeColumn(p_set2, minindex);
		}
		else {
			i++;
		}
	}

	p_set1 = p_mat1;
	p_set2 = p_mat2;

	p_set1.conservativeResize(3, match_num);
	p_set2.conservativeResize(3, match_num);

	return 0;
}

int MyCamera::getBestTrans(Eigen::MatrixXd p_view, Eigen::MatrixXd p_world) {

	int n = (int)p_world.cols();
	if (n != p_view.cols()) {
		printf("Columns not equal\n");
		return -1;
	}

	double sx_w = p_world.row(0).sum();
	double sx2_w = p_world.row(0).squaredNorm();
	double sy_w = p_world.row(1).sum();
	double sy2_w = p_world.row(1).squaredNorm();
	double sxy_ww = p_world.row(0).dot(p_world.row(1));

	double sx_v = p_view.row(0).sum();
	double sxx_vw = p_view.row(0).dot(p_world.row(0));
	double sxy_vw = p_view.row(0).dot(p_world.row(1));
	double sy_v = p_view.row(1).sum();
	double syy_vw = p_view.row(1).dot(p_world.row(1));
	double sxy_wv = p_world.row(0).dot(p_view.row(1));

	Eigen::Matrix3d A;
	Eigen::Vector3d b1, b2;
	Eigen::Vector3d t1, t2;

	A << sx2_w, sxy_ww, sx_w,
		sxy_ww, sy2_w, sy_w,
		sx_w, sy_w, n;

	b1 << sxx_vw,
		sxy_vw,
		sx_v;

	b2 << sxy_wv,
		syy_vw,
		sy_v;

	t1 = A.inverse() * b1;
	t2 = A.inverse() * b2;

	//std::cout << Trans << std::endl;

	this->trans.row(0) = t1.transpose();
	this->trans.row(1) = t2.transpose();

	//std::cout << Trans << std::endl;

	return 0;
}