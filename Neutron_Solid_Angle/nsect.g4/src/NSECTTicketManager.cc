#include "NSECTTicketManager.hh"

#include "globals.hh"

NSECTTicketManager::NSECTTicketManager(g4DistributedRunManager     * runManager, 
				       NSECTDetectorConstruction   * detector,   
				       NSECTPrimaryGeneratorAction * generator,
				       G4int num_positions, G4double events_p_pos, G4double events_p_inc) : NumberOfPositions(0),NumberOfEventsPerPosition(0),NumberOfEventsPerIncrement(0) 
{
  ResetTickets();

  the_run_manager = runManager;
  the_detector    = detector;
  the_generator   = generator; // not used
  
  if(the_run_manager->IsMaster()){
    NumberOfPositions          = num_positions;
    NumberOfEventsPerPosition  = events_p_pos;
    NumberOfEventsPerIncrement = events_p_inc;
    
    if(NumberOfPositions < 1){
      // error - Exception??
      G4cerr << "ERROR - Number of positions must be greater than or equal to 1" << G4endl;
      return;
    }
    
    
    if(NumberOfEventsPerPosition < 1){
      // error - Exception??
      G4cerr << "ERROR - Number of events must be greater than 1" << G4endl;
      return;
    }
    
    if((NumberOfEventsPerIncrement < 1) || (NumberOfEventsPerIncrement >= NumberOfEventsPerPosition)){
      NumberOfEventsPerIncrement = NumberOfEventsPerPosition;
    }
  }
}

int NSECTTicketManager::HandleTicket(g4Ticket * ticket)
{
  NSECTTicket * my_ticket = (NSECTTicket *)ticket;

  G4int    next_position = my_ticket->GetPositionNumber() + 1; // position is an index;
  G4double num_events    = my_ticket->GetNumberOfEvents();
  
  the_detector->setTargetPosition(next_position);
  
  the_run_manager->BeamOn((int)floor(num_events));
  
  return(0);
}

int  NSECTTicketManager::EndOfTicketAction(const G4String output_name, g4Ticket *)
{
  if(output_name.length()){
    
    char tmp[1024];
    sprintf(tmp, "gzip %s", output_name.data());
    system(tmp);
  }
  
  return(0);
}

void NSECTTicketManager::ResetTickets()
{
  current_position   = 0;
  current_increment  = 0; 
}

// Only runs in Master Process
// return 0 when tickets are exhausted
// 1 otherwise
G4int NSECTTicketManager::IncrementTicket(g4Ticket * ticket)
{
  NSECTTicket * next_ticket = (NSECTTicket *)ticket;
  
  while(current_position < NumberOfPositions){
    
    if((current_increment * NumberOfEventsPerIncrement) < NumberOfEventsPerPosition){
      // need to do this increment
      next_ticket->SetPositionNumber(current_position);
      next_ticket->SetIncrementNumber(current_increment);
      
      G4double remainder = ((current_increment+1) * NumberOfEventsPerIncrement) - NumberOfEventsPerPosition;
      
      if(remainder > 0)
	next_ticket->SetNumberOfEvents(NumberOfEventsPerIncrement - remainder);
      else
	next_ticket->SetNumberOfEvents(NumberOfEventsPerIncrement);	
      
      // next increment
      current_increment++;
      
      return(1);
      
    } // end if((current_increment...
    
    // finished with this position
    current_increment = 0;
    current_position++;

  } // end while
  
  return(0);

}

g4Ticket * NSECTTicketManager::GetNewTicket()
{
  NSECTTicket * return_ticket = new NSECTTicket();

  return((g4Ticket *)return_ticket);
}
