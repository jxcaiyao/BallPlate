#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <Eigen/Dense>

//负责相机标定与小球位置测算
class MyCamera
{
public:
	MyCamera();
	~MyCamera();

	int caliImage(cv::Mat img_raw);
	int getBallPosition(Eigen::Vector2d &ball_pos, cv::Mat img_raw);
	int drawPoint(cv::Mat img_raw, cv::Mat &img_disp, Eigen::Vector2d p_world, cv::Scalar color = cv::Scalar(127),int radius = 2,int thickness = 3);
	int getMask(cv::Mat &mask);

private:
	cv::Point center;
	int radius;
	cv::Mat mask;
	Eigen::Matrix3d trans;

	int getProcArea(cv::Mat img_raw, double proc_area[3], double mindist = 100, double param1 = 220, double param2 = 100, double minradius = 200, double maxradius = 400);
	int getCaliPnt(cv::Mat img_clp, std::vector<cv::Vec3f> &circles, double mindist = 50, double param1 = 200, double param2 = 10, int minRadius = 0, int maxRadius = 20);
	int getBallPnt(cv::Mat img_clp, std::vector<cv::Vec3f> &ball_pos, double mindist = 50, double param1 = 200, double param2 = 50, int minRadius = 20, int maxRadius = 40);
	void removeColumn(Eigen::MatrixXd& matrix, unsigned int colToRemove);
	int findNearest(Eigen::MatrixXd &p_set1, Eigen::MatrixXd &p_set2, double thres = 50);
	int getBestTrans(Eigen::MatrixXd p_view, Eigen::MatrixXd p_world);

};