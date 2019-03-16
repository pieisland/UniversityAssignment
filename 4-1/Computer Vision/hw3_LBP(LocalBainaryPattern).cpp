#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

int main()
{
	Mat img = imread("Lenna.png");
	cvtColor(img, img, CV_BGR2GRAY);

	int w = img.cols;
	int h = img.rows;

	//디스크립터 위치 정하는 행렬
	int arr[8][2] = { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1} };

	//디스크립터 동적할당
	char **descriptor = new char*[w*h];
	for (int i = 0; i < w*h; i++)
		descriptor[i] = new char[8];

	int index;
	//이미지의 모든 픽셀에 대해 디스크립터 정의
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			index = i * w + j;
			//경계의 디스크립터 값은 0으로 처리
			if (i == 0 || i == w - 1 || j == 0 || j == h - 1)
			{
				for (int a = 0; a < 8; a++)
					descriptor[index][a] = 0;
			}
			else
			{
				for (int a = 0; a < 8; a++)
				{
					//8개의 이웃픽셀값과 비교하여 이웃픽셀값이 더 크거나 같을 경우 디스크립터 값은 1
					if (img.at<uchar>(j, i) <= img.at<uchar>(j + arr[a][0], i + arr[a][1]))
						descriptor[index][a] = 1;
					else//아닐 경우 0
						descriptor[index][a] = 0;
				}
			}
		}
	}

	int sum;
	//구한 디스크립터를 이용해 LBP 이미지 생성
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			index = i * w + j;
			sum = 0;
			for (int a = 0; a < 8; a++)
				sum += descriptor[index][a] * pow(2, 7 - a);

			img.at<uchar>(j, i) = sum;
		}
	}

	imwrite("Result.jpg", img);

	//디스크립터 동적할당 해제
	for (int i = 0; i < w*h; i++)
	{
		delete[]descriptor[i];
	}
	delete[]descriptor;

	return 0;
}
