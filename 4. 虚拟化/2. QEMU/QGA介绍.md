# 原理

​	QGA是一个运行在虚拟机内部的普通应用程序（可执行文件名称默认为qemu-ga，服务器名称默认为qemu-guest-agent），其目的是实现一种宿主机和虚拟机进行交互的方式，这种方式不依赖于网络，而是依赖于virtio-serial（默认首选方式）或者isa-serial，而QEMU则提供了串口设备的模拟及数据交换的通道，最终呈现出来的是一个串口设备（虚拟机内部）和一个unix socket文件（宿主机上）。

​	QGA通过读写串口设备与宿主机上的socket通道进行交互，宿主机上可以使用普通的unix socket读写方式对socket文件进行读写，最终实现与QGA的交互，交互的协议与QMP（QEMU Monitor Protocol）相同（简单来说就是使用JSON格式进行数据交换），串口设备的速率通常都较低，所以比较适合小数据量的交换。

# 指令

## **查询反向命令状态**

guest-get-reverse-command-status

​	查询反向事件的开关状态及执行周期

## **查询反向事件状态**

​	guest-get-reverse-event-status

​	查询反向命令的开关状态及执行周期

## **设置反向命令状态**

​	guest-set-reverse-command-status

​	设置反向命令的开关状态及执行周期

## **设置反向时间状态**

​	guest-set-reverse-event-status

​	设置反向事件的开关状态及执行周期

## **虚拟机时钟同步功能**

​	虚拟机能周期同步主机的时间，此功能默认开启，每10分钟同步一次时间，该功能可以通过命令开启/关闭以及设置时钟同步周期。

## **设置时钟同步周期**

​	可以使用qga命令在主机端进行设置：

​	virsh qemu-agent-command –pretty <domain> <string>

​	参数说明：

​	<string>：要执行的命令，在设置时钟同步周期时，只能为如下的格式：

​	‘{“execute”:”guest-set-reverse-command-status”,”arguments”:{”command”:”host-get-time”,”enable”:true,”interval”:1}}’，其中interval参数的值可根据需要修改，host-get-time表示要设置的是时钟同步功能。

## **控制时钟同步开关**

​	可以使用qga命令在主机端进行开启或关闭：

​	virsh qemu-agent-command—pretty<domain> <string>

## **查询时钟同步状态**

​	virsh qemu-agent-command –pretty <domain> <string>

## **虚机内部日志控制模块和打印级别**

​	virsh qemu-agent-command –pretty <domain> <string>

​	参数说明：

​	<string>：{“execute”:”guest-log-control”,”arguments”:

{“module”:”qga”,”level”:4}}，其中module参数的值可以是qga，vdagent，netkvm，viostor，vioserial和all，为all时，打印所有模块的日志。

​	“level”:4：表示打印级别为4，打印级别范围0-5,0表示不打印。

# 问题

​	在平时测试使用的时候，出现过这样的一个问题L虚机绑定虚拟显卡后启动时间大概30s左右，但是关闭虚机的时候却需要数分钟

​	解决步骤：

1、 查看下发关机指令的时间，查看主机host接收和反馈的时间是否异常

2、 查看是否是QGA问题，使用指令查看虚机的QGA是否正常

virsh qemu-agent-command –pretty dom_id {“execute”:”guest-ping”}

发现QGA未启动起来，mlibvirt发送了关机指令，等待了1分钟无响应，然后libvirt通过ACPI方式关闭虚机，因此整个关机过程多等待了1分钟

3、 查看虚机代理是否正常：虚机代理异常关机必然超时