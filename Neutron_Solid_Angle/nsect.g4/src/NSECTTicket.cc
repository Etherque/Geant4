#include "NSECTTicket.hh"

NSECTTicket::NSECTTicket(G4int Position_Number, G4int Increment_Number, G4double Number_Of_Events)
{
  PositionNumber  = Position_Number;
  IncrementNumber = Increment_Number;
  NumberOfEvents  = Number_Of_Events;
}

NSECTTicket::~NSECTTicket()
{ 
  ;
}

void NSECTTicket::GetTicketID(G4String& the_id)
{
  
  char buffer[1024];

  sprintf(buffer, "p_%03d:i_%03d", PositionNumber,  IncrementNumber);
  
  the_id = (const char *)buffer;
}



void NSECTTicket::TicketToBuffer(char * tx_buffer)
{

  sprintf(tx_buffer, "%d %d %f", PositionNumber,  IncrementNumber, NumberOfEvents);

  return;
}

void NSECTTicket::BufferToTicket(char * tx_buffer)
{
  G4int pos    = 0;
  G4int incr   = 0;
  G4double num = 0;
  
  if(3 != sscanf(tx_buffer, "%d %d %lf", &pos, &incr, &num) || (tx_buffer == 0)){
    // error - Throw Exception??
    G4cerr <<  "error scanning ticket: " << __FILE__ << ":" << __LINE__ << G4endl;
    return;
  }

  PositionNumber  = pos;
  IncrementNumber = incr;
  NumberOfEvents  = num;
  
}

void  NSECTTicket::InValidate()
{
  PositionNumber  = -1;
  IncrementNumber = -1;
  NumberOfEvents  = -1;
}

G4int NSECTTicket::IsValid()
{
  if((PositionNumber  == -1) && (IncrementNumber == -1) && (NumberOfEvents  == -1))
    return(0);
  
  return(1);
}
