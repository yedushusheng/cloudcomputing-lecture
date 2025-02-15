# **背景**

## **本地调用VS远程调用**

***\*本地过程调用\****

RPC就是要像调用本地的函数一样去调远程函数。假设我们要调用函数Multiply来计算lvalue * rvalue的结果:

1 int Multiply(int l, int r) {

2   int y = l * r;

3   return y;

4 }

5 

6 int lvalue = 10;

7 int rvalue = 20;

8 int l_times_r = Multiply(lvalue, rvalue);

那么在第8行时，我们实际上执行了以下操作：将lvalue和rvalue的值压栈进入Multiply函数，取出栈中的值10和20，将其赋予l和r执行第2行代码，计算l * r ，并将结果存在y将y的值压栈，然后从Multiply返回第8行，从栈中取出返回值200 ，并赋值给l_times_r以上5步就是执行本地调用的过程。（注：以上步骤只是为了说明原理，事实上编译器经常会做优化，对于参数和返回值少的情况会直接将其存放在寄存器，而不需要压栈弹栈的过程，甚至都不需要调用call，而直接做inline操作。仅就原理来说，这5步是没有问题的。）

 

RPC要解决的两个问题：

***\*1、解决分布式系统中，服务之间的调用问题。\****

2、远程调用时，要能够像本地调用一样方便，让调用者感知不到远程调用的逻辑。

***\*远程过程调用带来的新问题\****

在远程调用时，我们需要执行的函数体是在远程的机器上的，也就是说，Multiply是在另一个进程中执行的。这就带来了几个新问题：

**1、*****\*Call ID映射。\****我们怎么告诉远程机器我们要调用Multiply，而不是Add或者FooBar呢？在本地调用中，函数体是直接通过函数指针来指定的，我们调用Multiply，编译器就自动帮我们调用它相应的函数指针。但是在远程调用中，函数指针是不行的，因为两个进程的地址空间是完全不一样的。所以，***\*在RPC中，所有的函数都必须有自己的一个ID。这个ID在所有进程中都是唯一确定的。客户端在做远程过程调用时，必须附上这个ID。然后我们还需要在客户端和服务端分别维护一个{函数<--> Call ID}的对应表。\****两者的表不一定需要完全相同，但相同的函数对应的Call ID必须相同。当客户端需要进行远程调用时，它就查一下这个表，找出相应的Call ID，然后把它传给服务端，服务端也通过查表，来确定客户端需要调用的函数，然后执行相应函数的代码。

**2、*****\*序列化和反序列化。\****客户端怎么把参数值传给远程的函数呢？在本地调用中，我们只需要把参数压到栈里，然后让函数自己去栈里读就行。但是在远程过程调用时，客户端跟服务端是不同的进程，不能通过内存来传递参数。甚至有时候客户端和服务端使用的都不是同一种语言（比如服务端用C++，客户端用Java或者Python）。这时候就需要客户端把参数先转成一个字节流，传给服务端后，再把字节流转成自己能读取的格式。这个过程叫序列化和反序列化。同理，从服务端返回的值也需要序列化反序列化的过程。

**3、*****\*网络传输。\****远程调用往往用在网络上，客户端和服务端是通过网络连接的。所有的数据都需要通过网络传输，因此就需要有一个网络传输层。网络传输层需要把Call ID和序列化后的参数字节流传给服务端，然后再把序列化后的调用结果传回客户端。只要能完成这两者的，都可以作为传输层使用。因此，它所使用的协议其实是不限的，能完成传输就行。尽管大部分RPC框架都使用TCP协议，但其实UDP也可以，而gRPC干脆就用了HTTP2。Java的Netty也属于这层的东西。

有了这三个机制，就能实现RPC了，具体过程如下：

// Client端 

//   int l_times_r = Call(ServerAddr, Multiply, lvalue, rvalue)

\1. 将这个调用映射为Call ID。这里假设用最简单的字符串当Call ID的方法

\2. 将Call ID，lvalue和rvalue序列化。可以直接将它们的值以二进制形式打包

\3. 把2中得到的数据包发送给ServerAddr，这需要使用网络传输层

\4. 等待服务器返回结果

\5. 如果服务器调用成功，那么就将结果反序列化，并赋给l_times_r

 

// Server端

\1. 在本地维护一个Call ID到函数指针的映射call_id_map，可以用std::map<std::string, std::function<>>

\2. 等待请求

\3. 得到一个请求后，将其数据包反序列化，得到Call ID

\4. 通过在call_id_map中查找，得到相应的函数指针

\5. 将lvalue和rvalue反序列化后，在本地调用Multiply函数，得到结果

\6. 将结果序列化后通过网络返回给Client

所以要实现一个RPC框架，其实只需要按以上流程实现就基本完成了。其中：

1、Call ID映射可以直接使用函数字符串，也可以使用整数ID。映射表一般就是一个哈希表。

2、序列化反序列化可以自己写，也可以使用Protobuf或者FlatBuffers之类的。

3、网络传输库可以自己写socket，或者用asio，ZeroMQ，Netty之类。

当然，这里面还有一些细节可以填充，比如如何处理网络错误，如何防止攻击，如何做流量控制，等等。但有了以上的架构，这些都可以持续加进去。

 

## **技术演变**

***\*RPC是构建分布式的基石\****：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD4FC.tmp.jpg) 

举例：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD50D.tmp.jpg) 

​	对于上述一个电商架构平台而言，如果业务量比较小的时候，还可以维护，但是如果业务体量很大，需要多个人员分别维护多个模块，那么会存在下面的问题：

1、某个人维护自己代码的时候，需要将所有的代码都打包编译，其他人等待完成后才可以继续（这样导致测试困难，需要进行回归，从而影响版本发布）；

2、其中一个模块有问题，则整个模块就会出错。

​	大型企业的做法如下：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD50E.tmp.jpg) 

​	

​	从单机走向分布式，产生了很多分布式的通信方式：

1、 TCP/UDP二进制传输：最古老，也是最有效的，并且永不过时，事实上所有的通信方式归根结底都是TCP/UDP；

2、 CORBA（Common Object Request Broker Architecture）：古老而复杂，支持面向对象的通信协议；

3、 Web Service（SOA SOAP RDDI WSDL）：基于http+xml的标准化Web API

4、 RestFul（Reprensentational State Transfer）：http+json

5、 RMI（Remote Method Invocation）：Java内部的分布式通信协议

6、 JMS（Java Message Service）：JavaEE的消息框架标准，为很多MQ支持

7、 RPC（Remote Procedure Call）：远程过程调用，这只是一个统称，重点在于方法调用（不支持对象的概念），具体实现甚至可以用RMI RestFul等去实现，但一般不用，因为RMI不能跨语言，而ResuFul效率太低。

多用于服务器集群间的通信（分布式或微服务架构），因此常使用更加高效、短小精悍的传输模式以提高效率。

# **概述**

RPC (Remote Procedure Call)即远程过程调用，是分布式系统常见的一种通信方法。除RPC之外，常见的多系统数据交互方案还有分布式消息队列、HTTP请求调用、数据库和分布式缓存等。

 

rpc是一种概念，http也是rpc实现的一种方式。

通过RPC能解耦服务，这才是使用RPC的真正目的。RPC的原理主要用到了动态代理模式，至于http协议，只是传输协议而已。简单的实现可以参考spring remoting，复杂的实现可以参考dubbo。

rpc=socket +动态代理

服务器通讯原理就是一台socket服务器A，另一台socket客户端B，现在如果要通讯的话直接以流方式写入或读出。这样能实现通讯，但有个问题。如何知道更多信息？

比如需要发送流大小，编码，IP等。这样就有了协议，协议就是规范，就是发送的流中携带了很多的内容。那回到刚刚的问题。发送的内容就是文本类型，客户端就得序列化，那么常用的就有json，xml之类，如果想把内容变得更小，那就有二进制了。把文本变成二进制传递。

说到rpc与http接口，不要太复杂了。rpc协议更简单内容更小，那么来说效率是要高一点。

 

RPC的调用关系里，我们把提供具体的调用方法的系统叫服务提供者（Provider），调用服务的系统称为服务消费者（Consumer）。把对象转换为以便于网络传输的二进制或文本数据的过程，叫做序列化（Serialization）；二进制或文本数据再还原为对象的过程，叫做反序列化（Deserialization）。 我们可以看到，典型的RPC处理机制包括两部分：

通信协议，可以是基于tcp的，也可以是基于http的。

数据格式，一般是一套序列化+反序列化机制。

# **分类**

常见的RPC技术有Cobra、RMI、.NET Remoting、WebService、JSON-RPC、XML-RPC、Hessian、Thrift、Protocol Buffer、gRPC等等。

按照序列化机制的特点，我们可以把RPC技术分为文本的（WebService、JSON-RPC、XML-RPC等）和二进制的（RMI、Hessian、Thrift、Protocol Buffer等）。

按照常见的通信协议来看，我们又可以分为基于HTTP的（WebService、Hessian等）和基于TCP的（RMI、.NET Remoting等）。

按照是否可以用于多个不同平台，又可以分为平台特定的（RMI是Java平台特定的、.NET Remoting是.NET平台特定的）和平台无关的（比如WebService、JSON-RPC、Hessian等可以用于http://Java.Net\PHP\Python等就是平台无关的）。 在Java里，我们一般可以基于JDK自带的动态代理机制+Java的对象序列化方式实现一个简单的RPC，但是由于动态代理和Java对象序列化都比较低效，导致这种方式性能较低。目前更常见的是基于AOP和代码生成技术实现stub和skeleton，然后用一个紧凑的二进制序列化方式，实现一个高效的RPC框架。

# **模式**

按照调用方式来看，RPC有四种模式：

RR（Request-Response）模式，又叫请求响应模式，指每个调用都要有具体的返回结果信息。

Oneway模式，又叫单向调用模式，调用即返回，没有响应的信息。

Future模式，又叫异步模式，返回拿到一个Future对象，然后执行完获取到返回结果信息。

Callback模式，又叫回调模式，处理完请求以后，将处理结果信息作为参数传递给回调函数进行处理。

这四种调用模式中，前两种最常见，后两种一般是RR和Oneway方式的包装，所以从本质上看，RPC一般对于客户端的来说是一种同步的远程服务调用技术。与其相对应的，一般来说MQ恰恰是一种异步的调用技术。

 

而对于MQ，根据消息处理的特点，我们又可以总结两种消息模式：

点对点模式（Point to Point，PTP），一个生产者发送的每一个消息，都只能有一个消费者能消费，看起来消息就像从一个点传递到了另外一个点。

发布订阅模式（Publish-Subscribe，PubSub），一个生产者发送的每一个消息，都会发送到所有订阅了此队列的消费者，这样对这个消息感兴趣的系统都可以拿到这个消息。

# **原理**

​	RPC的全称是Remote Procedure Call，是一种**进程**（服务最小粒度）间通信方式（只是一个概念，不是协议），它允许程序调用另一个地址空间（通常是共享网络的另一台机器上）的过程或函数，而不是程序员显式编码这个远程调用的细节。即程序员无论是调用本地还是远程的，本质上编写的调用代码基本相同（而***\*网络socket则明显区分客户端和服务端\****）。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD50F.tmp.jpg) 

RPC的一般需要经历4个步骤：

1、建立通信

首先要解决通讯的问题：即A机器想要调用B机器，首先得建立起通信连接，主要是通过在客户端和服务器之间建立TCP连接。

2、服务寻址

要解决寻址的问题，A服务器上如何连接到B服务器（如主机或IP地址）以及特定的端口，方法的名称是什么。

3、网络传输

1）序列化

当A服务器上的应用发起一个RPC调用时，调用方法和参数数据都需要先进行序列化。

2）反序列化

当B服务器接收到A服务器的请求之后，又需要对接收到的参数等信息进行反序列化操作。

4、服务调用

B服务器进行本地调用（通过代理Proxy）之后得到了返回值，此时还需要再把返回值发送回A服务器，同样也需要经过序列化操作，然后再经过网络传输将二进制数据发送回A服务器。

通常，一次完整的PRC调用需要经历如上4个步骤。

 

## **架构**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD510.tmp.jpg) 

## **组件**

一个RPC框架一般包含以下几个组件：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD521.tmp.jpg) 

​	RPC Server：负责导出（export）远程接口

​	RPC Client：负责导入（import）远程接口的代理实现

​	RPC Proxy：远程接口的代理实现

​	RPC Invoker：

客户方实现：负责编码调用信息和发送调用请求到服务方并等待调用结果返回；

服务方实现：负责调用服务端接口的具体实现并返回调用结果

​	RPC Protocol：负责协议编/解码

​	RPC Connector：负责维持客户方和服务方的连接通道和发送数据到服务方

​	RPC Acceptor：负责接收客户方请求并返回请求结果

​	RPC Processor：负责在服务方控制调用过程，包括管理调用线程池、超时时间等

​	RPC Channel：数据传输通道

 

## **序列化**

RPC实现另一台机器的调用通信，***\*本质上是借助底层TCP/IP协议，通过序列化和反序列化实现的\****。

序列化把对象转换为可传输的***\*二进制\****。可以采用JDK（仅适用于Java），***\*JSON（跨语言，文本化，比二进制包大，性能稍差）\****，Hessian，***\*Protobuf\****（使用IDL文件，对数据类型做了约定，跨语言能力强）。

反序列化安全问题，采用白名单：

1、扫描接口类声明的类型

2、系统内置白名单

3、用户定义白名单

 

## **服务发现**

***\*服务发现：\****

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD522.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD523.tmp.jpg) 

调用方和提供方通过注册中心实现函数（地址）的发布和订阅，提供方将自己的函数注册到注册中心，调用方到注册中心订阅相应的函数。类似于DNS（通过域名获取IP），但是没有采用DNS，因为DNS有缓存，不够及时。我们期望服务方启动后，调用方能够在秒级感受到提供服务。

***\*健康检查：\****

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD524.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD534.tmp.jpg) 

通过状态机实现节点假死的转移。

通过心跳机制将亚健康的节点转换为死亡状态，然后还可以通过探活机制实现节点状态的检查。

## **负载均衡**

***\*负载均衡：\****

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD535.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD536.tmp.jpg) 

负载方式：

1、随机负载

2、一致性hash负载

3、自适应负载（计算TPS，利用率等计算出权重）

 

***\*集群策略：\****

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD537.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD548.tmp.jpg) 

集群策略：快速失败，失败重试，定点调用

 

## **流量控制**

服务端流量控制：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD549.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD54A.tmp.jpg) 

服务端在对调用方的请求需要控制，防止恶意攻击，流量控制方案：

1、识别调用来源

2、单节点限流：平滑限流算法

3、集群限流

 

## **分组部署**

***\*业务分组：\****

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD54B.tmp.jpg) 

 

## **安全认证**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD54C.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD55C.tmp.jpg) 

# **特点**

## **优点**

http接口是在接口不多、系统与系统交互较少的情况下，解决信息孤岛初期常使用的一种通信手段；

优点就是简单、直接、开发方便。

如果是一个大型的网站，内部子系统较多、接口非常多的情况下，RPC框架的好处就显示出来了：

1、首先就是长链接，不必每次通信都要像http一样去3次握手什么的，减少了网络开销；

2、其次就是RPC框架一般都有注册中心，有丰富的监控管理；发布、下线接口、动态扩展等，对调用方来说是无感知、统一化的操作。

3、最后是安全性。

4、提升系统可扩展性

5、提升系统可维护性和持续交付能力

6、实现系统高可用

## **缺点**

1、一个完善的RPC框架开发难度大

2、RPC框架调用成功率受限于网络状况

3、调用远程方法对初学者来说难度大

# **框架**

RPC框架包括webservice，RMI，Dubbo，thrift，HTTP等。

## **Dubbo**

​	Dubbo是阿里巴巴开源的一个Java高性能优秀的服务框架，使得应用可通过高性能的RPC实现服务的输出和输入功能，可以和Spring框架无缝集成。

## **Montan**

​	Montana是新浪微博开源的一个Java框架。

## **Spring** **Cloud**

## **rpcx**

​	rpcx是go语言生态圈的Dubbo，比Dubbo更加轻量，实现了许多Dubbo的特性，借助于Go语言优秀的并发特性和简洁语法，可以使用较少的代码实现分布式的RPC框架。

## **gRPC**

​	gRPC是Google开发的高性能、通用的开源RPC框架，其由Google主要面向移动应用并发并给予HTTP/2协议标准而设计。基于ProtocolBuf（Protocol Buffer）序列化协议开发，且支持众多开发语言。但是由于它本身不是分布式的，所以要实现上述框架的功能需要做进一步开发。

## **gsaop**

​	gsoap更适合C/C++程序，重量级应用。

## Thrift

​	适合Java程序，中量级应用。

## **rest**

​	适合脚本语言，轻量级应用。

 

## **对比**

|                  | Dubbo | Montana                   | Rpcx | gRPC               |
| ---------------- | ----- | ------------------------- | ---- | ------------------ |
| 开发语言         | Java  | Java                      | Go   | 跨语言             |
| 分布式           | √     | √                         | √    | ×                  |
| 多序列化框架支持 | √     | √                         | √    | ×                  |
| 多种注册中心     | √     | √[Hessian2，JSON，可拓展] | √    | ×[支持protocolbuf] |
| 管理中心         | √     | √                         | ×    | ×                  |
| 跨编程语言       | ×     | ×                         | ×    | √                  |

 

# **应用**

RPC在我们熟知的各种中间件中都有它的身影。Nginx、Redis、MySQL、Dubbo、Hadoop、Spark、Tensorflow等重量级开源产品都是在RPC技术的基础上构建出来的，我们这里说的 RPC指的是广义的RPC，也就是分布式系统的通信技术。

## **Nginx与RPC**

Ngnix是互联网企业使用最为广泛的代理服务器。它可以为后端分布式服务提供负载均衡的功能，它可以将后端多个服务地址聚合为单个地址来对外提供服务。如图，Django是Python技术栈最流行的Web框架。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD55D.tmp.jpg) 

Nginx和后端服务之间的交互在本质上也可以理解为RPC数据交互。也许你会争辩说Nginx和后端服务之间使用的是HTTP协议，走的是短连接，严格上不能算是RPC调用。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD55E.tmp.jpg) 

你说的没错，不过Nginx和后端服务之间还可以走其它的协议，***\*比如uwsgi协议、fastcgi协议等，这两个协议都是采用了比HTTP协议更加节省流量的二进制协议\****。如上图所示uWSGI是著名的Python容器，使用它可以启动uwsgi协议的服务器对外提供服务。

uwsgi通讯协议在Python语言体系里使用非常普遍，如果一个企业内部使用Python语言栈搭建Web服务，那么他们在生产环境部署Python应用的时候不是在使用HTTP协议就是在使用uwsgi协议来和Nginx之间建立通讯。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD56F.tmp.jpg) 

Fastcgi协议在PHP语言体系里非常常见，Nginx和PHP-fpm进程之间一般较常使用Fastcgi协议进行通讯。

 

## **Hadoop与RPC**

在大数据技术领域，RPC也占据了非常重要的地位。大数据领域广泛应用了非常多的分布式技术，分布式意味着节点的物理隔离，隔离意味着需要通信，通信意味着RPC的存在。大数据需要通信的量比业务系统更加庞大，所以在数据通信优化上做的更深。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD570.tmp.jpg) 

比如最常见的Hadoop文件系统hdfs，一般包括一个NameNode和多个DataNode，NameNode和DataNode之间就是通过一种称为Hadoop RPC的***\*二进制协议\****进行通讯。

 

## **TensorFlow与RPC**

在人工智能领域，RPC也很重要，著名的TensorFlow框架如果需要处理上亿的数据，就需要依靠分布式计算力，需要集群化，当多个分布式节点需要集体智慧时，就必须引入RPC技术进行通讯。Tensorflow Cluster的RPC通讯框架使用了Google内部自研的gRPC框架。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD571.tmp.jpg) 

# **对比**

## **RPC与微服务**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD572.tmp.jpg) 

随着企业IT服务的不断发展，单台服务器逐渐无法承受用户日益增长的请求压力时，就需要多台服务器联合起来构成服务集群共同对外提供服务。同时业务服务会随着产品需求的增多越来越肿，架构上必须进行服务拆分，一个完整的大型服务会被打散成很多很多独立的小服务，每个小服务会由独立的进程去管理来对外提供服务，这就是微服务。当用户的请求到来时，我们需要将用户的请求分散到多个服务去各自处理，然后又需要将这些子服务的结果汇总起来呈现给用户。那么服务之间该使用何种方式进行交互就是需要解决的核心问题。RPC就是为解决服务之间信息交互而发明和存在的。

在微服务中，服务之间调用多采用RPC，而非HTTP。

 

## **RPC与HTTP**

和一般基于Message的分布式框架（如MPI）相比，RPC更加抽象，也更方便理解。***\*不过在底层，无论是RPC还是MPI，终究还是要转化成TCP/UDP包发来发去的\****。

***\*对比：\****

|          | RPC                                                          | HTTP                                                   |
| -------- | ------------------------------------------------------------ | ------------------------------------------------------ |
| 性能     | 主要差别在序列化和反序列化RPC通过thrift实现***\*二进制传输\**** | HTTP一般是***\*json格式\*******\*序列化更消耗性能\**** |
| 传输协议 | ***\*RPC基于TCP，也可以基于HTTP（\*******\*长连接\*******\*）\**** | HTTP只能是HTTP                                         |
| 负载均衡 | RPC***\*自带负载均衡\****                                    | HTTP需自己处理，如nginx                                |
| 传输效率 | 可自定义TCP协议，***\*报文相对较小\****或者使用HTTP2协议，也可以很好的减少报文的体积，提高传输效率 | HTTP报文有很多无用信息（头部信息）                     |
| 通知     | RPC自动通知                                                  | HTTP事先通知自行修改Nginx配置或其他负载均衡配置        |

RPC适用于公司内部使用，性能消耗低，传输效率高***\*（采用二进制传输协议、长链接，因此比较高效）\****，服务治理方便，但是***\*不建议传输较大的文本、视频等\****。

HTTP适用于对外的技术，公司上下游的调用可以使用MQ。主要用于对外的异构环境，浏览器接口调用，APP接口调用，第三方接口调用等。

 

***\*HTTP调用其实也是一种特殊的RPC\****

RPC和HTTP调用是没有经过中间件的，它们是端到端系统的直接数据交互。HTTP 调用其实也可以看成是一种特殊的RPC，只不过***\*传统意义上的RPC是指长连接数据交互\****，而***\*HTTP一般是指即用即走的短链接\****（新版本已经不是短链接）。

HTTP1.0协议时，HTTP调用还只能是短链接调用，一个请求来回之后连接就会关闭。HTTP1.1在HTTP1.0协议的基础上进行了改进，引入了KeepAlive特性可以保持HTTP连接长时间不断开，以便在同一个连接之上进行多次连续的请求，进一步拉近了HTTP和RPC之间的距离。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD583.tmp.jpg) 

当HTTP协议进化到2.0之后，Google开源了一个建立在HTTP2.0协议之上的通信框架直接取名为gRPC，也就是Google RPC，这时HTTP和RPC之间已经没有非常明显的界限了。所以在后文我们不再明确强调RPC和HTTP请求调用之间的细微区别了，直接统一称之为RPC。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wpsD584.tmp.jpg) 

 

***\*HTTP VS RPC（普通话VS方言）\****

HTTP与RPC的关系就好比普通话与方言的关系。要进行跨企业服务调用时，往往都是通过HTTP API，也就是普通话，虽然效率不高，但是通用，没有太多沟通的学习成本。

但是在企业内部还是RPC更加高效，同一个企业公用一套方言进行高效率的交流，要比通用的HTTP协议来交流更加节省资源。整个中国有非常多的方言，正如有很多的企业内部服务各有自己的一套交互协议一样。虽然国家一直在提倡使用普通话交流，但是这么多年过去了，你回一趟家乡探个亲什么的就会发现身边的人还是流行说方言。

如果再深入一点说，普通话本质上也是一种方言，只不过它是官方的方言，使用最为广泛的方言，相比而言其它方言都是小语种，小语种之中也会有几个使用比较广泛比较特色的方言占比也会比较大。这就好比开源RPC协议中Protobuf和Thrift一样，它们两应该是RPC协议中使用最为广泛的两个。

 

## **RPC与IPC**

​	我们通常所说管道、FIFO等是同一机器的进程间通信方式（IPC），**RPC是不同机器之间****进程****通信方式**。

## **RPC与Socket**

**RPC(Remote Procedure Call，远程过程调用)是建立在Socket之上的**，在一台机器上运行的主程序，可以调用另一台机器上准备好的子程序，就像LPC(本地过程调用)。RPC协议假定某些传输协议的存在，如TCP或UDP，为通信程序之间携带信息数据。在OSI网络通信模型中，RPC跨越了传输层和应用层。RPC使得开发包括网络分布式多程序在内的应用程序更加容易。

 

RPC带来了开发C/S程序的简单可靠的手段，它通过一种叫XDR的数据表达方法描述数据，程序员书写伪代码，然后由rpcgen程序翻译为真正的可编译的C语言源代码，再编译成真正的Client端和Server端程序。 

**RPC作为普遍的C/S开发方法，开发效率高效，可靠**。但RPC方法的基本原则是－－以模块调用的简单性忽略通讯的具体细节，以便程序员不用关心C/S之间的通讯协议，集中精力对付实现过程。这就决定了RPC生成的通讯包不可能对每种应用都有最恰当的处理办法，**与Socket方法相比，传输相同的有效数据，RPC占用更多的网络带宽**。

RPC是在Socket的基础上实现的，它比socket需要更多的网络和系统资源。另外，在对程序优化时，程序员虽然可以直接修改由rpcgen产生的令人费解的源程序，但对于追求程序设计高效率的RPC而言，获得的简单性则被大大削弱。

 

## **RPC与MQ**

参考：

https://www.cnblogs.com/AIPAOJIAO/p/10241655.html

 

### 区别

1、在架构上，RPC和MQ的差异点是，***\*Message有一个中间结点Message Queue，可以把消息存储\****。

2、同步调用：***\*对于要立即等待返回处理结果的场景，RPC是首选\****。

3、MQ的使用，一方面是基于性能的考虑，比如服务端不能快速的响应客户端（或客户端也不要求实时响应），需要在队列里缓存。另外一方面，***\*它更侧重数据的传输\****，因此方式更加多样化，除了点对点外，还有订阅发布等功能。

4、而且随着业务增长，有的处理端处理量会成为瓶颈，会进行同步调用改造为异步调用，这个时候可以考虑使用MQ。

 

​	***\*比较适合客户端调用哪个服务器比较明确\****。RPC让你远程调用象本地调用，一般是***\*同步\****的，例如，你读一个文件，象调用本地的函数，就是时间久点。

消息代理框架一般是***\*异步\****的，一个线程send，另外一个线程recv。

最大的区别是，**rpc没有broker, 而消息队列是需要管理消息的存储的，rpc没有存储，只有通信**。

 

### **应用**

消息队列服务一般用于设计***\*多系统\****之间的信息传输，一般这种传输不需要对方对数据做出回应。它最常见的方式是构建异步的生产者-消费者模式。我们在系统开发中，有些业务并不需要及时返回结果，我们可以把这些操作放到队列中，然后另起一个消费者去处理它。比如日志，数据库异步更新。

\- 消息的发送者和消费者需要解耦的情况

\- 发送者并不明确谁是消费者

\- 发送者并不关心谁来消费消息

\- 各个消费者可以从不同的角度入手处理消息

\- 消费者的处理结果也不返回给发送者

\- 消息的发送和处理是异步的

\- 消息的关注者不止一个

在一个由多个微服务构成的大系统中，会有一些非关键服务，用来执行一些不需要立刻得到结果的计算。而且它们的计算结果并不会返回给消息的发送者。这个时候就应该使用MQ。比如在一个ERP系统中有一些日志服务、业务监控服务等。这些服务会发布一些系统事件，针对这些事件可能有多个应用关注。对于日志服务，当系统出现某些异常情况时需要浏览日志，查找问题的根源；也可以在分析系统运行的瓶颈时提供关键数据。对于业务监控系统，例如货物入仓出仓的消息，可以被报表系统关注，生成报表；也可以被配货系统关注，及时补足所需库存。

 

RPC一般是用于服务器与服务器进程之间通信，这种通信**有请求和应答**。它是建立在底层的socket通信之上的。封装为RPC之后，更加方便建立通信。就像在同一个进程中调用对方的方法一样。它本地的方法名一般和请求到达的服务器的方法名一一对应。这样可以更好的把模块划分。所以它是应对分布式而生的。比如一个网站，一开始可能所有的服务在一个进程中，但是随着业务的增长，一个进程处理不过来，这时就需要把业务拆分成多个，分部到不同的机器上去。

在一个由多个微服务构成的大系统中，某些关键服务间的调用应当在较短的时间内返回，而且各个微服务的专业化程度较高，同一个请求的关注者只有一个。这个时候就应该用RPC。比如在一个ERP系统中，有一个管理仓储的微服务，以及一个负责订单的微服务。新建订单时需要查知当前的存货是否充足，如果不充足就通知用户；提交订单时预订指定数量的货物，如果此时货物不错，也要终止订单的提交，并通知用户。显然在这种场景下是不允许较大的延迟，否则会影响用户体验。所以应该使用RPC，及时返回仓储情况。

 

### **总结**

关注下游执行执行结果，用RPC；

不关注下游执行结果，用MQ，不用RPC；

一句话：**异步MQ，同步RPC**。

 

# **手写RPC**

参考：

https://www.cnblogs.com/itoak/p/13370031.html

https://www.cnblogs.com/2YSP/p/13545217.html

 