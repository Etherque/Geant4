// 

#ifndef _SYNC_LOCK_H_
#define _SYNC_LOCK_H_

#include <pthread.h>

class SyncLock {

public:
  SyncLock();

  ~SyncLock();

  void Lock();
  
  void Unlock();

private:

  pthread_mutex_t m_mutex;
  pthread_cond_t  m_cond;
  int             m_cond_var;

};

#endif
