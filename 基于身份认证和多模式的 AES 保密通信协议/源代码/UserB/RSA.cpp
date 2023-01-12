#include"largeInt.h"

// 产生一个待测素数,保证此数为奇数,且不能被小于2000的素数整除
void Pre_Prime(largeInt& n)
{
	int i = 0;
	largeInt divisor;
	// 2000以内素数
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

// RabinMiller检测某个数是否是素数
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

	//随机产生一个小于N-1的检测数a
	a.RandomSmall();

	//y = a的r次幂模n
	y = Power(a, r, n);

	//检测J=2至J<S轮
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

//产生一个素数
largeInt GeneratePrime()
{
	largeInt n;
	int i = 0;
	// 记录总共产生了多少次素数
	int time = 0;

	//无限次循环，不断产生素数，直到i==5时（通过五轮RabinMiller测试）才会跳出while循环
	while (i < 5)
	{
		//记录生成了多少次大奇数
		time++;

		//产生一个待测素数
		cout << endl;
		cout << "正在生成第" << time << "个大奇数：" << endl;
		Pre_Prime(n);
		cout << n;

		i = 0;
		//进行五轮RABINMILLER测试,五轮全部通过则素数合格
		for (; i < 5; i++)
		{
			cout << "正在进行第" << i + 1 << "轮Miller-Rabin测试：";
			if (!RabinMiller(n))
			{
				cout << "======== 失败" << endl;
				break;
			}
			cout << "======== 通过" << endl;
		}
	}
	return n;
}

// 辗转相除法求两个大数的最大公约数
largeInt Gcd(const largeInt& m, const largeInt& n)
{
	if (n == 0)
		return m;
	else
		return Gcd(n, m % n);
}

// 扩展欧几里德求乘法逆元
largeInt ExtendedGcd(const largeInt& a, const largeInt& b, largeInt& x, largeInt& y)
{
	largeInt t, d;
	//如果一个操作数为零则无法进行除法运算
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

// RSA加密
largeInt Encrypt(largeInt& m, largeInt& e, largeInt& n) {
	// 加密后的结果
	largeInt c;
	c = Power(m, e, n);
	return c;
}

// RSA解密
largeInt Decode(largeInt& c, largeInt& d, largeInt& n) {
	// 解密后的结果
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

