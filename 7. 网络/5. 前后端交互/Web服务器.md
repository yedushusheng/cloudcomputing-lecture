目前比较普遍的网页（HTTP）服务器有：

1、 Apache HTTP Server

2、 Google Web Server

3、 Nginx

4、 Lighttpd

# Tomcat

Tomcat是一个小型的轻量级应用服务器，在中小型系统和并发访问用户不是很多的场景下呗普遍使用，是开发和调试jsp程序的首选。

在Tomcat中，应用程序的部署是，将你的war包放在tomcat的webapp目录下，第一次将jsp转换为servlet比较慢。

# WAR

WAR：Sun提出的一种Web应用程序格式，与JAR类似，也是许多文件的一个压缩包。这个包中的文件按照一定的目录结构来组织：通常在其根目录下包含有html和jsp文件或者包含这两种文件的目录，另外还会有一个WEB-INF目录，这个目录下有一个web.xml文件和一个classes目录，web.xml文件是这个应用的配置文件，而classes目录下则包含编译好的Servlet类和jsp或者servlet所以来的其他类（如JavaBean）

一般在应用程序部署的时候，把apche和tomcat集成在一起，使得性能提高（因为Apache是一个专业的http容器，是专门用来提供http服务及相关配置的，比如虚拟主机、URL转发等），Apache只是作为一个转发，对jsp的处理还是有tomcat处理的，apche可以支持php、CGI、Perl，但是要使用Java的话，需要在tomcat在apache后台支撑，将java请求转发给tomcat

# JBOSS

JBOSS需要内存和硬盘空间比较小

# WebLogic

有最高水平的可拓展性和可用性，出色的集群技术

 

与Tomcat区别：应用服务器集成更多的J2EE特性，如EJB、JMS、JAAS等，同时也支持jsp和servlet，而tomcat功能没有那么强大，不提供EJB支持，但是如和JBOSS集成在一起，则可以实现J2EE的全部功能。

# WebSphere

强项在负载均衡，但是小号资源非常厉害，门槛很高，决定于硬件，非常适合生产环境的部署，WevSphere最小只需要512MRAM，WebLogic最小需要64MRAM，但是不准备128M是不合适的。

 

# IIS

 