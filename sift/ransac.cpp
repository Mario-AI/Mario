/*
 *@function SiftDetect.cpp
 *@brief ��sift��������ƥ����в��ԣ���ʵ��RANSAC�㷨���й��˴����
 *@author ltc
 *@date 11:20 Saturday��28 November��2015
 */
#include "opencv2\highgui\highgui_c.h"
#include "opencv2\highgui\highgui.hpp"
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "features2d.hpp"
#include<iostream>
#include <string>
#include<opencv2\opencv.hpp>
#include "colorDetect.h"
#include <stdlib.h>
#include <io.h>
using namespace std;
using namespace cv;

const int BLOCK_NUM = 13;//�ֿ���
const int TOP_NUM = (int)BLOCK_NUM/3;
const int MIN_NUM = 2;
const char* AIM_DIR_PATH = "\\aim";

//RANSAC�㷨
vector<DMatch> ransac(vector<DMatch> matches,vector<KeyPoint> queryKeyPoint,vector<KeyPoint> trainKeyPoint);

//��ȡ���е��ļ���  
void GetAllFilesPath( string path, vector<string>& files)    
{    
  
    long   hFile   =   0;    
    //�ļ���Ϣ    
    struct _finddata_t fileinfo;//�����洢�ļ���Ϣ�Ľṹ��    
    string p;    
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  //��һ�β���  
    {    
        do    
        {     
            if((fileinfo.attrib &  _A_SUBDIR))  //������ҵ������ļ���  
            {    
                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  //�����ļ��в���  
                {  
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
                    GetAllFilesPath( p.assign(path).append("\\").append(fileinfo.name), files );   
                }  
            }    
            else //������ҵ��Ĳ������ļ���   
            {    
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));  //���ļ�·�����棬Ҳ����ֻ�����ļ���:    p.assign(path).append("\\").append(fileinfo.name)  
            }   
  
        }while(_findnext(hFile, &fileinfo)  == 0);    
  
        _findclose(hFile); //��������  
    }   
  
}

int** sift(int** output, cv::Mat img2)
{
	vector<string> files ;
    GetAllFilesPath(AIM_DIR_PATH,files);

	//ͼ���ȡ
	vector<Mat> img1;//Ŀ��С��
	//Mat img2;
	int k=0;
	for(k =0;k<files.size();k++)
		img1.push_back(imread(files[k],CV_WINDOW_AUTOSIZE));
	//img2=pic;

	if(img1.size()==0||img2.empty())
		return NULL;

	//sift������ȡ
	SiftFeatureDetector detector;	
	
	vector<vector<KeyPoint>> keyPoint1;	
	for(k =0;k<files.size();k++){
 		vector<KeyPoint> temp; 
		detector.detect(img1[k],temp);
		keyPoint1.push_back(temp);
	}
	vector<KeyPoint> keyPoint2;
	detector.detect(img2,keyPoint2);
	//sift���������Ӽ���
	SiftDescriptorExtractor desExtractor;

	vector<Mat> des1;
	for(k =0;k<files.size();k++){
		Mat temp;
		desExtractor.compute(img1[k],keyPoint1[k],temp);
		des1.push_back(temp);
	}
	Mat des2;
	desExtractor.compute(img2,keyPoint2,des2);

	//sift������(������)ƥ��
	BruteForceMatcher<L2<float>> matcher;
	vector<DMatch> matches;
	vector<Mat> img_match;
	vector<DMatch> matches_ransac;
	for(k =0;k<files.size();k++){//-----for
	    matcher.match(des1[k],des2,matches);
		matches_ransac=ransac(matches,keyPoint1[k],keyPoint2);
	
	
	Mat img_temp ; 
	drawMatches(img1[k],keyPoint1[k],img2,keyPoint2,matches_ransac,img_temp);
	img_match.push_back(img_temp);

	//imshow(files[k],img_match[k]);
	//ransac(matches[k],keyPoint1[k],keyPoint2);

	//what i do
	int i = 0,j = 0;
	int width = img2.size().width;
	int height = img2.size().height;
	int column_width = width/BLOCK_NUM;
	int raw_height = height/BLOCK_NUM;
	int raw_id = 0,column_id = 0;
	int** temp = new int*[BLOCK_NUM];
	for(i=0;i<BLOCK_NUM;i++){
		temp[i] = new int[BLOCK_NUM];
		for(j=0;j<BLOCK_NUM;j++)
			temp[i][j] = 0;
	}
	for(i=0;i<matches_ransac.size();i++){//��ƥ�����������ԭͼ�б�ǳ���
		int trainIdx = matches_ransac[i].trainIdx;  
		int colunmn_id = (int)keyPoint2[trainIdx].pt.x/column_width;
		int raw_id = (int)keyPoint2[trainIdx].pt.y/raw_height;
		if(raw_id>=0&&raw_id<=2)
			continue;
		if(column_id==BLOCK_NUM)
			column_id-=1;
		if(raw_id==BLOCK_NUM)
			raw_id-=1;
		temp[raw_id][colunmn_id]++;
	}
	for(i=0;i<BLOCK_NUM;i++){
		for(j=0;j<BLOCK_NUM;j++){
			if(temp[i][j]>MIN_NUM)
				output[i][j]=-1;
		}
	}
	}//---------end for
	//what i do

	return output;
}

//RANSAC�㷨ʵ��
vector<DMatch> ransac(vector<DMatch> matches,vector<KeyPoint> queryKeyPoint,vector<KeyPoint> trainKeyPoint)
{
	//���屣��ƥ��������
	vector<Point2f> srcPoints(matches.size()),dstPoints(matches.size());
	//����ӹؼ�������ȡ����ƥ���Ե�����
	for(int i=0;i<matches.size();i++)
	{
		srcPoints[i]=queryKeyPoint[matches[i].queryIdx].pt;
		dstPoints[i]=trainKeyPoint[matches[i].trainIdx].pt;
	}
	//�������ĵ�Ӧ�Ծ���
	Mat homography;
	//�������Ƿ����ı�־
	vector<unsigned char> inliersMask(srcPoints.size()); 
	//ƥ���Խ���RANSAC����
	homography = findHomography(srcPoints,dstPoints,CV_RANSAC,5,inliersMask);
	//RANSAC���˺�ĵ��ƥ����Ϣ
	vector<DMatch> matches_ransac;
	//�ֶ��ı���RANSAC���˺��ƥ����
	for(int i=0;i<inliersMask.size();i++)
	{
		if(inliersMask[i])
			matches_ransac.push_back(matches[i]);
	}
	//����RANSAC���˺�ĵ��ƥ����Ϣ
	return matches_ransac;
}

int* getPicOutput(cv::Mat pic)
{
	//##############���ͼ��ʶ���������ؾ���
	int **output = new int*[BLOCK_NUM];
    int i = 0 ,j = 0;
    for(i = 0;i<BLOCK_NUM;i++){
	    output[i] = new int[BLOCK_NUM];
	    for(j = 0;j<BLOCK_NUM;j++)
		    output[i][j] = 0;
    }
    color_detect(output, pic);
	sift(output, pic);

	int* result = new int[BLOCK_NUM*BLOCK_NUM];
	for(i = 0;i<BLOCK_NUM;i++){
		for(j = 0;j<BLOCK_NUM;j++){
			result[i*BLOCK_NUM+j] = output[i][j];
		    cout<<output[i][j]<<" ";
		}
	   cout<<""<<endl;
   }
	return result;
}


extern int cuteEdge(const cv::Mat& src, cv::Mat& dst);

int main(int argc, char* argv[])
{
	Mat cameraFeed;
	VideoCapture capture;
	Mat img, cropImg;
	double execTime = 0;

	capture.release();
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	
	while(1)
	{
		execTime = (double)getTickCount();
		capture.read(img);

		if(img.empty())
		{
			cout<<"err png:"<<endl;
			waitKey(30);
			continue;
		}

		cuteEdge(img, cropImg);
		//imshow("aaaa", cropImg);
		//waitKey(10000000);
		getPicOutput(cropImg);
		execTime = ((double)getTickCount() - execTime)*1000/getTickFrequency();
		cout<<"exec ms: "<<execTime<<endl;
		waitKey(30);
	}

	waitKey(0);
}


