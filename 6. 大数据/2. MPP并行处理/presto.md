# 概述

Presto是Faceboo开源的MPP（Massive Parallel Processing）SQL引擎，本身并不存储数据，但是可以接入多种数据源，并且支持跨数据源的级联查询。

Presto是一个OLAP的工具，擅长对海量数据进行复杂的分析；但是对于OLTP场景，并不是presto擅长的，所以不要把presto当作数据库来使用。

Presto的设计和编写完全是为了解决像Facebook这样规模的商业数据仓库的交互式分析和处理速度的问题。目前国内的美团、京东、滴滴等在业务中都有使用presto。

 

## Persto VS MySQL

首先，MySQL是一个数据库，具备存储和计算分析能力，而presto只有计算分析能力；其次数据量方面，MySQL作为传统单点关系型数据库不能满足当前大数据量的需求，于是有各种大数据的存储和分析工具产生，presto就是这样一个可以满足大数据量分析计算需求的一个工具。

Presto需要从数据源获取数据来进行运算分析，它可以连接多种数据源，包括Hive、RDBMS（MySQL、Oracle、TiDB等）、Kafka、MongoDB、Redis等。

一个presto查询可以将多个数据源的数据进行合并分析。比如select * from a join b where a.id=b.id;，其中a表可以来自Hive，b表可以来自MySQL。

## 数据模型

Presto使用Catalog、Schema和Table这3层结构来管理数据。

Catalog：就是数据源。Hive是数据源，MySQL也是数据源，Hive和MySQL都是数据源类型，可以连接多个Hive和多个MySQL，每个连接都有一个名字。一个Catalog可以包含多个Schema，大家可以通过show catalogs命令看到presto连接的所有数据源。

Schema：相当于一个数据库实例，一个Schema包含多张数据表。Show schemas from ‘catalog_name’可以列出catalog_name下面的所有schema。

Table：数据表，与一般意义上的数据库表相同。Show tables from ‘catalog_name.schema_name’可以查看catalog_name.schema_name下的所有表。

在presto中定位一张表，一般是catalog为根，例如：一张表的全称为hive.test_data.test，标识hive（catalog）下的test_date（schema）中的test表。可以理解为：数据源的大类.数据库.数据表。

 

## 接入方式

Presto的接入方式有多种Lpresto-cli、pyhive、jdbc、http、golang、SQLAlchemy、PHP等，其中presto-cli是presto官方提供的。

 

## 优缺点

数据源可以是多个不同种类的数据库，PB级海量数据复杂分析，交互式SQL查询，但是不擅长多个大表的JOIN操作，因为presto是基于内存的，多个大表JOIN计算会有多次内存读写，速度会变慢（类似于MySQL的innodb_buffer_pool_size）

# 架构