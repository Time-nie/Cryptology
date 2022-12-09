#include<iostream>
#include<bitset>
#include<bitset>
#include<iomanip>
using namespace std;

int S[16][16];
int rS[16][16];

// 十六进制转二进制
bitset<8> Hex_Bin(const int num_dec)
{
	bitset<8> num_bin;

	//高位和低位
	int high = num_dec / 16;
	int low = num_dec % 16;
	for (int j = 0; j < 4; j++)
	{
		num_bin[j] = low % 2;
		num_bin[4 + j] = high % 2;
		low /= 2;
		high /= 2;
	}
	return num_bin;
}

//最大公因数
long gcd(long a, long b)
{
	long tmp;
	while (b)
	{
		tmp = a;
		a = b;
		b = tmp % b;
	}
	return a;
}

//GF(2^8)的多项式乘法
int GfMulti(int a, int b)
{
	int tmp[8] = { 0 };
	int i;
	for (i = 0; i < 8; i++)
	{
		tmp[i] = (a << i) * ((b >> i) & 0x1);
	}

	tmp[0] = tmp[0] ^ tmp[1] ^ tmp[2] ^ tmp[3] ^ tmp[4] ^ tmp[5] ^ tmp[6] ^ tmp[7];

	return tmp[0];
}

//找到最高位
int findHigherBit(int val)
{
	int i = 0;
	while (val)
	{
		i++;
		val = val >> 1;
	}
	return i;
}

//GF(2^8)的多项式除法
int Gfdiv(int div_ed, int div, int* remainder)
{
	int r0 = 0;
	int  qn = 0;
	int bitCnt = 0;

	r0 = div_ed;

	bitCnt = findHigherBit(r0) - findHigherBit(div);
	while (bitCnt >= 0)
	{
		qn = qn | (1 << bitCnt);
		r0 = r0 ^ (div << bitCnt);
		bitCnt = findHigherBit(r0) - findHigherBit(div);
	}
	*remainder = r0;
	return qn;
}

//GF(2^8)多项式的扩展欧几里得算法
int externEuc(int a, int m)
{
	int r0, r1, r2;
	int  qn, v0, v1, v2, w0, w1, w2;
	r0 = m;
	r1 = a;
	v0 = 1;
	v1 = 0;
	w0 = 0;
	w1 = 1;
	while (r1 != 1)
	{
		qn = Gfdiv(r0, r1, &r2);
		v2 = v0 ^ GfMulti(qn, v1);
		w2 = w0 ^ GfMulti(qn, w1);
		r0 = r1;
		r1 = r2;
		v0 = v1;
		v1 = v2;
		w0 = w1;
		w1 = w2;
	}
	return w1;
}

//S盒字节变换
int byteTransformation(int a, int x)
{
	int tmp[8] = { 0 };

	for (int i = 0; i < 8; i++)
	{
		tmp[i] = (((a >> i) & 0x1) ^ ((a >> ((i + 4) % 8)) & 0x1) ^ ((a >> ((i + 5) % 8)) & 0x1) ^ ((a >> ((i + 6) % 8)) & 0x1) ^ ((a >> ((i + 7) % 8)) & 0x1) ^ ((x >> i) & 0x1)) << i;
	}
	tmp[0] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
	return tmp[0];
}

//逆S盒字节变换
int invByteTransformation(int a, int x)
{
	int tmp[8] = { 0 };

	for (int i = 0; i < 8; i++)
	{
		tmp[i] = (((a >> ((i + 2) % 8)) & 0x1) ^ ((a >> ((i + 5) % 8)) & 0x1) ^ ((a >> ((i + 7) % 8)) & 0x1) ^ ((x >> i) & 0x1)) << i;
	}
	tmp[0] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
	return tmp[0];
}

//S盒产生
void s_box_gen(void)
{
	int i, j;
	int s_box_ary[16][16] = { 0 };

	//初始化S盒
	for (i = 0; i < 0x10; i++)
		for (j = 0; j < 0x10; j++)
			s_box_ary[i][j] = ((i << 4) & 0xF0) + (j & (0xF));

	//求在GF(2^8)域上的逆，0映射到自身
	for (i = 0; i < 0x10; i++)
	{
		for (j = 0; j < 0x10; j++)
			if (s_box_ary[i][j] != 0)
				s_box_ary[i][j] = externEuc(s_box_ary[i][j], 0x11B);
	}

	//对每个字节做变换
	for (i = 0; i < 0x10; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			s_box_ary[i][j] = byteTransformation(s_box_ary[i][j], 0x63);
			S[i][j] = s_box_ary[i][j];
		}
	}
}


//逆S盒产生
void inv_s_box_gen(void)
{
	int i, j;
	int s_box_ary[16][16] = { 0 };
	int b = 0, bb = 0;

	//初始化S盒
	for (i = 0; i < 0x10; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			s_box_ary[i][j] = ((i << 4) & 0xF0) + (j & (0xF));
		}
	}
	//对每个字节做变换
	for (i = 0; i < 0x10; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			s_box_ary[i][j] = invByteTransformation(s_box_ary[i][j], 0x05);
		}
	}

	//求在GF(2^8)域上的逆，0映射到自身
	for (i = 0; i < 0x10; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			if (s_box_ary[i][j] != 0)
			{
				s_box_ary[i][j] = externEuc(s_box_ary[i][j], 0x11B);
				rS[i][j] = s_box_ary[i][j];
			}
		}
	}
}


//轮常量
static int rC[10] = {
	0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36
};

//AES的乘法运算（在GF(2^8)中）
int aesMult(int a, int b)
{
	int thr = b & 0x8;  //b&1000
	int sec = b & 0x4;  //b&0100
	int fir = b & 0x2;  //b&0010
	int firstMod = b % 2;    //b&0001
	int res = 0;

	if (thr)
	{
		int temp = a;
		for (int i = 1; i <= 3; ++i)
		{
			temp = temp << 1;    //总共移3位
			if (temp >= 256)
			{
				temp = temp ^ 0x11b;   //x^8+x^4+x^3+x+1
			}
		}
		temp = temp % 256;
		res = res ^ temp;
	}
	if (sec)
	{
		int temp = a;
		for (int i = 1; i <= 2; ++i)
		{
			temp = temp << 1;     //总共移2位
			if (temp >= 256)
			{
				temp = temp ^ 0x11b;
			}
		}
		temp = temp % 256;
		res = res ^ temp;
	}
	if (fir)
	{
		int temp = a;
		temp = temp << 1;         //总共移1位
		if (temp >= 256)
		{
			temp = temp ^ 0x11b;
		}
		temp = temp % 256;
		res = res ^ temp;
	}
	if (firstMod)
	{
		res = res ^ a;          //不用移位
	}
	return res;
}

//密钥扩展
void KeyExpansion(int key[4][4], int w[11][4][4])
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; j++)
			w[0][i][j] = key[j][i];

	for (int i = 1; i < 11; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			int temp[4];
			if (j == 0)    
			{
				//先进行字节为单位的循环左移位
				temp[0] = w[i - 1][3][1];
				temp[1] = w[i - 1][3][2];
				temp[2] = w[i - 1][3][3];
				temp[3] = w[i - 1][3][0];
				for (int k = 0; k < 4; ++k)
				{
					int m = temp[k];
					int row = m / 16;
					int col = m % 16;
					temp[k] = S[row][col];     //根据行号列号到S盒中取值
					if (k == 0)
						temp[k] = temp[k] ^ rC[i - 1];
				}
			}
			else     //mod4!=0
			{
				temp[0] = w[i][j - 1][0];
				temp[1] = w[i][j - 1][1];
				temp[2] = w[i][j - 1][2];
				temp[3] = w[i][j - 1][3];
			}
			for (int x = 0; x < 4; x++)
				w[i][j][x] = w[i - 1][j][x] ^ temp[x];
		}
	}
}

// 字节代换
void ByteSub(int in[4][4], int type)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int temp = in[i][j];
			int row = temp / 16;
			int col = temp % 16;
			if (type == 1)
				in[i][j] = S[row][col];

			if (type == 0)
				in[i][j] = rS[row][col];
		}
	}
}

void shiftRow(int in[4][4], int type) {
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (type == 1)
			{
				int temp = in[i][0];
				in[i][0] = in[i][1];
				in[i][1] = in[i][2];
				in[i][2] = in[i][3];
				in[i][3] = temp;
			}
			else
			{
				int temp = in[i][3];
				in[i][3] = in[i][2];
				in[i][2] = in[i][1];
				in[i][1] = in[i][0];
				in[i][0] = temp;
			}
		}
	}
}

//列混合
void MixColumn(int in[4][4], int type)
{
	for (int i = 0; i < 4; i++)
	{
		int t0 = in[0][i];
		int t1 = in[1][i];
		int t2 = in[2][i];
		int t3 = in[3][i];
		if (type == 1)
		{
			in[0][i] = aesMult(t0, 2) ^ aesMult(t1, 3) ^ t2 ^ t3;
			in[1][i] = t0 ^ aesMult(t1, 2) ^ aesMult(t2, 3) ^ t3;
			in[2][i] = t0 ^ t1 ^ aesMult(t2, 2) ^ aesMult(t3, 3);
			in[3][i] = aesMult(t0, 3) ^ t1 ^ t2 ^ aesMult(t3, 2);
		}
		else
		{
			in[0][i] = aesMult(t0, 14) ^ aesMult(t1, 11) ^ aesMult(t2, 13) ^ aesMult(t3, 9);
			in[1][i] = aesMult(t0, 9) ^ aesMult(t1, 14) ^ aesMult(t2, 11) ^ aesMult(t3, 13);
			in[2][i] = aesMult(t0, 13) ^ aesMult(t1, 9) ^ aesMult(t2, 14) ^ aesMult(t3, 11);
			in[3][i] = aesMult(t0, 11) ^ aesMult(t1, 13) ^ aesMult(t2, 9) ^ aesMult(t3, 14);
		}
	}
}

//轮密钥加
void AddRoundKey(int in[4][4], int key[4][4])
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; j++)
			in[i][j] = in[i][j] ^ key[j][i];
}

// 加密总函数
void Encrypt(int in[4][4], int key[4][4])
{
	int type = 1;
	int subKey[11][4][4];
	KeyExpansion(key, subKey);    // 密钥扩展
	AddRoundKey(in, subKey[0]);     //轮密钥加
	for (int i = 1; i <= 10; ++i)
	{
		ByteSub(in, type);  //字节代换
		shiftRow(in, type);    //行移位
		if (i != 10)      //最后一次计算不需要列混合
			MixColumn(in, type);   //列混合
		AddRoundKey(in, subKey[i]);   //密钥加
	}
}

// 解密总函数
void Decode(int in[4][4], int key[4][4])
{
	int type = 0;
	int subKey[11][4][4];
	KeyExpansion(key, subKey);
	AddRoundKey(in, subKey[10]);    //轮密钥加，和加密是反过来的
	for (int i = 9; i >= 0; --i)
	{
		ByteSub(in, type);  
		shiftRow(in, type);
		AddRoundKey(in, subKey[i]);        //subkey与加密相反
		if (i != 0)
			MixColumn(in, type);
	}
}

int main()
{
	cout << "================= AES 加密&解密 ====================" << endl << endl;
	s_box_gen();   //生成S盒
	inv_s_box_gen();  //生成逆S盒
	int type;
	cout << "========== 1——检验密钥雪崩效应  2——检验明文雪崩效应 =========" << endl;

	while (true)
	{
		int encrypt_text_original[4][4];
		int txt[4][4];
		int key[4][4];
		cin >> type;
		int total = 0;
		switch (type)
		{
			// 加密过程
		case 1:
			cout << "========== 1——检验密钥雪崩效应 =========" << endl;
			cout << "请输入128位明文（十六进制）：" << endl;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
				{
					cin >> (hex) >> txt[j][i];
					encrypt_text_original[j][i] = txt[j][i];
				}

			cout << "请输入128位密钥（十六进制）：" << endl;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					cin >> (hex) >> key[j][i];
			cout << endl;
			Encrypt(encrypt_text_original, key);  //加密过程

			cout << endl;
			cout << "========================================" << endl << endl;


			// 分别改变密钥每一位检验雪崩效应
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					for (int k = 0; k < 8; k++)
					{
						int temp_key[4][4], temp_txt[4][4];
						for (int u = 0; u < 4; u++)
							for (int v = 0; v < 4; v++)
							{
								temp_key[v][u] = key[j][i];
								temp_txt[v][u] = txt[j][i];
							}

						bitset<8> m = Hex_Bin(temp_key[j][i]);
						m[k] = 1 - m[k];
						int res = 0;
						for (int i = 0; i < 8; i++)
						{
							res *= 2;
							res += m[7 - i];
						}
						temp_key[j][i] = res;
						Encrypt(temp_txt, temp_key);  //加密过程

						int num = 0;
						for (int u = 0; u < 4; u++)
							for (int v = 0; v < 4; v++)
							{
								bitset<8> bit_current = Hex_Bin(temp_txt[v][u]);
								bitset<8> bit_original = Hex_Bin(encrypt_text_original[v][u]);
								for (int x = 0; x < 8; x++)
									if (bit_current[x] != bit_original[x])
										num++;
							}
						total += num;

						if ((i * 32 + j * 8 + k) % 10 == 0)
							cout << "改变第" << (i * 32 + j * 8 + k) << "位密钥—密文改变位数为：" << num << endl;
					}
				}
			}
			cout << setprecision(4) << total / 128 << endl;
			cout << "========================================" << endl;
			cout << endl << "========== 1——检验密钥雪崩效应  2——检验明文雪崩效应 =========" << endl;
			continue;

		case 2:
			cout << "========== 2——检验明文雪崩效应 =========" << endl;
			cout << "请输入128位明文（十六进制）：" << endl;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
				{
					cin >> (hex) >> txt[j][i];
					encrypt_text_original[j][i] = txt[j][i];
				}

			cout << "请输入128位密钥（十六进制）：" << endl;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					cin >> (hex) >> key[j][i];
			cout << endl;
			Encrypt(encrypt_text_original, key);  //加密过程

			cout << endl;
			cout << "========================================" << endl << endl;
			// 分别改变明文每一位检验雪崩效应
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					for (int k = 0; k < 8; k++)
					{
						int temp_key[4][4], temp_txt[4][4];
						for (int u = 0; u < 4; u++)
							for (int v = 0; v < 4; v++)
								temp_txt[v][u] = txt[j][i];

						bitset<8> m = Hex_Bin(temp_txt[j][i]);
						m[k] = 1 - m[k];
						int res = 0;
						for (int i = 0; i < 8; i++)
						{
							res *= 2;
							res += m[7 - i];
						}
						temp_txt[j][i] = res;
						Encrypt(temp_txt, key);  //加密过程

						int num = 0;
						for (int u = 0; u < 4; u++)
							for (int v = 0; v < 4; v++)
							{
								bitset<8> bit_current = Hex_Bin(temp_txt[v][u]);
								bitset<8> bit_original = Hex_Bin(encrypt_text_original[v][u]);
								for (int x = 0; x < 8; x++)
									if (bit_current[x] != bit_original[x])
										num++;
							}
						total += num;

						if ((i * 32 + j * 8 + k) % 10 == 0)
							cout << "改变第" << (i * 32 + j * 8 + k) << "位明文—密文改变位数为：" << num << endl;
					}
				}
			}
			cout << setprecision(4) << total / 128 << endl;
			cout << "========================================" << endl;
			cout << endl << "========== 1——检验密钥雪崩效应  2——检验明文雪崩效应 =========" << endl;
			continue;
		}
	}
	system("pause");
}