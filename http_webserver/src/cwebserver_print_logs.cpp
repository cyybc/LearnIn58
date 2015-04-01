#include "cwebserver.h"

//输出debug日志
void CWebServer::PrintDebug(client_information* pclient, string event)
{
    TestValueError(pclient, true, NULL == pclient, event + " in PrintDebug");
    int client_sockfd = pclient->sockfd;
    //pthread_rwlock_wrlock(&m_debug_rwlock);
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
    //pthread_rwlock_unlock(&m_debug_rwlock);
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
