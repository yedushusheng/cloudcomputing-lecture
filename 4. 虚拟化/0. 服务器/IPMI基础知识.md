# 概述

很多人使用IPMI做带外管理，如果直接使用是有问题的。

在OS层面上，你是可以修改IPMI设置的，也就是说，对于Linux来说，有专门的驱动，让你修改底层IPMI的设置，包括修改你的IPMI的用户名和密码。

 

# *IPMI* tool

## **安装IPMI** **tool**

​	安装方法：

​	yum –y install ipmitool

​	apt-get –y install upmitool

## **内核加载**

​	modprobe ipmi_msghandler

​	modprobe ipmi_decintf

​	modprobe ipmi_si

## **查看IPMI用户**

​	ipmitool user list l

## **设置IPMI** **IP地址**

​	使用静态地址：ipmitool lan set <channel_no> ipsrc static

​	使用动态地址：ipmitool lan set <channel_no> ipsrc dhcp

设置IP：ipmitool lan set <channel_no> ipaddr ****

设置掩码：ipmitool lan set <channel_no> netmask ****

设置网关：ipmitool lan set <channel_no> defgw_ipaddr ****

本地操作：ipmitool –I open lan print 1

操作远程机器：ipmitool –I lan –H **** -U root–Ppasswd lan print 1

# IIPMI view

​	这是超威研发的一个工具，可以在windows和linux使用，统一管理和查看IPMI信息，支持中文，不是超威的机器也可以管理。