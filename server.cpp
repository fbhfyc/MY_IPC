#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MAX_MESSAGE_SIZE 100

struct Message {
    int type;
    char sp[20];
    char fun[20];
    char text[MAX_MESSAGE_SIZE];
};
struct MessageReply {
    int type;
    char text[MAX_MESSAGE_SIZE];
};

void Fun1(char* res){
	std::cout << "i amd Fun1 and been called " <<std::endl;
	strcpy(res,"i am Fun1's result");
}	

void ProcessResQuest(Message& mesRequest,MessageReply& mesReply){
      	std::cout << "Server: Received message from client! name of sendProcess is:["<<mesRequest.sp <<"]funname is:["<<mesRequest.fun<< "] " << ": " << mesRequest.text << std::endl;
    
	if( strcmp(mesRequest.fun,"Fun1") == 0 )
	{
		Fun1(mesReply.text);
	}

	return;
}

int main() {
    const char* SERVER_QUEUE_NAME = "/server_queue";
    const char* CLIENT_QUEUE_NAME = "/client_queue";
    const mode_t QUEUE_PERMISSIONS = 0666;

    // 创建消息队列
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Message);
    attr.mq_curmsgs = 0;

    mqd_t serverQueue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
    if (serverQueue == -1) {
        std::cerr << "Failed to create server message queue" << std::endl;
        return 1;
    }
    struct mq_attr attr2;
    attr2.mq_flags = 0;
    attr2.mq_maxmsg = 10;
    attr2.mq_msgsize = sizeof(MessageReply);
    attr2.mq_curmsgs = 0;
    mqd_t clientQueue = mq_open(CLIENT_QUEUE_NAME, O_CREAT | O_WRONLY, QUEUE_PERMISSIONS, &attr2);
    if (clientQueue == -1) {
        std::cerr << "Failed to create server message queue" << std::endl;
        return 1;
    }

    std::cout << "Server: Message queue created with name: " << SERVER_QUEUE_NAME << std::endl;
    std::cout << "Server: Message queue created with name: " << CLIENT_QUEUE_NAME << std::endl;
    std::cout << "------------------------ " << std::endl;

    // 接收客户端消息
    Message message;
    unsigned int priority;
    MessageReply messageReply;

    memset(&message,0,sizeof(message));
    memset(&messageReply,0,sizeof(messageReply));

    while(true)
    {
    	 if (mq_receive(serverQueue, reinterpret_cast<char*>(&message), sizeof(message), &priority) == -1) {
        	 std::cerr << "Failed to receive message from client" << std::endl;
         } else {

 
                ProcessResQuest(message,messageReply);
	//发送返回信息
      	if (mq_send(clientQueue, reinterpret_cast<const char*>(&messageReply), sizeof(messageReply), 0) == -1)
	{
        	std::cerr << "Failed to send back to client" << std::endl;
    	} 
	else {
       		std::cout << "Server: Sent message back to client" <<messageReply.text<< std::endl;
    	}

     	   }
    }

    // 关闭消息队列
    mq_close(serverQueue);
    mq_unlink(SERVER_QUEUE_NAME);

    return 0;
}

