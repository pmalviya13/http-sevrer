#include<iostream>
#include<map>
#include<forward_list>
#include<string.h>
#include<arpa/inet.h> //#include<Winsock2.h> for win
#include<sys/socket.h>
#include<unistd.h>

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
        int serverSocketDescriptor  = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        char requestBuffer[4096];
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
            char a[101];
            sprintf(a,"Unable to bind socket to port : %d",portNumber);
            Error error(a);
            callBack(error);
            return;
        }
        sucessCode=::listen(serverSocketDescriptor,10);
        if(sucessCode<0){
            close(serverSocketDescriptor);
            Error error("Unable to accept client connection.");
            callBack(error);
            return;
        }

        Error error("");
        callBack(error);
        struct sockaddr_in clientSocketInformation;
        socklen_t len=sizeof(clientSocketInformation);
        int clientSocketDescriptor;
        while(1){
            clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
            cout<<"clientSocketDescriptor"<<clientSocketDescriptor<<endl;
            if(clientSocketDescriptor<0){
                //not yet decided, 
            }
            requestLength=recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer),0);
            if(requestLength>0){
                for(x=0;x<requestLength;x++)printf("%c",requestBuffer[x]);
                const  char *response=
                "HTTP/1.0 200 OK\r\n"
                "Connection: close\r\n"
                "Content-type: text/html\r\n"
                "Content-Length: 141\r\n\r\n"
                "<html><head><title>Thinking Machines</title></head>"
                "<body><h1>Thinking Machines</h1></h3>We teach more than we promise to teach</h3></body></html>";
                send(clientSocketDescriptor,response,strlen(response),0);
                
            }
            //lot of code will be wriiter to close
        } //loop end here
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
    bro.listen(7070, [](Error & error) void {
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


