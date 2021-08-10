# 流

## **简介**

​	流可以简单理解为**文件**。

 

​	**低级IO与标准（高级）IO的区别：**

**open和fopen的区别：**

前者属于低级IO，后者是高级IO。

前者返回一个文件描述符，后者返回一个文件指针。

**前者无缓冲，后者有缓冲。**

前者与read，write等配合使用， 后者与fread，fwrite等配合使用。

后者是在前者的基础上扩充而来的，在大多数情况下，用后者。

 

## **文件**

​	文件在计算机系统中的重要性：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps8A23.tmp.jpg) 

### **内容**

​	比较两个文件内容vimdiff file1 file2

### **属性**

## **IO**

​	输入输出(I/O)是任何一种编程环境中最基本的功能。

​	任何系统，程序读写文件的内容前，首先在程序与文件之间建立连接。

## **流**

​	C库提供一种机制描述程序与文件的这种连接（流即文件的连接）。

​	C库的IO函数就是完成流操作。

​	用流函数打开一个文件，它返回一个流。

### **打开流**

​	流的数据类型：FILE类型（代表了文件的所有内部状态）

​	FILE对象由IO库函数内部分配、管理

​	在打开流的时候需要考虑：哪个文件（新建），打开的方式，光标位置

​	

​	在Linux系统中使用man fopen查看：

​	原型：FILE *fopen(const char *path, const char *mode);

​	参数：path表示需要打开的文件，mode表示光标所在位置以及以什么方式打开(r/r+/w/w+/a/a+)。

​	r：读，光标在头部

​	r+：读写，光标在头部

​	w：写，光标在文件头（对已有内容清空）

​	w+：读写

​	a：追加，写操作，光标定位到文件尾部

​	a+：追加，读写

 

​	示例：

\#include <stdio.h>

int main(int argc,char **argv)

{

​	FILE *fp = NULL;

​	fp = fopen("a.c","w");

​	if ( NULL == fp )

​	{

​		perror("fopen");

​		return -1;

​	}

​	fclose(fp);

}

### **关闭流**

​	释放IO库函数分配的资源，中断文件和程序的连接。

## **操作**

### **文件拷贝**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps8A24.tmp.jpg) 

​	硬盘是不可寻址的，所以无法在硬盘内部直接进行数据的复制，需要借助内存寻址。

​	***\*示例：\****

\#include <stdio.h>

\#include <stdlib.h>  //man free, man malloc

int main(int argc,char **argv)

{

​	int ret = -1;

​	//拷贝源文件（source.c）到目标文件（dst.c)

​	FILE *fp_s = NULL;//源文件流

​	fp_s = fopen("source.c","r");//只读 r

​	if ( NULL == fp_s )

​	{

​		perror("fopen source.c");

​		return -1;

​	}

 

​	FILE *fp_d = NULL;//目标文件流

​	fp_d = fopen("dst.c","w");//写w 

​	if ( NULL == fp_d )

​	{

​		perror("fopen dst.c");

​		return -1;

​	}

​	//准备内存

​	char *buff = NULL;

​	buff = malloc(20);//堆  20字节

​	if( NULL == buff )

​	{

​		perror("malloc buff");

​		return -1;

​	}

​	//读 

​	//从源文件默认光标（文件位置处）读取一个长度信息，

//存放到用户内存里面

​	ret = fread(buff,1,20,fp_s);

​	if ( 0 == ret )

​	{

​		//bug...

​		perror("fread");

​		return -1;

​	}

​	//写

​	//

​	ret = fwrite(buff,1,20,fp_d);

​	if ( 0 == ret )

​	{

​		//bug...

​		perror("fwrite");

​		return -1;

​	}

​	free(buff);

​	buff = NULL;

​	fclose(fp_s);

​	fp_s = NULL;

​	fclose(fp_d);

​	fp_d = NULL;

}

### **读写**

#### fgets

​	原型：char *fgets(char *s, int size, FILE *stream);

​	可以处理行信息，与fputs成对使用。fread/fwrite是操作任意大小，fgets/fputs是操作行大小

#### fputs

#### getc

​	getc/putc操作的是字节

#### putc

#### fread

​	原型：size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

​	参数：ptr文件指针（没有操作指针位置的参数，即只能接收已经设置好的文件指针，即只能从当前光标处操作），size*nmemb表示需要读取的文件大小（不应该超过用户空间分配的内存大小，否则容易产生内存越界的风险）

​	返回值：成功—读取的数据量，失败—比较短的数据值（没有错误，但是快到文件尾部的时候返回，比如总共30字节，一次读20字节，读两次会超出10，返回10）或0

​	当遇到失败的时候，应该调用feof(3)和ferror(3)判断是哪一种错误，feof表示是否到达文件尾部，ferror用来判断错误。

 

​	示例：

\#include <stdio.h>

\#include <stdlib.h>  //man free, man malloc

\#include <string.h>

int main(int argc,char **argv)

{

​	int ret = -1;

​	//拷贝源文件（source.c）到目标文件（dst.c)

​	FILE *fp_s = NULL;//源文件流

​	fp_s = fopen("source.c","r");//只读 r

​	if ( NULL == fp_s )

​	{

​		perror("fopen source.c");

​		return -1;

​	}

 

​	FILE *fp_d = NULL;//目标文件流

​	fp_d = fopen("dst.c","w");//写w 

​	if ( NULL == fp_d )

​	{

​		perror("fopen dst.c");

​		return -1;

​	}

 

 

​	//准备内存

​	char *buff = NULL;

​	buff = malloc(20);//堆  20字节

​	if( NULL == buff )

​	{

​		perror("malloc buff");

​		return -1;

​	

​	}

​	while(1)

​	{

​		memset(buff,'\0',20);//内存每次完成后清0（防止该次读取20，下次读取10，导致下次读取的时候有第一次的10个残留信息）

​		//读 

​		//从源文件默认光标（文件位置处）读取一个长度信息，存放到用户内存里面

​		ret = fread(buff,1,20,fp_s);

​		if ( 20 > ret )

​		{

​			if ( feof(fp_s))//ferror

​			{

​				//文件尾

​				ret = fwrite(buff,1,ret,fp_d);

​				if ( 0 == ret )

​				{

​					//bug...

​					perror("fwrite");

​					return -1;

​				}

​				break;

​			}else

​			{

​				//出错

​				perror("fread");

​				return -1;

 

​			}

​		}

​		//写

​		//

​		ret = fwrite(buff,1,ret,fp_d);

​		if ( 0 == ret )

​		{

​			//bug...

​			perror("fwrite");

​			return -1;

​		}

​	}

 

​	free(buff);

​	buff = NULL;

​	fclose(fp_s);

​	fp_s = NULL;

​	fclose(fp_d);

​	fp_d = NULL;

}



|      |                                                              |
| ---- | ------------------------------------------------------------ |
|      | ![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps8A25.tmp.jpg) |

​	GDB跟踪后分析：



​	在source中有20字节，buffer提供的是20字节大小，但是打印buffer的时候显示20字节后为什么后面还有别的信息？

buff填充的时候没有字符串结束符，即m字符后面是没有字符串结束符填充的，系统在解析buff的时候，认为buffer是个字符串，只有遇到结束符的时候才会停止，并不理解这段内存到底多大，所以读到buff的m时还会继续往下读，直到结束符停止。

 

 

 

 

 

 

 

#### fwrite

​	原型：size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

#### 文件结束函数

#### 错误指示函数

### **缓冲**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps8A35.tmp.jpg) 

​	上述第一个过程用户调用函数是用户来操作的，第二个过程是由系统完成的，这种用内存代替机械硬盘效率高，但是二者存在数据同步不及时（即数据存在于内存中，系统崩溃，没有将数据刷新到磁盘，产生脏数据）。

 

​	示例：

\#include <stdio.h>

\#include <stdlib.h>  //man free, man malloc

\#include <string.h>

int main(int argc,char **argv)

{

​	int ret = -1;

​	//拷贝 源文件（source.c）到目标文件（dst.c)

​	FILE *fp_s = NULL;//源文件流

​	fp_s = fopen("source.c","r");//只读 r

​	if ( NULL == fp_s )

​	{

​		perror("fopen source.c");

​		return -1;

​	}

 

​	FILE *fp_d = NULL;//目标文件流

​	fp_d = fopen("dst.c","w");//写w 

​	if ( NULL == fp_d )

​	{

​		perror("fopen dst.c");

​		return -1;

​	}

 

 

​	//准备内存

​	char *buff = NULL;

​	buff = malloc(20);//堆  20字节

​	if( NULL == buff )

​	{

​		perror("malloc buff");

​		return -1;

​	

​	}

​	while(1)

​	{

​		memset(buff,'\0',20);//内存每次完成后清0

​		//读 

​		//从源文件的默认光标（文件位置处）读取一个长度信息，存放到用户内存里面

​		ret = fread(buff,1,20,fp_s);

​		if ( 20 > ret )

​		{

​			if ( feof(fp_s))//ferror

​			{

​				//文件尾

 

​				ret = fwrite(buff,1,ret,fp_d);

​				if ( 0 == ret )

​				{

​					//bug...

​					perror("fwrite");

​					return -1;

​				}

​				break;

​			}else

​			{

​				//出错

​				perror("fread");

​				return -1;

 

​			}

​		}

​		//写

​		//

​		ret = fwrite(buff,1,ret,fp_d);

​		if ( 0 == ret )

​		{

​			//bug...

​			perror("fwrite");

​			return -1;

​		}

​		fflush(fp_d);//主动将库函数内存与磁盘强制同步，防止脏数据

​		while(1);//ctrl+c 程序异常退出（查看数据有没有存到磁盘，此时显示未存到磁盘，由于写到库的内存，但是库内存没有完成与磁盘的同步）

​	}

 

​	free(buff);

​	buff = NULL;

​	fclose(fp_s);

​	fp_s = NULL;

​	fclose(fp_d);

​	fp_d = NULL;

}

 

​	分析：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps8A36.tmp.jpg) 

​	文件open之后，打印FILE结构体就已经初始化了（关注IO_read/IO_write/IO_buff的成员变量）。调用fread的时候，观察源文件的流*fp_s，可以看到这里缓冲的数据非常多，但是读出来的数据，即buff内容非常少。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps8A37.tmp.jpg) 

​	当执行完write的时候，目标文件流会变化。

 

### **定位**

​	在Linux中通过man fseek获取定位函数：

#### fseek

​	原型：int fseek(FILE *stream, long offset, int whence);

​	参数：offset表示偏移量，whence表示具体移动到某个标志性位置（可以是文件头，文件尾或者当前位置）。

​	作用：针对某个标志性位置做偏移

***\*举例：\****

\#include <stdio.h>

\#include <stdlib.h>  //man free, man malloc

\#include <string.h>

int main(int argc,char **argv)

{

​	//一次性完成数据的传递

​	//计算出源文件的大小

​	//准备一个合适的内存，大小（源文件本身大小）

​	//一次性读取源文件

​	//一次性写入源文件

​	//关闭

 

​	int ret = -1;

​	//拷贝源文件（source.c）到目标文件（dst.c)

​	FILE *fp_s = NULL;//源文件流

​	fp_s = fopen("source.c","r");//只读 r

​	if ( NULL == fp_s )

​	{

​		perror("fopen source.c");

​		return -1;

​	}

 

​	FILE *fp_d = NULL;//目标文件流

​	fp_d = fopen("dst.c","w");//写w 

​	if ( NULL == fp_d )

​	{

​		perror("fopen dst.c");

​		return -1;

​	}

 

​	//准备内存

​	char *buff = NULL;

​	//计算出源文件的大小

​	ret = fseek(fp_s,0,SEEK_END);//定位到文件尾部

​	if ( -1 == ret )

​	{

​		perror("fseek");

​		return -1;

​	}

​	long file_size;

​	file_size = ftell(fp_s);//计算出大小

​	if ( -1 == file_size )

​	{

​		perror("ftell");

​		return -1;

​	}

​	//文件位置－－－文件头

​	ret = fseek(fp_s,0,SEEK_SET);

​	if ( -1 == ret )

​	{

​		perror("fseek");

​		return -1;

​	}

​	//准备一个合适的内存，大小（源文件本身大小）

​	buff = malloc(file_size);//堆 

​	if( NULL == buff )

​	{

​		perror("malloc buff");

​		return -1;

​	}

​	memset(buff,'\0',file_size);//内存每次完成后清0

​	//读 

​	//从源文件默认光标（文件位置处）读取一个长度信息，

//存放到用户内存里面

​	ret = fread(buff,1,file_size,fp_s);

​	if ( 20 > ret )

​	{

​		if ( feof(fp_s))//ferror

​		{

​			//文件尾

​			ret = fwrite(buff,1,ret,fp_d);

​			if ( 0 == ret )

​			{

​				//bug...

​				perror("fwrite");

​				return -1;

​			}

​		}else

​		{

​			//出错

​			perror("fread");

​			return -1;

 

​		}

​	}

​	//写

​	//

​	ret = fwrite(buff,1,ret,fp_d);

​	if ( 0 == ret )

​	{

​		//bug...

​		perror("fwrite");

​		return -1;

​	}

​	free(buff);

​	buff = NULL;

​	fclose(fp_s);

​	fp_s = NULL;

​	fclose(fp_d);

​	fp_d = NULL;

}

#### ftell

​	原型：long ftell(FILE *stream);

​	作用：测试当前光标位置距离文件头的位置（该距离使用long类型标识，单位是按照字节计算）。

​	可以利用fseek定位到文件头，然后ftell计算偏移量，这样就可以计算出文件的大小。

#### rewind

#### fgetpos

#### fsetpos