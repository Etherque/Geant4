#include "SyncLock.hh"

SyncLock::SyncLock()
{
  pthread_mutexattr_t mutex_attr;
  pthread_condattr_t  cond_attr;
  
  pthread_mutexattr_init(&mutex_attr);
  pthread_condattr_init(&cond_attr);

  // initialize
  pthread_mutex_init(&m_mutex, &mutex_attr);  

  pthread_cond_init(&m_cond, &cond_attr);  

  m_cond_var = 0;
  
}

SyncLock::~SyncLock()
{
  pthread_mutex_destroy(&m_mutex);
  pthread_cond_destroy(&m_cond);
}

// obtains lock OR blocks
void SyncLock::Lock()
{
  // obtain lock
  pthread_mutex_lock(&m_mutex);  
  
  while(m_cond_var)
    pthread_cond_wait(&m_cond, &m_mutex);
  
  m_cond_var = 1;
  
  pthread_mutex_unlock(&m_mutex);   
}

void SyncLock::Unlock()
{
  // release lock
  pthread_mutex_lock(&m_mutex);  

  m_cond_var = 0;
  pthread_cond_signal(&m_cond);  

  pthread_mutex_unlock(&m_mutex);   
}

