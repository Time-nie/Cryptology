#include"largeInt.h"

// ����һ����������,��֤����Ϊ����,�Ҳ��ܱ�С��2000����������
void Pre_Prime(largeInt& n)
{
	int i = 0;
	largeInt divisor;
	// 2000��������
	const int length = sizeof(prime) / sizeof(int);

	n.Random();
	while (!n.IsOdd())
		n.Random();

	while (i != length)
	{
		largeInt TWO(2);
		n = n + TWO;
		i = 0;
		for (; i < length; i++)
			if ((n % prime[i]) == 0)
				break;
	}
}

// RabinMiller���ĳ�����Ƿ�������
bool RabinMiller(const largeInt& n)
{
	largeInt r, a, y;
	unsigned int s, j;
	r = n - 1;
	s = 0;

	while (!r.IsOdd())
	{
		s++;
		r >> 1;
	}

	//�������һ��С��N-1�ļ����a
	a.RandomSmall();

	//y = a��r����ģn
	y = Power(a, r, n);

	//���J=2��J<S��
	if ((!(y == 1)) && (!(y == (n - 1))))
	{
		j = 1;
		while ((j <= s - 1) && (!(y == (n - 1))))
		{
			y = (y * y) % n;
			if (y == 1)
				return false;
			j++;
		}
		if (!(y == (n - 1)))
			return false;
	}
	return true;
}

//����һ������
largeInt GeneratePrime()
{
	largeInt n;
	int i = 0;
	// ��¼�ܹ������˶��ٴ�����
	int time = 0;

	//���޴�ѭ�������ϲ���������ֱ��i==5ʱ��ͨ������RabinMiller���ԣ��Ż�����whileѭ��
	while (i < 5)
	{
		//��¼�����˶��ٴδ�����
		time++;

		//����һ����������
		cout << endl;
		cout << "�������ɵ�" << time << "����������" << endl;
		Pre_Prime(n);
		cout << n;

		i = 0;
		//��������RABINMILLER����,����ȫ��ͨ���������ϸ�
		for (; i < 5; i++)
		{
			cout << "���ڽ��е�" << i + 1 << "��Miller-Rabin���ԣ�";
			if (!RabinMiller(n))
			{
				cout << "======== ʧ��" << endl;
				break;
			}
			cout << "======== ͨ��" << endl;
		}
	}
	return n;
}

// շת��������������������Լ��
largeInt Gcd(const largeInt& m, const largeInt& n)
{
	if (n == 0)
		return m;
	else
		return Gcd(n, m % n);
}

// ��չŷ�������˷���Ԫ
largeInt ExtendedGcd(const largeInt& a, const largeInt& b, largeInt& x, largeInt& y)
{
	largeInt t, d;
	//���һ��������Ϊ�����޷����г�������
	if (b == 0)
	{
		x = 1;
		y = 0;
		return a;
	}
	d = ExtendedGcd(b, a % b, x, y);
	t = x;
	x = y;
	y = t - ((a / b) * y);
	return d;
}

// RSA����
largeInt Encrypt(largeInt& m, largeInt& e, largeInt& n) {
	// ���ܺ�Ľ��
	largeInt c;
	c = Power(m, e, n);
	return c;
}

// RSA����
largeInt Decode(largeInt& c, largeInt& d, largeInt& n) {
	// ���ܺ�Ľ��
	largeInt m;
	m = Power(c, d, n);
	return m;
}
void to_arr(unsigned int a[], char s[])
{
	int k = 0;
	for (int i = strlen(s) / 8 - 1; i >= 0; i--)
	{
		unsigned int temp = 0;
		for (int j = 0; j < 8; j++)
		{
			if (s[i * 8 + j] - '0' >= 0 && s[i * 8 + j] - '0' <= 9)
				temp += s[i * 8 + j] - '0';
			else
			{
				switch (s[i * 8 + j])
				{
				case 'A':
					temp += 10;
					break;
				case 'B':
					temp += 11;
					break;
				case 'C':
					temp += 12;
					break;
				case 'D':
					temp += 13;
					break;
				case 'E':
					temp += 14;
					break;
				case 'F':
					temp += 15;
					break;
				}
			}
			if (j < 7)
				temp *= 16;
		}
		a[k++] = temp;
	}
}

