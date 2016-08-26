#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv\cv.h"
#include <math.h>

using namespace cv;
using namespace std;

const int BLOCK_NUM = 13;//�ֿ���
const int TOP_NUM = (int)BLOCK_NUM/3;
int width = 0;
int height = 0;
int column_width = 0;//�п�
int raw_height = 0;

 int* bSums(Mat src)  
{  
      
    int* counter =new int[2];
	counter[0] = 0;//��ɫ���ص�
	counter[1] = 0;//��ɫ���ص�
    //�������������ص�  
    Mat_<uchar>::iterator it = src.begin<uchar>();  
    Mat_<uchar>::iterator itend = src.end<uchar>();    
    for (; it!=itend; ++it)  
    {  
        if((*it)>0) counter[0]+=1;//��ֵ�������ص���0����255 
		else counter[1]+=1;
    }             
    return counter;  
}  

void detectLine(Mat imgThresholded,int** output)//����任���ֱ��  
{    
	IplImage ipl_img(imgThresholded);
	IplImage* src = &ipl_img; 
    IplImage* dst;  
    IplImage* color_dst;  
    CvMemStorage* storage = cvCreateMemStorage(0);  
    CvSeq* lines = 0;  
    int i;  
    dst = cvCreateImage( cvGetSize(src), 8, 1 );  
    color_dst = cvCreateImage( cvGetSize(src), 8, 3 );  
    cvCanny( src, dst, 50, 200, 3 );//��Ե���  
    cvCvtColor( dst, color_dst, CV_GRAY2BGR );//��ɫ�ռ�ת����BGR->�Ҷ�ͼ��   
    lines = cvHoughLines2(dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 28, 40, 20);//����任���ֱ��  
    for (i = 0; i < lines->total; i++)  
    {  
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);  
        cvLine(color_dst, line[0], line[1], CV_RGB(255, 0, 0), 3, CV_AA, 0); 
		int x0 = line[0].x;
		int y0 = line[0].y;
		int x1 = line[1].x;
		int y1 = line[1].y;
		float slope = 0.0f;
		//cout<<"x0:"<<x0<<" ";
		//cout<<"y0:"<<y0<<" ";
		//cout<<"x1:"<<x1<<" ";
		//cout<<"y1:"<<y1<<endl;
		if(x1==x0){
			int column_id = x1/column_width;
			int y = 0;
			if(y1>y0){
				y = y0;
			    while(y1>y){
                    int raw_id = y/raw_height;
					if(raw_id >= TOP_NUM && raw_id<BLOCK_NUM)
						output[raw_id][column_id] = 1;
				    y += column_width/2;
			    }
			}
			if(y0>y1){
				y = y1;
                while(y0>y){
                    int raw_id = y/raw_height;
					if(raw_id >= TOP_NUM && raw_id<BLOCK_NUM)
					{
						cout<<"raw_id:"<<raw_id<<" column_id:"<<column_id<<endl;
						cout<<"y0"<<y0<<" y:"<<y<<endl;
						cout<<"column_width:"<<column_width<<" raw_height:"<<raw_height<<endl;
						output[raw_id][column_id] = 1;
					}
				    y += column_width/2;
			    }
			}
		}
		else if(y1==y0){
			int raw_id = y1/raw_height;
			if(raw_id<TOP_NUM)
				continue;
			int x = 0;
			if(x1>x0){
				x = x0;
			    while(x1>x){
                    int column_id = x/column_width;
					cout<<"x:"<<x<<endl;
					cout<<"raw_id:"<<raw_id<<" column_id:"<<column_id<<endl;
					if(raw_id>BLOCK_NUM)
						raw_id = BLOCK_NUM - 1;
				    output[raw_id][column_id] = 1;
				    x += column_width/2;
				}
			}
			if(x0>x1){
				x = x1;
			    while(x0>x){
                    int column_id = x/column_width;
					cout<<"x:"<<x<<endl;
					cout<<"raw_id:"<<raw_id<<" column_id:"<<column_id<<endl;
					if(raw_id>BLOCK_NUM)
						raw_id = BLOCK_NUM - 1;
				    output[raw_id][column_id] = 1;
				    x += column_width/2;
			    }
			}
		}
		else if(y1>y0){
			slope = (float)(y1-y0)/(x1-x0);
			int x = x0 ;
			float y = y0;
			while(y<y1){
				int column_id = x/column_width;
                int raw_id = (int)y/raw_height;
				if(raw_id >= TOP_NUM && raw_id<BLOCK_NUM)
				{
					cout<<"raw_id:"<<raw_id<<" column_id:"<<column_id<<endl;
					cout<<"x:"<<x<<" y:"<<y<<endl;
					cout<<"x0:"<<x0<<" y0:"<<y0<<endl;
					cout<<"column_width:"<<column_width<<" raw_height:"<<raw_height<<endl;
				    output[raw_id][column_id] = 1;
				}
				if(slope>0){
				    x += column_width/2;
				    y += (float)column_width/2*slope;
			    }
		        else if(slope<0){
					x -= column_width/2;
					y -= (float)column_width/2*slope;
				}
			}
		}
		else{
			slope = (float)(y0-y1)/(x0-x1);
			int x = x1;
			float y = y1;
			while(y<y0){
				int column_id = x/column_width;
                int raw_id = (int)y/raw_height;
				if(raw_id >= TOP_NUM && raw_id<BLOCK_NUM)
				    output[raw_id][column_id] = 1;
				if(slope>0){
				    x += column_width/2;
				    y += (float)column_width/2*slope;
			    }
				else if(slope<0){
					x -= column_width/2;
				    y -= (float)column_width/2*slope;
				}
			}
		}
    }
  
    cvNamedWindow( "Source", 1 );  
    cvShowImage( "Source", src );  
    cvNamedWindow( "Hough", 1 );  
    cvShowImage( "Hough", color_dst );  
  
}  

 void color_detect(int** output, cv::Mat imgOriginal)//����ɫ�ࡢ���Ͷȡ����ȶ�ͼ����й��ˣ���ת���ɶ�ֵͼ��
 {

  int iLowH = 42;
  int iHighH = 179;

  int iLowS = 49; 
  int iHighS = 255;

  int iLowV = 34;
  int iHighV = 255;


   Mat imgHSV;
   vector<Mat> hsvSplit;
   cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //����Ƭ��RBGģ��ת����HSVģ��

   //��Ϊ���Ƕ�ȡ���ǲ�ɫͼ��ֱ��ͼ���⻯��Ҫ��HSV�ռ���
   split(imgHSV, hsvSplit);
   equalizeHist(hsvSplit[2],hsvSplit[2]);
   merge(hsvSplit,imgHSV);
   Mat imgThresholded;

   //���imgHSVͼ���ÿһ�������ǲ�����lowerb��upperb֮�䣬����ǣ�������ؾ�����Ϊ255����������imgThresholdedͼ���У�����Ϊ0
   inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); 

   //������ (ȥ��һЩ���)
   Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
   morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);

   //�ղ��� (����һЩ��ͨ��)
   morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);

   imshow("Thresholded Image", imgThresholded); //show the thresholded image
   imshow("Original", imgOriginal); //show the original image


   //what i do
   width = imgThresholded.size().width;
   height = imgThresholded.size().height;
   column_width = width/BLOCK_NUM;//�п�
   raw_height = height/BLOCK_NUM;//�и�
   int i = 0, j = 0;
   /*Mat roiImage;
   for(i = 0;i< BLOCK_NUM;i++){//��
	   for(j = TOP_NUM;j< BLOCK_NUM;j++){//��
		   if(i==BLOCK_NUM-1){//���һ��
		       Rect rect(i*column_width,j*raw_height,width-i*column_width,raw_height);//rect(x,y,width,height)
			   imgThresholded(rect).copyTo(roiImage);
		   }
		   else if(j==BLOCK_NUM-1){//���һ��
			   Rect rect(i*column_width,j*raw_height,column_width,height-j*raw_height);
			   imgThresholded(rect).copyTo(roiImage);
		   }
		   else{
			   Rect rect(i*column_width,j*raw_height,column_width,raw_height);
			   imgThresholded(rect).copyTo(roiImage);
		   }
           //imshow("roi",roiImage);
           int* count = bSums(roiImage);
		   if(count[0] > 0 && count[1] > 0)
			   output[j][i] = 1;
	   }
   }*/
   detectLine(imgThresholded,output);//���ֱ�߱�Ե

   for(i=1; i<BLOCK_NUM; i++){//��ͼƬ�ֳ�BLOCK_NUM��BLOCK_NUM��С���ο�
		line(imgThresholded,Point(0,i*raw_height),Point(width,i*raw_height),Scalar(255,0,255),3);
        line(imgThresholded,Point(i*column_width,0),Point(i*column_width,height),Scalar(255,0,255),3);
	}
   imshow("imgThresholded", imgThresholded);
   
   //what i do
}

