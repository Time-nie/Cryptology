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
#pragma comment(lib,"ws2_32.lib")
using namespace std;

int main()
{
	cout << "======================开始建立通信连接===========================" << endl << endl;
	WSADATA wsaData;
	SOCKET ServerSocket;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//ipv4的地址类型；流结构的服务类型；Protocol（协议）为0代表系统自动选则
	ServerSocket = socket(AF_INET, SOCK_STREAM, 0);  

	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "套接字创建失败，请通过:" << WSAGetLastError() << "获取错误详情" << endl;
		return -1;
	}
	SOCKADDR_IN ServerAddr;
	USHORT uPort = 2023;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(uPort);
	//宏INADDR_ANY转换过来就是0.0.0.0，泛指本机的意思，也就是表示本机的所有IP
	ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)))
	{
		cout << "bind 失败，请通过" << WSAGetLastError() << "获取错误详情" << endl;
		closesocket(ServerSocket);
		return -1;
	}
	char serverName[10] = "User_A";
	cout << "请等待对方连接..." << endl;

	// 绑定完成后开始listen
	// 使socket进入监听状态，监听接收端
	if (listen(ServerSocket, 1) != 0)
	{
		cout << "监听失败，请通过" << WSAGetLastError() << "获取错误详情" << endl;
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}


	SOCKET Conn_new_Socket;
	SOCKADDR_IN Conn_new_Addr;
	int iClientAddrLen = sizeof(Conn_new_Addr);
	// accept()阻塞进程直到有客户端连接，接受一个特定socket请求等待队列中的连接请求
	Conn_new_Socket = accept(ServerSocket, (SOCKADDR*)&Conn_new_Addr, &iClientAddrLen);

	if (Conn_new_Socket == INVALID_SOCKET)
	{
		cout << "accept接受请求失败，请通过" << WSAGetLastError() << "获取错误详情" << endl;
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	//界面提示，显示连接者的信息
	cout << "已经与成功对方(User_B)建立连接，如果您想结束对话请输入：exit" << endl;
	cout << "User_B信息:" << endl;
	cout << "IP: " << inet_ntoa(Conn_new_Addr.sin_addr) << "    " << " 端口号: " << htons(Conn_new_Addr.sin_port) << endl << endl;


	char clientName[32] = { 0 }; // clientName保存客户端用户名
	char buffer[4096] = { 0 };
	int RecvLen = 0;  //实际收到的字节数
	int SendLen = 0;  //实际发送的字节数

	// 把serverName发给对方
	if (SOCKET_ERROR == send(Conn_new_Socket, serverName, strlen(serverName), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	//接收对方发过来的信息
	if (SOCKET_ERROR == recv(Conn_new_Socket, clientName, sizeof(clientName), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}
	
	cout << endl << "===========================RSA公钥私钥生成===========================" << endl << endl;
	cout << "现在开始生成您本人的RSA公钥和私钥" << endl;
	cout << "请选择：1 - 随机生成公钥和私钥   2 - 自行输入公钥和私钥 ：";

	bool pass = false;
	largeInt n;
	// 公钥e
	largeInt e;
	//d为秘密钥，即e模t的乘法逆元
	largeInt d;
	//y用于参与扩展欧几里得运算，存储t模e的乘法逆元
	largeInt y;
	int choose;
	do {
		cin >> choose;
		if (choose == 1)
		{
			cout << "=============生成第一个素数p=============" << endl;
			// 根据时间设置一个随机数
			srand((unsigned)time(NULL));
			largeInt P = GeneratePrime();
			cout << endl;
			cout << "==============素数p生成成功===============" << endl;
			cout << "生成的素数p为：" << endl;
			cout << P << endl;
			cout << "==============生成第二个素数q==============" << endl;
			largeInt Q = GeneratePrime();
			cout << endl;
			cout << "===============素数q生成成功================" << endl;
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
			//ifstream infile("RSA.txt");
			//int i = 0;
			//char line[100];
			//unsigned int num[100];
			//while (infile.getline(line, 1000, ' '))
			//{
			//	cout << line << endl;
			//	num[i] = stoi(line);
			//	i++;
			//}
			//int k;
			//cout << "i：" << i << endl;
			//for (k = 0; k < 32; k++)
			//	n.data[k] = num[k];
			//for (k = 0; k < 16; k++)
			//	e.data[k] = num[32 + k];
			//for (k = 0; k < 32; k++)
			//	d.data[k] = num[32 + 16 + k];
			//infile.close();

			//cout << "================生成n===============" << endl;
			//cout << n << endl;
			//cout << "==============生成公钥e=============" << endl;
			//cout << e << endl;
			//cout << "==============生成私钥d==============" << endl;
			//cout << d << endl;
			pass = true;
		}
		else
		{
			cout << "您输入不合规，请您重新输入：";
			pass = false;
		}
	} while (!pass);


	cout << endl << "===============================RSA公钥传输==============================" << endl;
	char e_str[512];
	e.to_str(e_str);
	// 发送公钥e_A
	if (SOCKET_ERROR == send(Conn_new_Socket, e_str, strlen(e_str), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	// 接收公钥e_B
	char e_b[512] = { 0 };
	if (SOCKET_ERROR == recv(Conn_new_Socket, e_b, sizeof(e_b), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	// 发送公钥n_A
	char n_str[512];
	n.to_str(n_str);
	if (SOCKET_ERROR == send(Conn_new_Socket, n_str, strlen(n_str), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	// 接收公钥n_B
	char n_b[512] = { 0 };
	if (SOCKET_ERROR == recv(Conn_new_Socket, n_b, sizeof(n_b), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	cout << "=========已收到对方RSA公钥===========" << endl;
	largeInt e_B, n_B;
	to_arr(e_B.data, e_b);
	to_arr(n_B.data, n_b);
	cout << "对方公钥e：" << endl;
	cout << e_B << endl;
	cout << "对方公钥n：" << endl;
	cout << n_B << endl;


	cout << endl << "=====================身份认证+AES会话密钥分配【保密性+认证性】=====================" << endl << endl;
	cout << "请输入您的身份ID(不超过6位)：";
	char id[6];
	cin >> id;
	srand((int)time(0));
    int	N1 = rand()%1000;
	char N1_str[6];
	snprintf(N1_str, sizeof(N1_str), "%d", N1);
	cout << "自动生成一次性随机数IDa：" << N1_str << endl;
	cout << "发送A公钥加密的一次性随机数N1和身份IDa" << endl;
	if (SOCKET_ERROR == send(Conn_new_Socket, id, sizeof(id), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}
	if (SOCKET_ERROR == send(Conn_new_Socket, N1_str, sizeof(N1_str), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}
	char N1_recv[6] = { 0 };
	char N2[6] = { 0 };
	if (SOCKET_ERROR == recv(Conn_new_Socket, N1_recv, sizeof(N1_recv), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}
	if (SOCKET_ERROR == recv(Conn_new_Socket, N2, sizeof(N2), 0))
	{
		cout << "recv失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}
	if (strcmp(N1_str, N1_recv) == 0)
		cout << "B传输回的一次性随机数N1经解密与初始生成的N1相同，B身份验证通过" << endl;
	cout << "使用B的RSA公钥加密收到的一次性随机数N2并再次发送给B" << endl << endl;
	if (SOCKET_ERROR == send(Conn_new_Socket, N2, sizeof(N2), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	cout << "请输入AES 128位密钥(十六进制)：" ;
	int txt[4][4];
	int key[4][4];
	largeInt key_largeInt;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			cin >> (hex) >> key[j][i];
			key_largeInt.data[i] += key[j][i];
			if (j < 3)
				key_largeInt.data[i] *= 16 * 16;
		}
	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//		cout << key[i][j] << " ";
	//	cout << endl;
	//}
	//for (int i = 0; i < 4; i++)
	//{
	//	cout << key_largeInt.data[i] << " ";
	//}
	//cout << endl;
	//cout << key_largeInt << endl;
	cout << endl << endl << "所输入的AES会话密钥largeInt形式为：" << endl;
	cout << key_largeInt << endl;
	cout << "=======加密AES会话密钥=======" << endl;
	largeInt c;
	c = Encrypt(key_largeInt, d, n);
	//c = Encrypt(c, e_B, n_B);
	cout << "RSA加密AES密钥后为：" << endl;
	cout << c << endl;

	char c_str[512];
	c.to_str(c_str);
	// 发送RSA加密后AES密钥
	if (SOCKET_ERROR == send(Conn_new_Socket, c_str, strlen(c_str), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}
	cout << "=====已完成密钥分配【保密性+认证性】=====" << endl;
	Sleep(1000);


	cout << endl << "=================================AES加密文件=================================" << endl << endl;
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

	do {
		inv_s_box_gen();  //生成逆S盒
		cout << "请您输入加密传输文件（任意类型）地址：";
		string TEST_FILE = "1.txt";
		cin >> TEST_FILE;
		ifstream file(TEST_FILE, ios::binary);
		// 读指针设置为文件结尾，获取当前位置即为文件大小size
		file.seekg(0, ios::end);
		int size = file.tellg();
		char* buf = new char[size];
		// 读指针设置为文件开始，读取所有文件数据至缓冲区
		file.seekg(0, ios::beg);
		file.read(buf, size);
		string sendbuf(buf, size);
		pass = true;


		int _length = sendbuf.length();
		bool first_round = true;
		int rounds = 0;
		int start = 0;
		int end = 0;
		char plaintext[30] = { 0 };
		unsigned char m_iv[16];
		unsigned char iv[] = { 103,35,148,239,76,213,47,118,255,222,123,176,106,134,98,92 };
		unsigned char cipher[20] = { 0 };
		unsigned char input[20] = { 0 };
		memcpy(m_iv, iv, 16);
		cout << "您选择哪种保密信息加密模式：1 - CBC  2 - CFB：";
		char ch[2];
		cin >> ch;
		if (SOCKET_ERROR == send(Conn_new_Socket, ch, strlen(ch), 0))
		{
			cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
			closesocket(Conn_new_Socket);
			closesocket(ServerSocket);
			WSACleanup();
			return -1;
		}
		if (_length) { // 发送缓冲区不为空时
			if (_length % 16 == 0)
				rounds = _length / 16;
			else
				rounds = _length / 16 + 1;

			// 循环发送分组
			for (int j = 0; j < rounds; j++)
			{
				start = j * 16;
				end = j * 16 + 16;
				if (end > _length)
					end = _length;
				memset(plaintext, 0, 16);
				memcpy(plaintext, &sendbuf[0] + start, end - start);
				plaintext[end - start] = '\0';

				cout << "分组" << j + 1 << ":" << endl;

				if (strcmp(ch, "1") == 0) {
					// 与向量IV进行异或
					for (int i = 0; i < 16; ++i) {
						if (first_round == true) {
							input[i] = plaintext[i] ^ m_iv[i];
						}
						else {
							input[i] = plaintext[i] ^ cipher[i];
						}
					}
					first_round = false; //错误点
					input[16] = '\0';

					// 明文分组
					for (int i = 0; i < 4; i++)
						for (int j = 0; j < 4; j++)
							txt[j][i] = input[i * 4 + j];

					cout << "【明文】：";
					for (int i = 0; i < 4; i++)
					{
						for (int j = 0; j < 4; j++)
						{
							if (txt[j][i] < 16)
								cout << "0";
							cout << (hex) << txt[j][i];
							if ((j + 1) % 2 == 0)
								cout << " ";
						}
					}

					// AES加密
					Encrypt_AES(txt, key);

					// 保存加密结果作为下一轮IV向量
					for (int i = 0; i < 4; i++)
						for (int j = 0; j < 4; j++)
							cipher[i * 4 + j] = txt[j][i];
				}
				else if (strcmp(ch, "2") == 0)
				{
					cout << "【明文】：";
					int iv_temp[4][4];
					if (first_round == false) {
						for (int i = 0; i < 4; i++)
							for (int j = 0; j < 4; j++)
								iv_temp[j][i] = cipher[i * 4 + j];
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
							input[i * 4 + j] = iv_temp[j][i];
					for (int i = 0; i < 16; i++) {
						if (i == 18) {
							cipher[i] = 0 ^ input[i];
						}
						else {
							cipher[i] = plaintext[i] ^ input[i];
						}
					}
					for (int i = 0; i < 4; i++)
						for (int j = 0; j < 4; j++)
							txt[j][i] = cipher[i * 4 + j];
				}
				
				cout << "    【加密后密文】：";
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						if (txt[j][i] < 16)
							cout << "0";
						cout << (hex) << txt[j][i];
						if ((j + 1) % 2 == 0)
							cout << " ";
					}
				}
				cout << endl;
				largeInt ciphertext;
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						ciphertext.data[i] += txt[j][i];
						if (j < 3)
							ciphertext.data[i] *= 16 * 16;
					}
				}
				char ciphertext_str[512];
				ciphertext.to_str(ciphertext_str);
				if (SOCKET_ERROR == send(Conn_new_Socket, ciphertext_str, strlen(ciphertext_str), 0))
				{
					cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
					closesocket(Conn_new_Socket);
					closesocket(ServerSocket);
					WSACleanup();
					return -1;
				}
				Sleep(1500);
			}
			send(Conn_new_Socket, "finish", strlen("finish"), 0);
			cout << endl << "成功发送该文件所有分组" << endl;
			cout << endl << "===========================完成本次文件加密传输==============================" << endl << endl;

		}
		else
			cout << "您发送内容为空" << endl;

		cout << "您是否需要再次加密传输信息： 1 - 再次加密传输      其他 - 结束并退出：";
		cin >> choose;
	} while (choose == 1);

	cout << endl << "=============================文件加密传输结束=================================" << endl << endl;
	// 退出
	if (SOCKET_ERROR == send(Conn_new_Socket, "exit", strlen("exit"), 0))
	{
		cout << "send发送失败，请通过" << WSAGetLastError() << "获取详情" << endl;
		closesocket(Conn_new_Socket);
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}
	//关闭socket
	closesocket(Conn_new_Socket);
	closesocket(ServerSocket);
	// 结束使用socket，释放Socket DLL资源
	WSACleanup();
	system("pause");
}


