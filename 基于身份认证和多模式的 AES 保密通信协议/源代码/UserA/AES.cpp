int S[16][16];
int rS[16][16];

//�������
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

//GF(2^8)�Ķ���ʽ�˷�
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

//�ҵ����λ
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

//GF(2^8)�Ķ���ʽ����
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

//GF(2^8)����ʽ����չŷ������㷨
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

//S���ֽڱ任
int byteTransformation(int a, int x)
{
	int tmp[8] = { 0 };

	for (int i = 0; i < 8; i++)
		tmp[i] = (((a >> i) & 0x1) ^ ((a >> ((i + 4) % 8)) & 0x1) ^ ((a >> ((i + 5) % 8)) & 0x1) ^ ((a >> ((i + 6) % 8)) & 0x1) ^ ((a >> ((i + 7) % 8)) & 0x1) ^ ((x >> i) & 0x1)) << i;
	tmp[0] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
	return tmp[0];
}

//��S���ֽڱ任
int invByteTransformation(int a, int x)
{
	int tmp[8] = { 0 };

	for (int i = 0; i < 8; i++)
		tmp[i] = (((a >> ((i + 2) % 8)) & 0x1) ^ ((a >> ((i + 5) % 8)) & 0x1) ^ ((a >> ((i + 7) % 8)) & 0x1) ^ ((x >> i) & 0x1)) << i;
	tmp[0] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
	return tmp[0];
}

//S�в���
void s_box_gen(void)
{
	int i, j;
	int s_box_ary[16][16] = { 0 };

	//��ʼ��S��
	for (i = 0; i < 0x10; i++)
		for (j = 0; j < 0x10; j++)
			s_box_ary[i][j] = ((i << 4) & 0xF0) + (j & (0xF));

	//����GF(2^8)���ϵ��棬0ӳ�䵽����
	for (i = 0; i < 0x10; i++)
	{
		for (j = 0; j < 0x10; j++)
			if (s_box_ary[i][j] != 0)
				s_box_ary[i][j] = externEuc(s_box_ary[i][j], 0x11B);
	}

	//��ÿ���ֽ����任
	for (i = 0; i < 0x10; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			s_box_ary[i][j] = byteTransformation(s_box_ary[i][j], 0x63);
			S[i][j] = s_box_ary[i][j];
		}
	}
}


//��S�в���
void inv_s_box_gen(void)
{
	int i, j;
	int s_box_ary[16][16] = { 0 };
	int b = 0, bb = 0;

	//��ʼ��S��
	for (i = 0; i < 0x10; i++)
	{
		for (j = 0; j < 0x10; j++)
			s_box_ary[i][j] = ((i << 4) & 0xF0) + (j & (0xF));
	}
	//��ÿ���ֽ����任
	for (i = 0; i < 0x10; i++)
		for (j = 0; j < 0x10; j++)
			s_box_ary[i][j] = invByteTransformation(s_box_ary[i][j], 0x05);

	//����GF(2^8)���ϵ��棬0ӳ�䵽����
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


//�ֳ���
static int rC[10] = {
	0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36
};


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
			temp = temp << 1;    //�ܹ���3λ
			if (temp >= 256)
				temp = temp ^ 0x11b;   //x^8+x^4+x^3+x+1
		}
		temp = temp % 256;
		res = res ^ temp;
	}
	if (sec)
	{
		int temp = a;
		for (int i = 1; i <= 2; ++i)
		{
			temp = temp << 1;     //�ܹ���2λ
			if (temp >= 256)
				temp = temp ^ 0x11b;
		}
		temp = temp % 256;
		res = res ^ temp;
	}
	if (fir)
	{
		int temp = a;
		temp = temp << 1;         //�ܹ���1λ
		if (temp >= 256)
			temp = temp ^ 0x11b;
		temp = temp % 256;
		res = res ^ temp;
	}
	if (firstMod)
		res = res ^ a;          //������λ
	return res;
}

//��Կ��չ
void KeyExpansion(int key[4][4], int w[11][4][4])
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; j++)
			w[0][i][j] = key[j][i];

	// 10������Կ
	for (int i = 1; i < 11; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			int temp[4];
			if (j == 0)   //��4�ı���
			{
				// T����������ѭ��
				temp[0] = w[i - 1][3][1];
				temp[1] = w[i - 1][3][2];
				temp[2] = w[i - 1][3][3];
				temp[3] = w[i - 1][3][0];
				// T���������ֽڴ���������ѭ�����ʹ��S�н����ֽڴ�����
				for (int k = 0; k < 4; ++k)
				{
					int m = temp[k];
					int row = m / 16;
					int col = m % 16;
					// ��S��
					temp[k] = S[row][col];
					// T���������ֳ������
					if (k == 0)
						temp[k] = temp[k] ^ rC[i - 1];
				}
			}
			else
			{
				temp[0] = w[i][j - 1][0];
				temp[1] = w[i][j - 1][1];
				temp[2] = w[i][j - 1][2];
				temp[3] = w[i][j - 1][3];
			}
			for (int x = 0; x < 4; x++)
				// W[i-4]��Ϊ w[i - 1][j][x]
				w[i][j][x] = w[i - 1][j][x] ^ temp[x];
		}
	}
}

// �ֽڴ���
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

// ����λ
void ShiftRow(int in[4][4], int type) {
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

//�л��
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

//����Կ��
// ������Կ�����������
void AddRoundKey(int in[4][4], int key[4][4])
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; j++)
			in[i][j] = in[i][j] ^ key[j][i];
}

// �����ܺ���
void Encrypt_AES(int in[4][4], int key[4][4])
{
	int type = 1;
	int subKey[11][4][4];
	KeyExpansion(key, subKey);    // ��Կ��չ
	AddRoundKey(in, subKey[0]);     //����Կ��
	for (int i = 1; i <= 10; ++i)
	{
		ByteSub(in, type);  //�ֽڴ���
		ShiftRow(in, type);    //����λ
		if (i != 10)      //���һ�μ��㲻��Ҫ�л��
			MixColumn(in, type);   //�л��
		AddRoundKey(in, subKey[i]);   //��Կ��
	}
}

// �����ܺ���
void Decode_AES(int in[4][4], int key[4][4])
{
	int type = 0;
	int subKey[11][4][4];
	KeyExpansion(key, subKey);
	AddRoundKey(in, subKey[10]);
	// ����128λ����Ҫѭ��ʮ��
	for (int i = 9; i >= 0; --i)
	{
		ByteSub(in, type);
		ShiftRow(in, type);
		AddRoundKey(in, subKey[i]);        //subkey������෴
		if (i != 0)
			MixColumn(in, type);
	}
}