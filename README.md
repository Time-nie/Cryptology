# Cryptology
NKU-COMP0012-密码学

以下实验及大作业（基于身份认证和多模式的 AES 保密通信协议）均为**满分**，如果需要1V1解析 or/and 全套实验报告[可编辑]欢迎邮件联系我



#### 一. 古典密码算法及攻击方法

1. 移位密码
2. 对移位密码的攻击
3. 单表置换密码
4. 对单表置换密码的攻击方法



#### 二. 分组密码DES算法

1. DES加密
2. DES解密
3. 雪崩效应检验



#### 三. 分组密码AES算法

1. AES加密
2. AES解密
3. 雪崩效应检验



#### 四. RSA密码算法

1. 随机大素数生成
2. RSA加密
3. RSA解密



#### 五. MD5密码算法

1. MD5算法
2. 雪崩实验



#### 六. 基于身份认证和多模式的 AES 保密通信协议

1. 该密码学大作业设计了一个高仿真的保密通信协议，全过程模拟真实用户访问状态，可以加密传输任意类型任意大小文件，自主选则 **CBC、CFB**加密模式，采用具有**保密性和认证性**的密钥分配协议，利用 RSA 公钥密码算法，进行身份认证后为双方分配一个 AES 算法的会话密钥，然后利用 AES 加密算法和分配的会话密钥，加解密传送的文件并保存在用户指定路径。
2. 详细内容见实验报告及源代码，主要分为三个大模块：通信连接模块、密钥分配模块和 AES 加解密模块，实验报告中协议设计、代码实现也都会围绕这三个模块为主线进行详细讲解。
