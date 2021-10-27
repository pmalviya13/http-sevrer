#include<iostream>
using namespace std;

int main(){
    #ifdef linux
    printf("linux platform")
    #endif

    #ifdef _WIN32
    printf("windows platform")
    #endif
    return 0;
}