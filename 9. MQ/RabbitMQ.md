# AMQP

## **概述**

​	AMQP定义：是具有现代特征的二进制协议。是一个提供统一消息服务的应用层标准高级消息队列协议，是应用层协议的一个开放标准，为面向消息的中间件设计。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E27.tmp.jpg) 

## **核心概念**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E38.tmp.jpg) 

### **Publisher**

消息的生产者。也是一个向交换器Exchange发送消息的客户端应用程序。

### **Consumer**

消息的消费者。表示一个从消息队列中取得消息的客户端应用程序。

### Server/Broker

又称Broker，接受客户端的连接，实现AMQP实体服务。

### **Virtual** **host**

虚拟地址，用于进行逻辑隔离，最上层的消息路由。

表示一批交换器，消息队列和相关对象。一个Virtual Host里面可以有若干个Exchange和Queue，同一个Virtual Host里面不能有相同名称的Exchange和Queue。

虚拟主机是共享相同的身份认证和加密环境的独立服务器域，每个vhost本质上就是一个mini版本的RabbitMQ服务器，拥有自己的队列、交换器、绑定和权限机制。vhost是AMQP概念的基础，必须在链接时指定，RabbitMQ默认的vhost是“/”。

### **Message**

消息，服务器和应用程序之间传送的数据。消息是不具名的，由Properties和Body组成（消息头和消息体）。Properties可以对消息进行修饰，比如消息的优先级、延迟等高级特性；Body这就是消息体内容。

消息是不透明的，而消息头则有一系列可选属性组成，这些属性包括routing-key（路由键）、property（相对于其他消息的优先权）、delivery-mode（指出消息可能持久性存储）等。

### **Exchange**

交换机，接收生产者发送的消息，根据路由键转发消息到绑定的队列。

三种常见的交换机类型：

1、 direct（发布与订阅，完全匹配）

2、 fanout（广播）

3、 topic（主题，规则匹配）

### **Binding**

绑定。Exchange和Queue之间的虚拟连接，binding中可以包含routing key。

一个绑定就是基于路由键将交换器和消息队列连接起来的路由规则，所以可以将交换器理解成一个由绑定构成的路由表。

### Routing key

路由键。一个路由规则，虚拟机可用它来确定如何路由一个特定消息。

队列通过路由键绑定到交换机。

消息发送到MQ服务器时，消息将拥有一个路由键，即便是空的。RabbitMQ也会将其和绑定使用的路由键进行匹配。

如果匹配，消息将投递到该队列；如果不匹配，消息将会进入黑洞。

### **Connection**

连接，应用程序与Broker的TCP网络连接。

### **Channel**

网络信道，是TCP里面的虚拟连接。几乎所有的操作都在Channel中进行，Channel是进行消息读写的通道。客户端可以建立多个Channel，每个Channel代表一个会话任务（类似数据库中Connection中的session）。例如：电缆相当于TCP，信道是一个独立光纤束，一条TCP连接上创建多条信道是没有问题的。

TCP一旦打开，就会创建AMQP信道。

***\*信道是创建在“真实”TCP上的虚拟连接，AMQP命令都是通过信道发送出去的，每个信道都会有一个唯一的ID，不论是发布消息，订阅队列或者介绍消息都是通过信道完成的\****。

***\*无论是发布消息、接收消息、订阅队列，这些动作都是通过信道完成的\****。

 

​	RabbitMQ为什么需要信道？为什么不是直接通信？

1、 TCP的创建和销毁开销特别大。创建需要3次握手，销毁需要4次分手；

2、 如果不用信道，那应用程序就会以TCP连接RabbitMQ，高峰时每秒成千上万条连接会造成资源巨大浪费，而且操作系统每秒处理TCP连接数也是有限制的，必定造成性能瓶颈；

3、 信道的原理是一条线程一条通道，多条线程多条通道同用一条TCP连接。一条TCP连接可以容纳无限的信道，即使每秒成千上万的请求也不会成为性能瓶颈。

### **Queue**

也称为Message Queue（消费者创建），消息队列，保存消息并将它们转发给消费者。它是消息的容器，也是消息的终点。一个消息可以投入一个或多个队列。消息一直在队列里面，等待消费者连接到这个队列上将其取走。

**交换器和队列的关系：**

交换器是通过路由键和队列绑定在一起的，如果消息拥有的路由键跟队列和交换器的路由键匹配，那么消息就会被路由到该指定的队列中。

也就是说，消息到队列的过程中，消息首先会经过交换器，接下来交换器在通过消息键匹配分发消息到具体的队列中。

路由键可以理解为匹配的规则。

# ***\*RabbitMQ\****

## **背景**

在我们秒杀抢购商品的时候，系统会提醒我们稍等排队中，而不是像几年前一样页面卡死或报错给用户。

像这种排队结算就用到了消息队列机制，放入通道里面一个一个结算处理，而不是某个时间断突然涌入大批量的查询新增把数据库给搞宕机，所以RabbitMQ本质上起到的作用就是削峰填谷，为业务保驾护航。

现在的市面上有很多MQ可以选择，比如ActiveMQ、ZeroMQ、Appche Qpid，那问题来了为什么要选择RabbitMQ？

1、除了Qpid，RabbitMQ是唯一一个实现了AMQP标准的消息服务器；

2、可靠性，RabbitMQ的持久化支持，保证了消息的稳定性；

3、高并发，RabbitMQ使用了Erlang开发语言，Erlang是为电话交换机开发的语言，自带高并发和高可用特性；

4、集群部署简单，正是应为Erlang使得RabbitMQ集群部署变的超级简单；

5、社区活跃度高，根据网上资料来看，RabbitMQ也是首选。

 

## **概述**

​	RabbitMQ（Advanced Message Queuing Protocol，高级消息队列协议）是一个开源的消息代理和队列服务器，用来通过普通协议在完全不同的应用之间共享数据，RabbitMQ是使用Erlang语言来编写的，并且RabbitMQ是基于AMQP协议的。

 

​	RabbitMQ高性能的原因：

​	Erlang语言是一种通用的面向并发的编程语言，最初在于交换机领域的架构模式，这样使得RabbitMQ在Broker之间通过数据交互的性能是非常优秀的。

​	Erlang的优点：Erlang有着和原生Socket一样的延迟	

 

## **架构**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E39.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E3A.tmp.jpg) 

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E4A.tmp.jpg) 

​	注：发消息的时候需要指定发往哪个Exchange，然后就是routing key，一个Exchange可以将消息发往一个绑定的消息队列。

## **安装使用**

## **命令行/控制台**

rabbitmqctl stop_app：关闭应用

rabbitmqctl start_app：启动应用

rabbitmqctl status：节点状态

rabbitmqctl add_user username password：添加用户

rabbitmqctl list_users：列出所有用户

rabbitmqctl delete_user username：删除用户

rabbitmqctl clear_permissions -p vhostpath username：清除用户权限

rabbitmqctl list_user_permissions username：列出用户权限

rabbitmqctl change_password username newpassword：修改密码

rabbitmqctl set_permission -p vhostpath username “.*”“.*”“.*”：设置用户权限

## **生产者消费者模型**

RabbitMQ是发布-订阅模式（可以设置多种订阅模式），Kafka支持点对点（P2P）和发布订阅两种模式。

## **虚拟主机**

​	虚拟地址，用于进行逻辑隔离，最上层的消息路由。

​	一个Virtual Host里面可以有若干个Exchange和Queue。

​	同一个Virtual Host里面不能有相同名称的Exchange和Queue。

## **交换机**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E4B.tmp.jpg) 

​	Exchange交换机，***\*接收消息，并根据路由键转发消息所绑定的队列\****。

​	交换机属性：

​	Name：交换机名称

​	Type：交换机类型direct、topic、fanout、headers

​	Durability：是否需要持久化（true表示需要持久化）

​	Auto Delete：当最后一个绑定到Exchange上的队列删除后，自动删除该Exchange

​	Internal：当前Exchange是否用于RabbitMQ内部使用，默认为false（很多场景都不会用到该设置）

​	Arguments：扩展参数，用于扩展AMQP协议自制定化使用

### **Direc** **Exchange**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E4C.tmp.jpg) 

所有发送到Direct Exchange的消息被转发到RouteKey中指定Queue。

​	注意：Direct模式可以使用RabbitMQ自带的Exchange：default Exchange，所以不需要将Exchange进行任何绑定（binding）操作，消息传递时，RouteKey必须完全匹配才会被队列接收，否则该消息会抛弃。

 

#### 应用场景

​	业务场景：系统日志处理场景

1、 微服务产生的日志，交给日志服务器处理；

2、 日志服务器分别有4个服务，分别为DEBUG，INFO，WARN，ERROR；

3、 服务直接通信采用direct发布订阅的方式。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E4D.tmp.jpg) 

#### 编程

##### 配置

##### 编写消费者

##### 编写生产者

### **Topic** **Exchange**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E5E.tmp.jpg) 

​	所有发送到Topic Exchange的消息被转发到所有关心RouteKey中指定Topic的Queue上。

​	Exchange将RouteKey和某个Topic进行***\*模糊匹配\****，此时队列需要绑定一个Topic。

​	注：可以使用通配符进行模糊匹配。

#### 应用场景

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E5F.tmp.jpg) 

#### 编程

##### 配置

##### 编写消费者

##### 编写生产者

### **Fanout** **Exchange**

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E60.tmp.jpg) 

​	Fanout（***\*群发\****）不处理路由键，只需要简单的将队列绑定到交换机上。

​	发送到交换机的消息都会被转发到与该交换机绑定的所有队列上。

​	***\*Fanout交换机转发消息是最快的\****。

#### 应用场景

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E61.tmp.jpg) 

​	以上是紧耦合的方案，采用消息队列fanout：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E72.tmp.jpg) 

#### 编程

##### 配置

##### 编写消费者

##### 编写生产者

## **绑定**

​	Exchange和Exchange、Queue之间的连接关系。

​	Binding中可以包含RoutingKey或参数。

## **消息队列**

​	消息队列，实际存储消息数据。

​	***\*Durability\****：是否持久化，Durable：是，Transient：否

​	***\*Auto\**** ***\*delete\****：如选yes，代表当最后一个监听被移出之后，该Queue会自动被删除

### **简单工作队列**

​	缺点：耦合性高，生产者一一对应消费者

### **工作队列**

#### Round-robin轮询分发

#### *Fair* Dispatch公平分发

## **消息**

​	服务器和应用程序之间传送的数据。

​	本质就是一段数据，由Properties和Payload（Body）组成。

​	常用属性：delivery mode、headers（自定义属性），其他属性：content_type、content_encoding、priority（优先级，0~9）、correlation_id（一般作为消息唯一ID，ACK、路由、幂等使用）、reply_to、expiration、message_id、timestamp、type、user_id、app_id、cluster_id。

### **消息可靠性**

#### 可靠性投递

​	***\*什么是生产端的可靠性投递？\****

***\*（即保证成功发送，成功接收，成功确认，失败重传）\****

1、 保障消息的成功发出；

2、 保障MQ节点的成功接收；

3、 发送端收到MQ节点（Broker）确认应答ACK；

4、 完善的消息进行***\*补偿机制\****。

#### 解决方案

***\*消息可靠性的解决方法归根结底就是——持久化\****，只有执行了持久化操作才真正可靠了，但是怎么持久化，什么时候持久化，每个公司在落地的时候采用的方案不同。

互联网大厂的解决方案：

**1、** ***\*消息落库\****，对消息状态进行打标；

2、 消息的***\*延迟投递，做二次确认，回调检查\****。

##### 消息落库/持久化

RabbitMQ的队列和交换器默认情况下重启服务器会导致消息丢失，那么怎么保证Rabbit在重启的时候不丢失呢？答案就是消息持久化。

消息持久化就是为了防止出现宕机的故障，导致消息的投递状态消失（比如我们已经投递了但是宕机导致信息丢失了）。

当你把消息发送到Rabbit服务器的时候，你需要选择你是否要进行持久化，但这并不能保证Rabbit能从崩溃中恢复，想要Rabbit消息能恢复必须满足3个条件：

**1、*****\*投递消息的时候durable设置为true，消息持久化\****；

2、消息已经到达持久化交换器上；

3、消息已经到达持久化的队列；

***\*持久化工作原理\*******\*：\****

Rabbit会将你的持久化消息写入磁盘上的持久化日志文件，等消息被消费之后，Rabbit会把这条消息标识为等待垃圾回收。

***\*持久化的缺点\*******\*：\****

消息持久化的优点显而易见，但缺点也很明显，那就是***\*性能\****，因为要写入硬盘要比写入内存性能较低很多，从而***\*降低了服务器的吞吐量\****，尽管使用SSD硬盘可以使事情得到缓解，但他仍然吸干了Rabbit的性能，当消息成千上万条要写入磁盘的时候，性能是很低的。

所以使用者要根据自己的情况，选择适合自己的方式。

 

消息信息落库（即***\*消息持久化\****），对消息状态进行打标：

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E73.tmp.jpg) 

​	注：这种方案需要对数据库进行***\*两次持久化\****操作。

 

##### 延迟投递

​	***\*消息落库在高并发场景下，数据库IO压力大，不适用\****。***\*互联网大厂一般采用的是延迟投递，做二次检查，回调检查。\****

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E74.tmp.jpg) 

​	注：upstream表示生产端，downstream表示消费端。

1、 首先，数据库持久化，然后发送first send消息；

2、 同时发送一个延迟的检查消息（检查第一次发送消息消费情况）；

3、 消费端消费消息；

4、 消费端发送一个确认消息给Broker；

**5、** ***\*回调服务检测到消费端的确认消息，进行数据库的状态持久化（这样相当于数据库一次操作，异步持久化）\****；

6、 回调服务响应第二个延时消息，确认消息成功消费，如果出现异常，回调服务调用RPC给生产者，再次发送。

### **消息幂等性**

​	**幂等性即对数据进行若干次操作，仍然保证正确**。

#### 消费端幂等性保障/消息重复消费

​	在海量订单产生的业务高峰期，如何避免消息的重复消费问题？

​	消费端实现幂等性，就意味着，我们的消息永远不会消费多次，即使收到多条一样的消息。

 

#### 解决方案

​	业界主流的幂等性操作：

##### 唯一ID+指纹码机制

**1、** ***\*唯一ID+指纹码机制\****，利用数据库主键去重（常用）；

使用唯一ID不能保证唯一性（用户可能在短时间内执行多次消费），还需要一个指纹码（这个可能是业务规则，比如时间戳等生成的），执行SQL：

SELECT COUNT(1) FROM T_ORDER WHERE ID=唯一ID+指纹码

如果返回0可以执行ISNERT操作，如果返回1则不执行。

注：这里需要的唯一ID可以通过雪花算法等获取分布式唯一ID。

***\*唯一ID的分配可以有几种方式：\****

1、由一个统一的ID分配中心来分配（性能稍差）。

2、由上游服务来生成唯一ID，但必须保证不产生冲突的ID（性能较好）。

采用统一的分配中心来分配唯一ID时，业务方每次调用接口都多了一次调用分配中心获取唯一ID的请求，这增加了额外的开销。获取唯一ID有一种方式，是借助mysql的自增索引，这其实也是一个ID分配中心。***\*对服务性能有苛刻要求时，可以采用第二种方式，由主调服务本身来生成这个唯一ID\****。为了保持不会产生重复的ID，可以使用一下几种ID生成方法：***\*UUID和Snowflake\****。

1、UUID是一个128bit的数字，用于标志计算机的信息，虽然UUID不能保证绝对不重复，但重复的概率小到可以被忽略。UUID的生成没有什么规律，为了保证UUID的唯一性，规范定义了包括网卡MAC地址、时间戳、名字空间（Namespace）、随机或伪随机数、时序等元素，以及从这些元素生成UUID的算法。这也就意味着：

 

128bit，占据了太多的内存空间

生成的ID不是人可以看懂的

无法保证ID的递增，某些场景需要按前后排序无法满足。

2、Snowflake

这是Twitter的一个开源项目，它是一个分布式ID的生成算法，它会产生一个long类型的唯一ID，其核心算法是：

时间部分：41bit作为毫秒数，大概可以使用69.7年

机器编号部分：10bit作为机器编号，支持1024个机器实例。

毫秒内的序列号：12bit，一毫米可以生成4096个序列号

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E84.tmp.jpg) 

 

**优点：**实现简单

**缺点：**高并发下有数据库写入的性能瓶颈

**解决方案：**跟进ID进行分库分表进行算法路由

**举例：**在云桌面应用中，控制中心选择虚机的时候，使用虚机的uuid防止重复选择。

**使用建议：**

1、共享存储

如果我们的幂等性服务是分布式的，那么存储唯一ID也需要采用共享的存储，这样每个服务就是无状态的了。可以使用mysql来存储，也可以使用k-v存储例如redis。

2、避免不必要的查询

并不是所有的请求都是重复的，生产环境下可能99%的请求都不是重复请求。如果每个请求在执行前都要去查询下唯一ID是否存在，可能会带来不必要的性能消耗。如果你使用mysql来存储唯一ID，那么可以直接进行insert，通过结果来判断是否插入记录成功，如果不成功则证明ID已经存在：

​	insert into ... values ... on DUPLICATE KEY UPDATE ...

而如果使用的是redis，也可以使用redis的setEx，设置成功则证明key不存在，否则key存在说明是重复请求。

 

##### 利用Redis的原子性实现

**2、** ***\*利用Redis的原子性实现\****。

使用Redis进行幂等，需要考虑的问题：

1、 我们是否要进行数据落库，如果落库的话，关键解决的问题是数据库和缓存如何做到原子性？

2、 如果不进行落库，那么都存储在缓存中，如何设置定时同步的策略？

 

### **消息投递**

#### Confirm确认消息

​	消息的确认，是指生产者投递消息后，如果Broker收到消息，则会给我们生产者一个应答。

​	生产者进行接收应答，用来确定这个消息是否正常的发送到Broker，这种方式也是消息的可靠性投递的核心保证！

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E85.tmp.jpg) 

​	如何实现Confirm确认消息？

1、 在channel上开启确认模式：channel.confirmSelect()

2、 在channel上添加监听addConfirmListener，监听成功和失败的返回结果，根据具体的结果对消息进行重新发送、或记录日志等后续处理。

#### Return返回消息

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E86.tmp.jpg) 

​	Return Listener用于处理一些不可路由的消息！

​	我们的消息生产者通过指定一个Exchange和RoutingKey，把消息送到到某一个队列中去，然后我们的消费者监听队列，进行消费处理操作。

​	但是在某些情况下，如果我们在发送消息的时候，当前的Exchange不存在或者指定的路由key路由不到，这个时候如果我们需要监听这种不可达的消息，就要使用Return Listener。

 

​	在基础API中有一个关键的配置项：

​	Mandatory：如果为true，则监听器会接收到路由不可达的消息，然后进行后续处理，如果为false，那么broker端自动删除该消息。

### **自定义消费者使用**

​	我们一般在代码中编写while循环，进行consumer.nextDelivery方法进行获取下一条消息，然后进行消费处理。但是，使用自定义的Consumer更加方便，解耦性更强，也是在实际工作中最常用的使用方式。

![img](file:///C:\Users\大力\AppData\Local\Temp\ksohtml\wps3E87.tmp.jpg) 

### **消息端限流策略**

​	***\*什么是消费端的限流？\****

​	假设这样一个场景，首先，RabbitMQ服务器有上万条未处理的消息，我们随便打开一个消费者客户端，会出现下面情况：

​	巨量的消息瞬间全部推送过来，但是我们单个客户端无法同时处理这么多数据。

​	RabbitMQ提供了一种qos（服务质量保证）功能，即在非自动确认消息的前提下，如果一定数目的消息（通过基于consumer或者channel设置Qos的值）未被确认前，不进行消费新的消息。

​	原型：void BasicQos(uint prefetchSize, ushort prefetchCount, bool global);

​	参数：

​	prefetchSize：0

​	prefetchCount：会告诉RabbitMQ不要同时给一个消费者推动多于N个消息，即一旦有N个消息还没有ACK，则该consumer将block掉，直到有消息ACK

​	global：true/false是否将上面设置应用于整个channel，即限制是channel级别的还是consumer级别

​	prefetchSize和global这两项，RabbitMQ没有实现，暂时不研究。prefetch_count在no_ack=false的情况下生效，即在自动应答的情况下这两个值是不生效的。

### **消息端ACK** 

​	***\*消费端的手工ACK和NACK：\****

​	消费端进行消费的时候，如果由于业务异常我们可以进行日志的记录，然后进行补偿。

​	如果由于服务器宕机等严重问题，那我们就需要手工进行ACK保障消费端消费成功。

 

​	消息确认ACK：如果在处理消息的过程中，消费者的服务器在处理消息时出现异常，那可能这条正在处理的消息就没有完成消息消费，数据就会丢失。为了确保数据不会丢失，RabbitMQ支持消息确认ACK。

​	ACK的消息确认机制是消费者从RabbitMQ收到消息并处理完成后，反馈给RabbitMQ，RabbitMQ收到反馈后才将此消息从队列中删除。

1、 如果一个消费者在处理消息出现了网络不稳定、服务器异常等现象，那么就不会有ACK反馈，RabbitMQ会认为这个消息没有正常消费，会将消息重新放入队列中。

2、 如果在集群的情况下：RabbitMQ会立即将这个消息推送给这个在线的其他消费者。这种机制保证了在消费者服务器故障的时候，不丢失任何消息和任务。

3、 消息永远不会从RabbitMQ中删除：只有当消费者正确发送ACK反馈，RabbitMQ确认收到后，消息才会从RabbitMQ服务器的数据中删除；

4、 消息的ACK确认机制默认是打开的。

ACK机制的开发注意事项：

如果忘记了ACK，那么后果很严重。当Consumer退出时，Message会一直重新分发，然后RabbitMQ会占用越来越多的额内存，由于RabbitMQ会长时间运行，因此这个“内存泄露”是致命的。

### **重回队列机制**

​	消费端重回队列是为了对没有处理成功的消息，把消息重新递给Broker。

​	一般在实际应用中，都会关闭重回队列，也就是设置为false。

### **TTL队列/消息**

​	TTL是Time To Live的缩写，也就是生存时间。

​	RabbitMQ支持消息的过期时间，在消息发送时可以进行指定。

​	RabbitMQ支持队列的过期时间，从消息入队列开始计算，只要超过了队列的超时时间配置，那么消息会自动的清除。

### **死信队列**

​	死信队列（DLX，Dead-Letter-Exchange）。利用DLX，当消息在一个队列中变成死信（dead message）之后，它能被重新publish到另一个Exchange，这个Exchange就是DLX。

​	消息变成死信的几种情况：

1、 消息被拒绝（basic.reject/basic.nack）并且request=false；

2、 消息TTL过期；

3、 队列达到最大长度。

DLX也是一个正常的Exchange，和一般的Exchange没有区别，它能在任何的队列上被指定，实际上就是设置某个队列的属性。

当这个队列中有死信时，RabbitMQ就会自动的将这个消息重新分布到设置的Exchange上去，进而被路由到另一个队列。

可以监听这个队列中消息做相应的处理，这个特性可以弥补RabbitMQ3.0以前的immediate参数的功能。

死信队列的设置：

1、 首先，需要设置死信队列的Exchange和queue，然后进行绑定：

Exchange：dlx.exchange

Queue：dlx.queue

RoutingKey:#

2、 然后，我们进行正常声明交换机、队列、绑定，只不过我们需要在队列机上一个参数即可：arguments.put(“x-dead-letter-exchange”,”dlx.exchange”);这样消息在过期、requeue、队列在达到最大长度时，消息就可以直接路由到死信队列。

 

# *Spring* AMQP

# *RabbitMQ与Spring* Boot整合

# *RabbitMQ与Spring* cloud整合

# RabbitMQ集群

## **架构**

## **镜像队列**

## **负载均衡**

## **高可用**

## **配置文件**

## **集群恢复/故障转移**

## **延迟队列插件**

# SET单元化架构

## **设计策略**

## **原则**

## **实现**

# MQ消息组件设计思想