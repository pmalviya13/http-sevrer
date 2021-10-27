#include<iostream>

using namespace std;

void kk(int &a){
    cout<<a;
}
int main(){
    kk(5);
    return 0;
}