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
	
	struct NWORKER *prev;
	struct NWORKER *next;
};

typedef struct NMANAGER{
	pthread_mutex_t mtx;	//每次从任务队列取数据需要加锁
	pthread_cond_t cond;	//定义线程挂起状态(如果任务队列不存在任务的时候，则线程挂起等待)
	
	struct NJOB *jobs;
	struct NWORKER *workers;
}nThreadPool;

// 创建线程池
int nThreadPoolCreate(nThreadPool pool, int numWorkers){
	if (numWorkers < 1)	numWorkers = 1;
	if (pool == NULL) return -1;
	memset(pool,0,sizeof(nThreadPool));
	
	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITILIZER;
	memcpy(&pool->mtx, &blank_mutex, sizeof(pthread_mutex_t));
	
	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
	memcpy(&pool->cond, &blank_cond, sizeof(pthread_cond_t));
	
	//workers
	int i = 0;
	for (i=0; i<numWorkers; i++){
		
	}
}
// 销毁线程池
int nThreadPoolDestroy(){
	
}
// 队列中添加任务
int nThreadPoolPushTask(){
	
}



