# 概述

# 操作步骤

# 应用

## TDSQL

## GoldenDB

### 操作系统

#### **开启内存巨页**

echo 20 >/proc/sys/vm/nr_hugepages

echo 4194304>/proc/sys/kernel/shmall

 

#### **关闭swap**

1、fdisk -l | grep swap	#查找swap分区

2、swapoff /dev/mapper/rhel-swap

3、vim /etc/fstab注释下面的行防止重启后swap开启

/dev/mapper/rhel-swap swap swap defaults 0 0

#### **进程绑核**

尽量保证在一个NUMA节点上：

Proxy绑核：

1、通过numactl --hardware查看

2、停止进程：dbmoni -stop

3、绑核：numactl --physcpubind=64-96 dbstart

DB绑核：

1、dbmoni -stop

2、numactl --physcpubind=0-63 mysql.server start --log-slave-updates=OFF

3、dbstart启动dbagent

### 组件

#### **Proxy**

Proxy配置项修改：

log_level=1

route_instance_number=10

update_grade=1

select_grade=1

judge_active_trans_num=500

exec_thread_num=40

netstat_checkperiod=1800000

shat_report_interval=0

stat_report_interval=0

stat_proxy_topnsql_number=0

enable_stmt_flag=1

thread_poll_size=5

connect_timeout=5000

read_timeout=5000

#### **DB**

数据库配置文件my.cnf：

enable_binlog_gtmgtid_index=OFF

performance_schema=OFF

innodb_buffer_pool_size=128G

innodb_buffer_pool_instances=64

innodb_io_capacity=10000

innodb_io_capacity_max=100000

innodb_lock_wait_timeout=30

innodb_flush_neighbors=0

max_binlog_size=1048576000

thread_pool_size=64

thread_pool_stall_limit=50

slave_preserve_commit_order=0

lock_wait_timeout=60

#### **DBAgent**

binlog_backup_interval=0

#### **GTM**

WriteIncFileMode=2

## OceanBase

 