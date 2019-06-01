
// C++ program to demonstrate working of regex_search() 
#include <iostream> 
#include <regex> 
#include<string.h> 
using namespace std; 
  
int main() 
{ 
    // Target sequence 
    // string s = R"(-rw------- 1 guilherme guilherme 80 Jun  1 11:45 /dev/mqueue/chat-ola)"; 
    string s = R"(ls: cannot access 'oasidasod': No such file or directory)";
  
    // An object of regex for pattern to be searched 
    regex r(R"(([^rwx0-9\-\s]).*?(?=\s))"); 
  
    // flag type for determining the matching behavior 
    // here it is for matches on 'string' objects 
    smatch m; 
  
    // regex_search() for searching the regex pattern 
    // 'r' in the string 's'. 'm' is flag for determining 
    // matching behavior. 
    regex_search(s, m, r); 
  
    // for each loop 
    cout << m[0] << endl;
    for (auto x : m) 
        cout << x << " "; 
  
    return 0; 
} 
