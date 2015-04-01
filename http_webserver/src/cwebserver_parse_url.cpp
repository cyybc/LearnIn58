#include "cwebserver.h"

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
        //PrintDebug(pclient, "Not Get or Post or Head");
        return CWebServer::CombineErrorPackage(pclient, BADREQUEST); 
    }

    //文件路径
    while (now < recv_len && ' ' == precv_buff[now]) 
    {
        now++;
    }
    if (now < recv_len && '/' == precv_buff[now]) 
    {
        now++;
    }
    next = now;
    while (next < recv_len && precv_buff[next] != ' ') 
    {
        next++;
    }
    if (now == recv_len || next == recv_len) 
    {
        return CWebServer::CombineErrorPackage(pclient, BADREQUEST); 
    }
    url_len = next - now + CHANGEURLLEN;
    purl = (char *)malloc(url_len);
    TestValueError(pclient, true, NULL == purl, "url memory alloc error");
    for (int i = 0; i < CHANGEURLLEN; i++) 
    {
        purl[i] = CHANGEURL[i];
    }
    for (int i = CHANGEURLLEN; i < url_len; i++) 
    {
        purl[i] = precv_buff[now++];
        if (NULL == ptr && '.' == purl[i]) ptr = &purl[i];
    }
    purl[url_len] = '\0';
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
    {
        if (!strcmp(TYPE[i], ptype)) break;
    }
    if (*ptr != purl[url_len] || SSIZE == i)
    {
        TestValueError(pclient, true, NULL == purl, "purl in DealData is NULL at delete");
        TestValueError(pclient, true, NULL == ptype, "ptype in DealData is NULL at delete");
        free(purl);
        free(ptype);
        purl = NULL;
        ptype = NULL;
        //PrintDebug(pclient, "file type error:have no this type");
        return CWebServer::CombineErrorPackage(pclient, BADREQUEST);
    }
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
