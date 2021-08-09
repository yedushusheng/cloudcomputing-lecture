# **版本**

# **单内核与微内核**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BAD.tmp.jpg) 

Linux操作系统是单内核。

## **单内核**

***\*单内核：\****操作系统的各个子系统（内存管理，进程管理，文件系统等）之间可以直接调用。

优点：性能较高（子系统可以直接调用）

缺点：可维护性差

 

## **微内核**

***\*微内核：\****只有核心代码（中断等）放在微内核中，其他的模块（如文件系统）以服务的形式存放在外部。

优点：可维护性高

缺点：效率低

 

 

# **结构**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BBE.tmp.jpg) 

# **内核**

## **目录**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BBF.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BC0.tmp.jpg) 

## **LKM**

可加载Linux内核模块（LKM）

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BC1.tmp.jpg) 

内核模块不是独立的可执行文件，但在运行时其目标文件被链接到内核中。***\*只有超级用于才能加载和卸载模块\****。

代码：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BD1.tmp.jpg) 

编译：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BD2.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BD3.tmp.jpg) 

模块插入/删除命令：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BE4.tmp.jpg) 

***\*内核模块与C应用\****

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BE5.tmp.jpg) 

## **双链表**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BE6.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps5BF6.tmp.jpg) 

## **哈希表**

# **书籍推荐**

《Linux内核设计与实现》（入门）

《深入理解Linux内核》

《Linux设备驱动程序》