#include<iostream>
#include<string.h>
#include<bitset>
#include "Table.h"
using namespace std;

// 十六进制转二进制
bitset<64> Hex_Bin(const int num_dec[8])
{
	bitset<64> num_bin;
	for (int i = 0; i < 8; i++)
	{
		//高位和低位
		int high = num_dec[i] / 16;
		int low = num_dec[i] % 16;
		for (int j = 0; j < 4; j++)
		{
			num_bin[63 - (8 * i + 7 - j)] = low % 2;
			num_bin[63 - (8 * i + 3 - j)] = high % 2;
			low /= 2;
			high /= 2;
		}
	}
	return num_bin;
}


// 二进制转十六进制
void Bin_Hex(bitset<64> Bin, char * Hex)
{
	int k = 0, j = 0;

	for (int i = 0; i < 16; i++)
	{
		int val = 0;
		for (int k = 4 * i; k <= 4 * i + 3; k++)
		{
			val *= 2;
			val += Bin[63 - k];
		}
		if (val < 10)
			Hex[i] = val + '0';
		else if (val >= 10 && val <= 15)
			Hex[i] = val - 10 + 'A';
	}
	Hex[16] = '\0';
}


//密钥循环左移位
bitset<28> leftShift(bitset<28> k, int shift)
{
	bitset<28> tmp = k;
	for (int i = 27; i >= 0; --i)
	{
		if (i - shift < 0)
			k[i] = tmp[i - shift + 28];
		else
			k[i] = tmp[i - shift];
	}
	return k;
}

// 生成16个48位的子密钥
void Gen_Subkeys(bitset<64> key, bitset<48>* subKey)
{
	bitset<28> left_key;    //子密钥左半部分
	bitset<28> right_key;   //子密钥右半部分
	bitset<56> new_key;  //保存更新的密钥

	// 密钥完成置换选则1：64位 -> 56位（去掉奇偶标记位）
	for (int i = 0; i < 56; ++i)
		new_key[55 - i] = key[64 - PC_1[i]];

	// 通过16轮迭代，得到每一次迭代的48位用作和明文F函数加密的子密钥
	for (int cycle = 0; cycle < 16; cycle++)
	{
		//把密钥分为左右两半
		for (int i = 0; i < 28; ++i)
			right_key[i] = new_key[i];
		for (int i = 28; i < 56; ++i)
			left_key[i - 28] = new_key[i];

		// 左右分别根据做循环移位表进行左移
		left_key = leftShift(left_key, Left_Shift[cycle]);
		right_key = leftShift(right_key, Left_Shift[cycle]);

		// 密钥完成置换选则2：56位->48位
		for (int i = 28; i < 56; ++i)
			new_key[i] = left_key[i - 28];
		for (int i = 0; i < 28; ++i)
			new_key[i] = right_key[i];
		for (int i = 0; i < 48; ++i)
			subKey[cycle][47 - i] = new_key[56 - PC_2[i]];
	}
}


// 函数F(R,K)计算过程
bitset<32> F(bitset<32> R, bitset<48> K)
{
	bitset<48> Right_Expand;  //保存选则扩展运算E后的48位
	bitset<32> res;      //保存P置换之后结果

	// 选则扩展运算E：32位 -> 48位
	for (int i = 0; i < 48; ++i)
		Right_Expand[47 - i] = R[32 - Extend_Table[i]];

	// 与子密钥进行异或运算
	Right_Expand = Right_Expand ^ K;

	// S盒代换：48位 -> 32位
	int index = 31;
	for (int i = 0; i < 48; i = i + 6)   //S1~S8 一组六个值
	{
		int row = Right_Expand[47 - i] * 2 + Right_Expand[47 - i - 5];
		int col = 0;
		for (int j = 1; j <= 4; j++)
		{
			col += Right_Expand[47 - i - j];
			if (j != 4)
				col *= 2;
		}
		int num = S_Box[i / 6][row][col];
		bitset<4> binary(num);
		for (int j = 0; j < 4; j++)
			res[index--] = binary[3 - j];
	}

	// P置换：32位 -> 32位
	bitset<32> tmp = res;
	for (int i = 0; i < 32; ++i)
		res[31 - i] = tmp[32 - P_Tabe[i]];

	return res;
}

//加密总函数
bitset<64> encrypt(bitset<64>& initial, bitset<48>* subKey)
{
	bitset<64> result;         //保存最终结果   
	bitset<64> combine;    //保存初始置换后的64位输入/左右合并的加密结果
	bitset<32> left;           //左边的32位
	bitset<32> right;          //右边的32位
	bitset<32> temp;        //对右明文进行备份

	// 初始置换IP过程
	for (int i = 0; i < 64; ++i)
		combine[63 - i] = initial[64 - IP_Table[i]];

	// 分为左右两部分
	for (int i = 32; i < 64; ++i)
		left[i - 32] = combine[i];
	for (int i = 0; i < 32; ++i)
		right[i] = combine[i];

	// 进行迭代（共16轮）
	for (int cycle = 0; cycle < 16; cycle++)
	{
		temp = right;
		right = left ^ F(right, subKey[cycle]);
		left = temp;
	}

	// 左右交换
	for (int i = 0; i < 32; ++i)
		combine[i] = left[i];        //把left的放在高位
	for (int i = 32; i < 64; ++i)
		combine[i] = right[i - 32];  //把right的放在低位

	// 逆初始置换
	for (int i = 0; i < 64; ++i)
		result[63 - i] = combine[64 - Rev_IP[i]];
	return result;
}


// 解密总函数
bitset<64> decrypt(bitset<64>& cipher, bitset<48>* subKey)
{
	bitset<64> result;    //保存最终结果   
	bitset<64> combine;   //保存初始置换后的64位输入/左右合并的解密结果
	bitset<32> left;	  //左边的32位
	bitset<32> right;     //右边的32位
	bitset<32> temp;     //对右明文进行备份
	// 初始置换IP
	for (int i = 0; i < 64; ++i)
		combine[63 - i] = cipher[64 - IP_Table[i]];

	// 拆分为左右两部分
	for (int i = 0; i < 32; ++i)
		right[i] = combine[i];
	for (int i = 32; i < 64; ++i)
		left[i - 32] = combine[i];

	// 进行迭代（共16轮），这里的子密钥和加密时使用的顺序相反
	for (int cycle = 0; cycle < 16; cycle++) {
		temp = right;
		right = left ^ F(right, subKey[15 - cycle]);
		left = temp;
	}

	// 左右交换
	for (int i = 0; i < 32; ++i)
		combine[i] = left[i];
	for (int i = 32; i < 64; ++i)
		combine[i] = right[i - 32];

	// 逆初始置换
	for (int i = 0; i < 64; ++i)
		result[63 - i] = combine[64 - Rev_IP[i]];
	// 返回明文
	return result;
}


int main()
{
	bitset<64> bin_key;                // 保存64位二进制密钥
	bitset<48> subKey[16];         // 存放16轮子密钥
	bitset<64> bin_text, result_text;
	int type;
	int hex_text[8], hex_key[8];       // 保存十六进制明文和密钥
	cout << "========== 1——加密  2——解密 =========" << endl;
	char str[16];
	while (true)
	{
		cout << "请输入：";
		cin >> type;
		switch (type)
		{
		// 加密过程
		case 1:
			cout << "=================加密===================" << endl;
			// 输入十六进制密钥并保存为64位二进制
			cout << "请输入十六进制密钥：" << endl;
			for (int i = 0; i <= 7; i++)
				cin >> (hex) >> hex_key[i];
			bin_key = Hex_Bin(hex_key);

			// 输入十六进制明文并保存为64位二进制
			cout << "请输入十六进制明文：" << endl;
			for (int i = 0; i <= 7; i++)
				cin >> (hex) >> hex_text[i];
			bin_text = Hex_Bin(hex_text);
			cout << "========================================" << endl;

			cout << "二进制初始密钥为：" << endl;
			cout << bin_key << endl;
			cout << "二进制明文为：" << endl;
			cout << bin_text << endl;

			// 得到十六轮密钥
			Gen_Subkeys(bin_key, subKey);

			// 加密过程
			result_text = encrypt(bin_text, subKey);

			cout << "二进制密文为：" << endl;
			for (int i = 0; i <= 63; i++)
				cout << result_text[63 - i];
			cout << endl;
			cout << "十六进制密文为：" << endl;
			Bin_Hex(result_text, str);
			for (int i = 0; i < 16; i++){
				if (i % 2 == 0)
					cout << " 0x";
				cout << str[i];
			}
			cout << endl;
			cout << "========================================" << endl;
			cout << endl << "========== 1——加密  2——解密 =========" << endl;
			continue;

		// 解密过程
		case 2:
			cout << "===================解密===================" << endl;
			cout << "请输入密钥（hex）：" << endl;
			for (int i = 0; i <= 7; i++)
				cin >> (hex) >> hex_key[i];
			bin_key = Hex_Bin(hex_key);
			cout << "请输入密文（hex）：" << endl;
			for (int i = 0; i <= 7; i++)
				cin >> (hex) >> hex_text[i];
			result_text = Hex_Bin(hex_text);
			cout << "========================================" << endl;

			cout << "二进制初始密钥为：" << endl;
			cout << bin_key << endl;
			cout << "二进制密文为：" << endl;
			cout << result_text << endl;

			// 得到十六轮密钥
			Gen_Subkeys(bin_key, subKey);

			// 解密过程
			result_text = decrypt(result_text, subKey);
			cout << "下面输出64位明文：" << endl;
			for (int i = 0; i <= 63; i++)
				cout << result_text[63 - i];
			cout << endl;
			cout << "明文的十六进制形式为：" << endl;
			Bin_Hex(result_text,str);
			for (int i = 0; i < 16; i++){
				if (i % 2 == 0)
					cout << " 0x";
				cout << str[i];
			}
			cout << endl;
			cout << "========================================" << endl;
			cout << endl << "========== 1——加密  2——解密 =========" << endl;
			continue;
		}
	}
	system("pause");
}