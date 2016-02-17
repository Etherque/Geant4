#ifndef _g4Ticket_hh
#define _g4Ticket_hh

#include "globals.hh"
#include "G4String.hh"

#define TICKET_BUFFER_LEN 1024

class g4Ticket 
{
public:

  g4Ticket();

  virtual ~g4Ticket();
  
  // string ID for the ticket
  virtual void GetTicketID(G4String& ticket_id);

  // marshalling routines
  virtual void TicketToBuffer(char * tx_string);

  virtual void BufferToTicket(char * tx_string);
  
  // set ticket to an illegal state
  // signifying 'all tickets finished'
  virtual void InValidate();

  // check validity
  virtual G4int IsValid();
};



#endif
