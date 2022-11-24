// 对单表置换攻击
#include<iostream>
#include<string>
using namespace std;

char Rep_Table[26][2];    //置换表
int frequency[26];  //统计频次

// 解密函数
void Decryption(string str)
{
	int length = str.length();
	string result;
	int flag = 0;
	for (int i = 0; i <= length - 1; i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
		if (str[i] == ' ')
		{
			result += ' ';
			continue;
		}
		flag = 0;
		if (str[i] < 'a' || str[i] > 'z')
			continue;

		for (int j = 0; j <= 25; j++)
			if (Rep_Table[j][1] == str[i])
			{
				flag = j;
				break;
			}
		result += Rep_Table[flag][0];
	}
	cout << "解密结果为：" << endl  << result << endl;
}


void Attack(string text) {
	// 遍历密文，统计频次
	int index;
	for(index = 0;text[index] != '\0';index += 1)
	{
		if (text[index] <= 'Z' && text[index] >= 'A') {
			// 如果是大写字母
			int i = text[index] - 'A';
			// 获取到的代换后的字母是小写字母，需要利用ASCII的关系转换成大写字母
			frequency[i] += 1;
		}
		else 
			if (text[index] <= 'z' && text[index] >= 'a')
			{
				// 如果是小写字母
				int i = text[index] - 'a';
				frequency[i] += 1;
			}
	}

	// 输出统计的频次
	cout << "各个字母频率为：";
	for (int i = 0; i < 26; i++) {
		if (i % 13 != 0)
			cout << char(97 + i) << ": " << frequency[i] << '\t';
		else
			cout << endl << char(97 + i) << ": " << frequency[i] << '\t';
	}
	cout << endl << endl;

	// 根据多轮更正，最终根据分析结果得到正确置换表
	char replaceTable[26] = { 'n','h','g','d','c','f','e','i','j','0','0','a','q','b','m','x','0','p','r','s','z','t','v','0','y','o' };

	// 构造置换表
	for (int i = 0; i <= 25; i++)
	{
		Rep_Table[i][0] = 97 + i;
		Rep_Table[i][1] = replaceTable[i];
	}

	//根据置换表进行解密
	Decryption(text);

}

int main()
{
	// 输入要统计的密文
	cout << "Please input encrypted message:\n";
	string ciphertext;
	getline(cin, ciphertext);
	cout << "==================================================" << endl << endl;
	Attack(ciphertext);

	system("pause");
}