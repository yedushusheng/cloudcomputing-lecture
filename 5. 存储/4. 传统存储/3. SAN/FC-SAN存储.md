# 概述

早期的SAN采用的是光纤通道（FC，Fibre Channel）技术，所以，以前的SAN多指采用光纤通道的存储局域网络，业内称为FCSAN。

SAN存储本质是块存储，主机端看到的是一个个的磁盘，并不区分底层是怎么组合的。

# 原理

FC-SAN一般需要配备：

1、服务端需要有FC HBA卡

2、存储阵列有FC端口

3、网络需要FC交换机

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsC45A.tmp.jpg) 

# 特点

**优点：**

传输带宽高，目前有1,2,4和8Gb/s四种标准，主流的是4和8Gb/s；

性能稳定可靠，技术成熟，是关键应用领域和大规模存储网络的不二选择。

**缺点：**

成本极其高昂，需要光纤交换机和大量的光纤布线；

维护及配置复杂，需要培训完全不同于LAN管理员的专业FC网络管理员。

# 操作

## 服务端

1、检查主机上是否有FC HBA 卡驱动标识：

运行cat /sys/class/fc_host/host1/node_name查询FC HBA卡的WWN号

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsC49A.tmp.jpg) 

如果没有FC HBA卡驱动标识，就必须手动安装HBA驱动。

2、对于Windows系统，重启电脑，在BIOS显示信息中会有HBA卡的WWN号

也可以用windows专门查看HBA卡工具来查看，比较方便，推荐使用这种方法

3、设置存储器端口到主机映射

## 存储端

 

# 多路径

# 应用

 