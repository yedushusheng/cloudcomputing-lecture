# 简介

Sysbench是一款开源的多线程性能测试工具，可以执行CPU/内存/线程/IO/数据库等方面的性能测试。

Sysbench支持以下几种测试模式：

1、CPU运算性能

2、磁盘IO性能

3、调度程序性能

4、内存分配及传输速度

5、POSIX线程性能

6、数据库性能（OLTP基准测试）

注：在GoldenDB分布式数据库中，使用sysbench的数据库性能（OLTP基准测试）测试模式来压测proxy性能。

 

# 安装

基本操作步骤：

1、获取安装包sysbench-1.0.tar.gz

2、解压：unzip sysbench-1.0.tar.gz

3、进入解压后的目录：cd /home/sysbench-1.0/

4、安装依赖：

yum -y install make automake libtool pkgconfig libaio-devel

yum -y install mariadb-devel openssl-devel

5、编译：

./autogen.sh

./configure

make -j

make install

6、创建自定义用户

useradd username

cp -r /home/sysbench-1.0 /home/username

chown 

 

# 测试模型

Sysbench通过脚本定义了若干个常用的压测模型，以下是几个常用模型：

| 压测模型                  | 描述           |
| ------------------------- | -------------- |
| bulk_insert.lua           | 批量插入数据   |
| oltp_delete.lua           | 删除数据       |
| oltp_insert.lua           | 单值插入数据   |
| oltp_point_select.lua     | 精准查询       |
| oltp_read_only.lua        | 只读           |
| oltp_read_write.lua       | 混合读写       |
| oltp_update_index.lua     | 更新索引字段   |
| oltp_update_non_index.lua | 更新非索引字段 |
| oltp_write_only.lua       | 只写           |

# 使用

使用sysbench进行压测，通常分为三个步骤：

1、prepare：准备数据

2、run：运行测试模型

3、cleanup：清理测试数据

通常仅需要准备一次数据，在此数据基础上测试各种模型即可。

## 常用配置参数

Usage:

sysbench [options]... [testname] [command]

Commands implemented by most tests: prepare run cleanup help

 

General options:

--threads=N   number of threads to use [1] # 创建测试线程的数量，默认是1

--events=N  limit for total number of events [0]  # 事件最大数量，0为不限制

--time=N limit for total execution time in seconds [10] # 最大执行时间，单位秒

--forced-shutdown=STRING   number of seconds to wait after the --time limit before forcing shutdown, or 'off' to disable [off]

--thread-stack-size=SIZE    size of stack per thread [64K]

--rate=N            average transactions rate. 0 for unlimited rate [0]

--report-interval=N       periodically report intermediate statistics with a specified interval in seconds. 0 disables intermediate reports [0]

--report-checkpoints=[LIST,...] dump full statistics and reset all counters at specified points in time. The argument is a list of comma-separated values representing the amount of time in seconds elapsed from start of test when report checkpoint(s) must be performed. Report checkpoints are off by default. []

--debug[=on|off]        print more debugging info [off]

--validate[=on|off]       perform validation checks where possible [off]

--help[=on|off]         print help and exit [off]

--version[=on|off]       print version and exit [off]

--config-file=FILENAME     File containing command line options

--tx-rate=N           deprecated alias for --rate [0]

--max-requests=N        deprecated alias for --events [0]

--max-time=N          deprecated alias for --time [0]

--num-threads=N         deprecated alias for --threads [1]

 

Pseudo-Random Numbers Generator options:

--rand-type=STRING random numbers distribution {uniform,gaussian,special,pareto} [special]

--rand-spec-iter=N number of iterations used for numbers generation [12]

--rand-spec-pct=N percentage of values to be treated as 'special' (for special distribution) [1]

--rand-spec-res=N percentage of 'special' values to use (for special distribution) [75]

--rand-seed=N   seed for random number generator. When 0, the current time is used as a RNG seed. [0]

--rand-pareto-h=N parameter h for pareto distribution [0.2]

 

Log options:

--verbosity=N verbosity level {5 - debug, 0 - only critical messages} [3]

--percentile=N    percentile to calculate in latency statistics (1-100). Use the special value of 0 to disable percentile calculations [95]

--histogram[=on|off] print latency histogram in report [off]

 

General database options:

--db-driver=STRING specifies database driver to use ('help' to get list of available drivers) [mysql]

--db-ps-mode=STRING prepared statements usage mode {auto, disable} [auto]

--db-debug[=on|off] print database-specific debug information [off]

 

Compiled-in database drivers:

mysql - MySQL driver

 

mysql options:

--mysql-host=[LIST,...]     MySQL server host [localhost]

--mysql-port=[LIST,...]     MySQL server port [3306]

--mysql-socket=[LIST,...]    MySQL socket

--mysql-user=STRING       MySQL user [sbtest]

--mysql-password=STRING     MySQL password []

--mysql-db=STRING        MySQL database name [sbtest]

--mysql-ssl[=on|off]  use SSL connections, if available in the client library [off]

--mysql-ssl-cipher=STRING    use specific cipher for SSL connections []

--mysql-compression[=on|off]   use compression, if available in the client library [off]

--mysql-debug[=on|off]      trace all client library calls [off]

--mysql-ignore-errors=[LIST,...] list of errors to ignore, or "all" [1213,1020,1205]

--mysql-dry-run[=on|off]     Dry run, pretend that all MySQL client API calls are successful without executing them [off]

 

Compiled-in tests:

fileio - File I/O test

cpu - CPU performance test

memory - Memory functions speed test

threads - Threads subsystem performance test

mutex - Mutex performance test

## 操作步骤

1、开始测试

开始测试前，还需要先连接proxy创建sysbenchtest数据库。

注意：sysbench内部采用prepare模式测试，因此需要开启数据库prepare模式。

2、准备数据

sysbench /usr/local/share/sysbench/oltp_insert.lua --mysql-host=** --mysql-port=** --mysql-user=** --mysql-password=** --mysql-db=** --db-driver=mysql --mysql_table_options=’distributed by hash(id)(g1,g2,g3,g4)’ --tables=** --table-size=** --report-interval=** --threads=** --time=** --skip_trx=true --mysql-ignore-errors=all prepare

3、执行测试

sysbench /usr/local/share/sysbench/oltp_insert.lua --mysql-host=** --mysql-port=** --mysql-user=** --mysql-password=** --mysql-db=sysbenchtest --db-driver=mysql --mysql_table_options=’distributed by hash(id)(g1,g2,g3,g4)’ --tables=** --table-size=** --report-interval=** --threads=** --time=** --skip_trx=true --mysql-ignore-errors=all run

4、清理环境

sysbench /usr/local/share/sysbench/oltp_insert.lua --mysql-host=** --mysql-port=** --mysql-user=** --mysql-password=** --mysql-db=sysbenchtest --db-driver=mysql --mysql_table_options=’distributed by hash(id)(g1,g2,g3,g4)’ --tables=** --table-size=** --report-interval=** --threads=** --time=** --skip_trx=true --mysql-ignore-errors=all cleanup

 