#include "g4TicketManager.hh"

g4TicketManager::g4TicketManager()
{
  ;
}

g4TicketManager::~g4TicketManager()
{
  ;
}

g4Ticket * g4TicketManager::GetNewTicket()
{
  g4Ticket * return_ticket = new g4Ticket;

  return(return_ticket);
}
