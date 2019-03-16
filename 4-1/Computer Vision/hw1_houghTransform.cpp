#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>

#define PI 3.141592
#define WIDTH 100
#define HEIGHT 100

int main()
{
	//이미지 로드
	//원본이미지로 할 시 작동되지 않아 사이즈를 반으로 줄임
	cv::Mat image = cv::imread("input_sizeDown.jpg");
	cv::Mat image2 = cv::imread("img1.bmp");

	//캐니 알고리즘을 위해 그레이 스케일 이미지로 변환
	cv::Mat gray;
	cv::cvtColor(image, gray, CV_BGR2GRAY);
	// 캐니 알고리즘 적용
	cv::Mat contours;
	cv::Canny(gray, contours, 50, 200);

	//이미지 가로, 세로, 대각선
	const int width = image.cols;
	const int height = image.rows;
	const int diagonal = sqrt(width * width + height * height);

	//반지름 최소/최대값, 각도 범위
	const int rad_min = 2;
	const int rad_max = 15;
	const int thetha = 360;

	//선택할 원의 최소 voting 수 
	const int threshold = 200;

	int vote[WIDTH][HEIGHT][15];

	//초기화
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < rad_max; k++)
			{
				vote[i][j][k] = 0;
			}
		}
	}

	//허프 변환 voting 구현
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int t = image.at<uchar>(i, j);
			int tt = image2.at<uchar>(i, j);

			if (contours.at<uchar>(i, j) > 0)
			{
				for (int k = rad_min; k < rad_max; k++)
				{
					//반지름을 변화시키며 원을 그려 voting
					for (int t = 0; t < thetha; t++)
					{
						int xpos = i + k * (double)cos(t * (PI / 180));
						int ypos = j + k * (double)sin(t * (PI / 180));

						//범위를 벗어나지 않을 경우에만
						if (xpos >= 0 && xpos < width && ypos >= 0 && ypos < height)
						{
							vote[xpos][ypos][k]++;
						}
					}
				}
			}
		}
	}

	//그리기
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = rad_min; k < rad_max; k++)
			{
				if (vote[i][j][k] > threshold)
				{
					cv::Point center(j, i);
					cv::circle(image, center, k, cv::Scalar(255, 0, 0), 1);
				}
			}
		}
	}

	//이미지 저장
	cv::imwrite("Result.jpg", image);

	return 0;
}
