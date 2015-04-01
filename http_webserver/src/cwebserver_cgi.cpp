#include "cwebserver.h"
//cgiģ��
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
    //�ȴ��ӽ�������
    pid_t pid = wait(NULL);
    TestValueError(pclient, true, NULL == pUrl, "purl in UseCgi is NULL at delete");
    free(pUrl);
    pUrl = NULL;
    //����������ҳɹ���
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
    //����û�гɹ�����
    else if (-1 == pid)
    {
        PrintError(pclient, "CGI end error.");
        return CWebServer::CombineErrorPackage(pclient, FORBIDDEN);
    }
    return CWebServer::CombineErrorPackage(pclient, FORBIDDEN);
}
