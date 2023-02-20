#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "thread.h"



int g_isMainRunning = 0;


void sigact_print_exit(int signo)
{
  printf("SIGNAL :: EXIT(%d, %d)\n", signo, getpid());
  g_isMainRunning = 0;
}

void sigact_print_ignore(int signo)
{
  printf("SIGNAL :: IGNORE(%d, %d)\n", signo, getpid());
}

void sigact_print_segv(int signo, siginfo_t* info, void* context)
{
  printf("SIGNAL :: SEGV(%d, %d)\n", signo, getpid());
  exit(1);
}

void set_signals(void)
{
  struct sigaction act_print_exit;
  struct sigaction act_print_ignore;
  struct sigaction act_print_segv;

  memset(&act_print_exit, 0, sizeof(struct sigaction));
  memset(&act_print_ignore, 0, sizeof(struct sigaction));
  memset(&act_print_segv, 0, sizeof(struct sigaction));

  act_print_exit.sa_handler = sigact_print_exit;
  act_print_ignore.sa_handler = sigact_print_ignore;
  act_print_segv.sa_sigaction = sigact_print_segv;

  act_print_exit.sa_flags = 0;
  act_print_ignore.sa_flags = 0;
  act_print_exit.sa_flags = SA_SIGINFO;

  sigemptyset(&act_print_exit.sa_mask);
  sigemptyset(&act_print_ignore.sa_mask);
  sigemptyset(&act_print_segv.sa_mask);

  sigaction(SIGINT, &act_print_exit, NULL);
  sigaction(SIGABRT, &act_print_exit, NULL);
  sigaction(SIGFPE, &act_print_exit, NULL);
  sigaction(SIGILL, &act_print_exit, NULL);
  sigaction(SIGSYS, &act_print_exit, NULL);
  sigaction(SIGTERM, &act_print_exit, NULL);
  
  
  sigaction(SIGUSR1, &act_print_ignore, NULL);
  sigaction(SIGUSR2, &act_print_ignore, NULL);
  sigaction(SIGHUP, &act_print_ignore, NULL);
  sigaction(SIGCHLD, &act_print_ignore, NULL);

  sigaction(SIGSEGV, &act_print_segv, NULL);
}

typedef struct _condVar
{
  pthread_cond_t cond;
  pthread_mutex_t mutex;
} condVar_t;


struct threadArg
{
  int flag;
  condVar_t* condVar;
};

void* pingThread(void* args)
{
  int i = 0;
  struct threadArg* arg = args;
 

  while(1)
  {
    if (arg->flag == 0)
    {
      pthread_mutex_lock(&arg->condVar->mutex);
      pthread_cond_wait(&arg->condVar->cond, &arg->condVar->mutex);
      arg->flag = 1;
      pthread_mutex_unlock(&arg->condVar->mutex);
      printf("%d: ping\n", i);
      ++i;
    }
    pthread_cond_signal(&arg->condVar->cond);
    usleep(100000);
  }

  return (void* )0;
}

void* pongThread(void* args)
{
  int i = 0;
  struct threadArg* arg = args;


  pthread_cond_signal(&arg->condVar->cond);

  while(1)
  {
    if (arg->flag == 1)
    {
      pthread_mutex_lock(&arg->condVar->mutex);
      pthread_cond_wait(&arg->condVar->cond, &arg->condVar->mutex);
      arg->flag = 0;
      pthread_mutex_unlock(&arg->condVar->mutex);

      printf("%d: pong\n", i);
      ++i;
    }
    pthread_cond_signal(&arg->condVar->cond);
    usleep(100000);
  }

  return (void* )0;
}

int main(int argc, char** argv)
{
  threadHandler_t ping_thread_handle;
  threadHandler_t pong_thread_handle;

  condVar_t condVar;


  pthread_cond_init(&condVar.cond, NULL);

  pthread_mutex_init(&condVar.mutex, NULL);

  struct threadArg arg = {0, &condVar};
  set_signals();
  
  if (createThread(&ping_thread_handle, DETACHABLE, pingThread, (void* )&arg) != 0)
  {
    printf("thread creation failed\n");
    exit(1);
  }

  if (createThread(&pong_thread_handle, DETACHABLE, pongThread, (void* )&arg) != 0)
  {
    printf("thread creation failed\n");
    exit(1);
  }


  g_isMainRunning = 1;

  while(g_isMainRunning)
  {
    usleep(1);
  }

  if (cancelThread(&ping_thread_handle) != 0)
  {
    printf("thread cancelation failed\n");
    exit(1);
  }

  if (cancelThread(&pong_thread_handle) != 0)
  {
    printf("thread cancelation failed\n");
    exit(1);
  }

  exit(0);
}