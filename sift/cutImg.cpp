#include "opencv2\highgui\highgui_c.h"
#include "opencv2\highgui\highgui.hpp"
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv\cv.h"

#include "features2d.hpp"
#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;



#define RGB_THRESHOLD		(200)				// ����ɫ��ֵ
#define SINGLE_THRESHOLD	(100)				// ��ɫ��ֵ
#define SCAN_NUM			(6)					// ������ɨ�������
#define ROW_COL_DIVISION	(50)				// ���з���
#define NOICE_POINTS_NUM	(3)					// �����

// remove the black border on both right and left side  
// src: input image.  
// dst: output image. 
int cuteEdge(const cv::Mat& src, cv::Mat& dst)
{
	//imshow("2222", src);
	int totalRow = src.rows;  // �������߶�
	int totalCol = src.cols;  // ���������

	int scanNum = 6; // �Ż��е�ɨ������
	int scan = 0, i = 0; 

	int rowDivision = totalRow/ROW_COL_DIVISION; // �ȷ��и���ڲ���������ͼƬ��Ե���
	int colDivision = totalCol/ROW_COL_DIVISION;

	int curRow = 0; // ��ǰ�������
	int curCol = 0; // ��ǰ�������
	int leftPoint = 0;
	int rightPoint = 0;
	int upPoint = 0;
	int downPoint = 0;
	int r = 0, g = 0, b = 0;
	int leftCut = 0;
	int rightCut = 0;
	int upCut = 0;
	int downCut = 0;
	int err = 0;

	for (scan = 0; scan < scanNum; scan++)
	{
		if (scan < scanNum/3) // ǰ�Ż�����
		{
			curRow += rowDivision;
			curCol += colDivision;
		}
		else if (scan < (scanNum*2)/3) // �м��Ż��м���
		{
			curRow = (curRow < (totalRow-1)/2 - rowDivision) ? ((totalRow-1)/2 - rowDivision) : (curRow + rowDivision);
			curCol = (curCol < (totalCol-1)/2 - colDivision) ? ((totalCol-1)/2 - colDivision) : (curCol + colDivision);

		}
		else // ���Ż�����
		{
			curRow = (curRow < (totalRow-1) - 2*rowDivision) ? ((totalRow-1) - 2*rowDivision) : (curRow + rowDivision);
			curCol = (curCol < (totalCol-1) - 2*colDivision) ? ((totalCol-1) - 2*colDivision) : (curCol + colDivision);
		}

		// ��ÿ�ŵ������ص���й����б�
		leftPoint = 0;  
		err = 0;

		for (i = colDivision; i <= totalCol - colDivision - 1; i++) // ɨ��ÿ�ŵ����ص㣬iΪ���ص��������
		{
			r = src.at<Vec3b>(curRow, i)[0];
			g = src.at<Vec3b>(curRow, i)[1];
			b = src.at<Vec3b>(curRow, i)[2];

			if ((r + g + b <= RGB_THRESHOLD) || (r < SINGLE_THRESHOLD) && (g < SINGLE_THRESHOLD) && (b < SINGLE_THRESHOLD))
			{	
				leftPoint = i;  
			}
			else
			{				
				if (err++ > NOICE_POINTS_NUM)
				{
					break; // ���������㲻������ֵ���˳���
				}
			}
		}
		//cout<<"left stop, "<<"curRow: "<<curRow<<" curCol:"<<i<<" rgb:"<<r<<" "<<g<<" "<<b<<" RGB_THRESHOLD"<<RGB_THRESHOLD<<endl;
		//cout<<"leftPoint:"<<leftPoint<<endl<<endl;
		if ((leftCut == 0) || (leftCut > leftPoint))
		{
			leftCut = leftPoint; // �洢��̹��˵�
		}

		// ��ÿ�ŵ������ص���й����б�
		rightPoint = totalCol-1;
		err = 0;		

		for (i = totalCol - colDivision - 1; i >= colDivision; i--) // ɨ��ÿ�ŵ����ص㣬iΪ���ص��������
		{
			r = src.at<Vec3b>(curRow, i)[0];
			g = src.at<Vec3b>(curRow, i)[1];
			b = src.at<Vec3b>(curRow, i)[2];

			if ((r + g + b <= RGB_THRESHOLD) || (r < SINGLE_THRESHOLD) && (g < SINGLE_THRESHOLD) && (b < SINGLE_THRESHOLD))
			{	
				rightPoint = i;  
			}
			else
			{				
				if (err++ > NOICE_POINTS_NUM)
				{
					break; // ���������㲻������ֵ���˳���
				}
			}
		}
		//cout<<"right stop, "<<"curRow: "<<curRow<<" curCol:"<<i<<" rgb:"<<r<<" "<<g<<" "<<b<<" RGB_THRESHOLD"<<RGB_THRESHOLD<<endl;
		//cout<<"rightPoint:"<<rightPoint<<endl<<endl;
		if ((rightCut == 0) || (rightCut < rightPoint))
		{
			rightCut = rightPoint; // �洢��̹��˵�
		}


		// ��ÿ�е������ص���й����б�
		upPoint = 0;  
		err = 0;

		for (i = rowDivision; i <= totalRow - rowDivision - 1; i++)  // ɨ��ÿ�е����ص㣬iΪ���ص��������
		{
			r = src.at<Vec3b>(i, curCol)[0];
			g = src.at<Vec3b>(i, curCol)[1];
			b = src.at<Vec3b>(i, curCol)[2];

			if ((r + g + b <= RGB_THRESHOLD) || (r < SINGLE_THRESHOLD) && (g < SINGLE_THRESHOLD) && (b < SINGLE_THRESHOLD))
			{	
				upPoint = i;  
			}
			else
			{				
				if (err++ > NOICE_POINTS_NUM)
				{
					break; // ���������㲻������ֵ���˳���
				}
			}
		}
		//cout<<"up stop, "<<"curRow: "<<i<<" curCol:"<<curCol<<" rgb:"<<r<<" "<<g<<" "<<b<<" RGB_THRESHOLD"<<RGB_THRESHOLD<<endl;
		//cout<<"upPoint:"<<upPoint<<endl<<endl;
		if ((upCut == 0) || (upCut > upPoint))
		{
			upCut = upPoint; // �洢��̹��˵�
		}

		// ��ÿ�е������ص���й����б�
		downPoint = totalRow - 1;
		err = 0;		

		for (i = totalRow - rowDivision - 1; i >= rowDivision; i--) // ɨ��ÿ�е����ص㣬iΪ���ص��������
		{
			r = src.at<Vec3b>(i, curCol)[0];
			g = src.at<Vec3b>(i, curCol)[1];
			b = src.at<Vec3b>(i, curCol)[2];

			if ((r + g + b <= RGB_THRESHOLD) || (r < SINGLE_THRESHOLD) && (g < SINGLE_THRESHOLD) && (b < SINGLE_THRESHOLD))
			{	
				downPoint = i;  
			}
			else
			{				
				if (err++ > NOICE_POINTS_NUM)
				{
					break; // ���������㲻������ֵ���˳���
				}
			}
		}
		//cout<<"down stop, "<<"curRow: "<<i<<" curCol:"<<curCol<<" rgb:"<<r<<" "<<g<<" "<<b<<" RGB_THRESHOLD"<<RGB_THRESHOLD<<endl;
		//cout<<"downPoint:"<<downPoint<<endl<<endl;
		if ((downCut == 0) || (downCut < downPoint))
		{
			downCut = downPoint; // �洢��̹��˵�
		}
	}

	cv::Rect roi;
	roi.x = 0;
	roi.y = 0;
	roi.width = totalCol;
	roi.height = totalRow;

	if (rightCut - leftCut > totalCol/2) // �и�Ŀ�ȣ�����̫С
	{
		//cout<<"left: "<<leftCut <<endl;  
		//cout<<"right:"<<rightCut<<endl;  
		roi.x = leftCut;
		roi.width = rightCut - leftCut;
	}	
	if (downCut - upCut > totalRow/2) // �и�ĸ߶ȣ�����̫С
	{
		//cout<<"up: "<<upCut <<endl;  
		//cout<<"down:"<<downCut<<endl;  
		roi.y = upCut;
		roi.height = downCut - upCut;
	}
	dst = (src)(roi);
	//imshow("SRC", src);
	//imshow("ROI", dst);
	//waitKey(1000000);
	return 0;
}

