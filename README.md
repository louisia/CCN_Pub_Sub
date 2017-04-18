# 配置IPV6
1. 使用ifconfig命令，查看是否有Scope为Link和Scope Global的地址
2. 配置IPV6 Global地址：
	在/etc/network/interfaces文件中，添加：iface eth0 inet6 dhcp
	重新启动网络：sudo service networking dhcp ,此时可以看到IPV6 Global地址
	修改/ect/hosts文件：http://blog.mythsman.com/?p=2677
	
# 安装openssl
1. sudo apt-get install openssl 
2. sudo apt-get install libssl-dev

# 安装jsoncpp
1. 安装scons，安装json，[参考])http://blog.csdn.net/khalily/article/details/8844546)
2. 将lib文件下的文件，更名为libjson.a和libjson.so放到/usr/local/lib目录下
3. 将include文件下的文件，放到/usr/include目录下
4. 使用ldconfig命令更新
	
# 工程文件编译器配置：
1. Project——Properties——C++  Build——Settings——Gross G++ Linker——Library——Libraries
-pthread（使用pthread线程库）
-crypto（使用OpenSSL的哈希计算）
-json(使用jsoncpp)

# 底层通信
 IPv6协议
 
# 关键技术
1. 布隆过滤器
2. HRW算法
3. IPv6单播/多播编程

# 如果某个进程异常终止
1. 查看消息的输出，由于内存的异常操作(这个异常不会被捕获),某个消息显示异常，则可能时这里导致程序异常终止

# 修改
## 汇聚节点添加两个新的数据成员m_cdGroupIPRelation和m_faceFlags
## 查询FIB表进行转发的操作必须在ST的更新之前，因为如果FIB表不存在的话，应该释放该消息，不可以去更新ST表
## 报文的抑制和ST表的聚合
## msg结构体添加_type和_multicast两个属性，用于指示是否使用组播，使用组播的话，使用哪个地址
## 修改EP收到多个publish报文，主要在于ACK应答的问题，RP发送时使用的地址时组播地址，RP接收后，发送ACK，使用的时主机的地址，两个地址不同，导致ACK的判断一直有误，从而不断重发
## RPS收到组播地址请求消息后，首先查看是否已经为该CD分配组播地址，若已分配，则计数加1,否则使用MD5算法计算该CD的组播地址，并添加到std::vector<CDGroupIPRelation> m_cdGroupIPRelation中
## RPS收到某个EPS的组播地址请求消息时，除了要为该CD分配组播地址，还要提示该EPS从该CD的子条目组播组中退出
## subscriber的订阅CD不是在程序中固定，而是每隔一段时间从文件中进行读取，当文件中的订阅CD更新(增加)时，在程序中实时反映



# 优化
## publisher也通过filename来指定读写的文件

# 注意
## 多个EP节点不要在同一台机器上运行，因为EP在相同的UDP socket上监听，这样会造成当某个节点加入一个组播组，而其他节点并没有加入时，其他节点也会收到这个报文





