# 简介

# 配置

涉及的配置文件：

/home/sqltest/lib/linxociclient.so

/home/sqltest/etc/sqltest.ini

/home/sqltest/bin/sqltest

其中sqltext.ini配置：

1、 general段：配置连接以及sql个数

[general]

​	;连接方式 0-使用IP+端口连接 1-使用unix socket文件连接

​	conn_style=0

​	;0-长连接 1-短连接，短连接一般用于连接测试

​	conn_type=0

​	ip=****

​	port=****

​	usename=root

​	password=****

​	socket=

​	sqlnum=4//如果为1则执行sql1，如果是2则执行sql1和sql2，以此类推

​	commitnum=1

​	database=test

​	;若配置为空，则将执行失败的sql写入配置文件（全路径）

2、 [pre_deal]段：配置预处理sql语句

statement_num=8

​	statement1=drop table if exist t1

​	……

3、 sqlN段：配置SQL语句的段

[sql1]

​	Statement=insert into t1 values ($(var1),……)

​	;变量定义，类型有：

​	;int：表示整型，支持从起始值到最大值的随机或递增取值

​	;min：会将配置中的变量当前值替换到该变量中，不作任何计算仅替换变量值

​	;expr：表达式，支持基本变量和整型的四则运算、括号、取余运算（变量必须为整型或整型字符串或表达式），程序计算出结果

​	var1=……

# 使用

​	sqltest –ts50 –te50 –p10000 –m100000 –s

​	参数说明：

​	ts：第一次运行线程数

​	te：最后一次运行线程数

​	p：期望每秒跑的sql数目

​	m：一共要发送的sql数目

​	s：实时打印出统计信息

 