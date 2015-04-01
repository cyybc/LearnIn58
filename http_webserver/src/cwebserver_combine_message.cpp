#include "cwebserver.h"

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
    message_package* ptrmes = (message_package*)malloc(sizeof(message_package));
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

