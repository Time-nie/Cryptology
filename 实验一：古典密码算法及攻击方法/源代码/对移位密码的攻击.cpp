// 对移位密码攻击
// 遍历移位数从并输出解密结果，人工识别哪个单词可读

#include<iostream>
#include<string>
using namespace std;

void Decryption(string str)       //因为不区分大小写，所以统一按照小写字母来处理的;
{
    int length = str.length();
    string result;

    for (int j = 1; j <= 25; j++)
    {
        result = { 0 };
        for (int i = 0; i <= length - 1; i++)
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] = str[i] + 32;
            if (str[i] >= 'a' && str[i] <= 'z')
            {
                int shift = (str[i] - 97 + 26 - j) % 26 + 97;
                result += (char)shift;
            }
            else
                result += " ";
        }
        cout << "移位位数为    : " << j << endl;
		cout << "攻击解密结果为: " << result << endl;
    }
}

int main()
{
	string ciphertext;
	int length = 0;

	//密文输入
	cout << "请输入密文：" << endl;
	getline(cin, ciphertext);
	cout << "================================================" << endl << endl;

	// 对移位密码进行攻击
	Decryption(ciphertext);

	system("pause");
}