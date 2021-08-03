# 网络配置文件

## **网络连接配置**

### **centos系**

配置文件：/etc/sysconfig/network-scripts/ifcfg-eth0

DEVICE=eth0

\#关联的设备名称

BOOTPROTO={static|dhcp|none|bootp}

 \#表示引导协议static|none表示静态；dhcp表示动态

IPADDR=192.168.10.10

 \#设置IP地址

NETMASK=255.255.255.0

 \#设置子网掩码

GATEWAY=192.168.10.1

 \#设置网关

ONBOOT=yes|no

 \#开机时是否自动激活网络接口

HWADDR=00:1E:0B:8F:B0:D0

 \#硬件地址要与硬件中的地址保持一致可省

DNS1=202.106.0.20

 \#指定DNS地址

USERCTL=yes|no

 \#表示是否允许普通用户启用或禁用此接口

PEERDNS=yes|no

 \#表示是否在BOOTPROTO为dhcp时接受由dhcp服务器指定的dns地址

### **ubuntu系**

配置文件：/etc/network/interfaces

\# The primary network interface

auto eth0

iface eth0 inet static

address 192.168.1.12

gateway 192.168.1.1

netmask 255.255.255.0

\#network 192.168.1.0

\#broadcast 192.168.1.255

\#dns-nameserver 8.8.8.8

## **DNS配置**

DNS配置文件：/etc/resolv.conf/

nameserver 127.0.0.1

nameserver 127.0.0.1

 

控制网络是否开启和IPv6是否有效和主机名设置文件：/etc/sysconfig/network

NETWORKING=YES|NO

NETWORKING_IPV6=yes|no

HOSTNAME=hostname

NETWORKING=YES|NO

NETWORKING_IPV6=yes|no

HOSTNAME=hostname

## **主机映像配置**

定义主机映像文件：/etc/hosts

172.168.10.1  www.baidu.com  alias

172.168.10.1  www.baidu.com  alias

# 网卡命名方式

lo：本地回环用于本机客户端访问本机服务器程序

ppp#：点对点

eth#：以太网网卡

RHEL5.0中/etc/modprobe.conf：根据alias定义网卡名称

RHEL6.0中/etc/udev/rules.d/70-persistent-net.rules：中根据MAC地址定义网卡名称（udev文件是Linux2.6内核的重大改变；在2.4内核时/dev目录中提供了大量的不同设备文件的访问入口而在2.6内核中缩减了/dev下大量的代码因为引入了udev机制可以自动根据内核识别到的硬件信息自动创建对应的设备文件并给一个特定的名称）

 

# 网络管理工具

Linux目前提供两种网络管理工具，一种是network，一种是NetworkManager。

## **network**

network是控制网络接口配置信息修改后，网络服务必须重新启动，来激活网络新配置，从而使得配置生效。这部分操作对服务而言和重新启动系统时是一样的作用。控制脚本是/etc/init.d/network这个文件，可以用这个文件后面加上下面的参数来操作网络服务。例如：

/etc/init.d/network restart

/etc/init.d/network restart

同样也可以用service这个命令来操作网络服务例如：

service network restart

service network restart

## **NetworkManager**

RHEL6新增加的网络管理工具，可用检测网络、自动连接网络的程序。无论是无线还是有线连接，它都可以令您轻松管理。对于无线网络,网络管理器可以自动切换到最可靠的无线网络。利用网络管理器的程序可以自由切换在线和离线模式。网络管理器可以优先选择有线网络，支持 VPN。网络管理器最初由 Redhat 公司开发，现在由 GNOME 管理。

NetworkManager 新版本增强设备以及协议的支持，NetworkManager 最新版有一个全新的漂亮的客户端界面nmtui。”nmtui” 是 networkmanager 的一个图形化前端。在没有X Window 的情况下可以用它来方便地配置及管理网络。启动nmtui：

[root@localhost ~]# nmtui

 

NetworkManager 最新版的最大特征： 命令行工具命令，一个NetworkManager的命令行接口。NetworkManager的CLI工具nmcli，使用nmcli用户可以查询网络连接的状态，也可以用来管理。优点：原始；语法相对简单；在CLI中使用NetworkManager很容易掌握。

 

### **nmcli基本配置选项**

nmcli con show

 \#获得一份UUID列表

nmcli dev

 \#查看网络设备及其状态

nmcli r wifi off

 \#关闭WiFi

nmcli con show

 \#获得一份UUID列表

nmcli dev

 \#查看网络设备及其状态

nmcli r wifi off

 \#关闭WiFi

NetworkManager守护进程启动后，会自动连接到已经配置的系统连接。用户连接或未配置的连接需要通过nmcli或桌面工具进行配置和连接。

### **开机启用 NetworkManager**

chkconfig NetworkManager on

或（CentOS7）

systemctl enable NetworkManager

chkconfig NetworkManager on

或（CentOS7）

systemctl enable NetworkManager

### **立即启动 NetworkManager**

service NetworkMnager start

 

或（CentOS7）

systemctl start NetworkManager

service NetworkMnager start

或（CentOS7）

systemctl start NetworkManager