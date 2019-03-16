#include <iostream>
#include <vector>

using namespace std;

int main()
{
	vector<int> vec1;//처음에 넣는 수
	vector<int> vec2;//바뀌는 수
	vector<int> vec3;//바뀌는 수의 개수

	//넣은 숫자
	int myCount1;
	int myCount2;
	int num;
	
	//숫자들의 개수
	int count = 0;

	myCount1 = 10;

	//Encode
	cout << "Input a sequence of "<< myCount1 <<" digits: ";

	for (int i = 0; i < myCount1; i++)
	{
		cin >> num;
		vec1.push_back(num);
	}

	for (int j = 0; j < myCount1; j++)
	{
		//맨 처음 숫자라면, 그 숫자를 저장하고 개수 증가
		if (j == 0)
		{
			vec2.push_back(vec1[j]);
			count++;
		}
		else//처음 숫자가 아닐 경우
		{
			//앞과 다를 경우에
			if (vec1[j - 1] != vec1[j])
			{
				//이때까지 센 개수를 저장하고, 바뀐 수를 저장. 개수 초기화
				vec3.push_back(count);
				vec2.push_back(vec1[j]);
				count = 1;
				if (j == myCount1-1)//마지막일 경우에는 개수도 넣어줘야함
				{
					vec3.push_back(count);
				}
			}
			else//앞과 같을 경우
			{
				//개수만 증가
				count++;
				if (j == myCount1-1)//마지막일 경우 개수도 넣어줌
				{
					vec3.push_back(count);
				}
			}
		}
	}

	//결과 출력
	for (int j = 0; j < vec2.size(); j++)
	{
		cout<< "(" << vec2[j] << " " << vec3[j] << ") ";	
	}

	vec2.clear();
	vec3.clear();
	myCount2 = 5;

	//Decode
	cout << "\nInput 5 pairs of RLE code: ";

	for (int i = 0; i < myCount2*2; i++)
	{
		cin >> num;

		//특정 수
		if (i % 2 == 0)
		{
			vec2.push_back(num);
		}
		else//의 개수
		{
			vec3.push_back(num);
		}
	}

	//디코드 결과
	for (int j = 0; j < myCount2; j++)
	{
		//개수만큼 써줌
		for (int k = 0; k < vec3[j]; k++)
		{
			cout << vec2[j] << " ";
		}
	}

	vec1.clear();
	vec2.clear();
	vec3.clear();

	return 0;
}
