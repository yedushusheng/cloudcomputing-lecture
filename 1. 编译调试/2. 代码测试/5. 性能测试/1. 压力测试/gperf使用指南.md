# 环境搭建

安装包：gperftools-gperftools-2.0.zip

编译安装：unzip à./configure –enable-frame-pointersàmakeàmake install

设置环境：env LD_PRELOAD=”/usr/local/lib/libtcmalloc.so.4.1.0” HEAP_PROFILE_MMAP=true HEAD_PROFILE_ALLOCATION_INTERVAL=* 

HEAPCHECK=normal HEAPPROFILE=”*.log” 可执行文件

运行参数说明：

新的tcmalloc库路径：/home/***/lib/libtcmalloc_minimal.so.4

HEAD_PROFILE_ALLOCATION_INTERVAL=***：***大小内存被申请出来，生成一个新的heap文件

HEAPPROFILE=***：heap文件保存的路径

# 使用方法

1、 运行1小时，通过Ctrl+C或者停止运行

env LD_PRELOAD=”/usr/local/lib/libtcmalloc.so.4.1.0” 

HEAP_PROFILE_MMAP=true HEAD_PROFILE_ALLOCATION_INTERVAL=* 

HEAPCHECK=normal HEAPPROFILE=”*.log” 可执行文件

2、 停止后等几秒后出现如下的信息，把最后的—gv替换为—text

\###pprof 可执行文件 “***.heap” –inuse_objects –lines –heapcheck –edgefraction=1e-10 –nodefraction=1e-10 --gv

3、 再次运行1的命令

4、 运行两个小时后，通过Ctrl+C或者停止运行

5、 停止后等几秒，把输出信息的—gv替换为—text

6、 对比两个快照：

pprof –base=***.heap 可执行文件 ***.heap –lines --text