#include <iostream>
#include <fstream>
using namespace std;
const int MAXN = 1000;

int main(int argc, char* argv[])
{
    cout<<argc<<endl;
    char* url = argv[0];
    char name[MAXN],value[MAXN];
    unsigned int i = 0;

    ofstream out("file/output_of_cgi.txt");
    if (!out.is_open())
    {
        cout<<"open error"<<endl;
    }

    while (i < strlen(url) && url[i] != '?') 
    {
        i++;
    }
    if (i < strlen(url))
    {
        out<<"HTTP/1.1 200 OK\n";
        out<<"Content-Type: text/html\n";
        out<<"\n";
        out<<"<html>\n";
        out<<"<head>\n";
        out<<"<title>\n";
        out<<"Hello\n";
        out<<"</title>\n";
        out<<"</head>\n";
        out<<"<body>\n";
        while (i < strlen(url))
        {
            i++;
            int name_len = 0;
            int value_len = 0;
            while (i < strlen(url) && url[i] != '=')
            {
                name[name_len++] = url[i++];
            }
            i++;
            while (i < strlen(url) && url[i] != '&')
            {
                value[value_len++] = url[i++];
            }
            if (name_len != 0 && value_len != 0)
            {
                name[name_len] = '\0';
                value[value_len] = '\0';
                cout<<"Your "<<name<<" is "<<value<<endl;
                out<<"Your "<<name<<" is "<<value<<".<br>\n";
            }
            else 
            {
                out<<"Your url is error\n";
                break;
            }
        }
        out<<"</body>\n";
        out<<"</html>\n";
    }
    out.close();
    return 0;
}
