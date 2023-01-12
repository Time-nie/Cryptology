#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include <time.h>
#include<ctime>
#include<fstream>
#include<bitset>
//#include"largeInt.h"
#include"AES.cpp"
#include"RSA.cpp"

# define IP "127.0.0.1"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);//声明使用socket2.2版本

	//创建套接字，IP地址类型为AF_INET（IPV-4 32位），服务类型为流式(SOCK_STREAM)，Protocol（协议）为0代表系统自动选则
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == INVALID_SOCKET)
	{
		cout << "套接字创建失败，请通过:" << WSAGetLastError() << "获取错误详情" << endl;
		return -1;
	}

	char clientName[32] = "User_B";
	//cout << "请输入你的昵称: " << endl;
	//cin >> clientName;

	//设定所连接服务器地址
	SOCKADDR_IN ServerAddr;
	USHORT uPort = 2023;
	ServerAddr.sin_family = AF_INET;  //指定IP地址类型（IPV-4 32位）
	ServerAddr.sin_port = htons(uPort);  //htons将主机序转网络序
	ServerAddr.sin_addr.S_un.S_addr = inet_addr(IP);


	cout << "开始连接......" << endl;
	//把自己的socket与远端连接：
	if (SOCKET_ERROR == connect(ClientSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)))
	{
		cout << "连接失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	cout << "已经成功与对方（User_A）建立连接，如果您想断开连接请输入 exit " << endl;
	cout << "User_A信息：" << endl;
	cout << "IP ： " << inet_ntoa(ServerAddr.sin_addr) << "      " << "端口号 : " << htons(ServerAddr.sin_port) << endl << endl;

	//下面是对收发消息的存储和显示
	char buffer[4096] = { 0 };
	char serverName[32] = { 0 };
	int RecvLen = 0;    //实际收到的字节数
	int SendLen = 0;    //实际发送的字节数

	//把自己的名字发过去
	if (SOCKET_ERROR == send(ClientSocket, clientName, strlen(clientName), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	//接收对方的名字，存进serverName
	//双方无论谁先发送结束字符，在等到对方回复一条消息后，双方都会结束。
	if (SOCKET_ERROR == recv(ClientSocket, serverName, sizeof(serverName), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	cout << endl << "===========================RSA加密会话密钥===========================" << endl << endl;
	cout << "现在开始生成您本人的RSA公钥和私钥" << endl;
	cout << "请选择：1 - 随机生成公钥和私钥   2 - 自行输入公钥和私钥 ：";
	int choose;
	bool pass = false;
	largeInt n;
	// 公钥e
	largeInt e;
	//d为秘密钥，即e模t的乘法逆元
	largeInt d;
	//y用于参与扩展欧几里得运算，存储t模e的乘法逆元
	largeInt y;
	do {
		cin >> choose;
		if (choose == 1)
		{
			cout << "=======================生成第一个素数p=======================" << endl;
			// 根据时间设置一个随机数
			srand((unsigned)time(NULL));
			largeInt P = GeneratePrime();
			cout << endl;
			cout << "=======================素数p生成成功=======================" << endl;
			cout << "生成的素数p为：" << endl;
			cout << P << endl;
			cout << "=======================生成第二个素数q=======================" << endl;
			largeInt Q = GeneratePrime();
			cout << endl;
			cout << "=======================素数q生成成功=======================" << endl;
			cout << "生成的素数q为：" << endl;
			cout << Q << endl;

			// 计算n
			n = P * Q;
			// 计算φ（n）
			largeInt fn = (P - 1) * (Q - 1);
			// 保存标志位，判断算法是否成功
			largeInt temp;
			// 计算产生互质的数
			while (1)
			{
				//产生与fn互质的e
				e.Random();
				while (!(Gcd(e, fn) == 1))
					e.Random();

				//用扩展欧几里德算法试图求出e模t的乘法逆元
				temp = ExtendedGcd(e, fn, d, y);

				//e*d模t结果为1，说明d确实是e模t的乘法逆元
				temp = (e * d) % fn;
				if (temp == 1)
					break;
				//否则重新生成e
			}
			cout << "================生成n===============" << endl;
			cout << n << endl;
			cout << "==============生成公钥e=============" << endl;
			cout << e << endl;
			cout << "==============生成私钥d==============" << endl;
			cout << d << endl;
			ofstream outfile("RSA.txt");
			for (int i = 0; i < 32; i++)
				outfile << n.data[i] << " ";
			outfile << '\n';
			for (int i = 0; i < 16; i++)
				outfile << e.data[i] << " ";
			outfile << '\n';
			for (int i = 0; i < 32; i++)
				outfile << d.data[i] << " ";
			outfile << '\n';
			outfile.close();
			pass = true;
		}
		else if (choose == 2)
		{
			ifstream infile("RSA.txt");
			int i = 0;
			char line[1000];
			unsigned int num[100];
			while (infile.getline(line, 1000, ' '))
			{
				cout << line << endl;
				num[i] = stoi(line);
				i++;
			}
			int k;
			cout << "i：" << i << endl;
			for (k = 0; k < 32; k++)
				n.data[k] = num[k];
			for (k = 0; k < 16; k++)
				e.data[k] = num[32 + k];
			for (k = 0; k < 32; k++)
				d.data[k] = num[32 + 16 + k];
			infile.close();

			cout << "================生成n===============" << endl;
			cout << n << endl;
			cout << "==============生成公钥e=============" << endl;
			cout << e << endl;
			cout << "==============生成私钥d==============" << endl;
			cout << d << endl;
			pass = true;
		}
		else
		{
			cout << "您输入不合规，请您重新输入：";
			pass = false;
		}
	} while (!pass);


	cout << endl << "==========================RSA公钥传输========================" << endl << endl;
	// 接收公钥e_A
	char e_a[512] = { 0 };
	if (SOCKET_ERROR == recv(ClientSocket, e_a, sizeof(e_a), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}
	largeInt e_A;
	to_arr(e_A.data, e_a);

	// 发送公钥e_B
	char e_str[512];
	e.to_str(e_str);
	if (SOCKET_ERROR == send(ClientSocket, e_str, strlen(e_str), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	//接收公钥n_A
	char n_a[512] = { 0 };
	if (SOCKET_ERROR == recv(ClientSocket, n_a, sizeof(n_a), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}
	largeInt n_A;
	to_arr(n_A.data, n_a);

	// 发送公n_B
	char n_str[512];
	n.to_str(n_str);
	if (SOCKET_ERROR == send(ClientSocket, n_str, strlen(n_str), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	cout << "=========已收到对方RSA公钥===========" << endl;
	cout << "对方公钥e：" << endl;
	cout << e_A << endl;
	cout << "对方公钥n：" << endl;
	cout << n_A << endl;

	cout << endl << "======================身份认证+AES会话密钥接收【保密性+认证性】=====================" << endl << endl;
	char IDa[6] = { 0 };
	char N1[6] = { 0 };
	if (SOCKET_ERROR == recv(ClientSocket, IDa, sizeof(IDa), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}
	if (SOCKET_ERROR == recv(ClientSocket, N1, sizeof(N1), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}
	
	cout << "接收到【解密后】对方ID为：" << IDa << "     一次性随机数为：" << N1 << endl;

	int	N2 = rand() % 1000;
	char N2_str[6];
	snprintf(N2_str, sizeof(N2_str), "%d", N2);
	cout << "自动生成一次性随机数IDa：" << N2_str << endl;
	cout << "发送A公钥加密的一次性随机数N1和N2" << endl;
	if (SOCKET_ERROR == send(ClientSocket, N1, strlen(N1), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}
	if (SOCKET_ERROR == send(ClientSocket, N2_str, strlen(N2_str), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}
	char N2_recv[6] = { 0 };
	if (SOCKET_ERROR == recv(ClientSocket, N2_recv, sizeof(N2_recv), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}
	if (strcmp(N2_str, N2_recv) == 0)
		cout << "A传输回的一次性随机数N2经解密与初始生成的N2相同，A身份验证通过" << endl << endl;

	// 接收加密的AES密钥
	char key_str[512] = { 0 };
	//接收对方发过来的信息
	if (SOCKET_ERROR == recv(ClientSocket, key_str, sizeof(key_str), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	cout << "======收到对方发来的加密AES会话密钥=====" << endl;
	largeInt key_largeInt;
	to_arr(key_largeInt.data, key_str);
	cout << key_largeInt << endl;

	cout << "============解密AES会话密钥============" << endl;
	largeInt key_decode;
	//key_decode = Decode(key_largeInt, d, n);
	//key_decode = Decode(key_decode, e_A, n_A);
	key_decode = Decode(key_largeInt, e_A, n_A);
	cout << key_decode << endl;
	int key[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 3; j >= 0; j--)
		{
			key[j][i] = key_decode.data[i] % (16 * 16);
			key_decode.data[i] = key_decode.data[i] / (16 * 16);
		}
	cout << "====已完成密钥分配【保密性+认证性】====" << endl << endl;


	cout << "============================ AES 解密文件 ========================================" << endl << endl;
	cout << "生成S盒：";
	s_box_gen();   //生成S盒
	cout << "\r\n    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F";
	for (int i = 0; i < 0x10; i++)
	{
		printf("\r\n%2x", i);
		for (int j = 0; j < 0x10; j++)
			printf(" %2x", S[i][j]);
	}
	cout << endl << endl;

	inv_s_box_gen();  //生成逆S盒
	string res;
	char ch[2] = { 0 };
	
	unsigned char m_iv[16];
	unsigned char iv[] = { 103,35,148,239,76,213,47,118,255,222,123,176,106,134,98,92 };
	int cipher[4][4];
	unsigned char input[20] = { 0 };
	unsigned char xor_input[20] = { 0 };
	char plaintext[20] = { 0 };
	int iv_temp[4][4];
	bool cont = true;
	while (cont)
	{
		if (SOCKET_ERROR == recv(ClientSocket, ch, sizeof(ch), 0))
		{
			cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
			closesocket(ClientSocket);
			WSACleanup();
			return -1;
		}
		if (strcmp(ch, "1") == 0)
			cout << "发送端选择的是CBC模式" << endl;
		else if (strcmp(ch, "2") == 0)
			cout << "发送端选择的是CFB模式" << endl;

		memcpy(m_iv, iv, 16);
		bool first_round = true;
		int total = 1;
		while (cont)
		{
			//接收密文
			char ciphertext_str[512] = { 0 };
			if (SOCKET_ERROR == recv(ClientSocket, ciphertext_str, sizeof(ciphertext_str), 0))
			{
				cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
				closesocket(ClientSocket);
				WSACleanup();
				return -1;
			}
			// 判断退出
			if (strcmp(ciphertext_str, "exit") == 0)
			{
				cont = false;
				break;
			}

			if (strcmp(ciphertext_str, "finish") == 0)
				break;

			largeInt ciphertext_largeInt;
			to_arr(ciphertext_largeInt.data, ciphertext_str);
			int ciphertext[4][4];
			for (int i = 0; i < 4; i++)
				for (int j = 3; j >= 0; j--)
				{
					ciphertext[j][i] = ciphertext_largeInt.data[i] % (16 * 16);
					ciphertext_largeInt.data[i] = ciphertext_largeInt.data[i] / (16 * 16);
				}

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					input[4 * i + j] = ciphertext[j][i];
			input[16] = '\0';
			cout << "分组" << dec << total++ << endl;
			cout << "【接收到加密后密文】：";
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (ciphertext[j][i] < 16)
						cout << "0";
					cout << (hex) << ciphertext[j][i];
					if ((j + 1) % 2 == 0)
						cout << " ";
				}
			}
			cout << "     【AES算法解密后明文】：";
			if (strcmp(ch, "1") == 0) {
				// AES解密
				Decode_AES(ciphertext, key);
				memset(buffer, 0, sizeof(buffer));
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						buffer[i * 4 + j] = ciphertext[j][i];
				buffer[16] = '\0';

				
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						if (ciphertext[j][i] < 16)
							cout << "0";
						cout << (hex) << ciphertext[j][i];
						if ((j + 1) % 2 == 0)
							cout << " ";
					}
				}
				// 异或运算
				for (int i = 0; i < 16; i++) {
					if (first_round == true) {
						plaintext[i] = m_iv[i] ^ buffer[i];
					}
					else {
						plaintext[i] = xor_input[i] ^ buffer[i];
					}
				}
				plaintext[16] = '\0';
				first_round = false;
				// 保存加密结果作为下一轮IV向量异或
				memcpy(xor_input, input, 16);
				xor_input[16] = '\0';
			}
			else if (strcmp(ch, "2") == 0)
			{
				if (first_round == false) {
					Encrypt_AES(iv_temp, key);
				}
				else {
					
					first_round = false;
					for (int i = 0; i < 4; i++)
						for (int j = 0; j < 4; j++)
							iv_temp[j][i] = m_iv[i * 4 + j];
					Encrypt_AES(iv_temp, key);
				}
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						if (iv_temp[j][i] < 16)
							cout << "0";
						cout << (hex) << iv_temp[j][i];
						if ((j + 1) % 2 == 0)
							cout << " ";
					}
				}
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						xor_input[i * 4 + j] = iv_temp[j][i];

				for (int i = 0; i < 16; i++) {
					if (i == 18) {
						plaintext[i] = input[i] ^ 0;
					}
					else {
						plaintext[i] = input[i] ^ xor_input[i];
					}
				}
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						iv_temp[i][j] = ciphertext[i][j];
				
			}
			cout << endl;
			res += string((const char*)plaintext, strlen(plaintext));
		}
		if (cont)
		{
			cout << endl << "请输入接收文件保存路径：";
			string TEST_FILE_REV = "recv_1.txt";
			cin >> TEST_FILE_REV;
			ofstream file2(TEST_FILE_REV, ios::binary);
			file2.write(res.data(), res.length());
			cout << "该文件保存成功" << endl << endl;
		}
	}

	cout << endl << "对方已结束" << endl << endl;
	cout << endl << "=============================文件加密传输结束=================================" << endl << endl;
	closesocket(ClientSocket);
	WSACleanup();
	system("pause");
}
