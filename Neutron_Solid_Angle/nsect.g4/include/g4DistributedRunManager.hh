#ifndef g4DistributedRunManager_hh
#define g4DistributedRunManager_hh

#include "G4RunManager.hh"
#include "G4Version.hh"
#include "globals.hh"

#include "g4TicketManager.hh"
#include "SyncLock.hh"

#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#include <unistd.h>
#include <fcntl.h>

using namespace std;
#include <list>
#include <iostream>
#include <fstream>

// modes
enum {
  MASTER,
  SLAVE
};

// restart flags
#define READ_RESTART  1
#define WRITE_RESTART 2

// socket info
#define DEFAULT_PORT_NUMBER 0x7782
#define S_BUFFER_LEN        1024

#define CMD_LEN  64
#define HOST_LEN 256
#define PID_LEN  32

#define RESTART_FILE "./restart_positions"

extern "C" void * wait_for_connection_wrapper(void * a);
extern "C" void * gather_requests_wrapper(void * a);

class g4DistributedRunManager;

struct _client_info {
  pthread_t thread_id;
  int       socket_id;
};
typedef struct _client_info client_info_t;


// for passing arguments to threads
struct extern_c_arg {
  g4DistributedRunManager * dis;
  int                    sock;
};
typedef struct extern_c_arg Arg_t;
typedef unsigned long       Ulong;

//
//
class g4DistributedRunManager : public G4RunManager
{
public:
  // non parallel - initialize random number generator
  // otherwise runs as a normal G4Run
  g4DistributedRunManager();

  // master
  g4DistributedRunManager(G4int use_restart, G4int port = 0);

  // slave
  g4DistributedRunManager(G4String master_host_name, G4int port = 0);

  ~g4DistributedRunManager();
  
public:

  void   SetUserTicketManager(g4TicketManager * ticket_manager) { user_ticket_manager = ticket_manager; }

  G4int  RunTickets();

  G4int  GetNextTicket(G4int mode, g4Ticket * ticket, char * clientInfo = 0);

  void   SetVerbose(G4int v) { Verbose = v; }

  G4int  GetStatus() { return Status; }

  G4int  IsMaster(){ return((mode == MASTER) ? 1 : 0); }


  // public for thread interface ONLY
  void * wait_for_connection(int sock);
  void * gather_requests(int sock);
 
private:
  void   zero_it();
  void   make_handshake_string();
  void   make_process_string();
  G4int  init_dispatch(G4int the_mode, G4String host);
  G4int  close_dispatch();
  void   initialize_rng();
  int    start_listening(); // returns a socket
  void   thread_accept(int sock);
  G4int  safely_get_ticket(g4Ticket * ticket, char * info);

  // master methods
  void   provide_handshake(char * buffer, int sock);
  void   provide_next_ticket(char * buffer, int sock);
  void   provide_seed(char * buffer, int sock);
  void   provide_args(char * buffer, int sock);
  void   assign_g4cout(g4Ticket * the_ticket, ofstream *filestr, streambuf ** backup);
  void   return_g4cout(ofstream * filestr, streambuf ** backup);
  void   assign_stdout(g4Ticket * the_ticket);
  void   wait_for_disconnect();

  // client methods
  G4int  connect_to_server(G4String host);
  void   initialize_client(int sock);
  G4int  request_handshake(int sock);
  Ulong  request_seed(int sock);
  G4int  request_args(int sock);
  G4int  request_next_ticket(int sock, g4Ticket * the_ticket);

  G4int  read_socket(int sd, char * buf, G4int n);
  G4int  write_socket(int sd, char * buf, G4int n);
  void   block_signals();
  void   unblock_signals();
  void   mark_as_finished(g4Ticket * ticket);
  G4int  position_is_finished(G4int position);
  G4int  check_if_finished(g4Ticket * ticket);
  G4int  handle_restart_file(g4Ticket * ticket, G4int restart_mode);
  void   mv_restart_file();

  void   PackNetworkTicket(char * buffer, char * command, char * client_host, char * pid, g4Ticket * tx_ticket);
  void   UnpackNetworkTicket(char * buffer, char * command, char * client_host, char * pid, g4Ticket *  tx_ticket);

  G4int             mode;
  G4int             server_port;
  
  G4String          current_output_name;

  G4String          HandshakeString;
  G4String          ProcessString;

  FILE            * RESTART_read_stream;
  FILE            * RESTART_write_stream;

  // flags
  G4int             Verbose;
  G4int             UseRestart;
  G4int             Status; 

  // synchronization
  SyncLock          FD_Lock;        // file descriptor lock
  SyncLock          CL_Lock;        // client list lock
  SyncLock          Ticket_Lock;    // get ticket lock
  SyncLock          Dispatch_Lock;  // stop/start dispatch lock
  SyncLock          Restart_Lock;   // restart ticket lock

  pthread_t         main_thread;
  pthread_t         listen_thread;
  pthread_mutex_t   position_mutex;
  int               listen_socket;
  int               client_socket;

  std::list<client_info_t*> client_list;    // list of client threads in server

  g4TicketManager * user_ticket_manager;

};

#endif
