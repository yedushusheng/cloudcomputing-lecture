# 什么是unit？

​	按说可以直接设置目录为访问的路径，即unit存储单元连接信息，但是我们将/dev/下面的设备目录通过/opt/zxve/storage下面的连接信息unit实现访问，这样做主要出于这样的考虑：

1、 可以将一个存储设备device设置为多个存储单元unit访问，提高效率；

2、 可以增加安全性，不至于直接操作硬件设备。

# 分类

​	常用的存储单元unit有：直接以磁盘形式呈现给用户（Disk），以分区形式呈现（partition），以访问路径形式（path），以存储池形式（pool）。

# 编程

## **本地存储**

1、 获取连接信息（即对应的路径），并判断该源路径是否存在，如果不存在，重新创建；

2、 创建软链接（/opt/zxve/storage/uuid/home/vpshareà/home/vpshare），即对外使用比较长的这个连接信息，用于区分不同存储池下面的主机，到本地后直接连接到本地的简单路径即可；

3、 获取该连接信息对应的磁盘信息；

df –ma unit_connection | sed ‘/%%!N;s/\\n;s/\\n//’ awk ‘NR==2{print $2,$4}’ | sed ‘s/%%//g’

 

## **IP-SAN**

1、 登录磁阵，检查符号连接；

2、 获取LUN大小：readlink unit_connection | awk –F ‘/’ ‘{print $NF}’

 cat /sys/class/block/dm/size

## **FC-SAN**

1、 获取光口数量：ls /sys/class/fc_host | grep host | wc –l

2、 针对每个光口：

ls /sys/class/fc_host | grep host | awk ‘NR==index+1 {print $1}’àbuffer

echo “---”>/sys/class/scsi_host/buffer/scan

3、 检查连接信息；

4、 获取LUN大小

## **EBS**

1、 获取总空间、剩余空间：

ebsadm lps | grep basename(unit_connection) | awk “{print $3,$5}”

2、 

## **Ceph-Pool**

1、 获取总空间、剩余空间：

ceph df | grep basename(unit_connection) | awk “print $3,$5”

2、 

## **Ceph-LUN**

1、 检查连接信息；

2、 获取LUN大小

## **NFS**

1、 获取挂载信息：

mount | grep source_path | grep dest_path | wc –l 若已经挂载，返回成功

若connection不存在，创建挂载点目录：mkdir –p connection

mount –t nfs –o soft,retrains=3,timeo=600,users source_path dest_path 挂载

mount | grep source_path | grep dest_path | wc –l 检查是否挂载成功

2、 获取大小：

df –ma unit_connection | sed ‘/%%!N;s/\\n;s/\\n//’ awk ‘NR==2{print $2,$4}’ | sed ‘s/%%//g’

 

 