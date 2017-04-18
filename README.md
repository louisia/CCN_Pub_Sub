# 配置IPV6
1. 使用ifconfig命令，查看是否有Scope为Link和Scope Global的地址
2. 配置IPV6 Global地址：
	在/etc/network/interfaces文件中，添加：
	```
	auto eth0
	iface eth0 inet6 dhcp
	```
	重新启动网络：sudo service networking restart ,此时可以看到IPV6 Global地址
	修改/ect/hosts文件：[参考](http://blog.mythsman.com/?p=2677)
	
# 安装openssl
1. sudo apt-get install openssl 
2. sudo apt-get install libssl-dev

# 安装jsoncpp
1. 安装scons，安装json，[参考](http://blog.csdn.net/khalily/article/details/8844546)
2. 将lib文件下的文件，更名为libjson.a和libjson.so放到/usr/local/lib目录下
3. 将include文件下的文件，放到/usr/include目录下
4. 使用ldconfig命令更新
	
# 工程文件编译器配置：
1. Project——Properties——C++  Build——Settings——Gross G++ Linker——Library——Libraries
```
-pthread（使用pthread线程库）
-crypto（使用OpenSSL的哈希计算）
-json(使用jsoncpp)
```

# 底层通信方式
1. IPv6 UDP通信
2. IPv6 Multicast，本质上使用IPv6 UDP通信完成，主要是RP向EP发送数据报文时的底层通信
 
# 关键技术
1. 布隆过滤器
2. HRW算法
3. IPv6单播/多播编程
4. ST表的聚合（先/sports/football后/sports）和ST表的抑制（先/sports后/sports/football）

# 说明
## 节点类型
1. 总控节点：
	- 获悉系统的拓扑结构，每个节点向总控节点注册，获得对应的节点类型和节点标志符
	- 为每个节点构建FIB，并下发给各个节点（NDN层）
	- 为每个节点构建邻接表，并下发给各个节点（IPv6层）
	- 将publishers周期性上传的subList，周期性下发给subscribers
2. 普通路由节点
	- 报文的转发或抑制
3. 汇聚路由节点
	- 接收发布报文，解析为数据报文，进行下发
	- 接收订阅报文
	- 接收组播地址请求报文，分配组播地址
4. 边缘路由节点
	- 接收订阅报文，构造为组播地址请求报文
	- 接收组播地址应答报文，加入组播组
	- 其他。。。
5. 发布内容节点
	- 产生发布报文
6. 订阅内容节点
	- 产生订阅报文

## 报文类型
1. 订阅报文：

```
PacketType:	7
role		2
id		2
rendezvous		RRot-1
contentID		/sports/football
sequenceNumber		0

```

2. 组播地址请求报文

```
PacketType:	10
role		2
id		1
rendezvous		RRot-1
contentID		/sports/football/Lionel Messi
sequenceNumber		0

```


3. 组播地址应答报文

```
PacketType:	11
contentname		/sports/football
joinGroupIp		ff02:c283:360b:f43c:63be:694b:4c7b:f77c
quitGroupIPNumber 1
contentname		/sports/football/Lionel Messi
quitGroupIp		ff02:87c7:bc07:9190:bf49:7bb9:9d3f:7438      

```

4. 发布报文

```
PacketType:	9
role		4
id		1
rendezvous		RRot-1
contentName		/sports/football
content		hello sports football
sequenceNumber		4

```


5. 数据报文

```
PacketType:	8
role		4
id		1
contentName		/sports/football/Lionel Messi
content		hello sports football lionel messi 
sequenceNumber		2

```



## 数据结构

1. ST表：Face|CBF|CDList
2. 邻接表：Face|IPv6Address|SPort|Rport
3. FIB表：Face|RRotId


# 优化
1.publisher也通过filename来指定读写的文件

# 编写代码中遇到问题的原因解析
1. 多个EP节点不要在同一台机器上运行，因为EP在相同的UDP socket上监听，这样会造成当某个节点加入一个组播组，而其他节点并没有加入时，其他节点也会收到这个报文
2. 汇聚节点添加两个新的数据成员m_cdGroupIPRelation和m_faceFlags
3. 查询FIB表进行转发的操作必须在ST的更新之前，因为如果FIB表不存在的话，应该释放该消息，不可以去更新ST表
4. 报文的抑制和ST表的聚合
5. msg结构体添加_type和_multicast两个属性，用于指示是否使用组播，使用组播的话，使用哪个地址
6. 修改EP收到多个publish报文，主要在于ACK应答的问题，RP发送时使用的地址时组播地址，RP接收后，发送ACK，使用的时主机的地址，两个地址不同，导致ACK的判断一直有误，从而不断重发
7. RPS收到组播地址请求消息后，首先查看是否已经为该CD分配组播地址，若已分配，则计数加1,否则使用MD5算法计算该CD的组播地址，并添加到std::vector<CDGroupIPRelation> m_cdGroupIPRelation中
8. RPS收到某个EPS的组播地址请求消息时，除了要为该CD分配组播地址，还要提示该EPS从该CD的子条目组播组中退出
9. subscriber的订阅CD不是在程序中固定，而是每隔一段时间从文件中进行读取，当文件中的订阅CD更新(增加)时，在程序中实时反映

# 隐患
1. Communicator中的重发机制，在真实网络中运行时，无法估算超时的时间，报文的丢失和报文的耽搁如何正确的判断，以及避免重传的错误






