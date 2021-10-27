#include<iostream>
#include<map>
#include<forward_list>
#include<string.h>
#include<unistd.h>
#ifdef _WIN32
#include<windows.h> 
#endif

#ifdef linux
#include<arpa/inet.h> 
#include<sys/socket.h>
#endif



using namespace std;

//AMIT [The Bro Programmer]
class Validator{
    private:
    Validator(){}
    public:
    static bool isValidMIMEType(string &mimeType){
        //rn do nothing
        return true;
    }
    static bool isValidPath(string &path){
        //rn do nothing
        return true;
    }
    static bool isValidURLFormat(string &url){
        //rn do nothing
        return true;
    }
};
class Error{
    private:
    string error;
    public:
    Error(string error){
        this->error = error;
    }
    bool hasError() { 
        return this->error.length() > 0; 
    }
    string getError() { 
        return this->error; 
    }
};

class Request{

};
class Response{
    private:
    string contenType;
    forward_list<string> content;
    forward_list<string>::iterator contentIterator;
    unsigned long contentLength;

    public:
    Response(){
        this->contentLength = 0;
        this->contentIterator = this->content.before_begin();
    }
    ~Response(){ 

    }
    void setContentType(string contentType){
        if(Validator::isValidMIMEType(contentType)){
        this->contenType = contentType;
        }
    }
    Response & operator<<(string content){
        this->contentLength = content.length();
        this->contentIterator = this->content.insert_after(this->contentIterator,content);
        return *this;
    }
};

class Bro{
    private:
    string staticResourcesFolder;
    map<string,void (*)(Request &, Response &)> urlMappings;

    public:
    Bro(){
           //not yet decided
    }
    ~Bro(){
           //not yet decided
    }
    void setStaticResourcesFolder(string staticResourcesFolder){
       if(Validator::isValidPath(staticResourcesFolder)){
           this->staticResourcesFolder = staticResourcesFolder;
       }
       else{
           //not yet decided
       }
    }
    void get(string url, void (*callBack)(Request &, Response &)){
        if(Validator::isValidURLFormat(url)){
            this->urlMappings.insert(pair<string, void (*)(Request &, Response &)>(url,callBack));
        }
    }

    void listen(int portNumber,void (*callBack)(Error &)){
        #ifdef _WIN32
        WSADATA wsaData;
        WORD ver;
        ver=MAKEWORD(1,1);
        WSAStartup(ver,&wsaData);
        #endif

        int serverSocketDescriptor  = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        char requestBuffer[4097];
        int requestLength;
        int x;
        if(serverSocketDescriptor < 0){
            Error error("Unable to create Socket.");
            callBack(error);
            return;
        }
        struct sockaddr_in serverSocketInformation;
        serverSocketInformation.sin_family= AF_INET;
        serverSocketInformation.sin_port = htons(portNumber);
        serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
        int sucessCode = bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
        if(sucessCode<0){
            close(serverSocketDescriptor);
            #ifdef _WIN32
            WSACleanup();
            #endif
            char a[101];
            sprintf(a,"Unable to bind socket to port : %d",portNumber);
            Error error(a);
            callBack(error);
            return;
        }
        sucessCode=::listen(serverSocketDescriptor,10);
        if(sucessCode<0){
            close(serverSocketDescriptor);
            #ifdef _WIN32
            WSACleanup();
            #endif
            Error error("Unable to accept client connection.");
            callBack(error);
            return;
        }

        Error error("");
        callBack(error);
        struct sockaddr_in clientSocketInformation;
        #ifdef linux
        socklen_t len=sizeof(clientSocketInformation);
        #endif
        #ifdef _WIN32
        int len=sizeof(clientSocketInformation);
        #endif
        
        int clientSocketDescriptor;
        while(1){
            clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
            cout<<"clientSocketDescriptor"<<clientSocketDescriptor<<endl;
            if(clientSocketDescriptor<0){
                //not yet decided, 
            }
            forward_list<string> requestBufferDS;
            forward_list<string>::iterator requestBufferDSIterator;
            requestBufferDSIterator=requestBufferDS.before_begin();
            int requestBufferDSSize=0;
            int requestDataCount=0;
            while(1){
                requestLength=recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer)-sizeof(char),0);
                if(requestLength==0)break;
                requestBuffer[requestLength] ='\0';
                requestBufferDSIterator = requestBufferDS.insert_after(requestBufferDSIterator,string(requestBuffer));
                requestBufferDSSize++;
                requestDataCount+=requestLength;

            }
            if(requestBufferDSSize>0){
                char *requestData = new char(requestDataCount+1);
                char *p;
                p=requestData;
                const char *q;
                requestBufferDSIterator=requestBufferDS.begin();
                while(requestBufferDSIterator != requestBufferDS.end()){
                    q = (*requestBufferDSIterator).c_str();
                    while(*q){
                        *p=*q;
                        p++;
                        q++;
                    }
                    ++requestBufferDSIterator;
                }
                *p = '\0';
                requestBufferDS.clear();
                printf("-----------------------request data begin -------------------\n");
                printf("%s\n",requestData);
                printf("-----------------------request data begin -------------------\n");

                //code to parse the request goes here
                delete [] requestData;
            }else{
                //someting if no data was received
            }

            
            //lot of code will be wriiter to close
        } //loop end here
        #ifdef _WIN32
        WSACleanup();
        #endif
    }   
    
};



// BOBBY [The web application developer - user of bro web server]

int main(){
    Bro bro;
    bro.setStaticResourcesFolder("whatever");
    bro.get("/",[](Request &request,Response &response) -> void  {
        const char *html = R""""(
            <!DOCTYPE html>
            <html lang="en">
                <head>
                    <title>Whatever</title>
                </head>
                <body>
                    <h1>Welcome</h1>
                    <a href="/getCustomers">List Customer</a>
                </body>
            </html>
        )"""";
        response.setContentType("text/html"); //setting mime type
        response<<html;
    });
    bro.get("/getCustomers",[](Request &request,Response &response) -> void {
        const char *html = R""""(
            <!DOCTYPE html>
            <html lang="en">
                <head>
                    <title>Whatever</title>
                </head>
                <body>
                    <h1>List of customers</h1>
                    <ul>
                        <li>amit</li>
                        <li>bobby</li>
                    </ul>
                    <a href="/">Home</a>
                </body>
            </html>
        )"""";
        response.setContentType("text/html");
        response<<html;
    });
    bro.listen(7000, [](Error & error) -> void {
        if(error.hasError())
        {
            cout<<"in iff";
            cout<<error.getError();
        }
        else{
            cout<<"in else";

            cout<<"Bro server is ready to accept request at 6060 port.";
        }
    });

    return 0;
};


