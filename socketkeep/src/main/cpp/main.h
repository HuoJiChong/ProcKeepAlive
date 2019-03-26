//
// Created by J.C. on 23/03/2019.
//

#ifndef PROCKEEPALIVE_MAIN_H
#define PROCKEEPALIVE_MAIN_H

#include "jni.h"
#include <unistd.h>
#include <syslog.h>
#include <android/log.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/signal.h>
#include <sys/socket.h>
#include <linux/un.h>


void WorkInChildProc();

int child_create_channel();
void child_listen_msg();


#define  TAG "Roger"
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__);

#define  MAX_BUFFER_SIZE 512

extern "C"
{

JNIEXPORT void
JNICALL Java_com_aly_roger_socketkeep_Watcher_createWatcher(JNIEnv * env,jobject obj,jstring uid) ;

JNIEXPORT void
JNICALL Java_com_aly_roger_socketkeep_Watcher_createMonitor(JNIEnv *env, jobject instance) ;

}

#endif //PROCKEEPALIVE_MAIN_H

