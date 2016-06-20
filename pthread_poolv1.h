#include<pthread.h>
#include<vector>
#include<list>
#ifndef _PTHREAD_POOL_V1_

#define ERROR_DIE(str, num) {cout<<"The program die in "<<str<<" exit "<<num<<"!!"<<endl; exit(num);}  //���λ��?
#define MSG_PRINT(str){cout<<str<<endl;}

using namespace std;
//typedef void*(*)(void) func;
//typedef void* PoolMsg

class ThreadAbstractClass{
	public:
		virtual void* run(void);
};


class ThreadPoolAbstractClass{
	public:
		virtual int startUp()=0;
		virtual int waitPool()=0;
		virtual int reStart()=0;
		virtual int endPool()=0;
		virtual int enqueue(ThreadAbstractClass)=0;
};

class ThreadPool:public ThreadPoolAbstractClass{
	public:
		//���캯���Ķ����Ƿ�������ʼ���б��ֿ�����cppʵ�֣�
		//�˴�mtx��cond��isCanceled��ִ��Ĭ�ϳ�ʼ����Ȼ����startUp����и�ֵ
		ThreadPool(int threadsNumber):isWaited(0), isCancelled(0), threads(threadsNumber){}
		//ThreadPool:threads(threadsNumber), isFree(isFree)(int threadsNumber, bool isFree){}
		~ThreadPool(){}
		int startUp();//����pool���߳������ȴ���Ϣ
		int endPool();//�ȴ�����ִ�е�Tִ�н�����ʣ����Ϣ���ٴ������ر��̳߳ص��߳�
		int waitPool();//�����ٽ�������ʱ���øú��������������߳�ֱ����Ϣ���ж���������
		//int enqueue(PoolMsg msg, func operation, bool isFree);//	��Ϣ������ӣ���ָ����Ϣ����Ϣ��������
		int reStart();
		int enqueue(ThreadAbstractClass athread);
	private:
		/*
		struct DataNode{
			PoolMsg msg;
			func afunc;
			bool isFree;

		};
		*/
		int runThread();
		vector<pthread_t> threads;//�����ʵ��ϸ��
		/*Υ��DIP*/
		//list<DataNode> nodes;
		list<ThreadAbstractClass> tasks;
		/*
		list<PoolMsg> msgs;//
		list<func> funcs;
		list<bool> isFree;
		*/
		pthread_mutex_t mtx;
		pthread_cond_t cond;
		bool isWaited;
		bool isCancelled;//��Ӧ�öԸ�poolֻ��һ��T�б�ʹ��������裬�����T�и�pool������ʱ��һ��T����wait����һ��Ӧ�ÿ���ͨ������end��wait��ǰ����������������ֳ�����ƴ���
};

#endif