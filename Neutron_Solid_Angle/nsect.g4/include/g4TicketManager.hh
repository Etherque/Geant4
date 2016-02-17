#ifndef _g4TicketManager_hh
#define _g4TicketManager_hh

#include "g4Ticket.hh"
#include "globals.hh"


class g4TicketManager 
{

public:

  g4TicketManager();

  virtual ~g4TicketManager();

  // returns 0 on sucess  and -1 on completion or error
  virtual G4int    HandleTicket(g4Ticket * new_ticket) = 0;

  // output_name is file name for this ticket
  virtual G4int    EndOfTicketAction(const G4String, g4Ticket *) { return(0);}

  // return 0 when tickets are exhausted
  // 1 otherwise
  virtual G4int    IncrementTicket(g4Ticket * ticket) = 0;

  virtual void     ResetTickets() = 0;

  virtual g4Ticket * GetNewTicket() = 0;
};



#endif
