# 代码回退

​	本地代码push远程的gerrit库之后，需要回退代码，基本分为两种情况：

## **push后未改动**

1、 git status查看状态

2、 git log查看commit情况

3、 git reset回退commit

--soft 参数才能保留上次commit之前的修改内容

--hard 直接把整个代码回退到上一个comit，之前的修改丢失

4、 git status再次查看状态

## push后有改动

1、 git status查看状态

2、 git checkout 文件全路径 放弃所有的修改

3、 git status查看状态