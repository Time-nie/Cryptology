#include<iostream>
#include<cstdio>
#include<string>
#include<map>

// 这个程序实现的是单表置换密码。
using namespace std;

// 用来标记那个英语字母之前是否已被添加到置换表，如果未添加，则bool值为1；添加以后bool值为0
map<char, bool> Book_Use;     
char Rep_Table[26][2];

// 构造置换表
void setTable(string str)
{
	int len = str.length();
	int mappingnum = 0;

	// 使用一个短语或句子作为置换表的前面的部分
	for (int i = 0; i <= len - 1; i++)
	{
		bool exist = false;
		// 不区分大小写
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;

		if (str[i] < 'a' || str[i] > 'z')
			continue;

		for (int j = 0; j <= mappingnum - 1; j++)
		{
			if (Rep_Table[j][1] == str[i])
				exist = true;
		}
		if (!exist)
		{
			Rep_Table[mappingnum][1] = str[i];
			mappingnum++;
			Book_Use[str[i]] = 0;
		}
	}

	// 把没有用到的字母按字母表的顺序依次放入置换表中
	map<char, bool>::iterator iter;
	for (iter = Book_Use.begin(); iter != Book_Use.end(); iter++)
	{
		if (iter->second == 1)
		{
			Rep_Table[mappingnum][1] = iter->first;
			iter->second = 0;
			mappingnum++;
		}
	}
}

// 加密函数
string Encrypt(string str)
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
			if (Rep_Table[j][0] == str[i])
			{
				flag = j;
				break;
			}
		result += Rep_Table[flag][1];
	}
	cout << "解密结果为：" << result << endl;
	return result;
}

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
	cout << "解密结果为：" << result << endl;
}


int main()
{
	for (int i = 0; i <= 25; i++)
	{
		Rep_Table[i][0] = 97 + i;
		Rep_Table[i][1] = '\0';
		Book_Use[Rep_Table[i][0]] = 1;
	}
	// 构造置换表
	string key;
	cout << "请输入密钥：";
	getline(cin, key);
	setTable(key);

	// 输出置换表
	for (int i = 0; i <= 25; i++)
		cout << Rep_Table[i][0] << " ";
	cout << endl;
	for (int i = 0; i <= 25; i++)
		cout << Rep_Table[i][1] << " ";
	cout << endl;


	// 输入明文字符串
	string text, Encode;
	cout << "请输入明文字符串：" << endl;
	getline(cin, text);
	cout << "========================================" << endl << endl;
	Encode = Encrypt(text);
	Decryption(Encode);

	system("pause");
}