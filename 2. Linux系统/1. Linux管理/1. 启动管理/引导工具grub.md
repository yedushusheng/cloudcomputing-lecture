# 概述

引导程序是驻留在硬盘第一个扇区（MPR、主引导记录）的程序。GRUB是一个功能强大的多系统引导程序，专门处理Linux与其它操作系统共存的问题。

 

# 配置

使用vi命令可以查看grub.conf文件内容：

\#vi /boot/grub/menu.lst

参数解释：

1、default=0

\# default后加一个数字n，表示n+1个“title”操作系统，0表示第一个“title” 的操作系统，以此类推。

2、timeout=0

\# timeout表示默认的等待时间，此处设置为0表示开机后系统将自动载入默认的操作系统。

3、splashimage=(hd0,3)/boot/grub/splash.xpm.gz

\# 指定GRUB引导时使用的屏幕图像文件存放的路径。

4.、hiddenmenu

\# 隐藏菜单选项

5、title Red Hat Enterprise Linux (2.6.32-220.e16.i686)

\# title定义启动菜单项的名称，title 后面的字符串就是在菜单项上显示的选项。

6、root(hd,0)

\# 设置GRUB的根分区，是/boot对应的分区或boot文件夹所在的分区。

7、kernel /vmkinuz-2.6.32-220.e16.i686 ro root=UUID=d67e208c-2791-4692-a615-c4e96b6ae355 rd_NO...... rd_NO_DM

\# 指定内核文件（vmkinuz-2.6.32-220.e16.i686）与要挂载的根设备（root=UUID=d67e208c-2791-4692-a615-c4e96b6ae355） ，ro是readonly的意思。

8、initrd /initramfs-2.6.32-220.e16.i686.img

\# 指定初始化内存映像盘。