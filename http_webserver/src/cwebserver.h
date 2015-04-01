#ifndef __CWEBSERVER_H_YEBAICHUAN_20150309__
#define __CWEBSERVER_H_YEBAICHUAN_20150309__
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sys/types.h>//unsigin
#include <sys/socket.h>//socket
#include <sys/epoll.h>//epoll
#include <netinet/in.h>//addr
#include <arpa/inet.h>//inet_ntoa
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>//execlp
#include <sys/wait.h>//wait
#include <errno.h>
using namespace std;

struct message_package
{
    char* pmes;
    long mes_len;
};

struct client_information
{
    int sockfd;
    int log_id;
};

class CWebServer
{
private:
    static const int SSIZE = 10;
    static const char* TYPE[SSIZE];
    static const char* CHANGEURL;
    static const char* BADREQUEST;
    static const char* NOTFOUND;
    static const char* FORBIDDEN;
    static const char* SUCCEED[SSIZE];
    static const int PORT_NUM = 6868; 
    static const int BACKLOG = 100000;
    static const int MAXEVENTS = 100000;
    static const int CHANGEURLLEN = 8;
    static const int BUFFSIZE = 1024;

    static pthread_rwlock_t m_error_rwlock, m_log_rwlock, m_debug_rwlock;
    int m_log_id;
    int m_server_sockfd, m_epoll_fd;
    client_information* m_no_client;
public:
    CWebServer();
    ~CWebServer();
private:
    static void PrintDebug(client_information* pclient, string event);
    static void PrintLog(client_information* pclient, string event);
    static void PrintError(client_information* pclient, string error_reason);
    static void TestValueError(client_information* pclient, bool exValue, bool acValue, string error_reason);
    static bool TestWord(client_information* pclient, int len, std::string st, char* pstart);
    void Init();
    void EpollWait();
    void AcceptRequest();
    static void* RecvAndSend(void *pclient);
    static message_package* DealData(client_information* pclient, int recv_len, char* recv_buff);
    static message_package* UseCgi(client_information* pclient, char* url);
    static message_package* CombineCorrectPackage(client_information* pclient, char* url, int num);
    static message_package* CombineErrorPackage(client_information* pclient, const char* error_name);
};

struct SMyData
{
    CWebServer* now;
    int client_sockfd;
};

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
        if (wanted[i] != pRealValue[i] && wanted[i]+32 != pRealValue[i]) 
        {
            return false;
        }
    }
    return true;
}

#endif
