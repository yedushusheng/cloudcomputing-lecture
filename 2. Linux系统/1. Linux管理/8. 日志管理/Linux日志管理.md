# **概述**

在CentOS6.x中日志服务已经由rsyslogd取代了原先的syslogd服务。rsyslogd日志服务更加先进，功能更多。但是不论该服务的使用，还是日志文件的格式其实都是和syslogd服务相兼容的。

 

# **rsyslogd**

## **特点**

rsyslogd新特点：

1、基于TCP网络协议传输日志信息；

2、更安全的网络传输方式；

3、有日志消息的及时分析框架；

4、后台数据库；

5、配置文件中可以写简单的逻辑判断；

6、与syslog配置文件相兼容。

 

## **服务启动**

查看服务是否启动：ps aux | grep rsyslogd

查看服务是否自启动：chkconfig --list | grep rsyslog

 

# **常见日志**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps4F1E.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps4F2E.tmp.jpg) 

除了系统默认的日志之外，采用RPM方式安装的系统服务也会默认把日志记录在/var/log目录下（源码包安装的服务日志时在源码包指定目录中）。不过这些日志不是由rsyslogd服务来记录和管理的，而是各个服务使用自己的日志管理文档来记录自身日志。

| 日志文件        | 说明                                |
| --------------- | ----------------------------------- |
| /var/log/httpd/ | RPM包安装的apache服务的默认日志目录 |
| /var/log/mail/  | RPM包安装的邮件服务的额外日志目录   |
| /var/log/samba/ | RPM包安装的samba服务的日志目录      |
| /var/log/sssd/  | 守护进程安全服务目录                |

 

# **日志文件格式**