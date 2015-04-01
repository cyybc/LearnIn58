#include "cwebserver.h"

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
            //����Ƿ������˿ɶ�
            if (events[i].data.fd == m_server_sockfd)
            {
                CWebServer::AcceptRequest();                
            }
            //����ǿͻ��˿ɶ�
            else if (events[i].events & EPOLLIN)
            {
                //ɾ���ÿͻ��˼���client ctl_del
                client_information* pclient  = (client_information*)events[i].data.ptr;
                int client_sockfd = pclient->sockfd;
                ev.events = EPOLLIN;
                ev.data.fd = client_sockfd;
                TestValueError(pclient, true, -1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, client_sockfd, &ev), "client ctl_del error");
    
                //��һ���߳�ȥ��
                pthread_t tid;
                TestValueError(pclient, false, 0 == pthread_create(&tid, NULL, CWebServer::RecvAndSend, (void *)pclient), "pthread create error");
            }
        }
    }
}

//������������
void CWebServer::AcceptRequest()
{
    int client_sockfd;
    //������������accept
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    client_sockfd = accept(m_server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

    client_information *pclient = (client_information*)malloc(sizeof(client_information));
    pclient->sockfd = client_sockfd;
    pclient->log_id = m_log_id++;
    TestValueError(pclient, true, -1 == client_sockfd, "accept error");

    char event_message[BUFFSIZE];
    snprintf(event_message, sizeof(event_message), "%s %s", "accept client", inet_ntoa(client_addr.sin_addr));
    //PrintLog(pclient, event_message);
    //PrintDebug(pclient, event_message);

    //ע��ͻ��˿ɶ��ļ���client ctl
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = pclient;
    TestValueError(pclient, true, -1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev), "client epoll_ctl error");
}

//�����շ���Ϣ
void* CWebServer::RecvAndSend(void* ppclient)
{

    client_information* pclient = (client_information *)ppclient; 
    TestValueError(pclient, true, NULL == pclient, "pclient in RecvAndSend is NULL");
    int client_sockfd = pclient->sockfd;
    int recv_len;
    char recv_buff[BUFFSIZE], else_buff[BUFFSIZE];

    //������Ϣrecv
    recv_len = recv(client_sockfd, recv_buff, BUFFSIZE, 0);
    TestValueError(pclient, true, -1 == recv_len, "recv error");
    
    //ѭ�����룬ֱ��û����Ϣ
    while (BUFFSIZE == recv_len) 
    {
        recv_len = recv(client_sockfd, else_buff, BUFFSIZE, 0);
        TestValueError(pclient, true, -1 == recv_len, "recv error");
    }

    //PrintDebug(pclient, "recv message");
    //������Ϣ
    message_package* ptrmes = DealData(pclient, recv_len, &recv_buff[0]);

    //������Ϣsend
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
    free(ptrmes);
    ptrmes = NULL; 
    //PrintDebug(pclient, "send message");

    //�رտͻ�������close
    TestValueError(pclient, true, -1 == close(client_sockfd), "close client error");
    //PrintLog(pclient, "close client ");
    //PrintDebug(pclient, "close client");
    TestValueError(pclient, true, NULL == pclient, "pclient is NULL in RecvAndSend at delete");
    free(pclient);
    pclient = NULL;

    pthread_exit(0);

    return ((void *)0);
}
