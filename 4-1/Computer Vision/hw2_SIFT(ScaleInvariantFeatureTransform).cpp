#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("img1.bmp");

	Mat simg[7][6];//7개의 옥타브, 6개의 가우시안 스무딩 된 영상을 담는 배열
	Mat DoG[7][5];//7개의 옥타브에 대해 DoG영상을 담는 배열

	//시그마 값 배열
	double sigma[6] = { 1.6, 2.0159, 2.5398, 3.2, 4.0317, 5.0797 };

	//6개 영상의 마스크 크기를 담는 배열. 시그마 값으로 계산
	int mask[6] = { 0 };

	//시그마 값에 6을 곱해서 나온 수보다 큰 홀수 중 가장 작은 홀수를 마스크 크기로 지정
	for (int t = 0; t < 6; t++)
	{
		int tt = sigma[t] * 6;

		tt++;
		while (1)
		{
			if (tt % 2 == 1)
			{
				mask[t] = tt;
				cout << "마스크 크기: " << mask[t] << endl;
				break;
			}
			else tt++;
		}
	}

	double t = 1.6;//초기 시그마 변수
	double k = pow(2, 1 / 3.0f);//시그마 변수에 곱해 가우시안 변화를 시키는 변수

	//7 개의 옥타브 만들기
	for (int i = 0; i < 7; i++)
	{
		t = 1.6;
		for (int j = 0; j < 6; j++)
		{
			t = t * pow(k, j);
			if (i == 0)//0옥타브 일 때
			{
				GaussianBlur(image, simg[i][j], Size(mask[j], mask[j]), t, t);
			}
			else
			{
				//0 이상의 옥타브인 경우 이전 옥타브의 4번째 영상을 반으로 줄인 값으로 변경
				pyrDown(simg[i - 1][3], simg[i][j], Size(simg[i - 1][3].cols / 2, simg[i - 1][3].rows / 2));
				if (j != 0)//옥타브 0이 아닌 옥타브의 처음 영상일 경우, 가우시안 스무딩을 하지 않음
				{
					GaussianBlur(simg[i][j], simg[i][j], Size(mask[j], mask[j]), t, t);
				}
			}
		}
	}

	//모든 영상을 그레이 스케일로 변환
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			cvtColor(simg[i][j], simg[i][j], CV_BGR2GRAY);
		}
	}

	//DoG를 구함
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			DoG[i][j] = simg[i][j + 1] - simg[i][j];
		}
	}

	int l, m, n;//27개의 픽셀 값 비교에 사용하는 변수
	//특징점 찾기
	for (int oct = 0; oct < 7; oct++)
	{
		//옥타브마다 영상의 크기가 다르므로 따로 구함
		int width = DoG[oct][0].cols;
		int height = DoG[oct][0].rows;

		for (int i = 1; i < width - 1; i++)
		{
			for (int j = 1; j < height - 1; j++)
			{
				//시그마 i번째 영상에 대해 특징점 찾기
				for (int k = 1; k < 4; k++)
				{
					int maxPixel = -100;
					int minPixel = 300;

					for (int a = -1; a <= 1; a++)
					{
						for (int b = -1; b <= 1; b++)
						{
							//측정하는 같은 위치의 세 점에 대해 지역적 최대, 최솟값을 구함
							int localMax, localMin;

							l = DoG[oct][k - 1].at<uchar>(j + a, i + b);//i-1번째 시그마

							if (a != 0 && b != 0)
							{
								m = DoG[oct][k].at<uchar>(j + a, i + b);//i번째 시그마
							}
							else m = l;//자기 자신 값을 구하지 않도록 하여 최댓값, 최솟값에 차질이 없도록 함

							n = DoG[oct][k + 1].at<uchar>(j + a, i + b);//i+1번째 시그마

							localMax = max(l, max(m, n));
							localMin = min(l, min(m, n));

							//최댓값과 최솟값을 업데이트
							if (localMax > maxPixel) maxPixel = localMax;
							if (localMin < minPixel) minPixel = localMin;
						}
					}

					if (DoG[oct][k].at<uchar>(j, i) > maxPixel || DoG[oct][k].at<uchar>(j, i) < minPixel)
					{
						//좌표를 원래 영상 위치에 맞도록 조정
						int x = i * pow(2, oct);
						int y = j * pow(2, oct);
						//스케일 조정
						double s = 1.6*pow(2, (k + oct) / 3.0f);

						//구한 위치와 스케일에 따라 원을 그림
						Point center(x, y);
						circle(image, center, s, Scalar(0, 255, 0), 1);
					}
				}
			}
		}
	}

	imwrite("Result.jpg", image);

	return 0;
}
