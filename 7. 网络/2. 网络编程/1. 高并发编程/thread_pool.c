//头插法
#define LL_ADD(item,list) do {	\
	item->prev = NULL;	\
	item->next = list;	\
	if (list != NULL)	list->prev = item;	\
	list = item;	\
}while(0)
//在预编译时,调用处最后加分号
	
#define LL_REMOVE(item,list) do{	\
	if (item->prev != NULL)	item->prev->next = item->next;	\
	if (item->next != NULL) item->next->prev = item->prev;	\
	if (list == item) list = item->next;	\
	item->prev = item->next = NULL;	\
}while(0)

// 任务队列
struct NJOB{
	void (*func)(void *arg);	//回调函数
	void *user_data;	//用户信息
	
	struct NJOB *prev;
	struct NJOB *next;
};

// 工作队列
struct NWORKER{
	pthread_t id;
	int terminate;//决定任务生存时间
	struct NWORKER *pool;
	
	struct NWORKER *prev;
	struct NWORKER *next;
};

typedef struct NMANAGER{
	pthread_mutex_t mtx;	//每次从任务队列取数据需要加锁
	pthread_cond_t cond;	//定义线程挂起状态(如果任务队列不存在任务的时候，则线程挂起等待)
	
	struct NJOB *jobs;
	struct NWORKER *workers;
}nThreadPool;

//每个执行队列的具体任务
void *thread_cb(void *arg)
{
	struct NWORKER *worker = (struct NWORKER*)arg;
	while(1)
	{
		//临界资源加锁
		pthread_mutex_lock(&worker->pool->mtx);
		while(NULL == worker->pool->jobs)
		{
			if(worker->pool->terminate)break;
			pthread_cond_wait(worker->pool->cond,worker->pool->mtx);
		}
		
		//处理任务队列
		struct NJOB *job = worker->pool->jobs;
		if(NULL!=job)
		{
			LL_REMOVE(job,worker->pool->jobs);//从任务队列删除
		}
		pthread_mutex_unlock(&worker->pool->mtx);
		
		job->func(job);		
	}
}

// 创建线程池(就是对结构体nThreadPool里面的变量初始化)
int nThreadPoolCreate(nThreadPool pool, int numWorkers){
	//判断大小
	if (numWorkers < 1)	numWorkers = 1;
	if (pool == NULL) return -1;
	memset(pool,0,sizeof(nThreadPool));
	
	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITILIZER;
	memcpy(&pool->mtx, &blank_mutex, sizeof(pthread_mutex_t));
	
	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
	memcpy(&pool->cond, &blank_cond, sizeof(pthread_cond_t));//初始化
	
	//listner(有时候会增加一个监听线程去监听任务队列)
	//workers
	int i = 0;
	for (i=0; i<numWorkers; i++){
		struct NWORKER *worker = (struct NWORKER*)malloc(sizeof(struct NWORKER));
		if(NULL==worker)
		{
			perror("malloc error");
			return -2;
		}
		memset(worker,0,sizeof(struct NWORKER));
		worker->pool = pool;
		
		pthread_create(&worker->id,NULL,thread_cb,worker);
		LL_ADD(worker,pool->workers);//将该线程加入到线程池
	}
	return 0;
}
// 销毁线程池
int nThreadPoolDestroy(){
	
}
// 队列中添加任务
int nThreadPoolPushTask(nThreadPool *pool, struct NJOB *job){
	pthread_mutex_lock(pool->mtx);//加锁保护临界资源
	LL_ADD(job,pool->jobs);
	pthread_cond_signal(pool->cond);//添加新的任务需要手动触发信号通知工作线程
	pthread_mutex_unlock(pool->mtx);
}



