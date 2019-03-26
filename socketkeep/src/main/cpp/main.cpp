//
// Created by J.C. on 23/03/2019.
//

#include "main.h"

const  char* userid;
const char* PATH = "my.sock";
int m_child;

static int BindLocalSocketToName(
        int sd,
        const char* name)
{
    int result = -1;

    struct sockaddr_un address;

    // Name length
    const size_t nameLength = strlen(name);

    // Path length is initiall equal to name length
    size_t pathLength = nameLength;

    // If name is not starting with a slash it is
    // in the abstract namespace
    bool abstractNamespace = ('/' != name[0]);

    // Abstract namespace requires having the first
    // byte of the path to be the zero byte, update
    // the path length to include the zero byte
    if (abstractNamespace)
    {
        pathLength++;
    }

    // Check the path length
    if (pathLength > sizeof(address.sun_path))
    {
        // Throw an exception with error number
        LOGE( "Name is too big.");
    }
    else
    {
        // Clear the address bytes
        memset(&address, 0, sizeof(address));
        address.sun_family = PF_LOCAL;

        // Socket path
        char* sunPath = address.sun_path;

        // First byte must be zero to use the abstract namespace
        if (abstractNamespace)
        {
            *sunPath++ = NULL;
        }


        // Append the local name
        strcpy(sunPath, name);

        // Address length
        socklen_t addressLength =
                (offsetof(struct sockaddr_un, sun_path))
                + pathLength;

        // Unlink if the socket name is already binded
        unlink(address.sun_path);

        LOGE(" sun_path : %s",sunPath);

        // Bind socket
        result = bind(sd, (struct sockaddr*) &address, addressLength);

        if (-1 == result)
        {
            // Throw an exception with error number
            LOGE("Error %d", errno);
        }
    }

    return result;
}

/**
 * Listens on given socket with the given backlog for
 * pending connections. When the backlog is full, the
 * new connections will be rejected.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @param backlog backlog size.
 * @throws IOException
 */
static void ListenOnSocket(
        int sd,
        int backlog)
{
    // Listen on socket with the given backlog
    LOGE("Listening on socket with a backlog of %d pending connections.");

    if (-1 == listen(sd, backlog))
    {
        // Throw an exception with error number
        LOGE("ListenOnSocket Error %d", errno);
    }
}


/**
 * Blocks and waits for incoming client connections on the
 * given socket.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @return client socket.
 * @throws IOException
 */
static int AcceptOnLocalSocket(int sd)
{
    // Blocks and waits for an incoming client connection
    // and accepts it
    LOGE( "Waiting for a client connection...");
    int clientSocket = accept(sd, NULL, NULL);

    // If client socket is not valid
    if (-1 == clientSocket)
    {
        // Throw an exception with error number
        LOGE("Error %d", errno);
    }

    return clientSocket;
}

/**
 * Block and receive data from the socket into the buffer.
 *
 * @param env JNIEnv interface.
 * @param obj object instance.
 * @param sd socket descriptor.
 * @param buffer data buffer.
 * @param bufferSize buffer size.
 * @return receive size.
 * @throws IOException
 */
static ssize_t ReceiveFromSocket(
        int sd,
        char* buffer,
        size_t bufferSize)
{
    // Block and receive data from the socket into the buffer
    LOGE( "Receiving from the socket...");
    ssize_t recvSize = recv(sd, buffer, bufferSize - 1, 0);

    // If receive is failed
    if (-1 == recvSize)
    {
        // Throw an exception with error number
        LOGE( "Error %d " ,errno);
    }
    else
    {
        // NULL terminate the buffer to make it a string
        buffer[recvSize] = NULL;

        // If data is received
        if (recvSize > 0)
        {
            LOGE( "Received %d bytes: %s", recvSize, buffer);
        }
        else
        {
            LOGE( "Client disconnected.");
        }
    }

    return recvSize;
}

JNIEXPORT int child_create_channel() {

    int listenfd = socket(AF_LOCAL,SOCK_STREAM,0);

    int res = BindLocalSocketToName(listenfd,PATH);

    LOGE(" 绑定结果代码 ： %d ",res);

    if (res<0) {
        LOGE("绑定错误");
        return -1;
    }

    //监听五个客户端
    ListenOnSocket(listenfd,5);

    int connfd;

    while(1){
        //返回值，客户端的地址  阻塞 函数
        if ((connfd = AcceptOnLocalSocket(listenfd)) < 0){
            if (errno == EINTR){
                continue;
            }else{
                LOGE("读取错误");
                return -1;
            }
        }

        m_child = connfd;
        LOGE("连接成功");
        break;

    }

    return 0;
}

void child_listen_msg() {

    fd_set rfds;
    struct timeval timeout = {3,0};
    while (1){
        FD_ZERO(&rfds);
        FD_SET(m_child,&rfds);
        int r = select(m_child+1,&rfds,NULL,NULL,&timeout);
        LOGE("读取消息 %d",r);
        if ( r > 0 ){
            char pkg[512] = {0};
//            保证所读取到的信息是 指定apk客户端
            if (FD_ISSET(m_child,&rfds)){
                int recvSize = ReceiveFromSocket(m_child,
                                             pkg, MAX_BUFFER_SIZE);

                execlp("am","am","startservice","--userid",userid,
                       "com.aly.roger.socketkeep/com.aly.roger.socketkeep.ProcService",(char*)NULL);

                break;
            }
        }
    }
}


void WorkInChildProc() {
    if(child_create_channel() == 0 ){
        child_listen_msg();
    }
}

extern "C"{

void JNICALL Java_com_aly_roger_socketkeep_Watcher_createWatcher(JNIEnv * env,jobject obj,jstring juserid) {
    userid = env->GetStringUTFChars(juserid,0);

    pid_t pid = fork();
    LOGE(" create Watcher %d" ,pid);
    if (pid < 0){
        LOGE("失败");
    }else if ( pid == 0){
        LOGE("子进程");
        WorkInChildProc();
    }else{
        LOGE("父进程");
    }

    env->ReleaseStringUTFChars(juserid,userid);
}

JNIEXPORT void JNICALL
Java_com_aly_roger_socketkeep_Watcher_createMonitor(JNIEnv *env, jobject instance){
    int socked;

    struct sockaddr_un address;

    // Name length
    const size_t nameLength = strlen(PATH);

    // Path length is initiall equal to name length
    size_t pathLength = nameLength;

    // If name is not starting with a slash it is
    // in the abstract namespace
    bool abstractNamespace = ('/' != PATH[0]);

    // Abstract namespace requires having the first
    // byte of the path to be the zero byte, update
    // the path length to include the zero byte
    if (abstractNamespace)
    {
        pathLength++;
    }

    while (1){
        LOGE("客户端 服务启动啦");

        socked = socket(AF_LOCAL,SOCK_STREAM,0);
        if (socked < 0 ){
            LOGE("链接失败");
            return;
        }

        memset(&address, 0, sizeof(address));
        address.sun_family = PF_LOCAL;

        // Socket path
        char* sunPath = address.sun_path;

        // First byte must be zero to use the abstract namespace
        if (abstractNamespace)
        {
            *sunPath++ = NULL;
        }


        // Append the local name
        strcpy(sunPath, PATH);

        // Address length
        socklen_t addressLength =
                (offsetof(struct sockaddr_un, sun_path))
                + pathLength;

        if (connect(socked,(const struct sockaddr*)&address, addressLength) < 0){
            close(socked);
            sleep(3);
            LOGE("connect failed");
            continue;
//            break;
        }
        LOGE("connect success");

        break;

    }
}

}
