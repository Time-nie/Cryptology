#include <math.h>
#include <Windows.h>
#include<iostream>
#include<stdio.h>
#include<string>
#include <random>
#include<iomanip>
using namespace std;

//循环左移-位数表
const unsigned int rolarray[4][4] = {
		{ 7, 12, 17, 22 },
		{ 5, 9, 14, 20 },
		{ 4, 11, 16, 23 },
		{ 6, 10, 15, 21 }
};

//数据坐标表(对消息中字的顺序做变换，得到的置换表)   对照即可获得X[i]
const unsigned short mN[4][16] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	{ 1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12 },
	{ 5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2 },
	{ 0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9 }
};

//常数表
const unsigned int T[64] = {
	0xD76AA478,0xE8C7B756,0x242070DB,0xC1BDCEEE,0xF57C0FAF,0x4787C62A,0xA8304613,0xFD469501,
	0x698098D8,0x8B44F7AF,0xFFFF5BB1,0x895CD7BE,0x6B901122,0xFD987193,0xA679438E,0x49B40821,
	0xF61E2562,0xC040B340,0x265E5A51,0xE9B6C7AA,0xD62F105D,0x02441453,0xD8A1E681,0xE7D3FBC8,
	0x21E1CDE6,0xC33707D6,0xF4D50D87,0x455A14ED,0xA9E3E905,0xFCEFA3F8,0x676F02D9,0x8D2A4C8A,
	0xFFFA3942,0x8771F681,0x6D9D6122,0xFDE5380C,0xA4BEEA44,0x4BDECFA9,0xF6BB4B60,0xBEBFBC70,
	0x289B7EC6,0xEAA127FA,0xD4EF3085,0x04881D05,0xD9D4D039,0xE6DB99E5,0x1FA27CF8,0xC4AC5665,
	0xF4292244,0x432AFF97,0xAB9423A7,0xFC93A039,0x655B59C3,0x8F0CCC92,0xFFEFF47D,0x85845DD1,
	0x6FA87E4F,0xFE2CE6E0,0xA3014314,0x4E0811A1,0xF7537E82,0xBD3AF235,0x2AD7D2BB,0xEB86D391
};

/*4组逻辑函数*/
inline unsigned int F(unsigned int X, unsigned int Y, unsigned int Z)
{
	return (X & Y) | ((~X) & Z);
}
inline unsigned int G(unsigned int X, unsigned int Y, unsigned int Z)
{
	return (X & Z) | (Y & (~Z));
}
inline unsigned int H(unsigned int X, unsigned int Y, unsigned int Z)
{
	return X ^ Y ^ Z;
}
inline unsigned int I(unsigned int X, unsigned int Y, unsigned int Z)
{
	return Y ^ (X | (~Z));
}



void ROL(unsigned int& s, unsigned short cx)//循环左移
{
	if (cx > 32)cx %= 32;
	s = (s << cx) | (s >> (32 - cx));
	return;
}


//MD5循环计算函数，lGroup 存储ABCD的值，M 数据分组（16组32位数指针）
void AccLoop(unsigned short rounds, unsigned int* lGroup, void* M)
{
	unsigned int tmpi = 0; //T是累加器
	unsigned int A, B, C, D;
	typedef unsigned int(*clac)(unsigned int X, unsigned int Y, unsigned int Z); //定义函数类型

	const unsigned int* pM = static_cast<unsigned int*>(M);//转换类型为32位的Uint
	clac clacArr[4] = { F, G, H, I }; //定义并初始化计算函数指针数组

	/*一轮循环开始（16组->16次）*/
	for (short i = 0; i < 16; ++i)
	{
		A = lGroup[0];
		B = lGroup[1];
		C = lGroup[2];
		D = lGroup[3];

		//计算A+F(B,C,D)+M[i]+T
		tmpi = A + clacArr[rounds](B, C, D) + pM[(mN[rounds][i])] + T[rounds * 16 + i];
		ROL(tmpi, rolarray[rounds][i % 4]);//循环左移

		//给缓冲区赋值
		lGroup[0] = D;
		lGroup[1] = tmpi + B;
		lGroup[2] = B;
		lGroup[3] = C;
	}
	return;
}


unsigned int* MD5(const char* mStr) {
	unsigned int mLen = strlen(mStr); //计算字符串长度

	unsigned int FillSize;
	if ((mLen * 8) % 512 <= 448) {
		FillSize = 448 - ((mLen * 8) % 512); //计算需填充的bit数
	}
	else {
		FillSize = 512 + 448 - ((mLen * 8) % 512);
	}

	if (FillSize == 0) {
		FillSize = 512;
	}

	unsigned int FSbyte = FillSize / 8; //以字节表示的填充数
	unsigned int BuffLen = mLen + 8 + FSbyte; //填充后的长度
	unsigned char* md5Buff = new unsigned char[BuffLen]; //分配缓冲区
	memcpy(md5Buff, mStr, mLen); //复制字符串到缓冲区

	/*数据填充开始*/
	md5Buff[mLen] = 0x80; //第一个bit填充1
	ZeroMemory(&md5Buff[mLen + 1], FSbyte - 1); //其它bit填充0
	unsigned long long lenBit = mLen * 8ULL; //计算字符串长度，准备填充
	memcpy(&md5Buff[mLen + FSbyte], &lenBit, 8); //填充消息长度
	/*数据填充结束*/

	/*运算开始*/
	unsigned int LoopNumber = BuffLen / 64; //以16个字为一分组，计算分组数量
	unsigned int A = 0x67452301, B = 0x0EFCDAB89, C = 0x98BADCFE, D = 0x10325476;//初始4个种子，小端类型

	unsigned int* lGroup = new unsigned int[4]{ A, B, C, D }; //种子副本数组,并作为返回值返回
	for (unsigned int Bcount = 0; Bcount < LoopNumber; Bcount++) //分组大循环开始
	{
		/*进入4次计算的小循环*/
		for (unsigned short Lcount = 0; Lcount < 4; Lcount++)
		{
			AccLoop(Lcount, lGroup, &md5Buff[Bcount * 64]);
		}
		/*数据相加作为下一轮的种子或者最终输出*/
		A = (lGroup[0] += A);
		B = (lGroup[1] += B);
		C = (lGroup[2] += C);
		D = (lGroup[3] += D);
	}

	delete[] md5Buff; //清除内存并返回
	return lGroup;
}

struct datas_t {
	unsigned char data[16];
};
void dex_bin(int num, int bin[])
{
	int length = 0, j = 0;
	do
	{
		bin[j] = num % 2;
		num = num / 2;
		j++;
		length++;
	} while (num != 0);
	while (length != 8)
		bin[length++] = 0;
}
int cal(int a,int b)
{
	int bin1[8], bin2[8], total = 0;
	dex_bin(a, bin1);
	dex_bin(b, bin2);
	for (int j = 0; j <= 8; j++)
		if (bin1[j] != bin2[j])
			total++;
	return total;
}

int main()
{
	string s;
	cout << "请输入要进行哈希运算的字符串：" << endl;
	getline(cin, s);
	cout << "哈希运算的结果是：" << endl;
	unsigned int* lGroup = MD5(s.c_str());
	datas_t* datas = (datas_t*)lGroup;

	for (int i = 0; i < 16; i++)
		cout << "0x" << hex << (unsigned int)(unsigned char)datas->data[i] << ", ";
	cout << endl << endl;

	cout << "========================检验雪崩效应========================" << endl << endl;
	// 改变文本
	int len = s.length();
	string text[8];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 7; i++) {
		int index = rand() % len;
		int randomNum = rand() % 91 + 34;
		text[i] = s;
		text[i][index] = randomNum;
	}
	text[7] = s;
	text[7][5] = ' ';
	int avg = 0;
	for (int i = 0; i < 8; i++)
	{
		cout << "=============第" << i + 1 << "次雪崩测试=============" << endl << endl;;
		cout << "文本改变后为：" << endl;
		cout << text[i] << endl;

		cout << "原始哈希运算的结果为：" << endl;
		for (int j = 0; j < 16; j++)
			cout << "0x" << (hex) << (unsigned int)(unsigned char)datas->data[j] << ", ";
		cout << endl;

		cout << "新哈希运算的结果是：" << endl;
		unsigned int* lGroup = MD5(text[i].c_str());
		datas_t* temp = (datas_t*)lGroup;
		for (int j = 0; j < 16; j++)
			cout << "0x" << (hex) << (unsigned int)(unsigned char)temp->data[j] << ", ";
		cout << endl;
		int total = 0;
		for (int k = 0; k < 16; k++) {
			int num, ori;
			num = (unsigned int)(unsigned char)temp->data[k];
			ori = (unsigned int)(unsigned char)datas->data[k];
			total += cal(num, ori);
		}
		avg += total;
		cout << "二进制相差位数为：" << dec << total << endl;
		cout << endl << endl;
	}
	cout << "8次雪崩测试二进制平均改变位数为：" << dec << avg / 8 << endl << endl;
	system("pause");
}