# Qt5 官方的 DTLS Server 样例程序

__英文文档:__
 - DTLS Server demo 样例程序: https://doc.qt.io/qt-5/qtnetwork-secureudpserver-example.html
 - QDtls API 接口文档 https://doc.qt.io/qt-5/qdtls.html

## 样例程序截图
![screenshot](images/secureudpserver-example.png)

## Qt 5.12 Examples 源文件列表
[Examples/Qt-5.12.2/network/secureudpserver/](./secureudpserver/) 目录文件如下:
- Examples/Qt-5.12.2/network/secureudpserver/main.cpp
- Examples/Qt-5.12.2/network/secureudpserver/mainwindow.cpp
- Examples/Qt-5.12.2/network/secureudpserver/mainwindow.h
- Examples/Qt-5.12.2/network/secureudpserver/mainwindow.ui
- Examples/Qt-5.12.2/network/secureudpserver/nicselector.cpp
- Examples/Qt-5.12.2/network/secureudpserver/nicselector.h
- Examples/Qt-5.12.2/network/secureudpserver/nicselector.ui
- Examples/Qt-5.12.2/network/secureudpserver/secureudpserver.pro
- Examples/Qt-5.12.2/network/secureudpserver/server.cpp
- Examples/Qt-5.12.2/network/secureudpserver/server.h

## API 兼容性
 - 要求 Qt 版本大于等于 5.12

# 如何调试 DTLS Server 样例程序
借助 openssl s_client 或 tcpdump 抓包工具可以辅助调试本程序.

# 安装 tcpdump / wireshark-gtk 工具监听本机网络报文
```
# Ubuntu 安装 wireshark 嗅探器
sudo apt install wireshark-gtk
```
安装过程中应选中允许普通用户以root管理员身份抓包...
```
# 以root管理员身份启动嗅探器
sudo wireshark-gtk
```
tcpdump抓包并过滤出特定端口的UDP报文的具体方法此处不进行详细介绍, 参见相应的抓包教程.

# 借助 openssl s_client 进行联调
首先运行上面的DTLS服务器程序, 绑定到本机的IP地址(例如笔者本地主机的IP地址为192.168.1.106, UDP端口号为22334);
然后在终端窗口中启动 openssl s_client 客户端访问相应的服务器端口号, 命令为:
```
openssl s_client -dtls -connect 192.168.1.106:22334 -psk 1a2b3c4d5e6f -psk_identity "client_bob"
```
- 选项 -psk 指定PSK密钥值时开头不加0x
- 选项 -psk_identity 后面的字符串可以随意填写

终端窗口中openssl输出的信息样例如下:
```
$ openssl s_client -dtls -connect 192.168.1.106:22334 -psk 1a2b3c4d5e6f -psk_identity "client_bob"
CONNECTED(00000003)
Can't use SSL_get_servername
---
no peer certificate available
---
No client certificate CA names sent
Server Temp Key: DH, 1024 bits
---
SSL handshake has read 757 bytes and written 800 bytes
Verification: OK
---
New, TLSv1.2, Cipher is DHE-PSK-AES256-GCM-SHA384
Secure Renegotiation IS supported
Compression: NONE
Expansion: NONE
No ALPN negotiated
SSL-Session:
    Protocol  : DTLSv1.2
    Cipher    : DHE-PSK-AES256-GCM-SHA384
    Session-ID: 756A363B21C5425588F3EC1901CACE888C5FB87F6C292FEF42C91DF1BAF366C1
    Session-ID-ctx:
    Master-Key: EC4C1B68B4C1195590CFF5594D90AB2394CAFE11C322AD42E001FA7501061D45F1175E9B1FA94EF264FBCDAE43E28ACD
    PSK identity: client_bob
    PSK identity hint: Qt DTLS example server
    SRP username: None
    TLS session ticket lifetime hint: 7200 (seconds)
    TLS session ticket:
    0000 - 92 f5 96 59 1a 6b 5d 82-50 20 a2 83 d6 ad 9d 86   ...Y.k].P ......
    0010 - 11 77 83 04 8f 46 7a 1e-90 0c d3 df 29 2f 45 34   .w...Fz.....)/E4
    0020 - 3b 79 b0 33 74 ca 6f 44-94 30 39 cd 55 cb 91 46   ;y.3t.oD.09.U..F
    0030 - c0 7f 7a d0 85 07 08 ef-b0 a1 75 d6 98 de e4 e9   ..z.......u.....
    0040 - cf c4 92 0a 0d 2a b1 87-7a fb 6f c0 fa 9f 5e 66   .....*..z.o...^f
    0050 - 4f 54 9e 8d 3e 28 36 c2-91 72 ea 38 58 6d 4f 5a   OT..>(6..r.8XmOZ
    0060 - 5b 58 04 ca 9d 5e 27 b0-2b 78 71 8d fa c1 0f f6   [X...^'.+xq.....
    0070 - fe ed 83 54 65 da 91 d3-6d f6 25 51 22 62 d7 69   ...Te...m.%Q"b.i
    0080 - 62 61 00 1d 33 ba 1a 06-ef 6e ff 7b 8f 1c f5 f9   ba..3....n.{....
    0090 - c4 b6 53 3c 4f 40 69 8c-72 3d 62 36 36 f8 19 e7   ..S<O@i.r=b66...
    00a0 - 7d 25 0d 58 90 80 c7 e9-8a 7b 63 9f d6 eb 6b d0   }%.X.....{c...k.

    Start Time: 1557110530
    Timeout   : 7200 (sec)
    Verify return code: 0 (ok)
    Extended master secret: yes
---
hello
to (192.168.1.106:39968): ACK
```
