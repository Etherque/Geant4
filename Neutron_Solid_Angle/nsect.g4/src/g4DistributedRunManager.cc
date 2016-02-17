#include "g4DistributedRunManager.hh"
#include "g4TicketManager.hh"

#include "G4UIcommand.hh"

// pass through to G4RunManager
g4DistributedRunManager::g4DistributedRunManager()
{
  zero_it();
  initialize_rng();  
  Status = 1;
}

// master
g4DistributedRunManager::g4DistributedRunManager(G4int use_restart, G4int port)
{
  G4String host;
  
  zero_it();

  mode = MASTER;

  if(use_restart > 0)
    UseRestart = 1;

  if(port != 0)
    server_port = port;

  main_thread = pthread_self();

  make_handshake_string();

  make_process_string();

  initialize_rng();

  if(-1 == init_dispatch(mode, host)){
    // error - Exception??
    G4cerr << "ERROR - Could not start dispatch" << G4endl;
    return;
  }

  Status = 1;
  
}

// client
g4DistributedRunManager::g4DistributedRunManager(G4String host, G4int port)
{
  zero_it();

  mode = SLAVE;
 
 if(port != 0)
    server_port = port;

  make_handshake_string();

  make_process_string();

  // sets client_socket
  if(-1 == init_dispatch(mode, host)){
    // error - Exception??
    G4cerr << "Could not connect to dispatcher on host " << host << G4endl;
    return;
  }

  // get random seed from master process
  initialize_client(client_socket);
  
  user_ticket_manager = 0;

  Status = 1;
}

g4DistributedRunManager::~g4DistributedRunManager()
{
  if(RESTART_read_stream)
    fclose(RESTART_read_stream);
  
  if(RESTART_write_stream)
    fclose(RESTART_write_stream);
  
}

// primary work loop - called by master and slave processes
//
G4int g4DistributedRunManager::RunTickets()
{
  if(!user_ticket_manager){
    G4cerr << "ERROR: You must register a g4TicketManager to use distribution features" << G4endl;
    return(-1);
  }
  
  ofstream    output_stream;
  streambuf * output_buffer;
  
  g4Ticket  * the_ticket = user_ticket_manager->GetNewTicket();

  G4int       num_complete = 0;
  
  G4int rc = GetNextTicket(mode, the_ticket);
  
  while(rc != -1){
    
    // prevent interrupts until ticket is finished
    //block_signals();
    
    // redirect G4cout to an output file
    assign_g4cout(the_ticket, &output_stream, &output_buffer);
    
    // run the simulation for this ticket
    user_ticket_manager->HandleTicket(the_ticket);
    
    // restore G4cout
    return_g4cout(&output_stream, &output_buffer);
    
    // user action
    rc = user_ticket_manager->EndOfTicketAction(current_output_name, the_ticket);
    
    // user action error - exit loop
    if(rc == -1)
      break;
    
    // a flag
    num_complete++;
    
    // again
    rc = GetNextTicket(mode, the_ticket);    

    //  ok - handle interruptions
    //unblock_signals();        
  }  
  
  if(mode == SLAVE)
    return(0);
  
  //
  // master process runs through list once more using restart file
  // to pick up any dropped network conections
  //
  if(num_complete == 0){
    // no point rerunning - there is no restart file
    return(0);
  }
  
  // this tells dispatcher to reset and open restart file
  user_ticket_manager->ResetTickets();

  delete the_ticket;
  
  the_ticket = user_ticket_manager->GetNewTicket();

  // and open restart file
  UseRestart = 2;
  
  rc = GetNextTicket(mode, the_ticket);
  
  while(rc != -1){
    
    // prevent interrupts until ticket is finished
    block_signals();
        
    G4String ticket_id;

    the_ticket->GetTicketID(ticket_id); 

    G4cerr << "Rerunning Ticket " << ticket_id  <<  G4endl;
    
    // redirect G4cout to an output file
    assign_g4cout(the_ticket, &output_stream, &output_buffer);

    // run the simulation for this ticket
    user_ticket_manager->HandleTicket(the_ticket);
    
    // restore G4cout
    return_g4cout(&output_stream, &output_buffer);

    // user action
    rc = user_ticket_manager->EndOfTicketAction(current_output_name, the_ticket);

    // user action error - exit loop
    if(rc == -1)
      break;

    // again
    rc = GetNextTicket(mode, the_ticket);

    //  ok - handle interruptions
    unblock_signals();     
  }  

  return(0);
}

// fetch the next ticket to be run
G4int g4DistributedRunManager::GetNextTicket(G4int mode, g4Ticket * the_ticket, char * client_info)
{
  G4int rc = -1;

  if(mode == MASTER){

    Ticket_Lock.Lock();
    rc = safely_get_ticket(the_ticket, client_info);
    Ticket_Lock.Unlock();
    
    if(-1 == rc){
      // no more tickets
      if(pthread_self() == main_thread)
	close_dispatch();
    }
  } 

  else if(mode == SLAVE){

    rc = request_next_ticket(client_socket, the_ticket);

  }

  return(rc);
}


// 
// private
//
void g4DistributedRunManager::zero_it()
{
  mode               = -1;

  UseRestart         = 0;

  Verbose            = 0;

  client_socket      = 0;

  main_thread        = 0;
  listen_thread      = 0;

  listen_socket      = 0;
  server_port        = DEFAULT_PORT_NUMBER;
  
  RESTART_read_stream  = NULL;
  RESTART_write_stream = NULL;
  
  Status             = 0;
}

void g4DistributedRunManager::make_handshake_string()
 {
  G4String vs = G4Version;
  vs = vs.substr(1,vs.size()-2);

  G4String UserID = G4UIcommand::ConvertToString((G4int)getuid());

  HandshakeString = "Geant4 version ";
  HandshakeString += vs;
  HandshakeString += "   ";
  HandshakeString += G4Date;
  HandshakeString += "\n";
  HandshakeString += "UserID: ";
  HandshakeString += UserID;
}

void g4DistributedRunManager::make_process_string()
{
  char   cmd_line[S_BUFFER_LEN];
  char   h_name[S_BUFFER_LEN];
  char * date;

  // gather info
  time_t t = time(NULL);
  date = ctime(&t);
 
  gethostname(h_name, S_BUFFER_LEN);

  G4String PID = G4UIcommand::ConvertToString((G4int)getpid());

  // try to get command line information
  G4String proc_name = "/proc/"+PID+"/cmdline";
  
  if(FILE * proc_file = fopen(proc_name, "r")){
    
    G4int rc = fread(cmd_line, 1, S_BUFFER_LEN, proc_file);
    
    fclose(proc_file);
    
    if(rc > 1){
      ProcessString  = "Command: ";
      ProcessString += cmd_line;
      ProcessString += "\n";
    }
  }

  // string it together
  ProcessString += "Host: ";
  ProcessString += h_name;
  ProcessString += "\n";
  ProcessString += "ProcessID: ";
  ProcessString += PID;
  ProcessString += "\n";
  ProcessString += date;
}

G4int g4DistributedRunManager::init_dispatch(G4int the_mode, G4String host)
{
  if((the_mode == MASTER) || (the_mode == SLAVE)){
    
    mode = the_mode;

  } else
    return(-1);
  
  if(mode == MASTER){

    if(!UseRestart){
      // move old restart file out of the way if it exists
      mv_restart_file();
    }

    // get the listening socket
    FD_Lock.Lock();
    listen_socket = start_listening();
    FD_Lock.Unlock();
    
    if( -1 == listen_socket)
      return(-1);
    
    thread_accept(listen_socket);
  }
  else {
    // SLAVE

    // establish connection to the master
    client_socket =  connect_to_server(host);
    
    if(-1 == client_socket)
      return(-1);

    // master and slave handshake strings should agree
    // i.e., same userid and same version of Geant4
    if(0 != request_handshake(client_socket)){
      G4cerr << "Client failed handshake with master" << G4endl;
      return(-1);
    }
  }
   
  return(0);
}

G4int g4DistributedRunManager::close_dispatch()
{
  // server code only
  // set static flag so safely_get_next_roi knows
  Dispatch_Lock.Lock();
  
  // shut down the listening thread
  pthread_cancel(listen_thread);

  // accept no more connections
  FD_Lock.Lock();
  close(listen_socket);
  FD_Lock.Unlock();
  
  // let client threads exit
  wait_for_disconnect();
  
  Dispatch_Lock.Unlock();

  return(0);
}

// seed the Master RNG
void g4DistributedRunManager::initialize_rng()
{
  Ulong  the_seed;
  int    fd = open("/dev/urandom", O_RDONLY );

  if(fd != -1){
    
    read(fd, &the_seed, sizeof(Ulong));

    close(fd);
 
  } else {
    // no /dev/random!
    struct timeval   the_time;
    int              bit_shift;    
    long             host_id = gethostid();
    
    gettimeofday(&the_time, NULL);
    
    time_t seconds = time(NULL);
    
    seconds *= the_time.tv_usec * the_time.tv_sec * host_id;
    
    bit_shift = 8 * (sizeof(time_t)/2);
    
    the_seed = (seconds << bit_shift) | (seconds >> bit_shift);
  }
  
  CLHEP::HepRandom::createInstance();
  CLHEP::HepRandom::setTheSeed((unsigned)the_seed);

  G4cout << "HepRandom seed is " << the_seed << G4endl;
}

// seed the Client RNG
void g4DistributedRunManager::initialize_client(int sock)
{
  Ulong    the_seed;

#if 0
  if(-1 == request_args(sock))
    G4cerr << "Error receiving arguments from master process. " << G4endl;
#endif
    
  the_seed = request_seed(sock);

  CLHEP::HepRandom::createInstance();
  CLHEP::HepRandom::setTheSeed(the_seed);

  G4cout << "HepRandom seed is " << the_seed << G4endl;
}

// thread safety for getting a ticket in master process
// this method is bracketed with mutex locking.
// The old (finished) ticket is passed in by requesting method
G4int g4DistributedRunManager::safely_get_ticket(g4Ticket * the_ticket, char * client_info)
{  
  // restart file update
  mark_as_finished(the_ticket);
  
  G4String ticket_id;

  the_ticket->GetTicketID(ticket_id); 

  if(Verbose  && the_ticket->IsValid()){

    if(client_info)
      G4cerr << "finished ticket: " << ticket_id  << " on " << client_info << G4endl;
    else
      G4cerr << "finished ticket: " << ticket_id  << " on " << "Master" << G4endl;
  }
  
  G4int rc = user_ticket_manager->IncrementTicket(the_ticket);
  
  while((rc != 0) && UseRestart){
    
    G4int fin =  check_if_finished(the_ticket);
    
    if(1 == fin){
      // already have this one
      
      if(Verbose && (UseRestart != 2)){
	the_ticket->GetTicketID(ticket_id); 
	
	G4cerr << "restart: skipping ticket: " <<  ticket_id  << G4endl;
      }
      
      rc = user_ticket_manager->IncrementTicket(the_ticket);
      
      continue;
    }

    break;
  }
  
  if(rc == 0)
    // all finished
    return(-1);
  
  if(Verbose){
    
    the_ticket->GetTicketID(ticket_id);   

    if(client_info)
      G4cerr << "starting ticket: " <<  ticket_id << " on " << client_info << G4endl;
    else
      G4cerr << "starting ticket: " <<  ticket_id << " on " << "Master" << G4endl;
  }      
  
  return(0);
}


//
// network code for MASTER - server
//
int g4DistributedRunManager::start_listening()
{
  int sock;

  // get an internet domain socket 
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
    G4cerr << "Failed to get a socket from the system" << G4endl;   
    return(-1);
  }
  
  // Do I need this ??
  //fcntl(sock, F_SETFL, O_NONBLOCK);

  struct sockaddr_in sock_addr;

  memset(&sock_addr, 0, sizeof(struct sockaddr_in));
  
  sock_addr.sin_port = htons((unsigned short)server_port);
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr))) {
    G4cerr <<  "bind failed: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
    return(-1);
  }
  
  if (listen(sock, 10)) {
    G4cerr <<  "listen failed: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
    return(-1);
  }

  return(sock);
}

void g4DistributedRunManager::thread_accept(int sock)
{
  pthread_attr_t      thread_attr;
  pthread_mutexattr_t mutex_attr;
  static Arg_t        arg;

  pthread_attr_init(&thread_attr);
  pthread_mutexattr_init(&mutex_attr);

  arg.dis = this;
  arg.sock = sock;

  /* initialize lock mutexes */
  pthread_mutex_init(&position_mutex, &mutex_attr);  

  // this thread will accept new connections from clients
  int rc = pthread_create(&listen_thread, &thread_attr, 
			  wait_for_connection_wrapper, (void *)&arg);
  
  if(rc != 0)
    G4cerr << "Error starting thread " << __FILE__ << ":" << __LINE__ << G4endl;
  
}

// this is started in a seperate thread
extern "C" void * wait_for_connection_wrapper(void * a)
{
  Arg_t * arg = (Arg_t *)a;

  g4DistributedRunManager * dis  = arg->dis;
  int                       sock = arg->sock;

  dis->wait_for_connection(sock);
  
  return(NULL);
}

void * g4DistributedRunManager::wait_for_connection(int sock)
{
  
  // wait for new connection forever
  while(1){
    int  sd;
    
    struct sockaddr_in new_addr;
    
    socklen_t i = sizeof(new_addr);
    
    sd = accept(sock, (struct sockaddr *)&new_addr, &i);
    
    if(sd == -1){     
      G4cerr <<  "accept failed: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
      // exit thread 
      // Exception?
      return(NULL);
    } 

    else {
      // add this socket/client to list
      pthread_t       thread_id;
      pthread_attr_t  thread_attr;	
      Arg_t           arg;
      
      // Do I need this ??
      //fcntl(sd, F_SETFL, O_NONBLOCK);
      
      arg.dis = this;
      arg.sock = sd;
      
      pthread_attr_init(&thread_attr);
      
      // this thread will accept new connections from clients
      int rc = pthread_create(&thread_id, &thread_attr, 
			      gather_requests_wrapper, (void *)&arg);
      
      if(rc != 0)
	G4cerr <<  "Error creating thread: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
      
      client_info_t * c_info = new client_info_t;
      c_info->thread_id = thread_id;
      c_info->socket_id = sd;
      
      CL_Lock.Lock();
      client_list.push_back(c_info);
      CL_Lock.Unlock();
      
    }  // end else if(sd == -1)
    
  } // end while(1)
  
}

void g4DistributedRunManager::wait_for_disconnect()
{
  if(client_list.empty())
    return;
  
  // wait for all 'connected' threads to return
  CL_Lock.Lock();
  
  std::list<client_info_t*>::iterator cl_info;
  
  for(cl_info = client_list.begin(); cl_info != client_list.end(); cl_info++){
    
    pthread_t worker_thread = (*cl_info)->thread_id;
    
    errno = 0;

    if(0 != pthread_join(worker_thread, NULL)){
      // throw Exception??
      G4cerr <<  "pthread_join failed: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
    }

    delete *cl_info;
  }

  client_list.clear();

  CL_Lock.Unlock();

  return;
}

// this is started in a seperate Master process thread
extern "C" void * gather_requests_wrapper(void * a)
{
  Arg_t * arg = (Arg_t *)a;

  g4DistributedRunManager * dis  = arg->dis;
  int                    sock = arg->sock;

  dis->gather_requests(sock);
  
  return(NULL);
}


void * g4DistributedRunManager::gather_requests(int sock)
{
  char   buffer[S_BUFFER_LEN];
  char * buf = buffer;
  int    n = S_BUFFER_LEN; 

  while(1){
    // service new requests from clients
    memset(buffer, 0, n);
    
    int red = read_socket(sock, buf, n);
    
    if(red != n){
      //G4cerr <<  "Failed to read from socket on MASTER: " << __FILE__ << ":" << __LINE__ << G4endl;
      return(NULL);
    }

    // provide new ticket
    if(strstr(buffer, "FINISHED"))
      provide_next_ticket(buffer, sock);
    
    // provide handshake
    else if(strstr(buffer, "handshake"))
      provide_handshake(buffer, sock);
    
    // provide random seed
    else if(strstr(buffer, "seed"))
      provide_seed(buffer, sock);
    
    // provide run arguments
    else if(strstr(buffer, "args"))
      provide_args(buffer, sock);
    
    else
      // error
      return(NULL);
    
  } // end while(1)

  return(NULL);
}

void g4DistributedRunManager::provide_handshake(char * buffer, int sock)
{
  G4int   n = S_BUFFER_LEN; 
  int     writ;

  G4String clientHandshake(buffer);

  memset(buffer, 0, n);
 
  if(clientHandshake.contains(HandshakeString))
    // return 0 to client
    sprintf(buffer, "%d", 0);

  else
    // return =1 to client
    sprintf(buffer, "%d", -1);

  writ = write_socket(sock, buffer, n);
  
  if(writ != n){
    G4cerr <<  "error writing to client: " << __FILE__ << ":" << __LINE__ << G4endl;
    return;
  }
  
}


// finished ticket returned from client
// buffer is S_BUFFER_LEN
void g4DistributedRunManager::provide_next_ticket(char * buffer, int sock)
{
  // the pieces of a network packet
  char command[CMD_LEN];
  char c_host[HOST_LEN];
  char c_pid[PID_LEN];

  g4Ticket  * client_ticket = user_ticket_manager->GetNewTicket();

  // unpack finished ticket buffer to ticket
  UnpackNetworkTicket(buffer, (char *)command, (char *)c_host, (char *)c_pid, client_ticket);

  char client_info[S_BUFFER_LEN];

  sprintf(client_info, "%s:%s", c_host, c_pid);

  // request a new ticket
  int rc = GetNextTicket(MASTER, client_ticket, client_info);

  if(rc != 0)
    // no tickets left - signal client
    client_ticket->InValidate();

  // return new ticket to client
  client_ticket->TicketToBuffer(buffer);
  
  delete client_ticket;
  
  int writ = write_socket(sock, buffer, S_BUFFER_LEN);
  
  if(writ != S_BUFFER_LEN){
    // error - Throw Exception??
    G4cerr <<  "error writing to client: " << __FILE__ << ":" << __LINE__ << G4endl;
    return;
  }

}

void g4DistributedRunManager::provide_seed(char * buffer, int sock)
{
  int  n = S_BUFFER_LEN; 
  int  writ;

  memset(buffer, 0, n);
  
  long client_seed = CLHEP::RandFlat::shootInt(LONG_MAX);

  sprintf(buffer, "%ld", client_seed);
  
  writ = write_socket(sock, buffer, n);
  
  if(writ != n){
    // error - Throw Exception??
    G4cerr <<  "error writing to client: " << __FILE__ << ":" << __LINE__ << G4endl;
  }
  
}

void g4DistributedRunManager::provide_args(char * buffer, int sock)
{
#if 0
  int  n = S_BUFFER_LEN; 
  int  writ;

  memset(buffer, 0, n);
  
  sprintf(buffer, "%d %d", UsingPositions, UsingIncrements);
  
  writ = write_socket(sock, buffer, n);
  
  if(writ != n){
    // error - Throw Exception??
    G4cerr <<  "error writing to client: " << __FILE__ << ":" << __LINE__ << G4endl;
  }
#endif  
}

//
// network code for SLAVE - client
//
G4int g4DistributedRunManager::connect_to_server(G4String host)
{
  int sock_desc;

  struct sockaddr_in pin;
  struct hostent    *hp;
  
  /* load host entry */
  if ((hp = gethostbyname((const char *)host)) == 0){ 
    // error
    G4cerr <<  "error getting host by name: " << __FILE__ << ":" << __LINE__ << G4endl;
    return(-1);
  }
  
  /* fill in socket struct */
  memset((char *)&pin, 0, sizeof(pin));
  pin.sin_family = AF_INET;
  pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
  pin.sin_port = htons((unsigned short)server_port);
  
  errno = 0;
  /* get an internet domain socket */
  if ((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    G4cerr <<  "Failed to get a socket: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
    return(-1);
  }

  errno = 0;
  /* connect to port @ host */
  if (connect(sock_desc, (struct sockaddr *)&pin, sizeof(pin)) == -1){
    // error - Throw Exception??
    G4cerr <<  "Connect failed: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
    G4cerr <<  "Check host " << host << " to be sure master is running." << G4endl; 
    return(-1);
  }
  
  // ??
  //fcntl (sock_desc, O_NONBLOCK, 1L);

  return(sock_desc);
}

G4int g4DistributedRunManager::request_handshake(int sock)
{
  char buffer[S_BUFFER_LEN];
  int  n = S_BUFFER_LEN;
    
  static pid_t pid = getpid();
  static char  host[HOST_LEN];
  int          host_len = HOST_LEN;
  static int   hostname = 0;
  int          rc = 0;

  if(!hostname){
    gethostname(host, host_len);
    host[HOST_LEN-1] ='\0';
    hostname = 1;
  } 

  // send server a request
  memset(buffer, 0, n);

  sprintf(buffer, "host %s pid %d handshake %s", host, pid, HandshakeString.data());
  
  int writ = write_socket(sock, buffer, n);
  
  if(writ != n){
    G4cerr <<  "Failed to write to master: " << __FILE__ << ":" << __LINE__ << G4endl;
    return(-1);
  }

  // read reply
  memset(buffer, 0, n);

  int red = read_socket(sock, buffer, n);

  if(red != n){
    G4cerr <<  "Failed to read from master: " << __FILE__ << ":" << __LINE__ << G4endl;
    return(-1);
  }

  rc = strtol(buffer, NULL, 0);

  return(rc);
}

Ulong  g4DistributedRunManager::request_seed(int sock)
{
  unsigned long seed = 0;

  char buffer[S_BUFFER_LEN];
  int  n = S_BUFFER_LEN;
    
  static pid_t pid = getpid();
  static char  host[HOST_LEN];
  G4int        host_len = HOST_LEN;
  static G4int hostname = 0;

  if(!hostname){
    gethostname(host, host_len);
    host[255] ='\0';
    hostname = 1;
  } 

  // send server a request
  memset(buffer, 0, n);

  sprintf(buffer, "host %s pid %d seed", host, pid);
  
  int writ = write_socket(sock, buffer, n);

  if(writ != n){
    G4cerr <<  "Failed to write to master: " << __FILE__ << ":" << __LINE__ << G4endl;
    G4cerr << "Client Random Seed set to zero" << G4endl;
    return(0);
  }

  // read reply
  memset(buffer, 0, n);

  int red = read_socket(sock, buffer, n);

  if(red != n){
    G4cerr <<  "Failed to read from master: " << __FILE__ << ":" << __LINE__ << G4endl;
    G4cerr << "Client Random Seed set to zero" << G4endl;
    return(0);
  }

  seed = strtol(buffer, NULL, 0);

  G4cout << "Client Random Seed: " << seed << G4endl;

  return(seed);
}

G4int g4DistributedRunManager::request_args(int sock)
{
#if 0
  char buffer[S_BUFFER_LEN];
  int  n = S_BUFFER_LEN;
    
  static pid_t pid = getpid();
  static char  host[HOST_LEN];
  G4int        host_len = HOST_LEN;
  static G4int hostname = 0;

  if(!hostname){
    gethostname(host, host_len);
    host[255] ='\0';
    hostname = 1;
  } 

  // send server a request
  memset(buffer, 0, n);

  sprintf(buffer, "host %s pid %d args", host, pid);
  
  int writ = write_socket(sock, buffer, n);

  if(writ != n){
    G4cerr <<  "Failed to write to master: " << __FILE__ << ":" << __LINE__ << G4endl;
    G4cerr << "Client Random Seed set to zero" << G4endl;
    return(-1);
  }

  // read reply
  memset(buffer, 0, n);

  int red = read_socket(sock, buffer, n);

  if(red != n){
    G4cerr <<  "Failed to read from master: " << __FILE__ << ":" << __LINE__ << G4endl;
    G4cerr << "Client Random Seed set to zero" << G4endl;
    return(-1);
  }

  if(2 != sscanf(buffer, "%d %d", &UsingPositions, &UsingIncrements))
    return(-1);

#endif

  return(0);
}

// client returns the finished ticket and fetches next ticket
G4int g4DistributedRunManager::request_next_ticket(int sock, g4Ticket * the_ticket)
{
  static char  command[CMD_LEN] = "FINISHED";
  static int   have_hostname = 0;
  static char  host[HOST_LEN];
  static char  pid[PID_LEN];

  if(!have_hostname){
    // get host name and pid
    if(-1 == gethostname(host, HOST_LEN)){
      // error
      G4cerr <<  "Failed to gethostname: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
      return(-1);
    }
    
    host[HOST_LEN-1] = '\0';

    pid_t my_pid = getpid();

    sprintf(pid, "%d", my_pid);

    have_hostname  = 1;
  } 

  // pack finished ticket into a buffer to return to master
  char   buffer[S_BUFFER_LEN];

  PackNetworkTicket(buffer, (char *)command, (char *)host, (char *)pid, the_ticket);

  int writ = write_socket(sock, buffer, S_BUFFER_LEN);

  if(writ != S_BUFFER_LEN){
    G4cerr <<  "Failed to write to master: " << __FILE__ << ":" << __LINE__ << G4endl;
    return(-1);
  }

  // read reply from master
  memset(buffer, 0, S_BUFFER_LEN);

  int red = read_socket(sock, buffer, S_BUFFER_LEN);

  if(red != S_BUFFER_LEN){
    G4cerr <<  "Failed to read from master: " << __FILE__ << ":" << __LINE__ << G4endl;
    return(-1);
  }

  the_ticket->BufferToTicket(buffer);

  if(!the_ticket->IsValid()){
    // tickets are all gone
    // G4cerr << "Completed all tickets!" << G4endl;
    return(-1);
  }

  return(0);
}


//
// basic socket read/write 
//
G4int g4DistributedRunManager::read_socket(int sd, char * buf, int n)
{
  int count;
  int red;
  
  count = red = 0;
  
  while(count < n){
    
    errno = 0;
    red =  recv(sd, buf, n - count, 0);
    
    if(red > 0){
      count += red;
      buf += red;
      continue;
    }
    else if( red == 0){
      //G4cerr <<  "recv failed: peer has shutdown: " << __FILE__ << ":" << __LINE__ << G4endl;
      return(-2);
    }
    else {
      G4cerr <<  "recv failed: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
      return(-1);
    }

  } /* end while */
  
  return(count);
}

G4int g4DistributedRunManager::write_socket(int sd, char * buf, int n)
{	
  int count;
  int rit;
  
  count = rit = 0;
  
  while(count < n){

    errno = 0;
    rit = send(sd, buf, n - count, 0);
    
    if(rit > 0){
      count += rit;
      buf += rit;
      continue;
    }
    
    else if(rit == 0){
      //G4cerr <<  "send failed: peer has shutdown: " << __FILE__ << ":" << __LINE__ << G4endl;
      return(-2);
    }
    else {
      G4cerr <<  "send failed: " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << G4endl;
      return(-1);
    }

  } /* end while */
  
  return(count);
  
}

// block all signals
void g4DistributedRunManager::block_signals()
{
  sigset_t	siginfo;

  sigfillset(&(siginfo));
  sigprocmask(SIG_BLOCK, &(siginfo), NULL);
}

// unblock all signals
void g4DistributedRunManager::unblock_signals()
{
  sigset_t	siginfo;

  sigfillset(&(siginfo));
  sigprocmask(SIG_UNBLOCK, &(siginfo), NULL);
}

void  g4DistributedRunManager::mark_as_finished(g4Ticket * finished_ticket)
{
  // starting tickets
  if(!finished_ticket->IsValid())
    return;
  
  Restart_Lock.Lock();
  
  handle_restart_file(finished_ticket, WRITE_RESTART);

  Restart_Lock.Unlock();

  return;
}
 
// check restart file to see if this ticket was done previously
G4int g4DistributedRunManager::check_if_finished(g4Ticket * ticket)
{
  Restart_Lock.Lock();
  
  G4int rc = handle_restart_file(ticket, READ_RESTART);

  Restart_Lock.Unlock();

  if((RESTART_read_stream == NULL) && (UseRestart == 1)){
    // user wants to use a restart file but there isn't one!!!
    UseRestart = 0;
  }

  return(rc);
}

void g4DistributedRunManager::assign_g4cout(g4Ticket * the_ticket, ofstream *filestr, streambuf ** backup)
{
  G4String ticket_id;
  the_ticket->GetTicketID(ticket_id); 

  char file_name[S_BUFFER_LEN];

  FD_Lock.Lock();

  streambuf *psbuf;

  sprintf(file_name, "./output_%s", ticket_id.data());
 
  current_output_name = file_name;

  filestr->open(file_name);

  G4cout.flush();
  
  *backup = G4cout.rdbuf();     // back up cout's streambuf
  
  psbuf = filestr->rdbuf();     // get file's streambuf

  G4cout.rdbuf(psbuf);          // assign streambuf to cout
 
  FD_Lock.Unlock();

  // put an informational header in the file
  G4cout << ProcessString << HandshakeString << G4endl << G4endl;
 
  G4cout << "Ticket: " << ticket_id << G4endl << G4endl;

}

void g4DistributedRunManager::return_g4cout(ofstream *filestr, streambuf ** backup)
{
  FD_Lock.Lock();

  G4cout.flush();

  G4cout.rdbuf(*backup);        // restore G4cout's original streambuf
  
  filestr->close();

  FD_Lock.Unlock();
  
}

// runs inside of Restart_Lock
G4int g4DistributedRunManager::handle_restart_file(g4Ticket * ticket, int restart_mode)
{  
  G4String ticket_id;
  ticket->GetTicketID(ticket_id); 
  
  G4String finished_ticket_id;

  G4long r_line_len = 128;
  char * restart_line = new char[r_line_len];

  switch(restart_mode){
    
  case READ_RESTART:
    // look through file for tickets
    
    // open file
    if(RESTART_read_stream == NULL){
      FD_Lock.Lock();
      errno = 0;
      RESTART_read_stream  = fopen(RESTART_FILE, "r+");
      if(RESTART_read_stream == NULL){
	G4cerr <<  "Failed to open " << RESTART_FILE << " for reading. " << strerror(errno) << " at " <<  __FILE__ << ":" << __LINE__ << G4endl;
	FD_Lock.Unlock();
	return(-1);
      }
      FD_Lock.Unlock();      
    }
    
    // rewind
    fseek(RESTART_read_stream, 0, SEEK_SET);
    
    while(-1 != getline(&restart_line,  (size_t *)&r_line_len, RESTART_read_stream)){ 
      
      // strip trailing '\n'
      restart_line[strlen(restart_line)-1] = '\0';

      finished_ticket_id = (const char *)restart_line;

      if(ticket_id == finished_ticket_id){
	// found ticket
	return(1);
      }      
    }
    
    break;
    
  case WRITE_RESTART:
    // append newly finished tickets to file
    
    // open fie
    if(RESTART_write_stream == NULL){      
      FD_Lock.Lock();
      errno = 0;
      RESTART_write_stream  = fopen(RESTART_FILE, "a+");
      if(RESTART_write_stream == NULL){
	G4cerr <<  "Failed to open " << RESTART_FILE << " for writing. " << strerror(errno) << " at " <<  __FILE__ << ":" << __LINE__ << G4endl;
	FD_Lock.Unlock();
	return(-1);
      }
      FD_Lock.Unlock();
    }
    
    // write this ticket info to the end of file
    errno = 0;
    int rc = fprintf(RESTART_write_stream, "%s\n", ticket_id.data());
    
    if(rc < 0){
      fprintf(stderr, "Failed to write to %s at %s:%d\n", RESTART_FILE,  __FILE__, __LINE__);
      fprintf(stderr, "%s\n", strerror(errno));
    }
    fflush(RESTART_write_stream);
    
    break;    
  }
  
  return(0);

}

// move old restart file to file.xx
void g4DistributedRunManager::mv_restart_file()
{
  struct stat stat_buffer;
  char   the_file_name[1024];
  G4int  index = 0;

  sprintf(the_file_name, "%s", RESTART_FILE);
  
  while(-1 != stat(the_file_name, &stat_buffer)){
    
    sprintf(the_file_name, "%s.%02d", RESTART_FILE, index++);
    
  }

  if(!index)
    // no restart file to begin with
    return;

  // else
  char command[1024];

  sprintf(command, "mv -f %s %s", RESTART_FILE, the_file_name);

  system(command);

  return;
}

 void  g4DistributedRunManager::PackNetworkTicket(char * buffer, char * command, char * client_host, char * pid, g4Ticket * tx_ticket)
 {
   sprintf(buffer, "%s %s %s ^", command, client_host, pid);

   char tx_buffer[TICKET_BUFFER_LEN];

   tx_ticket->TicketToBuffer((char *)tx_buffer);

   strcat(buffer, tx_buffer);

 }

void  g4DistributedRunManager::UnpackNetworkTicket(char * buffer, char * command, char * client_host, char *client_pid, g4Ticket * tx_ticket)
 {
   sscanf(buffer, "%s %s %s" , command, client_host, client_pid);

   char * ptr = strchr(buffer, '^');

   if(ptr == NULL)
     return;

   tx_ticket->BufferToTicket(ptr+1);

 }
