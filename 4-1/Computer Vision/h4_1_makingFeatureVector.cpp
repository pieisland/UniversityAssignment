#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>
#include <Windows.h> //파일 불러오기를 위해 사용
#include <atlbase.h>
#include <bitset>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

char* ConverWCtoC(wchar_t *str) //유니코드를 멀티바이트로 다시 변환
{
	char* pStr;
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	pStr = new char[strSize];

	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}

int randomRange(int n1, int n2)
{
	return (int)(rand() % (n2 - n1 + 1)) + n1;
}

//LBP 7*7 윈도우로 이미지 하나당 40개의 특징점, 50개의 비특징점의 LBP 값을 저장함
int main(void)
{
	vector<string> imagePath;
	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	LPCWSTR path = L"C:\\Users\\pieis\\OneDrive\\바탕 화면\\컴비마지막과제_1\\컴비마지막과제_1\\*.jpg";
	//Visual Studio의 프로젝트 환경이 유니코드이므로 위의 방법을 통해 문자열을 유니코드로 변환
	char* filename;
	string tmp;

	hFind = FindFirstFile(path, &FindData);
	if (hFind == INVALID_HANDLE_VALUE) return 0;
	else {
		do {
			if (FindData.cFileName[0] == '.') continue;
			filename = ConverWCtoC(FindData.cFileName);
			tmp = filename;
			imagePath.push_back(tmp);
		} while (FindNextFile(hFind, &FindData));
		FindClose(hFind);
	}

	//파일 읽어오기
	FILE *fp = fopen("C:\\Users\\pieis\\OneDrive\\바탕 화면\\컴비마지막과제_1\\컴비마지막과제_1\\0612_1.txt", "wt");

	char arr[48][2] = {};

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

	vector<char> allFP_Y;
	vector<char> allFP_X;

	Mat simg[2][6];//2개의 옥타브, 6개의 가우시안 스무딩 된 영상을 담는 배열
	Mat DoG[2][5];//2개의 옥타브에 대해 DoG영상을 담는 배열

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
				break;
			}
			else tt++;
		}
	}

	double t = 1.6;//초기 시그마 변수
	double k = pow(2, 1 / 3.0f);//시그마 변수에 곱해 가우시안 변화를 시키는 변수

	//특징점 40개에 대한 LBP 값 저장
	char **descriptor1 = new char*[40];
	for (int i = 0; i < 40; i++)
		descriptor1[i] = new char[48];

	//비특징점 50개에 대한 LBP 값 저장
	char **descriptor2 = new char*[50];
	for (int i = 0; i < 50; i++)
		descriptor2[i] = new char[48];

	int Cnt = 0;
	while (Cnt < imagePath.size())
	{
		cout << Cnt << endl;
		Mat image = imread(imagePath[Cnt]);
		//2 개의 옥타브 만들기
		for (int i = 0; i < 2; i++)
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
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				cvtColor(simg[i][j], simg[i][j], CV_BGR2GRAY);
			}
		}

		//DoG를 구함
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				DoG[i][j] = simg[i][j + 1] - simg[i][j];
			}
		}

		int l, m, n;//27개의 픽셀 값 비교에 사용하는 변수
		//특징점 찾기
		for (int oct = 0; oct < 2; oct++)
		{
			//옥타브마다 영상의 크기가 다르므로 따로 구함
			int width = DoG[oct][0].cols;
			int height = DoG[oct][0].rows;

			for (int i = 1; i < width - 1; i++)//x
			{
				for (int j = 1; j < height - 1; j++)//y
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

							allFP_Y.push_back(y);
							allFP_X.push_back(x);
						}
					}
				}
			}
		}

		int count_10 = 0;
		int count_100 = 0;

		//특징점의 갯수. 어차피 특징점은 x, y 한 쌍으로 존재하므로 개수는 동일함
		int length = allFP_X.size();

		//40개의 특징점 저장
		for (int i = 0; i < length; i++)
		{
			if (allFP_X[i] >= 3 && allFP_X[i] <= image.cols - 4 && allFP_Y[i] >= 3 && allFP_Y[i] <= image.rows - 4)
			{
				if (count_10 % 40 == 0)//40개 건너뛰며 하나씩 저장
				{
					//특징점에 대한 LBP
					for (int a = 0; a < 48; a++)
					{
						if (image.at<uchar>(allFP_Y[i], allFP_X[i]) <= image.at<uchar>(allFP_Y[i] + arr[a][0], allFP_X[i] + arr[a][1]))
							descriptor1[count_100][a] = 1;
						else
							descriptor1[count_100][a] = 0;
					}
					count_100++;
				}
				count_10++;
			}
			if (count_100 == 40) break;
		}

		//초기화..
		count_100 = 0;
		int count1 = 0;
		int count2 = 0;
		int x = 0;
		int y = 0;

		//50개의 비특징점 저장.
		while (1)
		{
			count2 = 0;
			x = randomRange(3, image.cols - 4);
			y = randomRange(3, image.rows - 4);

			//x, y 쌍이 모두 같아야 특징점
			for (int i = 0; i < length; i++)
			{
				count1 = 0;
				if (allFP_Y[i] == y) count1++;
				if (allFP_X[i] == x) count1++;

				if (count1 == 2) count2++;
			}
			if (count2 == 0)
			{
				//LBP 계산
				for (int a = 0; a < 48; a++)
				{
					if (image.at<uchar>(y, x) <= image.at<uchar>(y + arr[a][0], x + arr[a][1]))
						descriptor2[count_100][a] = 1;
					else
						descriptor2[count_100][a] = 0;
				}
				count_100++;
			}
			if (count_100 == 50) break;
		}

		//특징점 디스크립터 텍스트 파일로 저장
		for (int i = 0; i < 40; i++)
		{
			fprintf(fp, "%d ", 1);
			for (int j = 0; j < 48; j++)
			{
				fprintf(fp, "%d:%d ", j, descriptor1[i][j]);
			}
			fputc('\n', fp);
		}

		//비특징점 디스크립터 텍스트 파일로 저장
		for (int i = 0; i < 50; i++)
		{
			fprintf(fp, "%d ", -1);
			for (int j = 0; j < 48; j++)
			{
				fprintf(fp, "%d:%d ", j, descriptor2[i][j]);
			}
			fputc('\n', fp);
		}

		allFP_X.clear();
		allFP_Y.clear();

		Cnt++;
	}

	//디스크립터 동적할당 해제
	for (int i = 0; i < 40; i++)
	{
		delete[]descriptor1[i];
	}
	delete[]descriptor1;

	for (int i = 0; i < 50; i++)
	{
		delete[]descriptor2[i];
	}
	delete[]descriptor2;

	return 0;
}
