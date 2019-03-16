#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>
#include <bitset>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

//Lenna 이미지의 모든 픽셀에서 LBP를 추출해 텍스트 파일로 저장
int main(void)
{
	Mat img = imread("Lenna.png");

	cvtColor(img, img, CV_BGR2GRAY);

	FILE *fp = fopen("C:\\Users\\pieis\\OneDrive\\바탕 화면\\컴비_파이널_레나\\컴비_파이널_레나\\0610_3_lena.txt", "wt");

	int w = img.cols;
	int h = img.rows;
	int index;

	//LBP 7*7마스크
	int arr[48][2] = {};

	//모든 픽셀에 대해 디스크립터를 저장
	char **descriptor = new char*[w*h];
	for (int i = 0; i < w*h; i++)
		descriptor[i] = new char[48];

	//맞게 넣어줌
	int ta = 0;
	while (ta < 48)
	{
		for (int i = -3; i < 4; i++)
		{
			for (int j = -3; j < 4; j++)
			{
				if (i == 0 && j == 0) continue;
				arr[ta][0] = i;
				arr[ta][1] = j;
				ta++;
			}
		}
	}
	
	for (int i = 0; i < w ; i++)
	{
		for (int j = 0; j < h; j++)
		{
			index = i * w + j;

			if (i >= 0 && i<3 || i >= w - 3 && i<w || j >= 0 && j<3 || j >= h - 3 && j<h)
			{
				for (int a = 0; a < 48; a++)
					descriptor[index][a] = 0;
			}
			else
			{
				for (int a = 0; a < 48; a++)
				{
					if (img.at<uchar>(j, i) <= img.at<uchar>(j + arr[a][0], i + arr[a][1]))
						descriptor[index][a] = 1;
					else//아닐 경우 0
						descriptor[index][a] = 0;
				}
			}
		}
	}

	for (int i = 0; i < h; i++)//y
	{
		for (int j = 0; j < w; j++)//x
		{
			int index = i * h + j;
			fprintf(fp, "%d ", 0);//특징점인지 아닌지 모르므로 0으로 설정
			for (int k = 0; k < 48; k++)
			{
				fprintf(fp, "%d:%d ", k, descriptor[index][k]);
			}
			fputc('\n', fp);
		}
	}

	return 0;
}
