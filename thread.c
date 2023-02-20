#include "thread.h"

void* threadEntry(void* args);

int createThread(threadHandler_t* p_handle, threadAttr_t threadAttr, void* (*threadMain)(void* ), void* p_args)
{
  int ret = 0;
  pthread_t id;
  pthread_attr_t attr;

  p_handle->state = INITIALIZED;
  p_handle->attr = threadAttr;
  p_handle->p_entry = threadMain;
  p_handle->p_args = p_args;

  pthread_attr_init(&attr);

  switch (threadAttr)
  {
  case DETACHABLE:
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    break;
  case JOINABLE:
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    break;
  default:
    break;
  }

  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);

  ret = pthread_create(&id, &attr, threadEntry, (void* )p_handle);
  p_handle->id = id;

  return ret;
}

int cancelThread(threadHandler_t* p_handle)
{
  int ret = 0;
  if (p_handle->state != DONE && p_handle->state != INITIALIZED)
  {
    ret = pthread_cancel(p_handle->id);
  }

  return ret;
}

int joinThread(threadHandler_t* p_handle)
{
  int ret = 0;
  if (p_handle->attr == JOINABLE && p_handle->state != DONE)
  {
    ret = pthread_join(p_handle->id, NULL);
  }

  return ret;
}

void* threadEntry(void* args)
{
  threadHandler_t* p_handle = (threadHandler_t* )args;
  void* (* threadMain)(void* ) = p_handle->p_entry;

  p_handle->state = RUNNING;
  threadMain(p_handle->p_args);
  p_handle->state = DONE;

  return (void* )0;
}