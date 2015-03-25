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

pthread_rwlock_t CWebServer::m_error_rwlock;
pthread_rwlock_t CWebServer::m_log_rwlock;
pthread_rwlock_t CWebServer::m_debug_rwlock;

const char* CWebServer::CHANGEURL = "../data/";

const char* CWebServer::TYPE[SSIZE] = {"htm", "html", "txt", "bmp", "gif", "jpg", "jpeg", "doc", "rar", "zip"};
//如果真的需要很多种可以选择把表格存在数据库中，然后读取并且一对一映射存入map中，head中type部分可以空出来组装
//错误也是一样
const char* CWebServer::BADREQUEST = "HTTP/1.1 400 Bad Request\r\n\
Content-Type: text/html\r\n\r\n\
<html><head><title>400</title></head><body>400 Bad Request</body></html>\r\n";

const char* CWebServer::NOTFOUND = "HTTP/1.1 404 Not Found\r\n\
Content-Type: text/html\r\n\r\n\
<html><head><title>404</title></head><body>404 Not Found</body></html>\r\n";

const char* CWebServer::FORBIDDEN = "HTTP/1.1 403 Forbidden\r\n\
Content-Type: text/html\r\n\r\n\
<html><head><title>403</title></head><body>403 Forbidden</body></html>\r\n";

const char* CWebServer::SUCCEED[SSIZE] = {
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n",//htm
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: image/bmp\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n",//jpg
    "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: application/doc\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: application/rar\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: application/zip\n\r\n"
};

struct SMyData
{
    CWebServer* now;
    int client_sockfd;
};

#endif
