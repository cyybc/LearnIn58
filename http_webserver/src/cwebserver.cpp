#include "cwebserver.h"

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
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: image/bmp\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: application/doc\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: application/rar\r\n\r\n",
    "HTTP/1.1 200 OK\r\nContent-Type: application/zip\n\r\n"
};

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
