#include "cwebserver.h"

void CWebServer::Init()
{
    m_log_id = 0;
    m_no_client = new(client_information);
    m_no_client->sockfd = -1;
    m_no_client->log_id = -1;
    TestValueError(m_no_client, false, 0 == pthread_rwlock_init(&m_error_rwlock, NULL), "error rwlock init error");
    TestValueError(m_no_client, false, 0 == pthread_rwlock_init(&m_log_rwlock, NULL), "log rwlock init error");
    TestValueError(m_no_client, false, 0 == pthread_rwlock_init(&m_debug_rwlock, NULL), "debug rwlock init error");

    //����socket 
    m_server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    TestValueError(m_no_client, true, -1 == m_server_sockfd, "server socket creat error");

    //�󶨶˿�bind
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT_NUM);
    TestValueError(m_no_client, true, -1 == bind(m_server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)), "server socket bind error");

    //��ʼ����listen
    TestValueError(m_no_client, true, -1 == listen(m_server_sockfd, BACKLOG), "server socket listen error");

    //����epoll
    m_epoll_fd = epoll_create(MAXEVENTS);
    TestValueError(m_no_client, true, -1 == m_epoll_fd, "epoll fd create error");

    //ע��������˿ɶ��¼�server ctl
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = m_server_sockfd;
    TestValueError(m_no_client, true, -1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_server_sockfd, &ev), "server epoll_ctl error");
}
