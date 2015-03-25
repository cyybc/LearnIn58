#include "cwebserver.h"

//输出debug日志
void CWebServer::PrintDebug(client_information* pclient, string event)
{
    TestValueError(pclient, true, NULL == pclient, event + " in PrintDebug");
    int client_sockfd = pclient->sockfd;
    pthread_rwlock_wrlock(&m_debug_rwlock);
    ofstream out("../logs/debug.log", ios::app);
    if (!out.is_open())
    {
        PrintError(pclient, "debug.log open error");
        out.close();
        return;
    }
    time_t ticks = time(NULL);
    struct tm *ptime = localtime(&ticks);
    TestValueError(pclient, true, NULL == ptime, event + " in PrintDebug");
    char out_time[BUFFSIZE];
    strftime(out_time, sizeof(out_time), "%F %X  ", ptime);
    out << "DEBUG  " << pclient->log_id << "  " << out_time << " ";
    out << "client_sockfd=" << client_sockfd << "  ";
    out << "pthread_id=" << pthread_self() << "  ";
    out << "event=" << event << "  ";
    out << "\n";
    out.close();
    pthread_rwlock_unlock(&m_debug_rwlock);
}

//输出用户日志
void CWebServer::PrintLog(client_information* pclient, string event)
{
    TestValueError(pclient, true, NULL == pclient, event + " in PrintLog");
    int client_sockfd = pclient->sockfd;
    pthread_rwlock_wrlock(&m_log_rwlock);
    ofstream out("../logs/log.log", ios::app);
    if (!out.is_open())
    {
        PrintError(pclient, "log.log open error");
        out.close();
        return;
    }
    time_t ticks = time(NULL);
    struct tm *ptime = localtime(&ticks);
    TestValueError(pclient, true, NULL == ptime, event + " int PrintLog");
    char out_time[BUFFSIZE];
    strftime(out_time, sizeof(out_time), "%F %X  ", ptime);
    out << "LOG  " << pclient->log_id << "  " << out_time << " ";
    out << "client_sockfd=" << client_sockfd << "  ";
    out << "event=" << event << "  ";
    out << "\n";
    out.close();
    pthread_rwlock_unlock(&m_log_rwlock);
}

//输出错误日志
void CWebServer::PrintError(client_information* pclient, string error_reason)
{
    if (NULL == pclient)
    {
        pthread_rwlock_wrlock(&m_error_rwlock);
        ofstream out("../logs/error.log", ios::app);
        if (!out.is_open())
        {
            cout<<"error.log open error"<<endl;
            out.close();
            pthread_rwlock_unlock(&m_error_rwlock);
            exit(1);
        }
        time_t ticks = time(NULL);
        struct tm *ptime = localtime(&ticks);
        if (NULL == ptime)
        {
            out << "ERROR  " <<  errno << " ";
            out << "reason=" << "pclient is NULL and time init is error in ";
            out << error_reason  << "\n";
            out.close();
            pthread_rwlock_unlock(&m_error_rwlock); 
            exit(1);
        }
        char error_time[BUFFSIZE];
        strftime(error_time, sizeof(error_time), "%F %X", ptime);
        out<< "ERROR " << errno << " " << error_time << " ";
        out<< "reason=" << "pclient in is NULL ";
        out<< error_reason << "\n";
        out.close();
        pthread_rwlock_unlock(&m_error_rwlock);
        exit(1);
    }
    int client_sockfd = pclient->sockfd;
    pthread_rwlock_wrlock(&m_error_rwlock);
    ofstream out("../logs/error.log", ios::app);
    if (!out.is_open())
    {
        cout<<"error.log open error"<<endl;
        out.close();
        return;
    }
    time_t ticks = time(NULL);
    struct tm *ptime = localtime(&ticks);
    if (NULL == ptime)
    {
        out << "ERROR  " <<  errno << " "<< pclient->log_id << " ";
        out << "client_sockfd=" << client_sockfd << "  ";
        out << "reason=" << "time init is error in";
        out << error_reason << "\n";
        out.close();
        pthread_rwlock_unlock(&m_error_rwlock); 
        exit(1);
    }
    char error_time[BUFFSIZE];
    strftime(error_time, sizeof(error_time), "%F %X", ptime);
    out << "ERROR  " <<  errno << " "<< pclient->log_id << "  " << error_time << " ";
    out << "client_sockfd=" << client_sockfd << "  ";
    out << "reason=" << error_reason << "\n";
    out.close();
    pthread_rwlock_unlock(&m_error_rwlock);
}

//检测返回值是否正确
void inline CWebServer::TestValueError(client_information* pclient, bool exValue, bool acValue, string error_reason)
{
    if (exValue == acValue) 
    {
        PrintError(pclient, error_reason);
        exit(1);
    }
}
//这里是检测包的头是否是我想要的
bool inline CWebServer::TestWord(client_information* pclient, int len, string wanted, char* pRealValue)
{
    TestValueError(pclient, true, NULL == pRealValue, "pRealValue in TestWord is NULL");
    for (int i = 0; i <len; i++)
    {
        if (wanted[i] >= 97 && wanted[i] <= 122)
        {
            wanted[i] = wanted[i] - 32;
        }
        if (wanted[i] != pRealValue[i] && wanted[i]+32 != pRealValue[i]) return false;
    }
    return true;
}

CWebServer::CWebServer()
{
    Init();
    EpollWait();
}

CWebServer::~CWebServer()
{
    TestValueError(m_no_client, true, -1 == close(m_server_sockfd), "server socket close error");
    TestValueError(m_no_client, true, -1 == close(m_epoll_fd), "epoll fd close error");
    TestValueError(m_no_client, false, 0 == pthread_rwlock_destroy(&m_error_rwlock), "error rwlock destroy error");
    TestValueError(m_no_client, false, 0 == pthread_rwlock_destroy(&m_log_rwlock), "log rwlock destroy error");
    TestValueError(m_no_client, false, 0 == pthread_rwlock_destroy(&m_debug_rwlock), "debug rwlock destroy error");
    delete(m_no_client);
    m_no_client = NULL;
}

void CWebServer::Init()
{
    m_log_id = 0;
    m_no_client = new(client_information);
    m_no_client->sockfd = -1;
    m_no_client->log_id = -1;
    TestValueError(m_no_client, false, 0 == pthread_rwlock_init(&m_error_rwlock, NULL), "error rwlock init error");
    TestValueError(m_no_client, false, 0 == pthread_rwlock_init(&m_log_rwlock, NULL), "log rwlock init error");
    TestValueError(m_no_client, false, 0 == pthread_rwlock_init(&m_debug_rwlock, NULL), "debug rwlock init error");
    //创建socket 
    m_server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    TestValueError(m_no_client, true, -1 == m_server_sockfd, "server socket creat error");
    //绑定端口bind
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT_NUM);
    TestValueError(m_no_client, true, -1 == bind(m_server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)), "server socket bind error");
    //开始监听listen
    TestValueError(m_no_client, true, -1 == listen(m_server_sockfd, BACKLOG), "server socket listen error");
    //创建epoll
    m_epoll_fd = epoll_create(MAXEVENTS);
    TestValueError(m_no_client, true, -1 == m_epoll_fd, "epoll fd create error");
    //注册服务器端可读事件server ctl
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = m_server_sockfd;
    TestValueError(m_no_client, true, -1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_server_sockfd, &ev), "server epoll_ctl error");
}

void CWebServer::EpollWait()
{
    int nfds;
    struct epoll_event ev, events[MAXEVENTS];

    while (1)
    {
        nfds = epoll_wait(m_epoll_fd, events, MAXEVENTS, 0);
        TestValueError(m_no_client, true, -1 == nfds, "nfds error");
        for (int i = 0; i < nfds; i++)
        {
            //如果是服务器端可读
            if (events[i].data.fd == m_server_sockfd)
            {
                CWebServer::AcceptRequest();
                
            }
            //如果是客户端可读
            else if (events[i].events & EPOLLIN)
            {
                //删除该客户端监听client ctl_del
                client_information* pclient  = (client_information*)events[i].data.ptr;
                int client_sockfd = pclient->sockfd;
                ev.events = EPOLLIN;
                ev.data.fd = client_sockfd;
                TestValueError(pclient, true, -1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, client_sockfd, &ev), "client ctl_del error");
                //开一个线程去跑
                pthread_t tid;
                TestValueError(pclient, false, 0 == pthread_create(&tid, NULL, CWebServer::RecvAndSend, (void *)pclient), "pthread create error");
            }
        }
    }
}
//处理连接请求
void CWebServer::AcceptRequest()
{
    int client_sockfd;
    //接受连接请求accept
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    client_sockfd = accept(m_server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    client_information *pclient = new(client_information);
    pclient->sockfd = client_sockfd;
    pclient->log_id = m_log_id++;
    TestValueError(pclient, true, -1 == client_sockfd, "accept error");

    char event_message[BUFFSIZE];
    snprintf(event_message, sizeof(event_message), "%s %s", "accept client", inet_ntoa(client_addr.sin_addr));
    //PrintLog(pclient, event_message);
    //PrintDebug(pclient, event_message);

    //注册客户端可读的监听client ctl
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = pclient;
    //ev.data.fd = client_sockfd;
    TestValueError(pclient, true, -1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev), "client epoll_ctl error");

}
//处理收发消息
void* CWebServer::RecvAndSend(void* ppclient)
{
    client_information* pclient = (client_information *)ppclient; 
    TestValueError(pclient, true, NULL == pclient, "pclient in RecvAndSend is NULL");
    int client_sockfd = pclient->sockfd;
    int recv_len;
    char recv_buff[BUFFSIZE], else_buff[BUFFSIZE];

    //接收消息recv
    recv_len = recv(client_sockfd, recv_buff, BUFFSIZE, 0);
    TestValueError(pclient, true, -1 == recv_len, "recv error");
    
    //循环读入，直到没有消息
    while (BUFFSIZE == recv_len) 
    {
        recv_len = recv(client_sockfd, else_buff, BUFFSIZE, 0);
        TestValueError(pclient, true, -1 == recv_len, "recv error");
    }
    //PrintDebug(pclient, "recv message");
    message_package* ptrmes = DealData(pclient, recv_len, &recv_buff[0]);
    //发送消息send
    int len = ptrmes->mes_len;
    char* pmes = ptrmes->pmes;
    int k = 0, now = 0;
    now = send(client_sockfd, pmes, (len - k), 0);
    fflush(stdout);
    while (now  != (len - k))
    {
        TestValueError(pclient, true, -1 == len, "send error");
        k = k + now;
        pmes = pmes + k;
        len = len - now;
        now = send(client_sockfd, pmes, (len - k), 0);
        fflush(stdout);
    }
    TestValueError(pclient, true, NULL == ptrmes->pmes, "ptrmes->mes is NULL at delete");
    free(ptrmes->pmes);
    ptrmes->pmes = NULL;
    TestValueError(pclient, true, NULL == ptrmes, "ptrmes is NULL at delete");
    delete(ptrmes);
    ptrmes = NULL; 
    //PrintDebug(pclient, "send message");

    //关闭客户端连接close
    TestValueError(pclient, true, -1 == close(client_sockfd), "close client error");
    //PrintLog(pclient, "close client ");
    //PrintDebug(pclient, "close client");
    TestValueError(pclient, true, NULL == pclient, "pclient is NULL in RecvAndSend at delete");
    delete(pclient);
    pclient = NULL;

    pthread_exit(0);

    return ((void *)0);
}

//解析url
message_package* CWebServer::DealData(client_information* pclient, int recv_len, char* precv_buff)
{
    TestValueError(pclient, true, NULL == pclient, "pclient in DealData is NULL");
    TestValueError(pclient, true, NULL == precv_buff, "precv_buff in DealData is NULL");
    //头的部分
    int now = 0,next = 0, url_len = 0;
    char *purl = NULL, *ptype = NULL, *ptr = NULL;
    if (now + 3 < recv_len && TestWord(pclient, 4, "Get ", &precv_buff[now]))
    {
        now += 3;
    }
    else if (now + 4 < recv_len && TestWord(pclient, 4, "Post ", &precv_buff[now]))
    {
        now += 4;
    }
    else if (now + 4 < recv_len && TestWord(pclient, 4, "Head ", &precv_buff[now]))
    {
        now += 4;
    }
    else  
    {
        //cout<<"fuck"<<endl;
        //PrintDebug(pclient, "Not Get or Post or Head");
        return CWebServer::CombineErrorPackage(pclient, BADREQUEST); 
    }

    //文件路径
    while (now < recv_len && ' ' == precv_buff[now]) now++;
    if (now < recv_len && '/' == precv_buff[now]) now++;
    next = now;
    while (next < recv_len && precv_buff[next] != ' ') next++;
    if (now == recv_len || next == recv_len) 
    {
        //cout<<"url error"<<endl;
        return CWebServer::CombineErrorPackage(pclient, BADREQUEST); 
    }
    url_len = next - now + CHANGEURLLEN;
    purl = (char *)malloc(url_len);
    TestValueError(pclient, true, NULL == purl, "url memory alloc error");
    for (int i = 0; i < CHANGEURLLEN; i++) purl[i] = CHANGEURL[i]; 
    for (int i = CHANGEURLLEN; i < url_len; i++) 
    {
        purl[i] = precv_buff[now++];
        if (NULL == ptr && '.' == purl[i]) ptr = &purl[i];
    }
    purl[url_len] = '\0';
    //cout<<purl<<endl;
    //PrintDebug(pclient, purl);

    //文件类型
    ptype = (char *)malloc(6);
    TestValueError(pclient, true, NULL == ptype, "type memory alloc in DealData is error");
    if (!ptr)
    {
        //带？的让cgi去跑
        ptr = strpbrk(purl, "?");
        if (!ptr)
        {
            TestValueError(pclient, true, NULL == purl, "purl in DealData is NULL at delete");
            TestValueError(pclient, true, NULL == ptype, "ptype in DealData is NULL at delete");
            free(purl); 
            free(ptype);
            purl = NULL; 
            ptype = NULL;
            //PrintDebug(pclient, "file type error");
            return CWebServer::CombineErrorPackage(pclient, BADREQUEST);
        }
        else
        {
            TestValueError(pclient, true, NULL == ptype, "ptype in DealData is NULL at delete");
            free(ptype);
            ptype = NULL;
            return CWebServer::UseCgi(pclient, purl);
        }
    }
    //看程序是否支持这个文件类型
    int i = 0;
    for (i = 0; i < 5; i++)
    {
        ptr++;
        if (*ptr == purl[url_len]) break;
        ptype[i] = *ptr;
    }
    ptype[i] = '\0';
    for (i = 0; i < SSIZE; i++) 
        if (!strcmp(TYPE[i], ptype)) break;

    if (*ptr != purl[url_len] || SSIZE == i)
    {
        TestValueError(pclient, true, NULL == purl, "purl in DealData is NULL at delete");
        TestValueError(pclient, true, NULL == ptype, "ptype in DealData is NULL at delete");
        free(purl);
        free(ptype);
        purl = NULL;
        ptype = NULL;
        cout<<purl<<endl;
        //PrintDebug(pclient, "file type error:have no this type");
        return CWebServer::CombineErrorPackage(pclient, BADREQUEST);
    }
    //cout<<ptype<<endl;
    //PrintDebug(pclient, ptype);
    //组合正确的包
    message_package * ptrmes = CWebServer::CombineCorrectPackage(pclient, purl, i);
    //释放空间
    TestValueError(pclient, true, NULL == purl, "purl in DealData is NULL at delete");
    TestValueError(pclient, true, NULL == ptype, "ptype in DealData is NULL at delete");
    free(purl);
    free(ptype);
    purl = NULL;
    ptype = NULL;

    return ptrmes;
}
//cgi模块
message_package* CWebServer::UseCgi(client_information* pclient, char* pUrl)
{
    TestValueError(pclient, true, NULL == pclient, "pclient in UseCgi is NULL");
    pid_t cgiPid = fork();
    if (0 == cgiPid)
    {
        execlp("../data/CGI/web_server_cgi", pUrl, (char *)0);
        PrintError(pclient, "CGi start error.");
        exit(1);
    }
    //等待子进程跑完
    pid_t pid = wait(NULL);
    TestValueError(pclient, true, NULL == pUrl, "purl in UseCgi is NULL at delete");
    free(pUrl);
    pUrl = NULL;
    //进程跑完而且成功了
    if (pid == cgiPid)
    {
        //PrintDebug(pclient, "CGI start succeed");
        ifstream in("file/output_of_cgi.txt", ios::in | ios::ate);
        if (!in.is_open())
        {
            in.close();
            PrintError(pclient, "cgi output open error");
            exit(1);
        }
        unsigned long mesLen = in.tellg();
        if (mesLen != 0)
        {
            in.seekg(0, ios::beg);
            char* mes = (char *)malloc(mesLen);
            in.read(mes, mesLen);
            mes[mesLen] = '\0';
            message_package* ptrmes = new message_package;
            ptrmes->pmes = mes;
            ptrmes->mes_len = mesLen;
            return ptrmes;
        }
        else 
        {
            in.close();
            return CWebServer::CombineErrorPackage(pclient, BADREQUEST);
        }
    }
    //进程没有成功跑完
    else if (-1 == pid)
    {
        PrintError(pclient, "CGI end error.");
        return CWebServer::CombineErrorPackage(pclient, FORBIDDEN);
    }
    return CWebServer::CombineErrorPackage(pclient, FORBIDDEN);
}
//组合正确的包
message_package* CWebServer::CombineCorrectPackage(client_information* pclient, char* pUrl, int num)
{
    //combine       
    ifstream in(pUrl, ios::in | ios::ate | ios::binary);
    if (! in.is_open())
    {
        in.close();
        return CWebServer::CombineErrorPackage(pclient, NOTFOUND);
    }

    unsigned long mesHeadLen = 0, mesBodyLen = 0, mesLen = 0;
    mesHeadLen = strlen(SUCCEED[num]);
    mesBodyLen = in.tellg();
    in.seekg(0, ios::beg);
    mesLen = mesHeadLen  + mesBodyLen;

    char* pmes = (char *)malloc(mesLen + BUFFSIZE);
    TestValueError(pclient, true, NULL == pmes, "mes memory alloc error");
    pmes[0] = '\0';
    strcat(pmes, SUCCEED[num]);
    pmes[mesHeadLen] = '\0';
    char buffer[BUFFSIZE];
    while (!in.eof())
    {
        memset(buffer, 0, sizeof(buffer));
        in.read(buffer, sizeof(buffer));
        //strcat(pmes, buffer);
        //读不是纯文本文件会出错，我猜是因为似乎内置的函数后面的buff是以'\0'为结尾进行复制的
        //比较复杂的非纯文本会有一些奇奇怪怪的东西，说不定就有\0但是却是用两个字符拼接来表示汉字的
        for (unsigned int i = 0; i < sizeof(buffer); i++) pmes[mesHeadLen++] = buffer[i];
    }
    pmes[mesLen] = '\0';
    //PrintDebug(pclient, "combine message over");

    message_package* ptrmes = new message_package;
    ptrmes->pmes = pmes;
    ptrmes->mes_len = mesLen;
    
    in.close();

    return ptrmes;
}
//错误消息包
message_package* CWebServer::CombineErrorPackage(client_information* pclient, const char* pErrorName)
{
    message_package* ptrmes = new message_package;
    if (pErrorName == NOTFOUND)
    {
        char* pMes = (char *)malloc(strlen(NOTFOUND) + 1);
        TestValueError(pclient, true, NULL == pMes, "Errorpackage memory alloc error");
        strcpy(pMes, NOTFOUND);
        ptrmes->pmes = pMes;
        ptrmes->mes_len = strlen(NOTFOUND);;
        return ptrmes;
    }
    else if (pErrorName == BADREQUEST)
    {
        char* pMes = (char *)malloc(strlen(BADREQUEST) + 1);
        TestValueError(pclient, true, NULL == pMes, "Errorpackage memory alloc error");
        strcpy(pMes, BADREQUEST);
        ptrmes->pmes = pMes;
        ptrmes->mes_len = strlen(BADREQUEST);
        return ptrmes;
    }
    else if (pErrorName == FORBIDDEN)
    {
        char* pMes = (char *)malloc(strlen(FORBIDDEN) + 1);
        TestValueError(pclient, true, NULL == pMes, "Errorpackage memory alloc error");
        strcpy(pMes, FORBIDDEN);
        ptrmes->pmes = pMes;
        ptrmes->mes_len = strlen(FORBIDDEN);
        return ptrmes;
    }
    else return NULL;
}

int main()
{
    CWebServer webServer = CWebServer();
    return 0;
}

