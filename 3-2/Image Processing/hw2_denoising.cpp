#include <stdio.h>
#include <stdlib.h>

#define height 256
#define width 256

typedef unsigned char BYTE;

BYTE sortNgetValue(BYTE* arr, int size){
	//Using a bubble sort, one of the basic algorithm
	BYTE hold;

	//중간에 있는 자신의 값 미리 저장
	BYTE value = arr[size / 2];

	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size - 1 - i; j++) {
			if (arr[j] > arr[j + 1]) {
				hold = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = hold;
			}
		}
	}

	//salt나 pepper라면 더 효과가 뛰어났던 median 방법을 사용하고, 아닐 경우 자신의 값을 사용
	int d = 2;
	if (value <= arr[d] || value >= arr[size - 1 - d])
	{
		BYTE median = arr[size / 2 ];

		return median;
	}
	else
	{
		return value;
	}

	//alpha trimmed mean filter
	/*int pixel = 0;
	int d = 4;

	for (int k = d / 2; k < size - 1 - d / 2; k++)
	{
		pixel += arr[k];
	}

	BYTE atm = pixel / (size - d);

	return atm;*/

	//BYTE median = arr[size / 2 ];

	//return median;

	}

//size_x, size_y are mask size
//pos_x, pos_y are the location of the pixel to be a mask
//inpuArray is input data for use with a mask
BYTE getMask(int mask_size, int pos_x, int pos_y, BYTE *inputArray, int extened_width){

	BYTE *list = new BYTE[mask_size * mask_size];

	int index = pos_y * extened_width + pos_x;							//Convert (x, y)position to array index
	index -= (extened_width * (mask_size / 2)) + (mask_size / 2);		//Changed to the upper left of the mask

	for (int y = 0; y < mask_size; y++){
		for (int x = 0; x <mask_size; x++){
			list[y * mask_size + x] = inputArray[index + x];			//push the pixel values
		}
		index += extened_width;											//Change the current mask index next line
	}

	BYTE result = 0;													//result value
	result = sortNgetValue(list, mask_size * mask_size);			//custom function

	delete list;
	return result;
}

void main(){
	FILE * F_I, *F_O;
	//Open the raw Filse
	fopen_s(&F_I, "lena_noisy.raw", "rb");
	fopen_s(&F_O, "lena_result.raw", "wb");

	int mask_size = 3;										//mask size	

	int added_line = mask_size / 2;							//Increase the number of lines in one direction					
	int extend_width = width + (added_line * 2);			//extended image width
	int extend_height = height + (added_line * 2);			//extended image height


	BYTE *MemIn = new BYTE[width * height];
	BYTE *MemOut = new BYTE[width * height];
	BYTE *MemExtend = new BYTE[extend_width * extend_height];

	fread(MemIn, sizeof(BYTE), height*width, F_I);

	//do something	
	//Original image copy to extend image center
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			MemExtend[(y + added_line) * extend_width + x + added_line] = MemIn[y * width + x];
		}
	}

	//and then, Boundary pixels are copied to the neighboring pixels
	for (int y = 0; y < height; y++){
		for (int x = 0; x < added_line; x++){
			//left
			MemExtend[(y + added_line) * extend_width + x] = MemIn[y*width];
			//right
			MemExtend[(y + added_line) * extend_width + width + added_line + x] = MemIn[(y + 1)*width - 1];
		}
	}
	for (int x = 0; x < width; x++){
		for (int y = 0; y < added_line; y++){
			//top
			MemExtend[y * (width + added_line * 2) + added_line + x] = MemIn[x];
			//bottom
			MemExtend[((height + added_line * 2) - y - 1) * (width + added_line * 2) + added_line + x] = MemIn[(height - 1) * width + x];
		}
	}
	//last, fill the pixels of corner in extend image using the pixels of corner from input image
	for (int x = 0; x < added_line; x++){
		for (int y = 0; y < added_line; y++){
			//left-top
			MemExtend[y * (width + added_line * 2) + x] = MemIn[0];
			//right-top
			MemExtend[(y + 1) * (width + added_line * 2) - added_line + x] = MemIn[width - 1];
			//left-bottom
			MemExtend[(height + added_line + y) * (width + added_line * 2) + x] = MemIn[(height - 1) * width];
			//right-bottom 
			MemExtend[((height + added_line * 2) - y) * (width + added_line * 2) - added_line + x] = MemIn[height * width - 1];

		}
	}

	//Working through a mask algorithm
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			MemOut[y * width + x] = getMask(mask_size, x + added_line, y + added_line, MemExtend, extend_width);
		}
	}

	fwrite(MemOut, sizeof(BYTE), width * height, F_O);

	delete MemIn;
	delete MemOut;
	delete MemExtend;

	fclose(F_I);
	fclose(F_O);
}
