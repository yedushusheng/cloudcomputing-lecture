# 概述

参考：

https://w.cnblogs.com/AndyStudy/p/6409287.html

 

# 配置

# 使用

​	工具使用命令：

​	valgrind –tool=memcheck –leak-check=full –log-file=filename execute_bin

​	Ctrl+C结束检测，统计泄露

 

​	valgrind支持很多工具：memecheck、addrcheck、cachegrind、massif、helpgrind和callgrind等。

 

​	possibly lost是指仍然存在某个指针能够访问某块内存，但该指针指向的已经不是该内存首地址。

​	definitely lost是指已经不能够访问这块内存，分为两种：直接的（direct）和间接的（indirect）。直接和间接的区别就是，直接是没有任何指针指向该内存，间接是指指向该内存的指针都位于内存泄漏处。

​	

 

​	memcheck探测程序中内存管理存在的问题：

1、 使用未初始化的内存unitialised

2、 在内存被释放后进行读/写invalid read

3、 从已分配内存块的尾部进行读/写invalid read

4、 内存泄漏1 byte in 1block are definitely lost in loss record 1 of 1 //definite明确的

5、 不匹配地使用malloc/new/new[]和free/delete/delete[]

 

 