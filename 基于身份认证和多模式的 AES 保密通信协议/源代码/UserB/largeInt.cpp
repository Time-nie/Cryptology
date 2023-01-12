#include<iostream>
#include<stdlib.h>
#include<bitset>
#include"largeInt.h"

using namespace std;

largeInt::largeInt()
{
	for (int i = 0; i < MaxSize; i++)
		data[i] = 0;
	sign = true;
}


largeInt::largeInt(const int& input)
{
	for (int i = 0; i < MaxSize; i++)
		data[i] = 0;
	data[0] = abs(input); //取绝对值

	if (input >= 0)
		sign = true;
	else
		sign = false;
}

largeInt::largeInt(const largeInt& input)
{
	for (int i = 0; i < MaxSize; i++)
		data[i] = input.data[i];
	sign = input.sign;
}



// 赋值操作
void largeInt::operator= (const largeInt& input)
{
	for (int i = 0; i < MaxSize; i++)
		data[i] = input.data[i];
	sign = input.sign;
}

bool operator< (const largeInt& a, const largeInt& b)
{
	//注意正负
	if (a.sign == true && b.sign == false) {
		return false;
	}

	if (a.sign == false && b.sign == true) {
		return true;
	}

	if (a.sign == true && b.sign == true) {
		//从最高位比起
		for (int i = MaxSize - 1; i > 0; i--)
		{
			if (a.data[i] < b.data[i])
				return true;
			if (a.data[i] > b.data[i])
				return false;
		}
		return a.data[0] < b.data[0];
	}

	if (a.sign == false && b.sign == false) {
		//从最高位比起
		for (int i = MaxSize - 1; i > 0; i--)
		{
			if (a.data[i] < b.data[i])
				return false;
			if (a.data[i] > b.data[i])
				return true;
		}
		return a.data[0] > b.data[0];
	}
}

bool operator> (const largeInt& a, const largeInt& b)
{
	//注意正负
	if (a.sign == true && b.sign == false) {
		return true;
	}

	if (a.sign == false && b.sign == true) {
		return false;
	}

	if (a.sign == true && b.sign == true) {
		//从最高位比起
		for (int i = MaxSize - 1; i > 0; i--)
		{
			if (a.data[i] < b.data[i])
				return false;
			if (a.data[i] > b.data[i])
				return true;
		}
		return a.data[0] > b.data[0];
	}

	if (a.sign == false && b.sign == false) {
		//从最高位比起
		for (int i = MaxSize - 1; i > 0; i--)
		{
			if (a.data[i] < b.data[i])
				return true;
			if (a.data[i] > b.data[i])
				return false;
		}
		return a.data[0] < b.data[0];
	}
}

// 判断两个大数是否相等
bool operator== (const largeInt& a, const largeInt& b)
{
	if (a.sign != b.sign) return false;
	for (int i = 0; i < MaxSize; i++)
		if (a.data[i] != b.data[i])
			return false;
	return true;
}

// 判断一个大数和一个int值是否相等
bool operator== (const largeInt& a, const int& b)
{
	if (b >= 0 && a.sign != true) return false;
	if (b < 0 && a.sign != false) return false;
	for (int i = 1; i < a.GetLength(); i++)
		if (a.data[i] != 0)
			return false;
	return a.data[0] == b;
}



//竖式加法,计算两个大数的和
largeInt operator+ (const largeInt& a, const largeInt& b)
{
	largeInt result;
	//64位数据,存放每两位数相加的临时和
	unsigned __int64 sum;
	//carry 进位标志     sub 当两数符号相异时,存放每两位数相减的差
	unsigned int carry = 0, sub;

	//取a,b中长度较长的长度
	int length = a.GetLength() >= b.GetLength() ? a.GetLength() : b.GetLength();

	//当两数符号相同时,进行加法运算
	if (a.sign == b.sign)
	{
		//每一位进行竖式相加
		for (int i = 0; i < length; i++)
		{
			sum = (unsigned __int64)a.data[i] + b.data[i] + carry;
			result.data[i] = (unsigned int)sum; //取低32位
			//sum的高位为进位
			carry = (sum >> 32);
		}

		result.sign = a.sign;
		return result;
	}
	else
	{
		//两数符号不同时,进行减法运算
		largeInt tempa, tempb, ka(a), kb(b);
		ka.sign = 1;
		kb.sign = 1;

		//取出a,b中绝对值较大的作为被减数
		if (ka < kb)
		{
			tempa = b;
			tempb = a;
		}
		else
		{
			tempa = a;
			tempb = b;
		}

		//每一位进行竖式减
		for (int i = 0; i < length; i++)
		{
			sub = tempb.data[i] + carry;
			if (tempa.data[i] >= sub)
			{
				result.data[i] = tempa.data[i] - sub;
				carry = 0;
			}
			else
			{
				//借位减
				result.data[i] = (unsigned __int64)tempa.data[i] + (1 << 32) - sub;
				carry = 1;
			}
		}
		result.sign = tempa.sign;
		return result;
	}
}

//采用竖式减法,计算两个大数的差
largeInt operator- (const largeInt& a, const largeInt& b)
{
	largeInt result;
	//64位数据,存放每两位数相加的临时和
	unsigned __int64 sum;
	//carry为进位标志,sub为当两数符号相异时,存放每两位数相减的临时差
	unsigned int carry = 0, sub;

	//符号相同时,进行减法运算
	if (a.sign == b.sign)
	{
		largeInt tempa, tempb, ka(a), kb(b);
		ka.sign = 1;
		kb.sign = 1;

		//取出a,b中绝对值较大的作为被减数
		if (ka < kb)
		{
			tempa = b;
			tempb = a;
			tempa.sign = !tempa.sign;
		}
		else
		{
			tempa = a;
			tempb = b;
		}

		//每一位进行竖式减
		for (int i = 0; i < MaxSize; i++)
		{
			sub = tempb.data[i] + carry;
			if (tempa.data[i] >= sub)
			{
				result.data[i] = tempa.data[i] - sub;
				carry = 0;
			}
			else
			{
				//借位减
				result.data[i] = (unsigned __int64)tempa.data[i] + (1 << 32) - sub;
				carry = 1;
			}
		}
		result.sign = tempa.sign;
		return result;
	}

	//两数符号不同时,进行加法运算
	else
	{
		//每一位进行竖式相加
		for (int i = 0; i < MaxSize; i++)
		{
			sum = (unsigned __int64)a.data[i] + b.data[i] + carry;
			result.data[i] = (unsigned int)sum;
			//sum的高位为进位
			carry = (sum >> 32);
		}
		result.sign = a.sign;
		return result;
	}
}

// 大数减一个int数
largeInt operator- (const largeInt& a, const int& b)
{
	largeInt tem(b);
	largeInt result = a - tem;
	return result;
}

// 大数相乘,采用竖式乘
largeInt operator* (const largeInt& a, const largeInt& b)
{
	//last存放竖式上一行的积,temp存放当前行的积
	largeInt result, last, temp;
	//sum存放当前行带进位的积
	unsigned __int64 sum;
	//存放进位
	unsigned int carry;

	//进行竖式乘
	for (int i = 0; i < b.GetLength(); i++)
	{
		carry = 0;
		//B的每一位与A相乘
		for (int j = 0; j < a.GetLength() + 1; j++)
		{
			sum = ((unsigned __int64)a.data[j]) * (b.data[i]) + carry;
			if ((i + j) < MaxSize)
				temp.data[i + j] = (unsigned int)sum;
			carry = (sum >> 32);
		}
		result = (temp + last);
		last = result;
		temp.Clear();
	}

	//判断积的符号
	if (a.sign == b.sign)
		result.sign = true;
	else
		result.sign = false;

	return result;
}

// 大数乘以一个INT数
largeInt operator* (const largeInt& a, const unsigned int& b)
{
	largeInt result;

	//存放B乘以A的每一位的临时积
	unsigned __int64 sum;
	//存放进位
	unsigned int carry = 0;

	for (int i = 0; i < MaxSize; i++)
	{
		sum = ((unsigned __int64)a.data[i]) * b + carry;
		result.data[i] = (unsigned int)sum;
		//进位在SUM的高位中
		carry = (sum >> 32);
	}
	result.sign = a.sign;
	return result;
}

// 大数除
largeInt operator/ (const largeInt& a, const largeInt& b)
{
	//mul为当前试商,low,high为二分查找试商时所用的标志
	unsigned int mul, low, high;
	//sub为除数与当前试商的积,subsequent为除数与下一试商的积
	//dividend存放临时被除数
	largeInt dividend, quotient, sub, subsequent;
	int lengtha = a.GetLength(), lengthb = b.GetLength();

	//如果被除数小于除数,直接返回0
	if (a < b)
	{
		if (a.sign == b.sign)
			quotient.sign = true;
		else
			quotient.sign = false;
		return quotient;
	}

	//把被除数按除数的长度从高位截位
	int i;
	for (i = 0; i < lengthb; i++)
		dividend.data[i] = a.data[lengtha - lengthb + i];

	for (i = lengtha - lengthb; i >= 0; i--)
	{
		//如果被除数小于除数,再往后补位
		if (dividend < b)
		{
			for (int j = lengthb; j > 0; j--)
				dividend.data[j] = dividend.data[j - 1];
			dividend.data[0] = a.data[i - 1];
			continue;
		}

		low = 0;
		high = 0xffffffff;

		//二分查找法查找试商
		while (low < high)
		{
			mul = (((unsigned __int64)high) + low) / 2;
			sub = (b * mul);
			subsequent = (b * (mul + 1));

			if (((sub < dividend) && (subsequent > dividend)) || (sub == dividend))
				break;
			if (subsequent == dividend)
			{
				mul++;
				sub = subsequent;
				break;
			}
			if ((sub < dividend) && (subsequent < dividend))
			{
				low = mul;
				continue;
			}
			if ((sub > dividend) && (subsequent > dividend))
			{
				high = mul;
				continue;
			}

		}

		//试商结果保存到商中去
		quotient.data[i] = mul;
		//临时被除数变为被除数与试商积的差
		dividend = dividend - sub;

		//临时被除数往后补位
		if ((i - 1) >= 0)
		{
			for (int j = lengthb; j > 0; j--)
				dividend.data[j] = dividend.data[j - 1];
			dividend.data[0] = a.data[i - 1];
		}
	}

	//判断商的符号
	if (a.sign == b.sign)
		quotient.sign = true;
	else
		quotient.sign = false;
	return quotient;
}

// 大数求模运算
largeInt operator% (const largeInt& a, const largeInt& b)
{
	unsigned int mul, low, high;
	largeInt dividend, quotient, sub, subsequent;
	int lengtha = a.GetLength(), lengthb = b.GetLength();

	//如果被除数小于除数,返回被除数为模
	if (a < b)
	{
		dividend = a;
		//余数的商永远与被除数相同
		dividend.sign = a.sign;
		return dividend;
	}

	//进行除法运算
	int i;
	for (i = 0; i < lengthb; i++)
		dividend.data[i] = a.data[lengtha - lengthb + i];

	for (i = lengtha - lengthb; i >= 0; i--)
	{
		if (dividend < b)
		{
			for (int j = lengthb; j > 0; j--)
				dividend.data[j] = dividend.data[j - 1];
			dividend.data[0] = a.data[i - 1];
			continue;
		}

		low = 0;
		high = 0xffffffff;

		while (low <= high)
		{
			mul = (((unsigned __int64)high) + low) / 2;
			sub = (b * mul);
			subsequent = (b * (mul + 1));

			if (((sub < dividend) && (subsequent > dividend)) || (sub == dividend))
				break;
			if (subsequent == dividend)
			{
				mul++;
				sub = subsequent;
				break;
			}
			if ((sub < dividend) && (subsequent < dividend))
			{
				low = mul;
				continue;
			}
			if ((sub > dividend) && (subsequent > dividend))
			{
				high = mul;
				continue;
			}
		}

		quotient.data[i] = mul;
		dividend = dividend - sub;
		if ((i - 1) >= 0)
		{
			for (int j = lengthb; j > 0; j--)
				dividend.data[j] = dividend.data[j - 1];
			dividend.data[0] = a.data[i - 1];
		}
	}

	//临时被除数即为所求模
	dividend.sign = a.sign;
	return dividend;
}

//PC-1选位表
int pc1Table[56] = {
	57,49,41,33,25,17,9,1,
	58,50,42,34,26,18,10,2,
	59,51,43,35,27,19,11,3,
	60,52,44,36,63,55,47,39,
	31,23,15,7,62,54,46,38,
	30,22,14,6,61,53,45,37,
	29,21,13,5,28,20,12,4
};
//PC-2选位表
int pc2Table[48] =
{
	14,17,11,24,1,5,
	3,28,15,6,21,10,
	23,19,12,4,26,8,
	16,7,27,20,13,2,
	41,52,31,37,47,55,
	30,40,51,45,33,48,
	44,49,39,56,34,53,
	46,42,50,36,29,32
};
//左移位数表
int loopTable[16] =
{
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};
//IP表（初始置换表）
int ipTable[64] =
{
	58,50,42,34,26,18,10,2,
	60,52,44,36,28,20,12,4,
	62,54,46,38,30,22,14,6,
	64,56,48,40,32,24,16,8,
	57,49,41,33,25,17,9,1,
	59,51,43,35,27,19,11,3,
	61,53,45,37,29,21,13,5,
	63,55,47,39,31,23,15,7
};
//逆置换IP^-1表(解密时IP初始置换的逆)
int ipReverseTable[64] = {
	40,8,48,16,56,24,64,32,
	39,7,47,15,55,23,63,31,
	38,6,46,14,54,22,62,30,
	37,5,45,13,53,21,61,29,
	36,4,44,12,52,20,60,28,
	35,3,43,11,51,19,59,27,
	34,2,42,10,50,18,58,26,
	33,1,41,9,49,17,57,25
};
//扩展置换表(E盒)
int extendTable[48] =
{
	32,1,2,3,4,5,
	4,5,6,7,8,9,
	8,9,10,11,12,13,
	12,13,14,15,16,17,
	16,17,18,19,20,21,
	20,21,22,23,24,25,
	24,25,26,27,28,29,
	28,29,30,31,32,1
};

//八个S盒，三维数组存储
int sBox[8][4][16] =
{
	//S1
	14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
	0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
	4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
	15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,
	//S2
	15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
	3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
	0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
	13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9,
	//S3
	10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
	13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
	13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
	1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,
	//S4
	7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
	13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
	10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
	3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14,
	//S5
	2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
	14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
	4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
	11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,
	//S6
	12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
	10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
	9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
	4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,
	//S7
	4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
	13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
	1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
	6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,
	//S8
	13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
	1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
	7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
	2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
};

//置换表(P盒)
int pTable[32] =
{
	16,7,20,21,
	29,12,28,17,
	1,15,23,26,
	5,18,31,10,
	2,8,24,14,
	32,27,3,9,
	19,13,30,6,
	22,11,4,25
};

void shift_left(bitset<28>& k, int round) {
	string k_s = k.to_string();
	//将最左侧的loopTable[round]（要移动的位数）位放到最后，然后取最右侧28位即可
	k_s = (k_s + k_s.substr(0, loopTable[round])).substr(loopTable[round], 28);
	bitset<28> tem(k_s);
	k = tem;
}

void generate_keys(bitset<64>key, bitset<48>* keys) {
	//置换选择1
	bitset<56> tem;
	for (int i = 0; i < 56; i++) {
		tem[56 - 1 - i] = key[64 - 1 - (pc1Table[i] - 1)];
	}

	bitset<56> round_input = tem;
	for (int i = 0; i < 16; i++) {//16轮
		//拆分为左右两部分:R , L
		string L_s = round_input.to_string().substr(0, 28); //提取高28位，转为字符串
		bitset<28> L(L_s); //以字符串来初始化左半部分
		//右半部分同理
		string R_s = round_input.to_string().substr(28, 28); //提取低28位，转为字符串
		bitset<28> R(R_s); //以字符串来初始化右半部分

		//左循环移位
		shift_left(L, i);
		shift_left(R, i);

		//将结果作为下一轮的输入
		string round_input_s = L.to_string() + R.to_string();
		bitset<56> temp(round_input_s);
		round_input = temp;
		//此时，L、R拼接产生的结果存在round_input中

		//置换选择2
		for (int j = 0; j < 48; j++) {
			keys[i][48 - 1 - j] = round_input[56 - 1 - (pc2Table[j] - 1)];
		}

	}
}


void init_map(bitset<64>& txt) {
	bitset<64> tem = txt;
	for (int i = 0; i < 64; i++) {
		tem[64 - 1 - i] = txt[64 - 1 - (ipTable[i] - 1)];
	}
	txt = tem;
}

bitset<32> Sbox(bitset <48> k) {
	bitset<32> S_R;
	string k_s = k.to_string();
	//将k拆分为8个6位的小段
	bitset<6> ss[8];
	for (int i = 0; i < 8; i++) {
		string ss_s = k_s.substr(i * 6, 6);
		bitset<6> tem(ss_s);
		ss[i] = tem;
	}

	//最终生成8个4位小段
	bitset<4> SS[8];
	string S_Rs = "";
	for (int i = 0; i < 8; i++) {
		int row = ss[i][6 - 1 - 0] * 2 + ss[i][6 - 1 - 5]; //最后两位拼起来，决定行数
		int col = ss[i][6 - 1 - 1] * 8 + ss[i][6 - 1 - 2] * 4 + ss[i][6 - 1 - 3] * 2 + ss[i][6 - 1 - 4] * 1; //中间四位拼起来，决定列数

		bitset<4> tem(sBox[i][row][col]);
		SS[i] = tem;
		//8个4位小段拼成32位结果
		S_Rs += SS[i].to_string();
	}
	bitset<32> tem(S_Rs);
	S_R = tem;
	return S_R;
}

bitset<64> rounds(bitset<64> txt, bitset<48>* keys) {
	bitset<64> out;
	//分为左右32位:R , L
	string L_s = txt.to_string().substr(0, 32); //提取高32位，转为字符串
	bitset<32> L(L_s); //以字符串来初始化左半部分
	//右半部分同理
	string R_s = txt.to_string().substr(32, 32); //提取低28位，转为字符串
	bitset<32> R(R_s); //以字符串来初始化左半部分

	//16轮
	for (int i = 0; i < 16; i++) {
		bitset<32> temp = R; //将来赋给L

		//对于R
		//扩展置换
		bitset<48> E_R; //进行扩展置换后的R
		for (int i = 0; i < 48; i++) {
			E_R[48 - 1 - i] = R[32 - 1 - (extendTable[i] - 1)];
		}
		//和本轮的key[i]进行逐比特异或
		E_R = E_R ^ keys[i];

		//S盒
		bitset<32> S_R = Sbox(E_R); //进行S盒代换后的R

		//置换P
		bitset<32> P_R;
		for (int i = 0; i < 32; i++) {
			P_R[32 - 1 - i] = S_R[32 - 1 - (pTable[i] - 1)];
		}
		R = L ^ P_R;
		L = temp;
	}

	//左右交换
	bitset<32> tem = L;
	L = R;
	R = tem;

	//左右拼接
	string outs = L.to_string() + R.to_string();
	bitset<64> t(outs);
	out = t;

	return out;
}

//产生一个随机大数,大数的LENGTH为SIZE的1/4（512bit）
void largeInt::Random()
{
	for (int i = 0; i < (MaxSize / 4); i++) {
		//由于RAND()最大只能产生0X7FFF的数,为了能产生32位的随机数,需要
		//3次RAND()操作
		data[i] = (rand() << 17) + (rand() << 2) + rand() % 4;
		if ((i + 1) % 2 == 0) { //每生成两个，就进行一次des
			bitset<32> m1(data[i]), m2(data[i - 1]);
			string s = m1.to_string() + m2.to_string();
			bitset<64> m(s); //明文

			bitset<32> k1(data[i] + rand()), k2(data[i - 1] + rand());
			s = k1.to_string() + k2.to_string();
			bitset<64> key(s); //密钥

			bitset<48> keys[16];

			//生成16轮密钥
			generate_keys(key, keys);

			//初始置换
			init_map(m);

			//轮结构与左右交换
			bitset<64> out = rounds(m, keys);

			//逆初始置换
			bitset<64> tem;
			for (int i = 0; i < 64; i++) {
				tem[64 - 1 - i] = out[64 - 1 - (ipReverseTable[i] - 1)];
			}
			out = tem;

			//将out赋给data[i] data[i-1]
			data[i] = out.to_ullong() & 0xffffffff; //低32位
			data[i - 1] = (out.to_ullong() >> 32) & 0xffffffff;//高32位
		}
	}
}

// 产生一个较小的随机大数,大数的LENGTH为SIZE的1/8
void largeInt::RandomSmall()
{
	for (int i = 0; i < (MaxSize / 16); i++)
		//由于RAND()最大只能产生0X7FFF的数,为了能产生32位的随机数,需要
		//3次RAND()操作
		data[i] = (rand() << 17) + (rand() << 2) + rand() % 4;
}

void largeInt::to_str(char s[])
{
	int k = 0;
	unsigned int temp, result;
	unsigned int tempAnd = 0xf0000000;
	for (int i = GetLength() - 1; i >= 0; i--)
	{
		temp = data[i];
		//大数的每一位数字转换成16进制输出
		for (int j = 0; j < 8; j++)
		{
			result = temp & tempAnd;
			result = (result >> 28);
			temp = (temp << 4);
			if (result >= 0 && result <= 9)
				s[k++] = result + '0';
			else
			{
				switch (result)
				{
				case 10:
					s[k++] = 'A';
					break;
				case 11:
					s[k++] = 'B';
					break;
				case 12:
					s[k++] = 'C';
					break;
				case 13:
					s[k++] = 'D';
					break;
				case 14:
					s[k++] = 'E';
					break;
				case 15:
					s[k++] = 'F';
					break;
				}
			}
		}
	}
	s[k] = '\0';
}

// 将大数输出到输入输出流
void largeInt::Output(ostream& out) const
{
	unsigned int temp, result;
	unsigned int tempAnd = 0xf0000000;

	for (int i = GetLength() - 1; i >= 0; i--)
	{
		temp = data[i];
		//大数的每一位数字转换成16进制输出
		for (int j = 0; j < 8; j++)
		{
			result = temp & tempAnd;
			result = (result >> 28);
			temp = (temp << 4);
			if (result >= 0 && result <= 9)
				out << result;
			else
			{
				switch (result)
				{
				case 10:
					out << "A";
					break;
				case 11:
					out << "B";
					break;
				case 12:
					out << "C";
					break;
				case 13:
					out << "D";
					break;
				case 14:
					out << "E";
					break;
				case 15:
					out << "F";
					break;
				}
			}
		}
		cout << " ";
		if (i == GetLength() / 2)
			cout << endl;
	}
	out << endl;
}

//输出
ostream& operator<< (ostream& out, const largeInt& x)
{
	x.Output(out);
	return out;
}

// 大数置零
void largeInt::Clear()
{
	for (int i = 0; i < MaxSize; i++)
		data[i] = 0;
	sign = 1;
}

// 返回大数长度
// 返回的是int数组中不为0的长度
inline int largeInt::GetLength() const
{
	int length = MaxSize;
	for (int i = MaxSize - 1; i >= 0; i--)
	{
		//第一位不为0即为LENGTH
		if (data[i] == 0)
			length--;
		else
			break;
	}

	return length;
}

// 重载移位操作符,向右移a位
void largeInt::operator>> (const int& a)
{
	unsigned int bit;
	data[0] = (data[0] >> a);
	for (int i = 1; i < GetLength(); i++)
	{
		//先将每一位的低位移到BIT中
		bit = data[i] & 1;
		//再把BIT移到上一位的高位中
		bit = bit << (32 - a);;
		data[i - 1] = data[i - 1] | bit;
		data[i] = (data[i] >> a);
	}
}

// 判断大数和一个INT的大小
bool operator<= (const largeInt& a, const int& b)
{
	if (b >= 0 && a.sign == 1) {
		for (int i = 1; i < a.GetLength(); i++)
		{
			if (a.data[i] != 0)
				return false;
		}
		if (a.data[0] <= b)
			return true;
		else
			return false;
	}
	if (b < 0 && a.sign == 0) {
		for (int i = 1; i < a.GetLength(); i++)
		{
			if (a.data[i] != 0)
				return false;
		}
		if (a.data[0] >= abs(b))
			return true;
		else
			return false;
	}
	if (b < 0 && a.sign == 1) {
		return false;
	}
	if (b >= 0 && a.sign == 1) {
		return true;
	}
}


// 模幂运算——计算n的p次幂模m
// 利用Montgomery算法
largeInt Power(const largeInt& n, const largeInt& p, const largeInt& m)
{
	largeInt temp = p;
	largeInt base = n % m;
	largeInt result(1);

	//检测指数p的二进制形式的每一位
	while (!(temp <= 1))
	{
		//如果该位为1，则表示该位需要参与模运算
		if (temp.IsOdd())
		{
			result = (result * base) % m;
		}
		base = (base * base) % m;
		temp >> 1;
	}
	return (base * result) % m;
}