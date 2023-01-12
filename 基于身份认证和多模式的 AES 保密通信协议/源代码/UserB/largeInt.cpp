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
	data[0] = abs(input); //ȡ����ֵ

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



// ��ֵ����
void largeInt::operator= (const largeInt& input)
{
	for (int i = 0; i < MaxSize; i++)
		data[i] = input.data[i];
	sign = input.sign;
}

bool operator< (const largeInt& a, const largeInt& b)
{
	//ע������
	if (a.sign == true && b.sign == false) {
		return false;
	}

	if (a.sign == false && b.sign == true) {
		return true;
	}

	if (a.sign == true && b.sign == true) {
		//�����λ����
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
		//�����λ����
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
	//ע������
	if (a.sign == true && b.sign == false) {
		return true;
	}

	if (a.sign == false && b.sign == true) {
		return false;
	}

	if (a.sign == true && b.sign == true) {
		//�����λ����
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
		//�����λ����
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

// �ж����������Ƿ����
bool operator== (const largeInt& a, const largeInt& b)
{
	if (a.sign != b.sign) return false;
	for (int i = 0; i < MaxSize; i++)
		if (a.data[i] != b.data[i])
			return false;
	return true;
}

// �ж�һ��������һ��intֵ�Ƿ����
bool operator== (const largeInt& a, const int& b)
{
	if (b >= 0 && a.sign != true) return false;
	if (b < 0 && a.sign != false) return false;
	for (int i = 1; i < a.GetLength(); i++)
		if (a.data[i] != 0)
			return false;
	return a.data[0] == b;
}



//��ʽ�ӷ�,�������������ĺ�
largeInt operator+ (const largeInt& a, const largeInt& b)
{
	largeInt result;
	//64λ����,���ÿ��λ����ӵ���ʱ��
	unsigned __int64 sum;
	//carry ��λ��־     sub ��������������ʱ,���ÿ��λ������Ĳ�
	unsigned int carry = 0, sub;

	//ȡa,b�г��Ƚϳ��ĳ���
	int length = a.GetLength() >= b.GetLength() ? a.GetLength() : b.GetLength();

	//������������ͬʱ,���мӷ�����
	if (a.sign == b.sign)
	{
		//ÿһλ������ʽ���
		for (int i = 0; i < length; i++)
		{
			sum = (unsigned __int64)a.data[i] + b.data[i] + carry;
			result.data[i] = (unsigned int)sum; //ȡ��32λ
			//sum�ĸ�λΪ��λ
			carry = (sum >> 32);
		}

		result.sign = a.sign;
		return result;
	}
	else
	{
		//�������Ų�ͬʱ,���м�������
		largeInt tempa, tempb, ka(a), kb(b);
		ka.sign = 1;
		kb.sign = 1;

		//ȡ��a,b�о���ֵ�ϴ����Ϊ������
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

		//ÿһλ������ʽ��
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
				//��λ��
				result.data[i] = (unsigned __int64)tempa.data[i] + (1 << 32) - sub;
				carry = 1;
			}
		}
		result.sign = tempa.sign;
		return result;
	}
}

//������ʽ����,�������������Ĳ�
largeInt operator- (const largeInt& a, const largeInt& b)
{
	largeInt result;
	//64λ����,���ÿ��λ����ӵ���ʱ��
	unsigned __int64 sum;
	//carryΪ��λ��־,subΪ��������������ʱ,���ÿ��λ���������ʱ��
	unsigned int carry = 0, sub;

	//������ͬʱ,���м�������
	if (a.sign == b.sign)
	{
		largeInt tempa, tempb, ka(a), kb(b);
		ka.sign = 1;
		kb.sign = 1;

		//ȡ��a,b�о���ֵ�ϴ����Ϊ������
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

		//ÿһλ������ʽ��
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
				//��λ��
				result.data[i] = (unsigned __int64)tempa.data[i] + (1 << 32) - sub;
				carry = 1;
			}
		}
		result.sign = tempa.sign;
		return result;
	}

	//�������Ų�ͬʱ,���мӷ�����
	else
	{
		//ÿһλ������ʽ���
		for (int i = 0; i < MaxSize; i++)
		{
			sum = (unsigned __int64)a.data[i] + b.data[i] + carry;
			result.data[i] = (unsigned int)sum;
			//sum�ĸ�λΪ��λ
			carry = (sum >> 32);
		}
		result.sign = a.sign;
		return result;
	}
}

// ������һ��int��
largeInt operator- (const largeInt& a, const int& b)
{
	largeInt tem(b);
	largeInt result = a - tem;
	return result;
}

// �������,������ʽ��
largeInt operator* (const largeInt& a, const largeInt& b)
{
	//last�����ʽ��һ�еĻ�,temp��ŵ�ǰ�еĻ�
	largeInt result, last, temp;
	//sum��ŵ�ǰ�д���λ�Ļ�
	unsigned __int64 sum;
	//��Ž�λ
	unsigned int carry;

	//������ʽ��
	for (int i = 0; i < b.GetLength(); i++)
	{
		carry = 0;
		//B��ÿһλ��A���
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

	//�жϻ��ķ���
	if (a.sign == b.sign)
		result.sign = true;
	else
		result.sign = false;

	return result;
}

// ��������һ��INT��
largeInt operator* (const largeInt& a, const unsigned int& b)
{
	largeInt result;

	//���B����A��ÿһλ����ʱ��
	unsigned __int64 sum;
	//��Ž�λ
	unsigned int carry = 0;

	for (int i = 0; i < MaxSize; i++)
	{
		sum = ((unsigned __int64)a.data[i]) * b + carry;
		result.data[i] = (unsigned int)sum;
		//��λ��SUM�ĸ�λ��
		carry = (sum >> 32);
	}
	result.sign = a.sign;
	return result;
}

// ������
largeInt operator/ (const largeInt& a, const largeInt& b)
{
	//mulΪ��ǰ����,low,highΪ���ֲ�������ʱ���õı�־
	unsigned int mul, low, high;
	//subΪ�����뵱ǰ���̵Ļ�,subsequentΪ��������һ���̵Ļ�
	//dividend�����ʱ������
	largeInt dividend, quotient, sub, subsequent;
	int lengtha = a.GetLength(), lengthb = b.GetLength();

	//���������С�ڳ���,ֱ�ӷ���0
	if (a < b)
	{
		if (a.sign == b.sign)
			quotient.sign = true;
		else
			quotient.sign = false;
		return quotient;
	}

	//�ѱ������������ĳ��ȴӸ�λ��λ
	int i;
	for (i = 0; i < lengthb; i++)
		dividend.data[i] = a.data[lengtha - lengthb + i];

	for (i = lengtha - lengthb; i >= 0; i--)
	{
		//���������С�ڳ���,������λ
		if (dividend < b)
		{
			for (int j = lengthb; j > 0; j--)
				dividend.data[j] = dividend.data[j - 1];
			dividend.data[0] = a.data[i - 1];
			continue;
		}

		low = 0;
		high = 0xffffffff;

		//���ֲ��ҷ���������
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

		//���̽�����浽����ȥ
		quotient.data[i] = mul;
		//��ʱ��������Ϊ�����������̻��Ĳ�
		dividend = dividend - sub;

		//��ʱ����������λ
		if ((i - 1) >= 0)
		{
			for (int j = lengthb; j > 0; j--)
				dividend.data[j] = dividend.data[j - 1];
			dividend.data[0] = a.data[i - 1];
		}
	}

	//�ж��̵ķ���
	if (a.sign == b.sign)
		quotient.sign = true;
	else
		quotient.sign = false;
	return quotient;
}

// ������ģ����
largeInt operator% (const largeInt& a, const largeInt& b)
{
	unsigned int mul, low, high;
	largeInt dividend, quotient, sub, subsequent;
	int lengtha = a.GetLength(), lengthb = b.GetLength();

	//���������С�ڳ���,���ر�����Ϊģ
	if (a < b)
	{
		dividend = a;
		//����������Զ�뱻������ͬ
		dividend.sign = a.sign;
		return dividend;
	}

	//���г�������
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

	//��ʱ��������Ϊ����ģ
	dividend.sign = a.sign;
	return dividend;
}

//PC-1ѡλ��
int pc1Table[56] = {
	57,49,41,33,25,17,9,1,
	58,50,42,34,26,18,10,2,
	59,51,43,35,27,19,11,3,
	60,52,44,36,63,55,47,39,
	31,23,15,7,62,54,46,38,
	30,22,14,6,61,53,45,37,
	29,21,13,5,28,20,12,4
};
//PC-2ѡλ��
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
//����λ����
int loopTable[16] =
{
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};
//IP����ʼ�û���
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
//���û�IP^-1��(����ʱIP��ʼ�û�����)
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
//��չ�û���(E��)
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

//�˸�S�У���ά����洢
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

//�û���(P��)
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
	//��������loopTable[round]��Ҫ�ƶ���λ����λ�ŵ����Ȼ��ȡ���Ҳ�28λ����
	k_s = (k_s + k_s.substr(0, loopTable[round])).substr(loopTable[round], 28);
	bitset<28> tem(k_s);
	k = tem;
}

void generate_keys(bitset<64>key, bitset<48>* keys) {
	//�û�ѡ��1
	bitset<56> tem;
	for (int i = 0; i < 56; i++) {
		tem[56 - 1 - i] = key[64 - 1 - (pc1Table[i] - 1)];
	}

	bitset<56> round_input = tem;
	for (int i = 0; i < 16; i++) {//16��
		//���Ϊ����������:R , L
		string L_s = round_input.to_string().substr(0, 28); //��ȡ��28λ��תΪ�ַ���
		bitset<28> L(L_s); //���ַ�������ʼ����벿��
		//�Ұ벿��ͬ��
		string R_s = round_input.to_string().substr(28, 28); //��ȡ��28λ��תΪ�ַ���
		bitset<28> R(R_s); //���ַ�������ʼ���Ұ벿��

		//��ѭ����λ
		shift_left(L, i);
		shift_left(R, i);

		//�������Ϊ��һ�ֵ�����
		string round_input_s = L.to_string() + R.to_string();
		bitset<56> temp(round_input_s);
		round_input = temp;
		//��ʱ��L��Rƴ�Ӳ����Ľ������round_input��

		//�û�ѡ��2
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
	//��k���Ϊ8��6λ��С��
	bitset<6> ss[8];
	for (int i = 0; i < 8; i++) {
		string ss_s = k_s.substr(i * 6, 6);
		bitset<6> tem(ss_s);
		ss[i] = tem;
	}

	//��������8��4λС��
	bitset<4> SS[8];
	string S_Rs = "";
	for (int i = 0; i < 8; i++) {
		int row = ss[i][6 - 1 - 0] * 2 + ss[i][6 - 1 - 5]; //�����λƴ��������������
		int col = ss[i][6 - 1 - 1] * 8 + ss[i][6 - 1 - 2] * 4 + ss[i][6 - 1 - 3] * 2 + ss[i][6 - 1 - 4] * 1; //�м���λƴ��������������

		bitset<4> tem(sBox[i][row][col]);
		SS[i] = tem;
		//8��4λС��ƴ��32λ���
		S_Rs += SS[i].to_string();
	}
	bitset<32> tem(S_Rs);
	S_R = tem;
	return S_R;
}

bitset<64> rounds(bitset<64> txt, bitset<48>* keys) {
	bitset<64> out;
	//��Ϊ����32λ:R , L
	string L_s = txt.to_string().substr(0, 32); //��ȡ��32λ��תΪ�ַ���
	bitset<32> L(L_s); //���ַ�������ʼ����벿��
	//�Ұ벿��ͬ��
	string R_s = txt.to_string().substr(32, 32); //��ȡ��28λ��תΪ�ַ���
	bitset<32> R(R_s); //���ַ�������ʼ����벿��

	//16��
	for (int i = 0; i < 16; i++) {
		bitset<32> temp = R; //��������L

		//����R
		//��չ�û�
		bitset<48> E_R; //������չ�û����R
		for (int i = 0; i < 48; i++) {
			E_R[48 - 1 - i] = R[32 - 1 - (extendTable[i] - 1)];
		}
		//�ͱ��ֵ�key[i]������������
		E_R = E_R ^ keys[i];

		//S��
		bitset<32> S_R = Sbox(E_R); //����S�д������R

		//�û�P
		bitset<32> P_R;
		for (int i = 0; i < 32; i++) {
			P_R[32 - 1 - i] = S_R[32 - 1 - (pTable[i] - 1)];
		}
		R = L ^ P_R;
		L = temp;
	}

	//���ҽ���
	bitset<32> tem = L;
	L = R;
	R = tem;

	//����ƴ��
	string outs = L.to_string() + R.to_string();
	bitset<64> t(outs);
	out = t;

	return out;
}

//����һ���������,������LENGTHΪSIZE��1/4��512bit��
void largeInt::Random()
{
	for (int i = 0; i < (MaxSize / 4); i++) {
		//����RAND()���ֻ�ܲ���0X7FFF����,Ϊ���ܲ���32λ�������,��Ҫ
		//3��RAND()����
		data[i] = (rand() << 17) + (rand() << 2) + rand() % 4;
		if ((i + 1) % 2 == 0) { //ÿ�����������ͽ���һ��des
			bitset<32> m1(data[i]), m2(data[i - 1]);
			string s = m1.to_string() + m2.to_string();
			bitset<64> m(s); //����

			bitset<32> k1(data[i] + rand()), k2(data[i - 1] + rand());
			s = k1.to_string() + k2.to_string();
			bitset<64> key(s); //��Կ

			bitset<48> keys[16];

			//����16����Կ
			generate_keys(key, keys);

			//��ʼ�û�
			init_map(m);

			//�ֽṹ�����ҽ���
			bitset<64> out = rounds(m, keys);

			//���ʼ�û�
			bitset<64> tem;
			for (int i = 0; i < 64; i++) {
				tem[64 - 1 - i] = out[64 - 1 - (ipReverseTable[i] - 1)];
			}
			out = tem;

			//��out����data[i] data[i-1]
			data[i] = out.to_ullong() & 0xffffffff; //��32λ
			data[i - 1] = (out.to_ullong() >> 32) & 0xffffffff;//��32λ
		}
	}
}

// ����һ����С���������,������LENGTHΪSIZE��1/8
void largeInt::RandomSmall()
{
	for (int i = 0; i < (MaxSize / 16); i++)
		//����RAND()���ֻ�ܲ���0X7FFF����,Ϊ���ܲ���32λ�������,��Ҫ
		//3��RAND()����
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
		//������ÿһλ����ת����16�������
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

// ��������������������
void largeInt::Output(ostream& out) const
{
	unsigned int temp, result;
	unsigned int tempAnd = 0xf0000000;

	for (int i = GetLength() - 1; i >= 0; i--)
	{
		temp = data[i];
		//������ÿһλ����ת����16�������
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

//���
ostream& operator<< (ostream& out, const largeInt& x)
{
	x.Output(out);
	return out;
}

// ��������
void largeInt::Clear()
{
	for (int i = 0; i < MaxSize; i++)
		data[i] = 0;
	sign = 1;
}

// ���ش�������
// ���ص���int�����в�Ϊ0�ĳ���
inline int largeInt::GetLength() const
{
	int length = MaxSize;
	for (int i = MaxSize - 1; i >= 0; i--)
	{
		//��һλ��Ϊ0��ΪLENGTH
		if (data[i] == 0)
			length--;
		else
			break;
	}

	return length;
}

// ������λ������,������aλ
void largeInt::operator>> (const int& a)
{
	unsigned int bit;
	data[0] = (data[0] >> a);
	for (int i = 1; i < GetLength(); i++)
	{
		//�Ƚ�ÿһλ�ĵ�λ�Ƶ�BIT��
		bit = data[i] & 1;
		//�ٰ�BIT�Ƶ���һλ�ĸ�λ��
		bit = bit << (32 - a);;
		data[i - 1] = data[i - 1] | bit;
		data[i] = (data[i] >> a);
	}
}

// �жϴ�����һ��INT�Ĵ�С
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


// ģ�����㡪������n��p����ģm
// ����Montgomery�㷨
largeInt Power(const largeInt& n, const largeInt& p, const largeInt& m)
{
	largeInt temp = p;
	largeInt base = n % m;
	largeInt result(1);

	//���ָ��p�Ķ�������ʽ��ÿһλ
	while (!(temp <= 1))
	{
		//�����λΪ1�����ʾ��λ��Ҫ����ģ����
		if (temp.IsOdd())
		{
			result = (result * base) % m;
		}
		base = (base * base) % m;
		temp >> 1;
	}
	return (base * result) % m;
}