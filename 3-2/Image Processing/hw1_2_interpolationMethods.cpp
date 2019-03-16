#include<stdio.h>
#include<stdlib.h>

#define height 64
#define width 64

typedef unsigned char BYTE;

unsigned char bilinear(BYTE* MemIn, int i, int j, int scale)
{
	int l = (int)(i / scale);
	int k = (int)(j / scale);

	double a = (double)i / (double)scale - (double)l;
	double a_ = 1 - a;
	double b = (double)j / (double)scale - (double)k;
	double b_ = 1 - b;

	double w1 = a_*b_;
	double w2 = a*b_;
	double w3 = a_*b;
	double w4 = a*b;

	int p1 = MemIn[(j / scale)*width + (i / scale)];
	int p2 = MemIn[(j / scale)*width + (i / scale) + 1];
	int p3 = MemIn[(j / scale)*width + (i / scale) + width];
	int p4 = MemIn[(j / scale)*width + (i / scale) + width + 1];

	//last col
	if ((j / scale)*width + (i / scale) + 1 > width*(j / scale + 1) - 1)
	{
		p2 = p1;
		p4 = p3;
	}

	//last row
	if ((j / scale)*width + (i / scale) + width > width*height - 1)
	{
		p3 = p1;
		p4 = p2;
	}

	return w1*p1 + w2*p2 + w3*p3 + w4*p4;
}

unsigned char linear_row(BYTE* MemIn, int i, int j, int scale)
{
	int l = i / scale;
	double a = (double)i / (double)scale - (double)l;
	double a_ = 1 - a;
	int p1 = MemIn[(j / scale)*width + (i / scale)];
	int p2 = MemIn[(j / scale)*width + (i / scale) + 1];

	if ((j / scale)*width + (i / scale) + 1 > width*(j / scale + 1) - 1)
		p2 = p1;

	return a_*p1 + a*p2;
}

unsigned char linear_col(BYTE* MemIn, int i, int j, int scale)
{
	int k= j / scale;
	double b = (double)j / (double)scale - (double)k;
	double b_ = 1 - b;
	int p1 = MemIn[(j / scale)*width + (i / scale)];
	int p2 = MemIn[(j / scale)*width + (i / scale) + width];

	if ((j / scale)*width + (i / scale) + width > width * height-1)
		p2 = p1;

	return b_*p1 + b*p2;
}

unsigned char linear_mid(BYTE* MemIn, int i, int j, int scale)
{
	int p1 = MemIn[(j / scale)*width + (i / scale)];
	int p2 = MemIn[(j / scale)*width + (i / scale) + 1];
	int p3 = MemIn[(j / scale)*width + (i / scale) + width];
	int p4 = MemIn[(j / scale)*width + (i / scale) + width + 1];

	int mid12;
	int mid13;
	int midmid1234;

	unsigned char result;

	if ((j / scale)*width + (i / scale) + 1 > width*(j / scale + 1) - 1)
	{
		p2 = p1;
		p4 = p3;
	}

	if ((j / scale)*width + (i / scale) + width > width*height - 1)
	{
		p3 = p1;
		p4 = p2;
	}

	mid12 = (p1 + p2) / 2;
	mid13 = (p1 + p3) / 2;
	midmid1234 = (p1 + p2 + p3 + p4) / 4;
	
	if (j%scale == 0 && i%scale == 0)
		result = p1;
	else if (j%scale == 0 && i&scale != 0)
		result = mid12;
	else if (j%scale != 0 && i&scale == 0)
		result = mid13;
	else
		result = midmid1234;

	return result;
}

void comparison(BYTE* Original, BYTE* MemOut)
{
	int plus = 0;
	int minus = 0;

	int i, j;

	for (j = 0; j < height * 4; j++)
	{
		for (i = 0; i < width * 4; i++)
		{
			int difference = Original[j*width * 4 + i] - MemOut[j*width * 4 + i];

			if (difference < 0)
				minus -= difference;
			else
				plus += difference;
		}
	}

	printf("<bilinear> 차이 나는 픽셀 값의 정도\nminus : %d plus : %d\n합 : %d\n", minus, plus, minus+plus);
}

void main()
{
	int i, j;
	FILE *F_I;
	FILE *F_O;
	FILE *F_ORIGINAL;
	fopen_s(&F_I, "lena_LR.raw", "rb");
	fopen_s(&F_O, "lena_out(bilinear).raw", "wb");
	fopen_s(&F_ORIGINAL, "lena.raw", "rb");
	BYTE* MemIn = new BYTE[height*width];
	BYTE* MemOut = new BYTE[(height * 4)*(width * 4)];
	BYTE* Original = new BYTE[(height * 4)*(width * 4)];

	fread(MemIn, sizeof(BYTE), height*width, F_I);
	fread(Original, sizeof(BYTE), height*width*4, F_ORIGINAL);

	for (j = 0; j < height * 4; j++)
		for (i = 0; i < width * 4; i++){
			MemOut[j*width * 4 + i] = bilinear(MemIn, i, j, 4);
		}

	fwrite(MemOut, sizeof(BYTE) * 4, height*width*sizeof(BYTE) * 4, F_O);

	comparison(Original, MemOut);

	delete MemIn;
	delete MemOut;
	fclose(F_I);
	fclose(F_O);
	fclose(F_ORIGINAL);
}
