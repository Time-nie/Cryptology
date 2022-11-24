// 移位密码加解密（自定义移位位数）
#include<iostream>
#include<string>
using namespace std;

// 加密函数
string Encrypt(string str, int offset)
{
	int length = str.length();
	string result;

	for (int i = 0; i <= length - 1; i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			int shift = (str[i] - 'A' + offset) % 26 + 'A';
			result = result + (char)shift;
		}
		if (str[i] == ' ')
		{
			result += ' ';
			continue;
		}
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			int shift = (str[i] - 'a' + offset) % 26 + 'a';
			result = result + (char)shift;
		}
	}
	cout << "加密结果为：" << result << endl;
	return result;
}

//解密函数
void Decryption(string str, int offset)
{
	int length = str.length();

	string result;

	for (int i = 0; i <= length - 1; i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			int shift = (str[i] - 'A' + 26 - offset) % 26 + 'A';
			result = result + (char)shift;
		}
		if (str[i] == ' ')
		{
			result += ' ';
			continue;
		}

		if (str[i] >= 'a' && str[i] <= 'z')
		{
			int shift = (str[i] - 'a' + 26 - offset) % 26 + 'a';
			result = result + (char)shift;
		}
	}
	cout << "解密结果为：" << result << endl;
}

int main()
{
	string text, encode;
	int length = 0;

	//明文输入
	cout << "请输入明文：" << endl;
	getline(cin, text);
	cout << "请输入移位密码移位位数 " << endl;
	cin >> length;

	// 加密
	encode = Encrypt(text, length);

	// 解密
	Decryption(encode, length);
	system("pause");

}
