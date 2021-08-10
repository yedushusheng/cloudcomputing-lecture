# service

​	service是一个脚本命令，分析service可知是/etc/init.d目录下执行相关程序，service和chkconfig结合使用。

​	服务配置文件存放目录/etc/init.d/:

​	# 启动sshd服务

​	service sshd start

​	# 设置sshd服务开机启动

​	chkconfig sshd start

 

# systemctl

​	CentOS7版本中使用了system，system同时兼容service，对应的命令就是systemctl。

systemd是Linux系统中最新的初始化系统（init），它主要的设计目标是克服sysvinit固有的缺点，提高系统的启动速度。systemd目标是尽可能启动较少的进程，尽可能将更多的进程并发启动。

systemd把不同的资源称为Unit，每一个Unit都有一个配置文件，告诉systemd怎么启动这个Unit，存放目录：/etc/system/system和/usr/lib/system/system。

对于有先后依赖关系的任务，systemctl融合service和chkconfig功能。