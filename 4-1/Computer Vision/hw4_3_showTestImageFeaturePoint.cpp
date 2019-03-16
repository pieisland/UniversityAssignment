#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

//result 결과를 바탕으로 Lenna 사진 위에 원을 그리는 스크립트
int main(void)
{
	Mat img = imread("Lenna.png");

	int width = img.cols;
	int height = img.rows;

	vector<int> feature;

	string filePath = "0612_1.txt";

	ifstream ifile(filePath.data());//파일 열기

	if (ifile.is_open())
	{
		string lines;
		while (getline(ifile, lines))//한 줄씩 벡터에 넣어줌
			feature.push_back(atoi(lines.c_str()));
	}

	int index;
	for (int i = 0; i < width; i++)//y
	{
		for (int j = 0; j < height; j++)//x
		{
			index = i * width + j;

			if (feature[index] == 1)//특징점이라면, 
			{
				Point center(i, j);
				circle(img, center, 1, Scalar(0, 255, 255), 1);
			}
		}
	}

	ifile.close();//파일 닫기
	imwrite("0612_1.jpg", img);

	return 0;
}
