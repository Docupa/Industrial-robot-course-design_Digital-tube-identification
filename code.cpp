#include <stdio.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#include <errno.h> 
#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void deal(Mat src, char data[], bool &flag)
{
	Mat dst, roi;
	//int data[5];//储存数码管数字
	for (int i = 0; i<5; i++)
	{
		data[i] = 0;
	}
	int at[5];//储存数码管x坐标
	cvtColor(src, src, CV_BGR2GRAY);//灰度图
	threshold(src, roi, 110, 255, 0);//用于分割数字目标的二值化源图
	imshow("二值化", roi);
	//Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	//Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5));
	//erode(roi, roi, element); // 腐蚀
	//dilate(roi, roi, element1);// 膨胀
	threshold(src, dst, 110, 255, 0);
	//erode(dst, dst, element); // 腐蚀
	//dilate(dst, dst, element1);// 膨胀
	dst = dst(Rect(0, 0, src.rows, src.cols / 6));
	vector<vector<Point> > contours;
	Rect rrr;
	findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	char p = 0;//数组第一位
	for (size_t i = 0; i < contours.size(); i++)
	{
		int a = 60000, x, y, ssum, val;

		Rect rect = boundingRect(contours[i]);

		Mat K = roi(Rect(rect.tl(), rect.br()));
		float h, w;
		h = rect.height;
		w = rect.width;
		if (h / w<4 && h / w>1.2&&h*w > 100 && h*w < 800 && rect.tl().x < 480 && rect.tl().x>160)//限制数码管矩形框范围
		{
			rectangle(src, rect, Scalar(255, 255, 255), 2);//画矩形
			if (w < 10.0)
			{
				val = 1;
				rrr = rect;
			}
			else
			{
				for (int num = 1; num < 10; num++)
				{
					//逐一读取模板
					char filename[100];
					sprintf(filename, "model/%d.png", num);
					Mat model = imread(filename, 0);
					threshold(model, model, 180, 255, 0);//二值化
					resize(model, model, K.size());//重新调整像素
					imshow("model", model);
					Mat out;
					absdiff(K, model, out);//帧差   互减
					ssum = 0;
					//求差值后矩阵元素的和
					for (y = 0; y < out.rows; y++)
					{
						uchar* datax = out.ptr<uchar>(y);
						for (x = 0; x < out.cols; x++)
						{
							if (datax[x] != 0)
							{
								ssum = ssum + datax[x];
							}
						}
					}
					if (ssum <= a)
					{
						a = ssum;
						val = num;
						rrr = rect;
						printf("num=%d a=%d x=%d\n", num, a, rect.tl().x);
					}
				}
			}
			printf("\n");
			char str[100];
			sprintf(str, "%d", val);
			//putText(显示的图像，文字，起始点，字型，大小，颜色，线粗，线型)
			putText(src, str, rrr.tl(), 2, 0.5, Scalar(255, 255, 255), 1, 8);
			//cout<<val<<endl;
			//将数码管数字储存到数组b,数码管x坐标存放至len
			data[p] = val;
			at[p] = rect.tl().x;
			p++;
		}
	}
	if (p == 5)
	{
		printf("\n");
		for (int i = 0; i < 5 - 1; i++)
		{
			int tem, data_tem;
			for (int j = 1 + i; j<5; j++)
			{
				if (at[i]>at[j])
				{
					tem = at[i];
					at[i] = at[j];
					at[j] = tem;
					data_tem = data[i];
					data[i] = data[j];
					data[j] = data_tem;
				}
			}
		}
		for (int i = 0; i < 5; i++)//数码管数字
		{
			printf("%d \t", data[i]);
		}
		printf(" \n");
		flag = 1;
	}
	else
	{
		flag = 0;
	}


	imshow("src_1", src);
}

bool contrast_shumanum(char data1[], char data2[])
{
	bool flag = 0;
	int num = 0;
	for (int i = 0; i < 5; i++)
	{
		if (data1[i] == data2[i])
			num++;
	}

	if (num == 5)
		flag = 1;

	return flag;
}

int main()
{
	//VideoCapture cap(0);
	VideoCapture cap("1.avi");
	Mat src, src_1;
	char data[5];
	char flag = 0;
	bool deal_flag = 0;
	while (1)
	{
		if (1)
		{
			cap >> src;
			src_1 = src;
			if (flag == 0)
			{
				deal(src, data, deal_flag);//匹配九宫格数字并排列
				if (!deal_flag)	continue;
			}
			imshow("src", src_1);
			int a = 0;
			int key = waitKey(a);
			if (flag<4)
				flag++;
			else
				flag = 0;
		}
	}
	return 0;
}


