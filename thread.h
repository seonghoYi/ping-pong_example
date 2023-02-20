#include <pthread.h>


typedef pthread_t threadID_t;

typedef enum _threadState
{
  INITIALIZED = 0U,
  RUNNING,
  STOPPED,
  DONE,
} threadState_t;

typedef enum _threadAttr
{
  DETACHABLE = 0U,
  JOINABLE,
} threadAttr_t;

typedef struct _threadHandler
{
  threadID_t id;
  threadState_t state;
  threadAttr_t attr;

  void* (*p_entry)(void*);
  void* p_args;

} threadHandler_t;


int createThread(threadHandler_t* p_handle, threadAttr_t threadAttr, void* (*threadMain)(void* ), void* p_args);
int cancelThread(threadHandler_t* p_handle);
int joinThread(threadHandler_t* p_handle);

