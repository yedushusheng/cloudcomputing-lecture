# 生命周期管理

## 创建虚拟机

​	# virsh create <domain.xml>

## 关闭虚拟机

​	# virsh shutdown <id>|<name>|<uuid>| [-mode <agent>|<acpi>]

​	注：支持两种关闭虚机的方式：acpi模式和qga模式。

## 强制关闭虚机

​	# virsh destroy <id>|<name>|<uuid>

## 重启虚拟机

​	# virsh reboot <id>|<name>|<uuid> [-mode <acpi>|<agent>]

​	注：支持两种模式重启虚机：acpi模式和qga模式。

## 复位虚拟机

​	# virsh reset <id>|<name>|<uuid>

## 暂停虚拟机

​	# virsh suspend <id>|<name>|<uuid>

## 恢复暂停虚拟机

​	# virsh resume <id>|<name>|<uuid>

## 创建/恢复虚机快照

### 带内存快照

#### 创建raw格式快照

​	首先，用virsh save命令保存内存状态，保存完成后虚机将自动关机。

​	# virsh save <id>|<name>|<uuid> /path/file.save

​	然后通过拷贝的方式保存磁盘状态，virsh save时虚拟机所在磁盘将作为快照恢复后的运行节点。

​	注：快照与克隆的唯一区别在于快照只读不写，克隆可读写！

#### 恢复raw格式快照

​	# virsh restore /path/file.save

#### 创建qcow**2**格式快照

​	创建之前需要使虚拟机处于活动状态（raw格式则是处于关闭状态）。

​	管理层负责存储节点可用，将快照运行磁盘文件路径、磁盘备份文件路径、内存快照文件路径传给libvirt，所有的工作由libvirt和qemu完成。管理层负责删除已经没有作用的文件lv。

​	# virsh snapshot-create-as <id>|<name>|<uuid> [snapshot_name]

​		--diskspec vda,file=[vda_running_file],snapfile=[vda_snapshot_file]

​		--diskspec vdb,file=[vdb_running_file],snapfile=[vdb_snapshot_file]

​		--memspec file=[memory_file]

​		--external-opt

​		--cow

​		--no-metadata

​	说明：

​	--diskspec_file：快照后虚拟机运行节点（快照运行树结构中的叶子节点）

​	--diskspec_snapfile：快照后备份节点

​	--memspec：内存快照文件

​		--external-opt：说明要创建的为外置快照

​		--cow：说明创建cow快照

​		--no-metadata：创建快照时不写入libvirt数据库，否则不能迁移

​	如果管理层事先已经创建好qcow2格式磁盘文件，需要加上—reuse-external参数。

#### 恢复qcow**2**格式快照

​	快照恢复之前需要关闭虚机，然后执行恢复操作：

​	管理层将虚拟机运行磁盘文件路径、创建快照时的快照备份文件路径、内存快照文件路径传给libvirt。管理层负责删除已经没用的文件lv。

​	# virsh snapshot-revert [vm_name] [snapshot_name] 

–memspec file=[memory_file] 

--diskspec vda,backfile=[vda_back_file],snapshot=external,runfile=[vda_run_file]

--diskspec vdb,backfile=[vdb_back_file],snapshot=external,runfile=[vdb_run_file]

--external

说明：

--memspec：内存快照文件

--diskspec backfile：创建快照时创建的备份节点

--diskspec runfile：恢复快照后虚拟机运行的节点

​	--external：说明快照类型为外置快照

注意：diskspec runfile参数长度不能超过虚拟机配置中disk路径长度。

 

### 不带内存快照

​	raw格式虚拟机不支持不带内存的快照。

#### 创建qcow**2**格式快照

# virsh snapshot-create-as <id>|<name>|<uuid> [snapshot_name] –disk-only

​		--diskspec vda,file=[vda_running_file],snapfile=[vda_snapshot_file]

​		--diskspec vdb,file=[vdb_running_file],snapfile=[vdb_snapshot_file]

​		--no-metadata

​	说明：

​	--disk-only：只对磁盘做快照

​	--diskspec_file：快照后虚拟机运行节点

​	--diskspec_snapfile：快照后备份节点

​	--no-metadata：创建快照时不写入libvirt数据库，否则不能迁移

​	如果管理层事先已经创建好qcow2格式磁盘文件，需要加上—reuse-external参数。

​	如果只想拉一个叶子节点，--diskspec后只加file参数，不需要加snapfile参数。

​	注意：只支持对非系统盘进行不带内存的快照，否则可能会导致虚拟机系统异常。

 

#### 恢复qcow**2**格式快照

​	修改虚拟机的配置文件中磁盘路径为快照时备份磁盘路径，用virsh create启动虚拟机。

​	如果用virsh start启动虚拟机，需要重新对该虚拟机执行一次virsh define。

 

## 虚拟机迁移

​	先调用（获取磁盘设备名称）：

​	# virsh qemu-monitor-command <id>|<name>|<uuid> -hmp “migrate_set_capability auto-converge on”

​	再调用：

​	# virsh migrate <id>|<name>|<uuid> qemu+tcp://<des-ip>/system tcp://<des-ip> --live

​	说明：虚机迁移只是把虚拟机迁移到别的地方，但是存储本身不动。

 

## 虚拟机存储迁移

​	# virsh blockcopy <id>|<name>|<uuid> path dest –shallow –wait –reuse-external –pivot –verbose

​	该命令是针对一个磁盘的，若虚拟机有多个磁盘需要存储迁移，则执行多条命令

​	说明：

​	–shallow：若当前节点有backing_file，则只迁移当前节点，即叶子节点

–pivot：迁移完成后，虚拟机切换为dest磁盘

 –reuse-external：dest是已有文件

–verbose：动态显示迁移完成比率

 

如果虚拟机磁盘有父节点，需要按照下面的步骤执行迁移：

环境：storageA作为源存储，storageB作为目的存储，虚拟机名为win7，磁盘为vda_leaf_qcow2，父节点为vda_root_qcow2。

迁移父节点：

dd storage/vda_root.qcow2 storageB/vda_mig_root.qcow2

准备叶子节点（目的节点的叶子节点，不涉及源节点）：

qemu-img create –f qcow2 –F qcow2 –b storage vda_mig_root.qcow2 stoprageB storage_min_leaf.qcow2

存储迁移：

virsh blockcopy win7 vda storage vda_mig_leaf.qcow2 –shallow –wait –resume-external –pivot

删除原磁盘（删除命令根据不同存储而不同，在此以fs为例）：

rm –f storageA/vda_root.qcow2

rm –f storage/vda_leaf.qcow2

注：执行存储迁移前，该虚拟机必须为未定义状态。即如果该虚机已经执行过virsh define操作，需要先执行virsh undefine操作。

## 虚拟机整机迁移

​	先调用：

​	# virsh qemu-monitor-command <id>|<name>|<uuid> -hmp “migrate_set_capability auto-converge on”

​	再调用：

​	# virsh migrate <id>|<name>|<uuid> qemu+tcp://<des-ip>/system tcp://<des-ip> --live –copy-storage-inc –destblock <string>

​	说明：

​	–copy-storage-inc：若当前节点有backing_file，则只迁移当前节点，即叶子节点

–destblock：device_name:dest_path,…多个磁盘设备之间用“,”分割

 

如果虚拟机磁盘有父节点，需要按照下面步骤进行迁移：

环境：hostA为源主机，storageA作为源存储节点，hostB为目的主机，storageB作为目的存储节点，虚拟机名为win7，磁盘为vda_leaf.qcow2，vdb_leaf.qcow2，父节点分别为vda_root.qcow2，vdb_root.qcow2。

迁移父节点（在此拷贝命令以fs为例）：

[hostA] scp storage/ vda_root.qcow2 hostA:hostB/ vda_root.qcow2

[hostA] scp storage/ vdb_root.qcow2 hostA:hostB/ vdb_root.qcow2

准备叶子节点：

[hostB] qemu-img create –f qcow2 –F qcow2 -b storageB/ vda_mig_root.qcow2 storageB/ vda_mig_leaf.qcow2

[hostB] qemu-img create –f qcow2 –F qcow2 -b storageB/ vdb_mig_root.qcow2 storageB/ vdb_mig_leaf.qcow2

整机迁移：

[hostA] virsh migrate win7 qemu+tcp://hostB /system tcp:// hostB --live –copy-storage-inc –destblock vda:storageB/vda_min_leaf.qcow2, vdb:storageB/vdb_min_leaf.qcow2

删除原磁盘：

rm –f storage/vda_root.qcow2

rm –f storage/vda_leaf.qcow2

rm –f storage/vdb_root.qcow2

rm –f storage/vdb_leaf.qcow2

注：执行整机迁移前，该虚拟机必须为未定义状态。

 

## 虚拟机在线克隆

​	# virsh snapshot-create-as <id>|<name>|<uuid> 

--disk-only 

–diskspec vda,file=[vda_running_file],snapfile=[vda_snapshot]file],…

--no-metadata

# 动态资源管理

## 虚拟网卡出入向带宽设置

​	virsh update-device <domain> <file>

​	对应的配置文件为：

​	<bandwidth>

​		<inbound average=’’ peak=’’ burst=’’>

​		<outbond average=’’ peak=’’ burst=’’>

​	<bandwidth>

​	说明：

1、 默认带宽单位为hbit/sec（400bit=400000bit）

2、 默认只需要配置average，Linux TC将自动计算peak值和burst值

3、 peak（对应TC命令中的ceil）可选参数，表示该限速下最大能得到的带宽值（设置该值后，可能造成平均限速不准确）

4、 burst可选参数，表示在瞬间可允许的实发流量大小

## 虚拟网卡端口组共享限速

## 网卡优先级队列设置

## 虚拟网卡网络优先级参数设置

## 虚拟网卡vlan设置

## 内存上限动态设置

## 内存下限动态设置

## 内存优先级动态设置

## *动态设置CPU* Qos参数

### 动态设置CPU优先级

### 动态设置CPU上限

### 动态设置CPU下限

### 动态取消CPU下限

### 动态设置CPU下限

### 动态取消CPU下限

### 动态设置CPU预留

### 动态取消CPU预留

# 动态设备管理

## CPU动态绑定

## CPU热添加

## 内存热添加

## 在线添加磁盘

## 在线添加网卡

### passthrough设备

### SR-IOV+passthrough

## 在线删除网卡

### passthrough设备

### SR-IOV+passthrough

## 在线修改光驱镜像

## 在线添加/删除USB

在线增加磁盘容量

## 在线减小磁盘容量