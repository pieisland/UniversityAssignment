#include<stdio.h>
#include<stdlib.h>

#define height 256
#define width 256

typedef unsigned char BYTE;

unsigned char bilinear(BYTE* MemIn, int i, int j, int scale)
{
	int p1 = MemIn[(j / scale)*width + (i / scale)];
	int p2 = MemIn[(j / scale)*width + (i / scale) + 1];
	int p3 = MemIn[(j / scale)*width + (i / scale) + width];
	int p4 = MemIn[(j / scale)*width + (i / scale) + width + 1];

	int l = i / scale;
	int k = j / scale;

	double a = (double)i / (double)scale - (double)l;
	double a_ = 1 - a;
	double b = (double)j / (double)scale - (double)k;
	double b_ = 1 - b;

	double w1 = a_*b_;
	double w2 = a*b_;
	double w3 = a_*b;
	double w4 = a*b;

	//last col
	if ((j / scale)*width + (i / scale) + 1 > width*(j / scale + 1) - 1)
	{
		p2 = p1;
		p4 = p3;
	}
		
	//last row
	if ((j / scale)*width + (i / scale) + width > width*height-1)
	{
		p3 = p1;
		p4 = p2;
	}

	return w1*p1 + w2*p2 + w3*p3 + w4*p4;
}

void main()
{
	int i, j;
	FILE *F_I;
	FILE *F_O;
	fopen_s(&F_I, "lena.raw", "rb");
	fopen_s(&F_O, "lena_out.raw", "wb");
	BYTE* MemIn = new BYTE[height*width];
	BYTE* MemOut = new BYTE[(height*4)*(width*4)];

	fread(MemIn, sizeof(BYTE), height*width, F_I);

	for (j = 0; j < height * 4; j++)
		for (i = 0; i < width * 4; i++){
			MemOut[j*width * 4 + i] = bilinear(MemIn, i, j, 4);
		}

	fwrite(MemOut, sizeof(BYTE)*4, height*width*sizeof(BYTE)*4, F_O);

	delete MemIn;
	delete MemOut;
	fclose(F_I);
	fclose(F_O);
}
