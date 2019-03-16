#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define height 240
#define width 352

typedef unsigned char BYTE;

void main()
{
	FILE *F_I_Prev;
	FILE *F_I_Cur;
	FILE *F_I_Cur_Org;
	FILE *F_I_Next;

	fopen_s(&F_I_Prev, "fb001.raw", "rb");
	fopen_s(&F_I_Cur_Org, "fb002.raw", "rb"); // use only for evaluation
	fopen_s(&F_I_Next, "fb003.raw", "rb");

	fopen_s(&F_I_Cur, "fb002_mc.raw", "wb");

	BYTE* Prev = new BYTE[height*width];
	BYTE* Cur_Org = new BYTE[height*width];
	BYTE* Cur = new BYTE[height*width];
	BYTE* Next = new BYTE[height*width];

	fread(Prev, sizeof(BYTE), height*width, F_I_Prev);
	fread(Cur_Org, sizeof(BYTE), height*width, F_I_Cur_Org);
	fread(Next, sizeof(BYTE), height*width, F_I_Next);

	// define margin (will be 0 for this assignment)
	float total_margin = 0;
	int h_margin = int(float(height) * total_margin * 0.01f * 0.5f);
	int w_margin = int(float(width) *  total_margin * 0.01f * 0.5f);

	//613.737305 p=2일때가 가장 작게 나옴
	//556.176208...p=0일때.....
	int p = 0;

	int a;
	int n1, n2, n3, n4;
	int sum;
	int differ = 0;
	int vx, vy;
	int min = 10000000;
	int noError[4];

	//일단 0으로 초기화해준다
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			Cur[h*width + w] = 0;
		}
	}

	for (int h = 0; h < height / 2; h++)
	{
		for (int w = 0; w < width / 2; w++)
		{
			a = h * 2 * width + w * 2;
			min = 100000;
			for (int x = -p; x <= p; x++)
			{
				//잘못된 값일 때 값 조정
				for (int y = -p; y <= p; y++)
				{
					vx = x;
					vy = y;

					noError[0] = a + x*width + y;
					noError[1] = a + x*width + y + 1;
					noError[2] = a + x*width + y + width;
					noError[3] = a + x*width + y + width + 1;

					if (noError[0] < 0 || noError[1] < 0 || noError[2] < 0 || noError[3] < 0)
					{
						noError[0] = a;
						noError[1] = a + 1;
						noError[2] = a + width;
						noError[3] = a + width + 1;
						
						vx = 0;
						vy = 0;
					}

					if (noError[0] >= width*height || noError[1] >= width*height || noError[2] >= width*height || noError[3] >= width*height)
					{
						noError[0] = a;
						noError[1] = a + 1;
						noError[2] = a + width;
						noError[3] = a + width + 1;

						vx = 0;
						vy = 0;
					}

					n1 = Prev[a] - Next[noError[0]];
					if (n1 < 0) n1 = -n1;

					n2 = Prev[a + 1] - Next[noError[1]];
					if (n2 < 0) n2 = -n2;

					n3 = Prev[a + width] - Next[noError[2]];
					if (n3 < 0) n3 = -n3;

					n4 = Prev[a + width + 1] - Next[noError[3]];
					if (n4 < 0) n4 = -n4;
					sum = n1 + n2 + n3 + n4;

					//합이 가장 작을 때
					if (sum <= min)
					{
						min = sum;

						//넣어줌
						Cur[a + (vx / 2)*width + (vy / 2)] = (Prev[a] + Next[a + (vx)*width + (vy)]+1) / 2;
						Cur[a + (vx / 2)*width + (vy / 2) + 1] = (Prev[a + 1] + Next[a + (vx)*width + (vy) + 1]+1) / 2;
						Cur[a + (vx / 2)*width + (vy / 2) + width] = (Prev[a + width] + Next[a + (vx)*width + (vy) + width]+1) / 2;
						Cur[a + (vx / 2)*width + (vy / 2) + width + 1] = (Prev[a + width + 1] + Next[a + (vx)*width + (vy) + width + 1]+1) / 2;
					}
				}
			}
		}
	}

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			if (Cur[h*width + w] == 0)
			{
				//printf("검은색");
				Cur[h*width + w] = (Prev[h*width + w] + Next[h*width + w] + 1) / 2;
			}
		}
	}

	float MSE = 0.0f;

	// Compute MSE 
	for (int h = h_margin; h < height - h_margin; h++) {
		for (int w = w_margin; w < width - w_margin; w++) {
			MSE += powf(abs(Cur[h*width + w] - Cur_Org[h*width + w]), 2);
		}
	}
	MSE /= ((height - h_margin * 2) * (width - w_margin * 2));

	printf("MSE : %f \n", MSE);

	fwrite(Cur, sizeof(BYTE), height*width, F_I_Cur);

	delete Prev;
	delete Cur;
	delete Next;

	fclose(F_I_Prev);
	fclose(F_I_Cur);
	fclose(F_I_Next);
}
