
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>                                                                                                             
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include "reactor.hpp"
#include "buff.hpp"
#include "workthread.hpp"
#include "tcpaccept.hpp"
#include "test_interface.hpp"

#define PORT 4404
#define IP "10.1.1.182"
#define TIMEOUT 0

#define WORK_THREAD_NUM 3

volatile bool stop = false;

static void init_interface() {
    dll.handle_input = test_handle_input;
    dll.handle_process = test_handle_process;
}

static void sigterm_handler(int signo) {
    stop = true;
}

static void daemon_start()
{           
    struct rlimit rlim;
    rlim.rlim_cur = 20000;
    rlim.rlim_max = 20000;
    if (setrlimit(RLIMIT_NOFILE, &rlim) == -1) {
    }

    struct sigaction sa;
    sigset_t sset;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigterm_handler;
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGHUP,  &sa, NULL);

    signal(SIGPIPE,SIG_IGN);

    sigemptyset(&sset);
    sigaddset(&sset, SIGSEGV);
    sigaddset(&sset, SIGBUS);
    sigaddset(&sset, SIGABRT);
    sigaddset(&sset, SIGILL);
    sigaddset(&sset, SIGCHLD);
    sigaddset(&sset, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &sset, &sset);
    daemon(1, 1);
}


int main() {

    Reactor reactor;
    if (!reactor.init(20000)) {
        exit(-1);
    }

    init_interface();

    daemon_start();

    //创建套接字
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    TcpAccept* ta = new TcpAccept(reactor);
    if (!ta->start(server_addr, TIMEOUT)) {
        delete ta;
        exit(-1);
    }

    tptr = (Thread*)calloc(WORK_THREAD_NUM, sizeof(Thread));
    for (int i = 0; i < WORK_THREAD_NUM; i++) {
        create_work_thread(&reactor, i);
    }

    while (!stop) {
        reactor.handle_events(1000);
    }


   return 0;
}
