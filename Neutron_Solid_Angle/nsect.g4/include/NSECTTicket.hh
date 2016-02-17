#ifndef _NSECTTicket_hh
#define _NSECTTicket_hh

#include "g4Ticket.hh"

class NSECTTicket : public g4Ticket
{
public:

  NSECTTicket(G4int Position_Number = -1, G4int Increment_Number = -1, G4double Number_Of_Events = -1);

  ~NSECTTicket();

  void SetPositionNumber(G4int pn) {PositionNumber = pn;}
  void SetIncrementNumber(G4int in) {IncrementNumber = in;}
  void SetNumberOfEvents(G4double ne) {NumberOfEvents = ne;}

  int GetPositionNumber() {return  PositionNumber;}
  int GetIncrementNumber() {return  IncrementNumber;}
  int GetNumberOfEvents() {return  NumberOfEvents;}

  void Reset() {
    PositionNumber  = -1; 
    IncrementNumber = -1;
    NumberOfEvents =  -1;
  }

  void TicketToBuffer(char * tx_string);

  void BufferToTicket(char * tx_string);

  void GetTicketID(G4String& id_string);
  
  void InValidate();

  G4int IsValid();

private:

  // zero indexed
  G4int     PositionNumber;
  G4int     IncrementNumber;

  G4double  NumberOfEvents;  

};

#endif
