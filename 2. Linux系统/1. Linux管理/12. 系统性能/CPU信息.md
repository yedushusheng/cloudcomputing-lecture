# 架构

CPU架构主要包括桌面端和移动端两种。

对于桌面端，主要有Intel和AMD两家半导体厂商，在2006年以前主要是Intel 80386/80486等32位，又称X86或8086，2006年以后AMD研制出64位指令集，又称AMD64/X86_64，64位兼容32位，反之不可。

对于移动端，只有ARM一家厂商，在2010年之前，是ARMv1-v7系列指令集，都是32位，称为ARM指令，在2010年以后，调整为Contex系列，分为A系列（高端）、R系列（中端）、M系列（低端），称为ARM64指令。树莓派使用的是A系列芯片。

## X86

## 64

# 性能信息

在采集CPU相关的信息时， 涉及CPU核数和频率，具体采集方式如下：

| 采集项              | 采集方式                                                     |
| ------------------- | ------------------------------------------------------------ |
| CPU物理核           | cat /proc/cpuinfo \| grep 'physical id' \| sort -u \| wc -l  |
| CPU核数（单物理核） | cat /proc/cpu_info\| grep 'processor*' \| wc -l              |
| CPU Hz              | cat /sys/device/system/cpu/cpu0/cpufreq/cpuinfo_max_frepsed -n 's/cpu MHz\t*:\(.*\)\.*/\1/p' /proc/cpuinfo \| head -n 1cat /proc/cpu_info\| grep 'MHz' \| awk '{print $4}' |
| CPU使用率           | 1、每个1s在/proc/stat中采集2次数据2、cpu_usage=100-(idle2-idle1)*100/(sum2-sum1) |

监控CPU的性能一般包括以下3点：运行队列、CPU使用率和上下文切换。

对于每一个CPU来说运行队列最好不要超过3，例如，如果是双核CPU就不要超过6。如果队列长期保持在3以上，说明任何一个进程运行时都不能马上得到cpu的响应，这时可能需要考虑升级cpu。另外满负荷运行cpu的使用率最好是user空间保持在65%～70%，system空间保持在30%，空闲保持在0%~5%。

## 内核中的时间

HZ是系统时钟在一秒内固定发出时钟中断的次数。HZ在编译内核前是可以进行配置的，因此通过下述命令就可以查看当前系统的时钟中断频率：cat /boot/config-uname -r | grep CONFIG_HZ

tick为系统时钟每“滴答“一次的时间，其值为(1/HZ)秒。也就是连续两次时钟中断之间的时间间隔。

jiffies用来计算自系统启动以来tick的次数，也就是说系统时钟每产生一次时钟中断，该变量的值就增加一次。

 

## CPU时间组成

CPU的工作时间由三部分组成：用户态时间、系统态时间和空闲态时间。具体的组成为：

CPU时间包含User time、System time、Nice time、Idle time、Waiting time、Hardirq time、Softirq time、Steal time

### 空闲态时间

空闲态时间==idle time

### 用户态时间

用户态时间==user time+ Nice time。

### 内核态时间

内核态时间==system time+ Hardirq time+ Softirq time。

user time。指CPU在用户态执行进程的时间。

system time。指CPU在内核运行的时间。

nice time。指系统花费在调整进程优先级上的时间。

idle time。系统处于空闲期，等待进程运行。

waiting time。指CPU花费在等待I/O操作上的总时间，与ed相似。

steal time。指当前CPU被强制（involuntary wait）等待另外虚拟的CPU处理完毕时花费的时间，此时hypervisor在为另一个虚拟处理器服务。 

Softirq time、Hardirq time。分别对应系统在处理软硬中断时候所花费的CPU时间。

注：区分用户态和内核态时间，主要是为了在排查问题的时候确认是用户态进程还是内核导致的。

 

## CPU使用率

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A2A.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A3B.tmp.jpg) 

注：sy系统和ni&si软硬中断，基本系统自动控制，干涉部分不是太多.us，id，wa有一定的优化空间，有效的使用资源。

***\*CPU使用率主要包含以下几个方面：\****

用户CPU使用率，包括用户态CPU使用率（user）和低优先级用户态CPU使用率（nice），表示CPU在用户态运行的时间百分比。用户CPU使用率高，通常说明有应用程序比较繁忙。

系统CPU使用率，表示CPU在内核态运行的时间百分比（不包括中断）。系统 CPU 使用率高，说明内核比较繁忙。

等待I/O的CPU使用率，通常也称为iowait，表示等待I/O的时间百分比。iowait高，通常说明系统与硬件设备的I/O交互时间比较长。

软中断和硬中断的CPU使用率，分别表示内核调用软中断处理程序、硬中断处理程序的时间百分比。它们的使用率高，通常说明系统发生了大量的中断。

除在虚拟化环境中会用到的窃取CPU使用率（steal）和客户CPU使用率（guest），分别表示被其他虚拟机占用的CPU时间百分比，和运行客户虚拟机的CPU时间百分比。

 

### %usr

User mode CPU utilization

%usr。显示了用户方式下所花费CPU时间的百分比，用户使用CPU的进程包括：cpu运行常规用户进程，cpu运行niced process，cpu运行实时进程。一个进程可以在用户方式下执行，也可以在系统（内核）方式下执行，当一个进程在内核代码中运行时，我们称其处于内核态；当一个进程正在执行用户自己的代码时，我们称其处于用户态，在用户方式下执行时，进程在它自己的应用代码中执行，不需要内核资源来进行计算、管理内存或设置变量。

 

***\*问题原因：\****

用户空间CPU消耗，各种逻辑运算：

1、正在进行大量TPS

2、函数/排序/类型转化/逻辑IO访问（比如有时候使用STL函数不当导致耗费CPU）

用户空间消耗大量cpu，产生的系统调用是什么？那些函数使用了cpu周期？

可以使用perf工具抓取消耗CPU较多的函数，然后排查。

 

User mode CPU utilization+ System mode CPU utilization

***\*合理值\*******\*：\*******\*60-85%\****，如果在一个多用户系统中us+sy时间超过85%，则进程可能要花时间在运行队列中等待，响应时间和业务吞吐量会受损害；us过大，说明有用户进程占用很多cpu时间，需要进一步的分析其它软硬件因素；sy过大，说明系统管理方面花了很多时间，说明该系统中某个子系统产生了瓶颈，需要进一步分析其它软硬件因素。

 

### %sys

System mode CPU utilization

显示了系统方式下所花费cpu时间的百分比，包括内核进程（kprocs）和其他需要访问内核资源的进程所消耗的cpu资源，系统使用cpu的进程包括：用于系统调用，用于I/O管理（中断和驱动），用于内存管理（paging and swapping），用于进程管理（context switch and process start），如果一个进程需要内核资源，它必须执行一个系统调用，并由此切换到系统方式从而使该资源可用。

 

User mode CPU utilization+ System mode CPU utilization

合理值：***\*60-85%\****，如果在一个多用户系统中us+sy时间超过85%，则进程可能要花时间在运行队列中等待，响应时间和业务吞吐量会受损害；us过大，说明有用户进程占用很多cpu时间，需要进一步的分析其它软硬件因素；***\*sy过大，说明系统管理方面花了很多时间，说明该系统中某个子系统产生了瓶颈，需要进一步分析其它软硬件因素\****。

 

### %ni

用户进程空间内改变过优先级的进程占用CPU百分比

 

### %wa

显示了暂挂本地磁盘I/O和NFS加载的磁盘的cpu空闲百分比，是由于进程等待I/O而使cpu处于空闲状态的比率，I/O主要包括：I/O，raw I/O，VM-paging/swapins。如果在wait运行时至少有一个未完成的磁盘I/O，该事件就归为I/O等待时间，对磁盘的I/O请求会导致调用的进程阻塞（或睡眠），直到请求完成为止，一旦进程的I/O请求完成，该进程就放入运行队列中。如果I/O很快完成，该进程可以使用更多的cpu时间。

 

***\*问题原因：\****

用户和IO等待消耗了大部分cpu

1、吞吐量下降（tps）

2、查询响应时间增加

3、慢查询数增加

4、对mysql的并发陡增，也会产生上诉影响

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A3C.tmp.jpg) 

 

***\*参考值：\*******\*小于25%\****，超过25%的wa的值可以表示磁盘子系统可能没有被正确平衡，也可能是磁盘密集工作负载的结果，系统的磁盘或其它I/O可能有问题，可以通过iostat/sar -C命令进一步分解分析。

 

### %id

除了上面的WIO以外的空闲情况，显示了没有本地I/O时cpu空闲或等待的时间百分比。如果没有线程可以执行（运行队列为空），系统分派一个叫做wait的线程，可称为idle kproc。如果ps报告显示这个线程的总计时间较高，这表明存在时间段，其中没有其它线程准备在cpu上运行或等待执行。系统因此大部分时间空闲或等待新任务。

参考值：***\*大于40\****，如果r经常大于4，且id经常小于40，表示cpu的负荷很重。

 

### %hi

硬件中断

 

### %si

软件中断

 

### %st

steal值

 

### 平均负载

反应了系统的整体负载情况，可以查看过去1分钟、过去5分钟和过去15分钟的平均负载。

 

### r

​	r：runq-sz

运行进程队列的长度。对于可运行状态的进程个数的大小，这些进程在内存中准备就绪。

参考值：***\*小于4\****，队列大于4时，表明系统的cpu或内存可能有问题，如果r经常大于4，且id经常少于40，表示cpu的负荷很重。当队列变长时，队列中进程在等待cpu调度执行时所花的时间会变长。

# 性能瓶颈

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A3D.tmp.jpg) 

***\*判断cpu瓶颈的方法：\****

很慢的响应时间（slow response time）

CPU的空闲时间为零（zero percent idle cpu）

过高的用户占用cpu时间（high percent user cpu）

过高的系统占用cpu时间（high percent system cpu）

长时间的有很长的运行进程队列（large run queue size sustained over time）

 

线程的状态分析主要是分析线程的时间用在什么地方，而线程状态的分类一般分为：

a. on-CPU：执行中，执行中的时间通常又分为用户态时间user和系统态时间sys。

b. off-CPU：***\*等待下一轮上CPU，或者等待I/O、锁、换页\****等等，其状态可以细分为可执行、匿名换页、睡眠、锁、空闲等状态。

如果大量时间花在CPU上，对CPU的剖析能够迅速解释原因；如果系统时间大量处于off-cpu状态，定位问题就会费时很多。

 

***\*这里总结一下 CPU 使用率问题及排查思路：\****

1、用户CPU和Nice CPU高，说明用户态进程占用了较多的CPU，所以应该着重排查进程的性能问题。

2、系统CPU高，说明内核态占用了较多的CPU，所以应该着重排查内核线程或者系统调用的性能问题。

3、I/O等待CPU高，说明等待I/O的时间比较长，所以应该着重排查系统存储是不是出现了I/O问题。

4、软中断和硬中断高，说明软中断或硬中断的处理程序占用了较多的CPU，所以应该着重排查内核中的中断服务程序。

 

# 分析工具

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A3E.tmp.png)

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A4F.tmp.png)

 

# 性能统计

## CPU硬件信息

查看CPU硬件信息，使用lscpu、dmidecode命令。

### lscpu

### dmidecode

### bc

测试单cpu计算能力（bc）：

[root@localhost] $ time echo "scale=5000;4*a(1)" | bc -l -q

 

## 平均负载

### uptime

展示最近一段时间的平均负载（5、10、15分钟性能展示）。

注：top中也会显示uptime的内容。

uptime的输出包含一项内容是load average，显示了最近1,5,15分钟的负荷情况。它的值代表等待CPU处理的进程数，如果CPU没有时间处理这些进程，load average值会升高，反之则会降低。

load average的最佳值1，说明每个进程都可以马上处理并且没有CPU cycles被丢失。对于单CPU的机器，1或者2是可以接受的值；对于多路CPU的机器，load average值可能在8到10之间。

也可以用uptime命令来判断网络性能。例如，某个网络应用性能很低，通过运行uptime查看服务器的负荷是否很高，如果不是，那么问题应该是网络方面造成的。

### top

## CPU使用率

计算CPU的负载状况：uptime、sar、vmstat查看整体CPU负载，mpstat、dstat、bc可以查看整体或某个CPU的负载。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A5F.tmp.png)

说明：

uptime，vmstat，mpstat，top，pidstat只能查询到cpu及负载的的使用情况。

perf可以跟着到进程内部具体函数耗时情况，并且可以指定内核函数进行统计，指哪打哪。

使用方式：

//查看系统cpu使用情况top

//查看所有cpu核信息mpstat -P ALL 1

//查看cpu使用情况以及平均负载vmstat 1

//进程cpu的统计信息pidstat -u 1 -p pid

//跟踪进程内部函数级cpu使用情况perf top -p pid -e cpu-clock

 

top、vmstat和mpstat只可以动态查看当前，而sar可以查看历史。

### sar

sar是System Activity Reporter的缩写，系统活动状态报告，可以获取CPU、内存、swap、进程、网络、IO的基本情况。

使用sar -u查看CPU状况。

​	sar命令语法和vmstat一样。

​	例如每1秒采集一次CPU使用率，共采集5次。

[root@#### ~]# sar -u 1 5

Linux 3.18.5-x86_64-linode52 (li676-235) 01/20/2020 _x86_64_ (1 CPU)

02:41:25 PM CPU %user %nice %system %iowait %steal %idle

02:41:26 PM all 64.71 0.00 18.63 0.00 0.98 15.69

02:41:27 PM all 29.47 0.00 22.11 14.74 0.00 33.68

02:41:28 PM all 67.33 0.00 31.68 0.99 0.00 0.00

02:41:29 PM all 7.00 0.00 2.00 0.00 0.00 91.00

02:41:30 PM all 69.00 0.00 23.00 0.00 0.00 8.00

Average: all 47.79 0.00 19.48 3.01 0.20 29.52

***\*拓展：\****

sar -u 5 5	//CPU的使用情况和IOWAIT信息（同默认监控）

sar -P ALL 5 5	//每颗CPU使用状态信息和IOWAIT统计状态

sar -q 5 5	//队列的长度（等待运行的进程数）和负载的状态

sar -v 5 5	//inode,file and other kernel tablesd的状态信息

sar -w 5 5	//每秒上下文切换的数目

sar -r 5 5	//内存和swap空间使用情况（与perf结合，先使用这个命令判断CPU忙，然后perf确认具体函数）

sar -R 5 5 //内存的统计信息（内存页的分配和释放、系统每秒作为buffer使用内存页、每秒被cache到的内存页）

sar -W 5 5 //swap交换的统计信息（监控状态同iostat的si so）

sar -B 5 5 //页交换速率

sar -c 5 5 //进程创建的速率

sar -x 2906 5 5 //显示指定进程的统计信息，包括进程造成的错误、用户和系统级用户CPU占用情况，运行在哪颗CPU上

sar -n DEV 5 5 //网络设备的状态信息

sar -n SOCK5 5 //socket使用情况

sar -n ALL //所有网络状态信息

sar -b 5 5 //IO传送速率

sar -d 5 5	//块设备的活跃信息

### vmstat	

使用vmstat可以查看CPU、内存、swap、进程、IO的基本情况。

注：这些信息主要来自于/proc/slabinfo。

[root@li676-235 ~]# vmstat 1 5

procs -----------memory---------- ---swap-- -----io---- --system-- -----cpu-----

r b swpd free buff cache si so bi bo in cs us sy id wa st

2 0 34792 68376 147688 356132 16 16 421 385 2 2 5 4 91 1 0

3 0 34792 97368 147688 326884 0 0 0 0 133 134 15 6 79 0 0

1 0 34792 62432 147696 360704 0 0 0 88 267 278 63 18 19 0 0

0 0 34792 97160 147696 326904 0 0 0 0 71 82 12 4 84 0 0

1 0 34792 56572 147696 364540 0 0 0 4 261 246 61 23 16 0 0

注：这些信息来自/proc/slabinfo

***\*参数：\****

Procs（进程）：

r表示运行队列的大小

b表示由于IO等待而的线程数量

Memory（内存）：

swapd：使用虚拟内存大小

free：可用内存大小

buff：用作缓冲的内存大小

cache：用作缓存的内存大小

Swap：

si：每秒从交换区写到内存的大小

so：每秒写入交换区的内存大小

IO（现在的Linux版本块大小为1024bytes）：

bi：每秒读取的块数

bo：每秒写入的块数

CPU：

in表示中断的数量

cs表示上下文切换的数量。

### dstat

常用命令：

dstat -tcdrmnsgy

说明：可以观察CPU状态、磁盘、网络、分页以及系统信息。

| 分组     | 分组含义及子项字段含义                                       |
| -------- | ------------------------------------------------------------ |
| CPU状态  | CPU的使用率。显示了用户占比、系统占比、空闲占比、等待占比、硬中断和软中断情况。 |
| 磁盘统计 | 磁盘的读写，分别显示磁盘的读、写总数。                       |
| 网络统计 | 网络设备发送和接收的数据，分别显示网络收、发数据总数。       |
| 分页统计 | 系统的分页活动。分别显示换入（in）和换出（out）。            |
| 系统统计 | 统计中断（int）和上下文切换（csw）。                         |

***\*参数：\****

t：时间戳

c：CPU使用情况

d：disk读写量

r：IO请求统计，包括read requests,write requests

m：内存统计

n：网络流量统计

s：swap统计

p：分页统计

y：系统统计

 

### mpstat

这个命令也在sysstat包中，语法类似。

mpstat是Multiprocessor Statistics的缩写，是实时系统监控工具。其报告与CPU的一些统计信息，这些信息存放在/proc/stat文件中。***\*在多CPU系统里，其不但能查看所有CPU的平均状况信息，而且能够查看特定CPU的信息\****。

mpstat最大的特点是：可以查看多核心cpu中每个计算核心的统计数据（查看所有CPU核运行状况）；而类似工具vmstat只能查看系统整体cpu情况。

例如每1秒收集一次，共5次。

[root@#### ~]# mpstat 1 5

Linux 3.18.5-x86_64-linode52 (li676-235) 07/20/2015 _x86_64_ (1 CPU)

03:01:18 PM CPU %usr %nice %sys %iowait %irq %soft %steal %guest %idle

03:01:19 PM all 52.53 0.00 23.23 0.00 0.00 0.00 0.00 0.00 24.24

03:01:20 PM all 21.00 0.00 4.00 0.00 0.00 0.00 0.00 0.00 75.00

03:01:21 PM all 53.00 0.00 18.00 0.00 0.00 0.00 0.00 0.00 29.00

03:01:22 PM all 26.00 0.00 3.00 0.00 0.00 0.00 0.00 0.00 71.00

03:01:23 PM all 46.00 0.00 18.00 0.00 0.00 0.00 0.00 0.00 36.00

Average: all 39.68 0.00 13.23 0.00 0.00 0.00 0.00 0.00 47.09

cpu使用情况比sar更加详细些，也可以用-P指定某颗cpu 。

 

$ mpstat -P ALL

Linux 3.10.0-229.el7.x86_64 (localhost.localdomain) 05/30/2018  _x86_64_  (16 CPU)

04:03:55 PM  CPU %usr  %nice  %sys %iowait  %irq  %soft  %steal  %guest  %gnice  %idle

04:03:55 PM  all  3.67  0.00  0.61  0.71  0.00  0.00  0.00  0.00  0.00  95.02

04:03:55 PM  0  3.52  0.00  0.57  0.76  0.00  0.00  0.00  0.00  0.00  95.15

04:03:55 PM  1  3.83  0.00  0.61  0.71  0.00  0.00  0.00  0.00  0.00  94.85

04:03:55 PM  2  3.80  0.00  0.61  0.60  0.00  0.00  0.00  0.00  0.00  94.99

04:03:55 PM  3  3.68  0.00  0.58  0.60  0.00  0.00  0.00  0.00  0.00  95.13

04:03:55 PM  4  3.54  0.00  0.57  0.60  0.00  0.00  0.00  0.00  0.00  95.30

[...]

该命令用于每秒打印一次每个CPU的统计信息，可用于查看CPU的调度是否均匀。

 

### top

​	数据来自/proc/stat文件

第三行有显示CPU当前的使用情况：

[root@li676-235 ~]# top -bn 1 -i -c

top - 14:19:51 up 138 days, 7:15, 1 user, load average: 0.20, 0.33, 0.39

Tasks: 115 total, 1 running, 114 sleeping, 0 stopped, 0 zombie

Cpu(s): 4.5%us, 3.8%sy, 0.0%ni, 91.0%id, 0.6%wa, 0.0%hi, 0.0%si, 0.0%st

Mem: 1014660k total, 880512k used, 134148k free, 264904k buffers

Swap: 262140k total, 34788k used, 227352k free, 217144k cached

PID USER PR NI VIRT RES SHR S %CPU %MEM TIME+ COMMAND 

12760 root 20 0 15084 1944 1632 R 2.0 0.2 0:00.01 top -bn 1 -i –c

注：

%us =(User time + Nice time)/CPU时间*100%

%sy=(System time + Hardirq time +Softirq time)/ CPU时间*100%

%id=(Idle time)/CPU时间*100%

%ni=(Nice time)/CPU时间*100%

%wa=(Waiting time)/CPU时间*100%

%hi=(Hardirq time)/CPU时间*100%

%si=(Softirq time)/CPU时间*100%

%st=(Steal time)/CPU时间*100%

备注： top命令默认情况下，是每3秒刷新一次。也可以通过top -d <刷新时间间隔> 来指定刷新频率，如top -d 0.1或top -d 0.01等。top执行时，也可以按“s”键，修改时间间隔。 

 

​	如上所示，top命令可以看到总体的系统运行状态和cpu的使用率 。

**%us**：表示用户空间程序的cpu使用率（没有通过nice调度）

**%sy**：表示系统空间的cpu使用率，主要是内核程序。

**%ni**：表示用户空间且通过nice调度过的程序的cpu使用率。

 

注：User mode CPU utilization+ System mode CPU utilization

合理值:60-85%，如果在一个多用户系统中us+sy时间超过85%，则进程可能要花时间在运行队列中等待，响应时间和业务吞吐量会受损害；us过大，说明有用户进程占用很多cpu时间，需要进一步的分析其它软硬件因素；sy过大，说明系统管理方面花了很多时间，说明该系统中某个子系统产生了瓶颈，需要进一步分析其它软硬件因素。

 

**%id**：空闲cpu

参考值：大于40，如果r经常大于4，且id经常小于40，表示cpu的负荷很重

 

**%wa**：cpu运行时在等待io的时间

参考值：小于25%，超过25%的wa的值可以表示子系统可能没有被正确平衡，也可能是磁盘密集工作负载的结果，系统的磁盘或其它I/o可能有问题，可以通过iostat/sar -C命令进一步分解分析

 

**r**

参考值：小于4，队列大于4时，表明系统的cpu或内存可能有问题，如果r经常大于4，且id经常少于40，表示cpu的负荷很重。当队列变长时，队列中进程在等待cpu调度执行时所花的时间会变长

 

**%hi**：cpu处理硬中断的数量

**%si**：cpu处理软中断的数量

**%st**：被虚拟机偷走的cpu

 

### /proc

/proc/stat是其他性能工具的数据来源。

查看CPU的实时运行状况，一般是在/proc目录下，后面查看CPU负载或者进程占用CPU的情况都是基于该目录下的数据。

| /proc/cpuinfo               | CPU的基本信息            |
| --------------------------- | ------------------------ |
| /proc/state                 | 所有CPU活动的信息        |
| /proc/<pid>/stat            | 某一进程所有的活动的信息 |
| /proc/<pid>/task/<tid>/stat | 某一进程所有的活动的信息 |

 

#### /proc/cpuinfo

#### /proc/state

该文件包含了所有CPU活动的信息，该文件中的所有值都是从系统启动开始累计到当前时刻。不同内核版本中该文件的格式可能不大一致，以下通过实例来说明数据该文件中各字段的含义。

参数      解析（单位：jiffies）

user (38082)  从系统启动开始到当前时刻，处于用户态运行时间，不包含 nice值为负进程。

nice (627)   从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间

system (27594)  从系统启动开始累计到当前时刻，处于核心态的运行时间

idle (893908)  从系统启动开始累计到当前时刻，除IO等待时间以外的其它等待时间iowait (12256) 从系统启动开始累计到当前时刻，IO等待时间

irq (581)   从系统启动开始累计到当前时刻，硬中断时间

softirq (895)  从系统启动开始累计到当前时刻，软中断时间

***\*注：\****jiffies是内核中的一个全局变量，用来记录自系统启动一来产生的节拍数，在linux中，一个节拍大致可理解为操作系统进程调度的最小时间片，不同linux内核可能值有不同，通常在1ms到10ms之间。

结论2：总的cpu时间totalCpuTime = user + nice + system + idle + iowait + irq + softirq + stealstolen + guest

 

除了系统负载、上下文切换信息，最直观的CPU问题指标就是CPU使用率信息。Linux 通过/proc虚拟文件系统向用户控件提供系统内部状态信息，其中/proc/stat则是 CPU 和任务信息统计。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A60.tmp.jpg) 

这里每一列的含义如下：

user（通常缩写为 us），代表用户态 CPU 时间。注意，它不包括下面的 nice 时间，但包括了 guest 时间。

nice（通常缩写为 ni），代表低优先级用户态 CPU 时间，也就是进程的 nice 值被调整为 1-19 之间时的 CPU 时间。这里注意，nice 可取值范围是 -20 到 19，数值越大，优先级反而越低。

system（通常缩写为sys），代表内核态 CPU 时间。idle（通常缩写为 id），代表空闲时间。注意，它不包括等待 I/O 的时间（iowait）。

iowait（通常缩写为 wa），代表等待 I/O 的 CPU 时间。irq（通常缩写为 hi），代表处理硬中断的 CPU 时间。

softirq（通常缩写为 si），代表处理软中断的 CPU 时间。

steal（通常缩写为 st），代表当系统运行在虚拟机中的时候，被其他虚拟机占用的 CPU 时间。

guest（通常缩写为 guest），代表通过虚拟化运行其他操作系统的时间，也就是运行虚拟机的 CPU 时间。

guest_nice（通常缩写为 gnice），代表以低优先级运行虚拟机的时间。

这里我们可以使用top、ps、pidstat等工具方便的查询这些数据，可以很方便的看到 CPU 使用率很高的进程，这里我们可以通过这些工具初步定为，但是具体的问题原因还需要其他方法继续查找。

这里我们可以使用perf top方便查看热点数据，也可以使用perf record可以将当前数据保存起来方便后续使用perf report查看。

 

#### /proc/<pid>/stat                      

该文件包含了某一进程所有的活动的信息，该文件中的所有值都是从系统启动开始累计

到当前时刻。以下通过实例数据来说明该文件中各字段的含义。

[root@datacenter~]# cat /proc/6873/stat

6873 (a.out) R 6723 6873 6723 34819 6873 8388608 77 0 0 0 41958 31 0 0 25 0 3 0 5882654 1409024 56 4294967295 134512640 134513720 3215579040 0 2097798 0 0 0 0 0 0 0 17 0 0 0

说明：以下只解释对我们计算Cpu使用率有用相关参数

参数           解释

pid=6873         进程号

utime=1587     该任务在用户态运行的时间，单位为jiffies

stime=41958    该任务在核心态运行的时间，单位为jiffies

cutime=0      所有已死线程在用户态运行的时间，单位为jiffies

cstime=0      所有已死在核心态运行的时间，单位为jiffies

结论3：进程的总Cpu时间processCpuTime = utime + stime + cutime + cstime，该值包括其所有线程的cpu时间。

#### /proc/<pid>/task/<tid>/stat

该文件包含了某一进程所有的活动的信息，该文件中的所有值都是从系统启动开始累计到当前时刻。该文件的内容格式以及各字段的含义同/proc/<pid>/stat文件。

注意，该文件中的tid字段表示的不再是进程号，而是linux中的轻量级进程(lwp)，即我们通常所说的线程。

 

## 进程CPU使用率

查看进程占用CPU的命令：top、iostat、ps、perf、pidstat。

### top

查看程序的线程运行情况：top -H -p 进程号

 

### pidstat

pidstat是sysstat工具的一个命令，用于监控全部或指定进程的cpu、内存、线程、设备IO等系统资源的占用情况。pidstat的优势在于，可以滚动的打印进程运行情况，而不像top那样会清屏。

pidstat首次运行时显示自系统启动开始的各项统计信息，之后运行pidstat将显示自上次运行该命令以后的统计信息。用户可以通过指定统计的次数和时间来获得所需的统计信息。

pidstat [ 选项 ] [ <时间间隔> ] [ <次数> ]

常用的参数：

-u：默认的参数，显示各个进程的cpu使用统计

-r：显示各个进程的内存使用统计

-d：显示各个进程的IO使用情况

-p：指定进程号

-w：显示每个进程的上下文切换情况

-t：显示选择任务的线程的统计信息外的额外信息

-T { TASK | CHILD | ALL }

这个选项指定了pidstat监控的。TASK表示报告独立的task，CHILD关键字表示报告进程下所有线程统计信息。ALL表示报告独立的task和task下面的所有线程。

***\*注意：\****task和子线程的全局的统计信息和pidstat选项无关。这些统计信息不会对应到当前的统计间隔，这些统计信息只有在子线程kill或者完成的时候才会被收集。

-V：版本号

-h：在一行上显示了所有活动，这样其他程序可以容易解析。

-I：在SMP环境，表示任务的CPU使用率/内核数量

-l：显示命令名和所有参数

### ps

top和ps可以以排序方式展示进程CPU，pidstat不可排序展示。

### htop

top增强版。

### atop

htop和atop则以不同颜色展示各种类型更直观。

## 系统上下文切换

### vmstat

展示上下文切换此时、运行状态、不可中断状态进程数量。

 

## 进程上下文切换

### sar

### pidstat

展示项很多，包括进程上下文切换信息。

## 软中断

### top

查看软中断CPU利用率。

### /proc/softirqs

/proc/softirqs和mpstat查看每个CPU上的累积信息。

### mpstat

## 硬中断

### vmstate

查看总的中断次数信息。

### /proc/interrupts

查看各种中断在每个CPU核心上的累积信息。

## 网络

dstat和sar详细展示出总的网络收发状况。

### dstat

每秒cpu使用率情况获取

[root@#### ~]# dstat -c

----total-cpu-usage----

usr sys idl wai hiq siq

 5 4 91 1 0 0

 27 11 62 0 0 0

 52 11 37 0 0 0

 26 10 63 0 0 1

 50 13 37 0 0 1

 21 6 72 0 0 0

最占cpu的进程获取

 

[root@#### ~]# dstat --top-cpu

-most-expensive-

 cpu process 

mysqld 1.5

php-fpm: pool 24

mysqld 59

 

### sar

### tcpdump

## IO

查看IO整体状况：

### dstat

常用命令：dstat -tcdrmnsgy

参数：

t：时间戳

c：CPU使用情况

d：disk读写量

r：io请求统计，包括read requests,write requests

m：内存统计

n：网络流量统计

s：swap统计

p：分页统计

### sar

### iostat

这个命令主要用来查看io使用情况，也可以来查看cpu。

[root@#### ~]# iostat -c 1 2

Linux 3.18.5-x86_64-linode52 (li676-235) 07/20/2015 _x86_64_ (1 CPU)

avg-cpu: %user %nice %system %iowait %steal %idle

 4.53 0.01 3.81 0.63 0.04 90.99

avg-cpu: %user %nice %system %iowait %steal %idle

 38.00 0.00 14.00 0.00 0.00 48.00

注：-c表示查看CPU，-d表示查看disk磁盘。

 

***\*说明：\****

1、如果%iowait的值过高，表示硬盘存在I/O瓶颈

2、如果%idle值过高，表示CPU较空闲

3、如果%idle值高但系统响应慢时，可能是CPU等待分配内存，应加大内存容量

4、如果%idle值持续低于10，表示CPU处理能力相对较低，系统中最需要解决的资源是CPU

注意：iostat有一个弱点，就是不能对某个进程进行深入分析，仅对系统的整体情况进行分析。

 

## 系统分析

### perf

***\*Perf分析各种内核函数调用，热点函数信息\****。

perf是Linux kernel自带的系统性能优化工具。优势在于与Linux Kernel的紧密结合，它可以最先应用到加入Kernel的new feature，用于查看热点函数，查看cashe miss的比率，从而帮助开发者来优化程序性能。

性能调优工具如perf，Oprofile等的基本原理都是对被监测对象进行采样（基于事件采样原理），最简单的情形是根据tick中断进行采样，即在tick中断内触发采样点，在采样点里判断程序当时的上下文。假如一个程序90%的时间都花费在函数foo()上，那么90%的采样点都应该落在函数foo()的上下文中。运气不可捉摸，但我想只要采样频率足够高，采样时间足够长，那么以上推论就比较可靠。因此，通过tick触发采样，我们便可以了解程序中哪些地方最耗时间，从而重点分析。

***\*命令：\****

perf record -a -g -p pid

perf report -i perf.data

perf record -F 99 -g -p 1234 -o my.data --sleep 3

参数：

-F 指定从CPU采样频率（每秒采样99次）

-g 记录调用堆栈信息

-p 记录哪个进程的数据

-o 指定结果文件的名字，不指定默认文件名perf.data

--sleep 3 记录3秒程序数据

 

Perf生成火焰图：

1、通过perf record记录采样数据

perf record -g -p 1234 -o my.data --sleep 3

2、生成未折叠的堆栈信息

perf script -i my.data &> my.unfold

3、生成折叠的堆栈信息

FlameGraph-master/stackcollapse-perf.pl my.unfold &> my.fold

4、生成火焰图

FlameGraph-master/flamegraph.pl my.fold > result.svg

从上到下是函数的调用堆栈，相同的调用堆栈被合并，横向越宽表示占用CPU时间越多，也就是在总采样数中占比最多，纵向表示函数的调用堆栈。

5、火焰差分图

单独的火焰图只能记录一次，如果需要比对不同版本之间区别，可以使用火焰差分图。

***\*例如：比较版本A1 A2的性能指标：\****

1）使用perf record记录A1 A2的抽样结果

perf record -g -p 1234 -o A1.data --sleep 3

perf record -g -p 1234 -o A2.data --sleep 3

2）生成未折叠堆栈

perf script -i A1.data &> A1.unfold

perf script -i A2.data &> A2.unfold

3）生成折叠堆栈

FlameGraph-master/stackcollapse-perf.pl A1.unfold &> A1.fold

FlameGraph-master/stackcollapse-perf.pl A2.unfold &> A2.fold

4）生成差分图

difffolded.pl A1.fold A2.fold > diff.svg

FlameGraph-master/difffolded.pl -n A1.fold | FlameGraph-master/flamegraph.pl -negate > diff.svg

注意：在生成火焰图时需要保证可执行文件与跑的对应，否则可能获取不到调用堆栈关系。

 

### execsnoop

***\*监控短时进程\****。

 

想要更深的了解本工具可以参考：

http://blog.csdn.net/trochiluses/article/details/10261339

 

# 故障分析

系统平均负载升高的原因一般来说，系统平均负载升高意味着CPU使用率上升。但是他们没有必然联系，CPU密集型计算任务较多一般系统平均负载会上升，但是如果IO密集型任务较多也会导致系统平均负载升高但是此时的CPU使用率不一定高，可能很低因为很多进程都处于不可中断状态，等待CPU调度也会升高系统平均负载。

所以假如我们系统平均负载很高，但是CPU使用率不是很高，则需要考虑是否系统遇到了IO瓶颈，应该优化IO读写速度。

所以系统是否遇到CPU瓶颈需要结合CPU使用率，系统瓶颈负载一起查看（当然还有其他指标需要对比查看，下面继续讲解）

## CPU问题排查总体方向

有了以上性能工具，在实际遇到问题时我们并不可能全部性能工具跑一遍，这样效率也太低了，所以这里可以先运行几个常用的工具top、vmstat、pidstat分析系统大概的运行情况然后在具体定位原因。

top系统CPU => vmstat上下文切换次数=> pidstat非自愿上下文切换次数=>各类进程分析工具(perf strace ps execsnoop pstack)

top用户CPU => pidstat用户CPU =>一般是CPU计算型任务

top僵尸进程=> 各类进程分析工具(perf strace ps execsnoop pstack)

top平均负载=> vmstat 运行状态进程数=>pidstat用户CPU =>各类进程分析工具(perf strace ps execsnoop pstack)

top等待IO CPU => vmstat不可中断状态进程数=> IO分析工具(dstat、sar -d)

top硬中断=> vmstat中断次数=>查看具体中断类型(/proc/interrupts)

top软中断=>查看具体中断类型(/proc/softirqs) =>网络分析工具(sar -n、tcpdump) 或者 SCHE

 

## 系统负载**案例问题排查

参考：

[https://mp.weixin.qq.com/s?__biz=MzA4Nzg5Nzc5OA==&mid=2651678526&idx=1&sn=ea50bdf1764cc208aec3bbc6d163aea2&chksm=8bcba497bcbc2d8115fbf38e97f9b018bb158e5102d2f5f4190ccac22c0a353d08d9cfb11191&mpshare=1&scene=2&srcid=&from=timeline&clicktime=1562910386#rd](#rd)

### 概述

负载就是cpu在一段时间内正在处理以及等待cpu处理的***\*进程数之和\****的统计信息，也就是cpu使用队列的长度统计信息，这个数字越小越好（如果超过CPU核心*0.7就是不正常）

### 分类

负载分为两大部分：CPU负载、IO负载

#### CPU负载

例如，假设有一个进行大规模科学计算的程序，虽然该程序不会频繁地从磁盘输入输出，但是处理完成需要相当长的时间。因为该程序主要被用来做计算、逻辑判断等处理，所以程序的处理速度主要依赖于cpu的计算速度。此类cpu负载的程序称为“计算密集型程序”。

#### IO负载

还有一类程序，主要从磁盘保存的大量数据中搜索找出任意文件。这个搜索程序的处理速度并不依赖于cpu，而是依赖于磁盘的读取速度，也就是输入输出（input/output,I/O）.磁盘越快，检索花费的时间就越短。此类I/O负载的程序，称为“I/O密集型程序”。

需要运行处理但又必须等待队列前的进程处理完成的进程个数。具体来说，也就是如下两种情况：

等待被授权予CPU运行权限的进程

等待磁盘I/O完成的进程

### 工具

stress是一个施加系统压力和压力测试系统的工具，我们可以使用stress工具压测试 CPU，以便方便我们定位和排查CPU问题。

yum install stress // 安装stress工具

 

stress命令使用 

// --cpu 8：8个进程不停的执行sqrt()计算操作

​	// --io 4：4个进程不同的执行sync()io操作（刷盘）

​	// --vm 2：2个进程不停的执行malloc()内存申请操作

​	// --vm-bytes 128M：限制1个执行malloc的进程申请内存大小

​	stress --cpu 8 --io 4 --vm 2 --vm-bytes 128M --timeout 10s

我们这里主要验证 CPU、IO、进程数过多的问题：

### 排查方法

#### CPU问题排查

使用stress -c 1模拟CPU高负载情况，然后使用如下命令观察负载变化情况：

uptime：使用uptime查看此时系统负载：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A71.tmp.jpg) 

mpstat：使用mpstat -P ALL 1则可以查看每一秒的CPU每一核变化信息，整体和top类似，好处是可以把每一秒（自定义）的数据输出方便观察数据的变化，最终输出平均数据：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A72.tmp.jpg) 

由以上输出可以得出结论，当前系统负载升高，并且其中 1 个核跑满主要在执行用户态任务，此时大多数属于业务工作。所以此时需要查哪个进程导致单核 CPU 跑满：

pidstat：使用pidstat -u 1则是每隔 1 秒输出当前系统进程、CPU 数据：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A73.tmp.jpg) 

top：当然最方便的还是使用top命令查看负载情况：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A84.tmp.jpg) 

此时可以看到是stress占用了很高的CPU。

 

#### CPU缓存命中率

CPU的访问速度远大于内存访问，这样在CPU访问内存时不可避免的要等待内存响应。为了协调2者的速度差距出现了CPU缓存（多级缓存）。如果CPU缓存命中率越高则性能会更好，我们可以使用以下工具查看CPU缓存命中率，工具地址、项目地址perf-tools

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A85.tmp.jpg) 

 

#### IO问题排查

我们使用stress -i 1来模拟IO瓶颈问题，即死循环执行sync刷盘操作：

uptime：使用uptime查看此时系统负载：

$ watch -d uptime

load average: 1.06, 0.58, 0.37

mpstat：查看此时 IO 消耗，但是实际上我们发现这里 CPU 基本都消耗在了 sys 即系统消耗上。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A86.tmp.jpg) 

IO无法升高的问题：iowait 无法升高的问题，是因为案例中stress使用的是sync()系统调用，它的作用是刷新缓冲区内存到磁盘中。

对于新安装的虚拟机，缓冲区可能比较小，无法产生大的IO压力，这样大部分就都是系统调用的消耗了。所以，你会看到只有系统CPU使用率升高。

解决方法是使用stress的下一代stress-ng，它支持更丰富的选项，比如stress-ng -i 1 --hdd 1 --timeout 600（--hdd 表示读写临时文件）。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A87.tmp.jpg) 

可以看出CPU的IO升高导致系统平均负载升高。我们使用pidstat查找具体是哪个进程导致IO升高的。

top：这里使用top依旧是最方面的查看综合参数，可以得出stress是导致IO升高的元凶。

pidstat没有iowait选项：可能是CentOS 默认的sysstat太老导致，需要升级到11.5.5之后的版本才可用。

 

#### 进程数过多问题排查

进程数过多这个问题比较特殊，如果系统运行了很多进程超出了CPU运行能，就会出现等待CPU的进程。 

使用stress -c 24来模拟执行24个进程（我的CPU是8核） 

uptime：使用uptime查看此时系统负载：

$ watch -d uptime

...,  load average: 18.50, 7.13, 2.84

mpstat：同上（略）

pidstat：同上（略）

可以观察到此时的系统处理严重过载的状态，平均负载高达 18.50。top：我们还可以使用top命令查看此时Running状态的进程数，这个数量很多就表示系统正在运行、等待运行的进程过多。

### 总结

通过以上问题现象及解决思路可以总结出：

1、平均负载高有可能是 CPU 密集型进程导致的

2、平均负载高并不一定代表 CPU 使用率高，还有可能是 I/O 更繁忙了

3、当发现负载高的时候，你可以使用 mpstat、pidstat 等工具，辅助分析负载的来源

总结工具：mpstat、pidstat、top和uptime

## IO负载高CPU低负载高

cpu低而负载高也就是说等待磁盘I/O完成的进程过多，就会导致队列长度过大，这样就体现到负载过大了，但实际是此时cpu被分配去执行别的任务或空闲，具体场景有如下几种。

***\*场景一：磁盘读写请求过多就会导致大量I/O等待\****

cpu的工作效率要高于磁盘，而进程在cpu上面运行需要访问磁盘文件，这个时候cpu会向内核发起调用文件的请求，让内核去磁盘取文件，这个时候会切换到其他进程或者空闲，这个任务就会转换为不可中断睡眠状态。当这种读写请求过多就会导致不可中断睡眠状态的进程过多，从而导致负载高，cpu低的情况。

 

***\*场景二：MySQL中存在没有索引的语句或存在死锁等情况\****

我们都知道MySQL的数据是存储在硬盘中，如果需要进行sql查询，需要先把数据从磁盘加载到内存中。当在数据特别大的时候，如果执行的sql语句没有索引，就会造成扫描表的行数过大导致I/O阻塞，或者是语句中存在死锁，也会造成I/O阻塞，从而导致不可中断睡眠进程过多，导致负载过大。

具体解决方法可以在MySQL中运行show full processlist命令查看线程等待情况，把其中的语句拿出来进行优化。

 

***\*场景三：外接硬盘故障，常见有挂了NFS，但是NFS server故障\****

比如我们的系统挂载了外接硬盘如NFS共享存储，经常会有大量的读写请求去访问NFS存储的文件，如果这个时候NFS Server故障，那么就会导致进程读写请求一直获取不到资源，从而进程一直是不可中断状态，造成负载很高。

## 最耗CPU的线程/CPU 100%

排查思路：

1、找到最耗CPU的进程

工具：top

方法：

执行***\*top -c\****，显示进程运行信息列表

键入P (大写p)，进程按照CPU使用率排序

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A97.tmp.jpg) 

2、找到最耗CPU的线程

工具：top

方法：

***\*top -Hp 10765\****，显示一个进程的线程运行信息列表

键入P (大写p)，线程按照CPU使用率排序

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A98.tmp.jpg) 

3、将线程PID转化为16进制

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1A99.tmp.jpg) 

4、查看堆栈，找到线程在干嘛

工具：***\*pstack/jstack/grep\****

方法：***\*jstack 10765 | grep ‘0x2a34’ -C5 --color\****

打印进程堆栈

通过线程id，过滤得到线程堆栈

注：Java程序使用jstack查看堆栈，C++程序使用pstack，如果想要确定哪个函数耗费CPU，可以使用perf工具。

 

准备一行死循环代码：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AAA.tmp.jpg) 

### windows版

可能有人有疑问，我为什么要说windows版的！因为，我曾经给很多政府部门做过系统。我发现他们用的是windows server，不是linux系统。所有必要说一下！

**查消耗cpu最高的进程PID**

以win 10为例，截图给大家看一下，先调出PID显示项！

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AAB.tmp.jpg) 

然后发现进程PID如下图所示，为10856

|      |                                                              |
| ---- | ------------------------------------------------------------ |
|      | ![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AAC.tmp.jpg) |





接下来呢？

**根据PID查出消耗cpu最高的线程号**

这里用到微软的工具Process Explorer v16.22，地址如下

https://docs.microsoft.com/zh-cn/sysinternals/downloads/process-explorer

如图所示：



|      |                                                              |
| ---- | ------------------------------------------------------------ |
|      | ![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AAD.tmp.jpg) |

 



发现最耗cpu的线程的TId为6616

这是十进制的数据，转成十六进制为19d8

 

**根据线程号查出对应的java线程，进行处理**

执行命令,导出进程快照

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AAE.tmp.jpg) 

打开文件 c:/10856.stack，搜索19d8，如下图所示

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1ABE.tmp.jpg) 

根据文件就可以看出，我们的TestFor.java文件第七行一直在跑，至此定位到问题

### Linux版

Linux版本，步骤是一模一样的，就是命令换了一下

**1、****查消耗cpu最高的进程PID**

执行命令

执行***\*top -c\****，显示进程运行信息列表。按下P，进程按照cpu使用率排序

如下图所示，PID为3033的进程耗费cpu最高：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1ABF.tmp.jpg) 

**2、****根据PID查出消耗cpu最高的线程号**

执行命令

***\*top -Hp 3033\****，显示一个进程的线程运行信息列表。按下P，进程按照cpu使用率排序

如下图所示，PID为3034的线程耗费cpu最高：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AC0.tmp.jpg) 

这是十进制的数据，转成十六进制为0xbda

 

**3、****根据线程号查出对应的java线程，进行处理**

执行命令，导出进程快照

jstack -l 3033 > ./3033.stack

然后执行，grep命令，看线程0xbda做了什么

cat 3033.stack |grep 'bda' -C 8

输出如下：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AC1.tmp.jpg) 

至此定位到问题。

注：也可以使用perf定位耗时的函数。

 

 

## CPU上下文切换

CPU 上下文：CPU执行每个任务都需要知道任务从哪里加载、又从哪里开始运行，也就是说，需要系统事先帮它设置好CPU寄存器和程序计数器（Program Counter，PC）包括CPU寄存器在内都被称为CPU上下文。

CPU 上下文切换：CPU上下文切换，就是先把前一个任务的CPU上下文（也就是 CPU 寄存器和程序计数器）保存起来，然后加载新任务的上下文到这些寄存器和程序计数器，最后再跳转到程序计数器所指的新位置，运行新任务。

CPU上下文切换：分为进程上下文切换、线程上下文切换以及中断上下文切换。

 

### 进程上下文切换

从用户态切换到内核态需要通过系统调用来完成，这里就会发生进程上下文切换（特权模式切换），当切换回用户态同样发生上下文切换。

一般每次上下文切换都需要几十纳秒到数微秒的 CPU 时间，如果切换较多还是很容易导致 CPU 时间的浪费在寄存器、内核栈以及虚拟内存等资源的保存和恢复上，这里同样会导致系统平均负载升高。

Linux为每个CPU维护一个就绪队列，将 R 状态进程按照优先级和等待 CPU 时间排序，选择最需要的CPU进程执行。这里运行进程就涉及了进程上下文切换的时机：

1、进程时间片耗尽。

2、进程在系统资源不足（内存不足）。

3、进程主动sleep。

4、有优先级更高的进程执行。

5、硬中断发生。

### 线程上下文切换

线程和进程：

1、当进程只有一个线程时，可以认为进程就等于线程。

2、当进程拥有多个线程时，这些线程会共享相同的虚拟内存和全局变量等资源。这些资源在上下文切换时是不需要修改的。

3、线程也有自己的私有数据，比如栈和寄存器等，这些在上下文切换时也是需要保存的。

所以线程上下文切换包括了2种情况：

1、不同进程的线程，这种情况等同于进程切换。

2、通进程的线程切换，只需要切换线程私有数据、寄存器等不共享数据。

 

### 中断上下文切换

中断处理会打断进程的正常调度和执行，转而调用中断处理程序，响应设备事件。而在打断其他进程时，就需要将进程当前的状态保存下来，这样在中断结束后，进程仍然可以从原来的状态恢复运行。

对同一个 CPU 来说，中断处理比进程拥有更高的优先级，所以中断上下文切换并不会与进程上下文切换同时发生。由于中断会打断正常进程的调度和执行，所以大部分中断处理程序都短小精悍，以便尽可能快的执行结束。

### 查看系统上下文切换

vmstat：工具可以查看系统的内存、CPU 上下文切换以及中断次数：// 每隔1秒输出

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AD2.tmp.jpg) 

cs：则为每秒的上下文切换次数。

in：则为每秒的中断次数。

r：就绪队列长度，正在运行或等待 CPU 的进程。

b：不可中断睡眠状态的进程数，例如正在和硬件交互。

pidstat：使用pidstat -w选项查看具体进程的上下文切换次数：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AD3.tmp.jpg) 

其中cswch/s和nvcswch/s表示自愿上下文切换和非自愿上下文切换。

自愿上下文切换：是指进程无法获取所需资源，导致的上下文切换。比如说，I/O、内存等系统资源不足时，就会发生自愿上下文切换。

非自愿上下文切换：则是指进程由于时间片已到等原因，被系统强制调度，进而发生的上下文切换。比如说，大量进程都在争抢 CPU 时，就容易发生非自愿上下文切换。

### 案例问题排查

这里我们使用sysbench工具模拟上下文切换问题。先使用vmstat 1查看当前上下文切换信息：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AE4.tmp.jpg) 

然后使用sysbench --threads=64 --max-time=300 threads run模拟 64 个线程执行任务，此时我们再次vmstat 1查看上下文切换信息：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AE5.tmp.jpg) 

我们可以明显的观察到：

1、当前cs、in此时剧增。

2、sy+us的CPU占用超过90%。

3、r 就绪队列长度达到16个超过了CPU核心数8个。

 

### 分析cs上下文切换问题

我们使用pidstat查看当前 CPU 信息和具体的进程上下文切换信息：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AE6.tmp.jpg) 

所以我们可以看到大量的sysbench线程存在很多的上下文切换。

### 分析in中断问题

我们可以查看系统的watch -d cat /proc/softirqs以及watch -d cat /proc/interrupts来查看系统的软中断和硬中断（内核中断）。我们这里主要观察/proc/interrupts即可。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AE7.tmp.jpg) 

这里明显看出重调度中断（RES）增多，这个中断表示唤醒空闲状态 CPU 来调度新任务执行。

### 总结

1、自愿上下文切换变多了，说明进程都在等待资源，有可能发生了I/O等其他问题。

2、非自愿上下文切换变多了，说明进程都在被强制调度，也就是都在争抢CPU，说明CPU的确成了瓶颈。

3、中断次数变多了，说明CPU被中断处理程序占用，还需要通过查看/proc/interrupts文件来分析具体的中断类型。

 

## MySQL CPU问题定位

以往的MySQL案例中，因为使用上的一些问题，经常会导致高CPU使用率上升情况：这里包括连接数增加、执行差效率的查询SQL、哈希连接或多表合并连接、写和读IO慢、参数设置不合理等。

1、SQL语句

那些常见的SQL语句会导致cpu上升，先从最直观的SHOW PROCESSLIST，查询时间长、运行状态（State列）

“Sending data”、

“Copying to tmp table”、

“Copying to tmp table on disk”、

“Sorting result”、

“Using filesort”等都可能是有性能问题的查询（SQL）。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AE8.tmp.jpg) 

“Sending data”官网解释：

The thread is reading and processing rows for a SELECT statement, and sending data to the client. Because operations occurring during this state tend to perform large amounts of disk access (reads), it is often the longest-running state over the lifetime of a given query.

状态的含义，原来这个状态的名称很具有误导性，所谓的“Sending data”并不是单纯的发送数据，而是包括“收集 + 发送 数据”。

体现在：

1、没有使用索引

2、mysql索引表结构，要是没有使用主键查询的话，需要进行回表操作，在返回客户端。

3、返回的行数太多，需要频繁io交互

Copying to tmp table，Copying to tmp table on disk：官网解释：

Copying to tmp table

The server is copying to a temporary table in memory.

Copying to tmp table on disk

The server is copying to a temporary table on disk. The temporary result set has become too large

整体来说生成临时表内存空间，落磁盘临时表，临时表使用太

体现在多表join，buffer_size设置不合理，alter algrithem copy等方式

Sorting result：

For a SELECT statement, this is similar to Creating sort index, but for nontemporary tables.

结果集使用大的排序，基本上SQL语句上order by 字段上没有索引

 

上述的情况大量堆积，就会发现CPU飙升的情况，当然也有并发量太高的情况。

优化方向:

1、添加索引，组合索引，坚持2张表以内的join方式 这样查询执行成本就会大幅减少。

2、隐私转换避免，系统时间函数的调用避免

3、相关缓存大小设置：join_buffer_size，sort_buffer_size，read_buffer_size ，read_rnd_buffer_size，tmp_table_size。

在紧急情况下，无法改动下，通过参数控制并发度，执行时间 innodb_thread_concurrency ，max_execution_time都是有效的临时控制手段。

 

2、SQL语句

CPU对于IO方面的处理方式如下：等待的IO队列信息，会放置CPU里进行spin操作。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AF8.tmp.jpg) 

MySQL事务关联操作方面有redo，undo，binlog日志。但实际InnoDB实现方式是同步IO和异步IO两种文件读写方式

1、对于读操作，通常用户线程触发的数据请求都是同步读，其他后台线程触发的是异步读。

同步读写操作通常由用户线程来完成，当用户线程执行一句SQL时，如果请求的数据页不在buffer pool中，就需要将文件中的数据页加载到buffer pool中，如果IO有瓶颈，响应延迟，那么该线程就会被阻塞。

2、对于写操作，InnoDB是WAL（Write-Ahead Logging）模式，先写日志，延迟写数据页然后在写入磁盘，这样保证数据的安全性 数据不丢失；

异步写，主要在下面场景下触发

binlog，undo，redo log 空间不足时；

当参数innodb_flush_log_at_trx_commit，sync_binlog设置为1时，每次事务提交都会做一次fsync，相当于是同步写；

master线程每秒做一次redo fsync；

Checkpoint

undo，binlog切换时

Page cleaner线程负责脏页的刷新操作，其中double write buffer的写磁盘是同步写， 数据文件的写入是异步写。

大量的io堆积，等待的状态下，都会导致CPU使用率上升。

 

log方面多注意以下方面配置：

1、相关mysql参数 innodb_flush_log_at_trx_commit ，sync_binlog，innodb_io_capacity ，sync_relay_log的参数合理设置。

2、独立表空间 （innodb_file_per_table），日志文件伸缩大小，临时表使用，

3、尽量使用IOPS高的硬件设备

 

***\*SQL语句定位cpu核\****

通过sys库定位当前执行pid，先对应3247

mysql> select thd_id,conn_id,user,pid,program_name,command,current_statement  from sys.processlist where conn_id>0 and pid>0;

+--------+---------+----------------+------+--------------+---------+-------------------------------------------------------------------+

| thd_id | conn_id | user      | pid  | program_name | command | current_statement                         |

+--------+---------+----------------+------+--------------+---------+-------------------------------------------------------------------+

|   33 |    3 | root@localhost | 3247 | mysql     | Query  | select thd_id,conn_id,user,pid ... list where conn_id>0 and pid>0 |

+--------+---------+----------------+------+--------------+---------+-------------------------------------------------------------------+

● ps方式

通过ps工具查看对应的cpu是在哪个核上执行

[root@#### ~]# ps -o pid,psr,comm -p 3247

  PID PSR COMMAND

 3247  3 mysql

输出表示进程的 PID 为 3247（名为”mysql”）目前在编号为 3的CPU 上运行着。如果该过程没有被固定，PSR 列会根据内核可能调度该进程到不同CPU而改变显示。

 

● top方式

通过top方式查看对应的cpu是在哪个核上执行，

按下 “F”键->使用上下键选择P = Last Used Cpu，并按下空格键，出现 “*”即可->ESC 退出，这时候top界面上的Ｐ列就是对应的CPU信息.

[root@#### ~]# top -p 3247

 

***\*总结\****

以往的CPU案例中，优化的方向：

对于MySQL硬件环境资源，建议CPU起步8核开始，SSD硬盘；

索引 ，合理设计表结构，优化SQL。

读写分离，将对数据一致性不敏感的查询转移到只读实例上，分担主库压力。

对于由应用负载高导致的 CPU 使用率高的状况，从应用架构、实例规格等方面来解决。

使用 Memcache 或者 Redis缓存技术，尽量从缓存中获取常用的查询结果，减轻数据库的压力。

 

MySQL性能测试CPU优化方向：

系统参数：磁盘调度算，SHELL资源限制,numa架构，文件系统ext4，exfs

刷新mysql log相关刷新参数：

临近页（innodb_flush_neighbors）

死锁检查机制（innodb_deadlock_detect），

双1刷新：sync_binlog，innodb_flush_log_at_trx_commit

并发参数: innodb_buffer_pool_instances, innodb_thread_concurrency等

因为一些服务器的特性，导致cpu通道 和 内存协调存在一些问题，导致cpu性能上去得案例也存在

# CPU问题优化方向

性能优化往往是多方面的，CPU、内存、网络等都是有关联的，这里暂且给出 CPU 优化的思路，以供参考。

## 绑核

## 减少CPU消耗

***\*1、减少等待\****

● 减少IO量

SQL/index，使用合适的索引减少扫描的行数（需平衡索引的正收益和维护开销，空间换时间）

● 提升IO处理能力

加cache/加磁盘/SSD

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1AF9.tmp.jpg) 

***\*2、减少计算\****

● 减少逻辑运算量

避免使用函数，将运算转移至易扩展的应用服务器中 如substr等字符运算，dateadd/datesub等日期运算，abs等数学函数

减少排序，利用索引取得有序数据或避免不必要排序 如union all代替 union，order by 索引字段等

禁止类型转换，使用合适类型并保证传入参数类型与数据库字段类型绝对一致 如数字用tiny/int/bigint等，必需转换的在传入数据库之前在应用中转好

简单类型，尽量避免复杂类型，降低由于复杂类型带来的附加运算。更小的数据类型占用更少的磁盘、内存、cpu缓存和cpu周期

 

● 减少逻辑IO量

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1B0A.tmp.jpg) 

index，优化索引，减少不必要的表扫描 如增加索引，调整组合索引字段顺序，去除选择性很差的索引字段等等

table，合理拆分，适度冗余 如将很少使用的大字段拆分到独立表，非常频繁的小字段冗余到“引用表”

SQL，调整SQL写法，充分利用现有索引，避免不必要的扫描，排序及其他操作 如减少复杂join，减少order by，尽量union all，避免子查询等

数据类型，够用就好，减少不必要使用大字段 如tinyint够用就别总是int，int够用也别老bigint，date够用也别总是timestamp

 

● 减少query请求量（非数据库本身）

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps1B0B.tmp.jpg) 

适当缓存，降低缓存数据粒度，对静态并被频繁请求的数据进行适当的缓存 如用户信息，商品信息等

优化实现，尽量去除不必要的重复请求 如禁止同一页面多次重复请求相同数据的问题，通过跨页面参数传递减少访问等

合理需求，评估需求产出比，对产出比极端底下的需求合理去除

….

升级cpu若经过减少计算和减少等待后还不能满足需求，cpu利用率还高T_T 是时候拿出最后的杀手锏了，升级cpu，是选择更快的cpu还是更多的cpu了？

低延迟（快速响应），需要更快的cpu（每个查询只能使用一个cpu）

高吞吐，同时运行很多查询语句，能从多个cpu处理查询中收益

 

## 程序优化

1、基本优化：程序逻辑的优化比如减少循环次数、减少内存分配，减少递归等等。

2、编译器优化：开启编译器优化选项例如gcc -O2对程序代码优化。

3、算法优化：降低算法复杂度，例如使用nlogn的排序算法，使用logn的查找算法等。

4、异步处理：例如把轮询改为通知方式

5、多线程代替多进程：某些场景下多线程可以代替多进程，因为上下文切换成本较低

6、缓存：包括多级缓存的使用（略）加快数据访问

 

## 系统优化

1、CPU绑定：绑定到一个或多个CPU上，可以提高CPU缓存命中率，减少跨CPU调度带来的上下文切换问题

2、CPU独占：跟CPU绑定类似，进一步将CPU分组，并通过CPU亲和性机制为其分配进程。

3、优先级调整：使用nice调整进程的优先级，适当降低非核心应用的优先级，增高核心应用的优先级，可以确保核心应用得到优先处理。

4、为进程设置资源限制：使用Linux cgroups来设置进程的CPU使用上限，可以防止由于某个应用自身的问题，而耗尽系统资源。

5、NUMA优化：支持NUMA的处理器会被划分为多个Node，每个Node有本地的内存空间，这样CPU可以直接访问本地空间内存。

6、中断负载均衡：无论是软中断还是硬中断，它们的中断处理程序都可能会耗费大量的CPU。开启irqbalance服务或者配置smp_affinity，就可以把中断处理过程自动负载均衡到多个CPU上。