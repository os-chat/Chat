#include <bits/stdc++.h>
using namespace std;

typedef struct __opt_msg
{
    int opt;
    string msg;
} opt_msg;

void iai(void *ptr){
    opt_msg control = *(opt_msg *)ptr;
    int opt=*((int*)(&ptr));
    string *msg=static_cast<string*>(ptr);
    string t = *msg;
    printf("%d %s\n", opt, t.c_str());
}

vector<string> split(const char *str, char c = ':')
{
    vector<string> result;

    do
    {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}

int main () {
    // opt_msg xd;
    // xd.opt=5;
    // xd.msg="abracadabra";

    // iai((void *)&xd);
    string a="iai:ola";
    auto x = split(a.c_str());
    cout<<x[0]<<endl;
    // for (auto e : x)
    //     cout << e << endl;
}