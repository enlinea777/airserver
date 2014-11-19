#include "global_data.hpp"
#include "buff.hpp"

Queue<BufBlock_t*> g_receive_queue;  //接受队列
Queue<BufBlock_t*> g_send_queue;     //发送队列

MutexLock g_receive_lock;
MutexLock g_send_lock;

dll_func_t dll;