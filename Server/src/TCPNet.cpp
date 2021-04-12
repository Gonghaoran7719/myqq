#include <TCPNet.h>
#include <TCPKernel.h>

TcpNet * TcpNet::m_pThis = 0;
int TcpNet::InitNetWork()
{
    pool_t *pool = NULL;
    m_pool = new thread_pool;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    if(inet_pton(AF_INET,_DEF_SERVERIP,&serveraddr.sin_addr.s_addr) == -1)
    {
        perror("Init Ip Error:");
        return FALSE;
    }
    serveraddr.sin_port = htons(_DEF_PORT);
    //创建Socket
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("Create Socket Error:");
        return FALSE;
    }
    int mw_optval;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char*)&mw_optval,sizeof(mw_optval));
    //绑定端口号
    if(bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1)
    {
        perror("Bind Socket Error:");
        return FALSE;
    }
    //监听socket
    if(listen(sockfd,_DEF_LISTEN) == -1)
    {
        perror("Listen Error:");
        return FALSE;
    }
    epfd = epoll_create(_DEF_EPOLLSIZE);//创建epoll的句柄，可以监听的文件描述符为_DEF_EPOLLSIZE

    Addfd(sockfd,TRUE);
    //创建拥有10个线程的线程池 最大线程数100 环形队列最大值50
    if((pool = (m_pool->Pool_create(200,10,50))) == NULL)
        err_str("Create Thread_Pool Failed:",-1);

    m_pool->Producer_add(pool, EPOLL_Jobs, pool);
    return TRUE;
}

void TcpNet::Addfd(int fd,int enable_et/*是否为边缘触发*/)
{
    struct epoll_event eptemp;
    eptemp.events = EPOLLIN;
    eptemp.data.fd = fd;
    if(enable_et)
        eptemp.events |= EPOLLET;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&eptemp);
}
void TcpNet::Deletefd(int fd)
{
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,0);
}

void * TcpNet::EPOLL_Jobs(void * arg)
{
    pool_t *pool = (pool_t*)arg;
    int ready;
    int i = 0;
    while(1)
    {
        //阻塞-1监听socket
        printf("%d\n",i++);
        if((ready = epoll_wait(m_pThis->epfd,m_pThis->epollarr,_DEF_EPOLLSIZE,-1)) == -1)
            err_str("Epoll Call Failed:",-1);//出错
        m_pThis->Epoll_Deal(ready,pool);
        bzero(m_pThis->epollarr,sizeof(epollarr));
    }
}

void TcpNet::Epoll_Deal(int ready,pool_t *pool)
{
    int i = 0;
    for(i=0; i<ready; i++)
    {
        int fd = epollarr[i].data.fd;
        if(sockfd == fd)   //客户端建立链接
            m_pool->Producer_add(pool,Accept_Deal,NULL);
        else if(epollarr[i].events & EPOLLIN)
        {
            Deletefd(fd);
            m_pool->Producer_add(pool,Info_Recv,(void*)fd);
        }
    }
}

void *TcpNet::Accept_Deal(void *arg)
{
    struct sockaddr_in clientaddr;
    int clientsize = sizeof(clientaddr);
    int clientfd;
    char ipstr[_DEF_IPSIZE];
    pthread_mutex_lock(&m_pThis->alock);
    if((clientfd = accept(m_pThis->sockfd,(struct sockaddr*)&clientaddr,(socklen_t*)&clientsize)) == -1)
    {
        err_str("Custom Thread Accept Error",-1);
    }
    pthread_mutex_unlock(&m_pThis->alock);
    m_pThis->Addfd(clientfd,TRUE);
    printf("Custom Thread TID:0x%x\tClient IP:%s\tClient PORT:%d\t\n",(unsigned int)pthread_self()
           ,inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,sizeof(ipstr)),ntohs(clientaddr.sin_port));
    return 0;
}

void *TcpNet::Info_Recv(void *arg)
{
    int clientfd = (long)arg;
    int nRelReadNum = 0;
    int nPackSize = 0;
    char *pSzBuf = NULL;
    nRelReadNum = recv(clientfd,&nPackSize,sizeof(nPackSize),0);
    if(nRelReadNum <= 0)
    {
        close(clientfd);
        return NULL;
    }
    pSzBuf = (char*)malloc(sizeof(char)*nPackSize);
    int nOffSet = 0;
    nRelReadNum = 0;
    //接收包的数据
    while(nPackSize)
    {
        nRelReadNum = recv(clientfd,pSzBuf+nOffSet,nPackSize,0);
        if(nRelReadNum > 0)
        {
            nOffSet += nRelReadNum;
            nPackSize -= nRelReadNum;
        }
    }
    m_pThis->m_kernel->DealData(clientfd,pSzBuf,nOffSet);
    m_pThis->Addfd(clientfd,TRUE );
    printf("pszbuf = %p \n",pSzBuf);
    if(pSzBuf != NULL)
    {
        free(pSzBuf);
        pSzBuf = NULL;
    }
    return 0;
}





int TcpNet::SendData(int clientfd,char* szbuf,int nlen)
{
    if(send(clientfd,(const char *)&nlen,sizeof(int),0)<0)
        return FALSE;
    if(send(clientfd,szbuf,nlen,0)<0)
        return FALSE;
    return TRUE;
}



void TcpNet::UnInitNetWork()
{
    close(sockfd);
}
