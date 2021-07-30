# 简介

thrift的全名叫做Apache thrift，是一款软件开发RPC框架，可以很高效地实现跨语言的RPC服务。

Thrift是一个轻量级、语言无关的软件栈，它具有一套为RPC通信生成代码的机制。Thrift为数据的传输、序列化，以及应用层处理提供了干净的抽象。采用这种抽象栈，它的代码生成器仅使用一种简洁的定义语言作为输入，便能够生成跨编程语言的代码，以构建互操作的RPC客户端和服务器。

Thrift是专门为客户端和服务器之间非原子地版本更新而设计的（注：这个表述的含义是，当客户端和服务端之间的RPC请求或响应体发生变更时，不必同时升级客户端&服务端程序版本，即客户端、服务端同时具有向后兼容和向前兼容性。对于7*24的互联网服务来说，这个特性允许在不停服的情况下对分布式系统的不同组件或节点逐台滚动升级）。

Thrift主要通过自定义的Interface Definition Language(IDL)，创建基于RPC的客户端和服务端的服务代码。Thrift 的跨语言性体现在，它可以通过thrift编译器生成C++、Java、Python、PHP、Ruby、Erlang、Perl、Haskell、 C#、Cocoa、JavaScript、Node.js、Smalltalk、OCaml、Delphi等语言的代码，它们之间可以进行透明的通信，并由生成的代码负责RPC协议层和传输层的实现。

注：公司云电脑的oasagent模块就是采用Thrift上报相关的主机信息的。

# 安装

# 原理

## 架构

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps859B.tmp.jpg)

​	其中，Input code是用户实现的业务逻辑，Service client是根据Thrift定义的服务接口描述文件生成的客户端和服务器端代码框架，write/read是根据Thrift文件生成代码实现数据的读写操作。其以下是 Thrift 的传输体系、协议以及底层 I/O 通信，使用 Thrift 可以很方便的定义一个服务并且选择不同的传输协议和传输层而不用重新生成代码。

## TProcessor层

Processor封装了从输入数据流中读数据和向数据流中写数据的操作,与服务相关的Processor是由编译器编译IDL文件产生的，它的主要工作是：从连接中读取数据，把处理交给用户实现impl，最后把结果写到连接上。

 

　　处理层就是服务器端定义的处理业务逻辑，它的主要代码是**Service.java文件中的Iface接口和Processor类。 

　　Iface接口：这个接口中的所有方法都是用户定义在IDL文件中的service的方法，它需要抛出TException这个检查异常，服务器端需要定义相应的实现类去 implements **.Iface 接口，完成服务的业务逻辑。 

Processor类：这个类中定义了一个processMap，里面包含了service中定义的方法，服务器端在构造这个Processor对象的时候，唯一需要做的就是把实现service（Iface）的对象作为参数传递给Processor的构造函数。

## TTransport层

　　传输层实现数据的传输，使用TCP、Http等协议实现，它包含了各种socket调用中的方法，如open，close，read，write。由于是框架中的最后一层，所以，最重要的实现部分当然是数据的读出和写入（read 和 write），它有阻塞和非阻塞的实现方式：

​	TSocket：阻塞式socker； 

TFramedTransport：以frame为单位进行传输，非阻塞式服务中使用； 

TFileTransport：以文件形式进行传输； 

TMemoryTransport：将内存用于I/O，java实现时内部实际使用了ByteArrayOutputStream； 

TZlibTransport：使用zlib进行压缩，与其他传输方式联合使用，当前无java实现；

 

## TProtocol层

协议层是定义数据会以怎样的形式到达传输层。它首先对IDL中的各个数据结构进行了定义，且对每一种类型都定义了read和write方法。我们需要在服务器端和客户端声明相同的实现协议来作为内存和网络传输格式之间的映射。 

常见的协议有：

​	TBinaryProtocol：二进制格式，最简单/最常用的实现协议，很高效； 

TCompactProtocol：压缩二进制格式，它会采用压缩算法对数据进行再压缩，减少实际传输的数据量，提高传输效率； 

TJSONProtocol：JSON格式； 

TSimpleJSONProtocol：提供JSON只写协议, 生成的文件很容易通过脚本语言解析； 

TDebugProtocol：使用易懂的可读的文本格式，以便于debug

 

## Server层

Thrift实际上是实现了C/S模式，通过代码生成工具将thrift文件生成服务器端和客户端代码（可以为不同语言），从而实现服务端和客户端跨语言的支持。用户在Thirft文件中声明自己的服务，这些服务经过编译后会生成相应语言的代码文件，然后客户端调用服务，服务器端提服务便可以了。

服务模型包括：

***\*TSimpleServer:\****

简单的单线程服务模型，这种工作模式只有一个线程，循环监听传过来的请求并对其进行处理，处理完才能接受下一个请求，是一种阻塞式IO的实现，因为效率比较低，实际线上环境一般用不到。一般用于开发时候演示工作流程时使用。　　

***\*TNonblockingServer:\****

这种模式与TsimpleServer最大的区别就是使用NIO，也就是非阻塞是IO的方式实现IO的多路复用，它可以同时监听多个socket的变化，但因为业务处理上还是单线程模式，所以在一些业务处理比较复杂耗时的时候效率还是不高，因为多个请求任务依然需要排队一个一个进行处理。

***\*TThreadPoolServer:\****

多线程服务模型，使用标准的阻塞式IO。这种模式引入了线程池，主线程只负责accept，即监听Socket，当有新的请求（客户端Socket）来时，就会在线程池里起一个线程来处理业务逻辑，这样在并发量比较大的时候（但不超过线程池的数量）每个请求都能及时被处理，效率比较高，但一旦并发量很大的时候（超过线程池数量），后面来的请求也只能排队等待。

***\*TThreadedSelectorServer:\****

这是一种多线程半同步半异步的服务模型，是Thrift提供的最复杂最高级的服务模型，内部有一个专门负责处理监听Socket的线程，有多个专门处理业务中网络IO的线程，有一个专门负责决定将新Socket连接分配给哪一个线程处理的起负载均衡作用的线程，还有一个工作线程池.这种模型既可以响应大量并发连接的请求又可以快速对wangluoIO进行读写，能适配很多场景，因此是一种使用比较高频的服务模型。

 

　　Server是Thrift框架中的最高层，它创建并管理下面的三层，同时提供了客户端调用时的线程调度逻辑。 

服务层的基类是TServer，它相当于一个容器，里面包含了TProcessor，TTransport，TProtocol，并实现对它们的管理和调度。TServer有多种实现方式,可以是单线程阻塞式IO模型和多线程非阻塞式IO模型（实践中常用）。

 

# 特点

## 优点

Thrift的好处主要是以下几点：

1、One-stop shop，相对于protobuf，序列化和RPC支持一站式解决，如果是pb的话，还需要考虑选择RPC框架，现在Google是开源了gRpc，但是几年以前是没有第一方的标准解决方案的；

2、特性丰富，idl层面支持map，protobuf应该是最近才支持的，map的key支持任意类型，avro只支持string，序列化支持自定义protocol, rpc支持thread pool, hsha, no blocking多种形式，必有一款适合你，对于多语言的支持也非常丰富；

3、 RPC和序列化性能都不错，这个到处都有benchmark，并不是性能最好的，但是基本上不会成为瓶颈或者短板；

4、有很多开源项目的周边支持都是thrift的，hbase提供thrift服务，hive，spark sql，cassandra等一系列对外的标准服务接口都是thrift的以支持多语言；

5、Column Storage的话，parquet支持直接通过thrift idl转换，如果在Hadoop集群上存储数据，elephant-bird支持得很好，你可以很方便地针对thrift的数据通过pig写dsl，如果你希望在rpc服务外做一系列工作，可以用finagle包装一层。不过，这部分对于protobuf和avro支持一般也不错。

## 缺点

Thrift最大的缺点之一是缺乏对大批量流式读写数据的支持。

具体：

1、基本没有官方文档，使用参考可以看看有人专门写的这个 Thrift: The Missing Guide；

2、RPC在0.6.1升级到0.7.0是不兼容的！这个对于早于0.6.1开始使用的用户来说是个大坑；

3、bug fix和更新不积极，好在序列化和RPC服务都不是太复杂的问题，需要考量的设计问题不多，自己维护patch的成本不高，如果我没有记错的话，0.6.1的java的ThreadPool Server是会有Thread死亡之后的Thread泄露问题的；

4、 Facebook今年说，我们开源了一个新的performance更好的 fbthrift，你说你该用apache thrift还是fbthrift呢?

 

# 使用

thrift中的类型包括基础类型、结构、容器、异常、服务等几个部分。

## 基础类型

​	thrift不支持无符号的类型，无符号类型可以简单理解为不能表示负数，只能表示正数的类型，像java的基本数据类型都是有符号的类型。

（1）bool：布尔类型（true或false）

（2）byte：8位有符号整数

（3）i16：16位有符号整数

（4）i32：32位有符号整数

（5）i64：64位有符号整数

（6）double：64位浮点数

（7）string：文本字符串，使用UTF-8编码

## 容器

thrift容器包括了各种语言中最常用的容器，共三种：

（1）list容器：一个元素可重复的有序列表。会被转换成C++中的vector，Java中的ArrayList，脚本语言中的数组等。

（2） set容器：一个元素不可重复的无序集合。会转换成C++中的set，Java中的HashSet、Python中的Set等。

（3）map容器：一个含有多个key:value键值对的结构。会被转换成C++中的map，Java中的HashMap，Python/Ruby中的dictionary等。

对于上述三种容器，其元素的类型原则上可以是任何一种thrift类型。但是值得注意的是，map的key类型需要是基础类型，因为很多开发语言并不支持map的key类型为复杂数据类型。

## 结构体

结构体类型，在形式上和C/C++中的结构体类型非常相似，就是一坨类型的组合，比如上文图中的UserGradeInfo便是一个thrift结构体类型。

thrift接口文件中的结构体类型，都会被转换成一个独立的类（Class）。类的属性便是结构体中的各个类型，而类的方法便是对这些类型进行处理的相关函数。

我们来看一个结构体定义的例子：

可以看到，结构体中每一个域都有一个正整数标识符，这个标识符并不要求连续，但一旦定义，不建议再进行修改。

另外，每个域前都会有required或optional的限定，前者表示是必填域，后者则表示是可选域。域是可以有默认值的，比如上例中的“Anonymous”和0。

（1）如果一个域设置了required，但是在实际构造结构体时又没有给这个域赋值，那么thrift会认为这是一个异常。

（2）如果一个域设置为optional且在构造结构体时没有给这个域赋值，那么在使用这个结构体时，就会忽略掉这个optional的域。

## 异常

除了使用exception来替代struct以外，“异常”这个类型，在语法上和刚才介绍过的结构体的用法是完全一致的。但是从语义上讲，exception和struct却大相径庭。exception是在远程调用发生异常时用来抛出异常用的。

## 服务

服务的定义，与面向对象技术中定义一个接口很类似，而这些接口其实就是纯虚函数。thrift编译工具会根据服务的定义来产生相应的方法和函数。

每个服务，都包括了若干个函数，每个函数包括了若干参数和一个返回值（返回值可以是void）。

**（小技巧：返回值为void的函数，你可以在函数名前加上oneway标识符，将此函数以异步模式执行，这样在调用此函数后，函数会立即返回。）**

对于返回void的函数，thrift仍然会确保函数返回，这样就表示这个函数已被正确执行，且服务器端已有返回信息了。但是如果给void的函数前加上oneway，那么此函数的返回只能表示数据已经进入传输层，并不能表示服务器端已经收到并返回了数据。

# 实践

使用thrift开发的基本流程：

编写IDL文件->编译生成代码->服务器重载实现接口->实现服务端和客户端