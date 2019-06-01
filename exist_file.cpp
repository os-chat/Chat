#include <sys/stat.h>

#include <iostream>

using namespace std;

inline bool exists_test3 (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int main(){
    cout << exists_test3("/dev/mqueue/chat-testando") << endl;
    cout << exists_test3("/dev/mqueue/adlcsd12312iro") << endl;
    cout << exists_test3("test.cpp") << endl;
}