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


void CallServerFuntion( mqd_t mq_id ,const char*  funName, unsigned int msg_prio){
    // 发送消息到服务器
    Message message;
    message.type = 1;
    //strncpy(message.text, "Hello from client", sizeof(message.text));
    strcpy(message.sp, "sp1");
    strcpy(message.fun,funName);
   // strcpy(message.text, str);

    if (mq_send(mq_id, reinterpret_cast<const char*>(&message), sizeof(message), msg_prio) == -1) {
        std::cerr << "Failed to send message to server" << std::endl;
    } else {
        std::cout << "Client: Sent message to server" << std::endl;
    }

}

int main() {
    const char* SERVER_QUEUE_NAME = "/server_queue";
    const char* CLIENT_QUEUE_NAME = "/client_queue";
    const mode_t QUEUE_PERMISSIONS = 0666;

    // 打开服务器消息队列
    mqd_t serverQueue = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (serverQueue == -1) {
        std::cerr << "Failed to open server message queue" << std::endl;
        return 1;
    }

    mqd_t clientQueue = mq_open(CLIENT_QUEUE_NAME, O_RDONLY);
    if (serverQueue == -1) {
        std::cerr << "Failed to open server message queue" << std::endl;
        return 1;
    }

    std::cout << "Client: Connected to server message queue with name: " << SERVER_QUEUE_NAME << std::endl;

    CallServerFuntion(serverQueue,"Fun1",1);
    /*
    CallServerFuntion(serverQueue,"client@second call",2);
    CallServerFuntion(serverQueue,"client@thired call",3);
    CallServerFuntion(serverQueue,"client@fourth call",4);
    */

    MessageReply messageReply; 
    memset(&messageReply,0,sizeof(messageReply));
    if (mq_receive(clientQueue, reinterpret_cast<char*>(&messageReply), sizeof(messageReply), 0) == -1) {
                 std::cerr << "Failed to receive message from client" << std::endl;
         } else {
		 std::cout << "client: receive message from server:---"<< messageReply.text << std::endl;
	 }

    // 关闭消息队列
    mq_close(serverQueue);

    return 0;
}

