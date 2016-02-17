#ifndef _NSECTTicketManager_hh
#define _NSECTTicketManager_hh

#include "g4DistributedRunManager.hh"
#include "g4TicketManager.hh"

#include "NSECTDetectorConstruction.hh"
#include "NSECTPrimaryGeneratorAction.hh"
#include "NSECTTicket.hh"

class NSECTTicketManager : public g4TicketManager
{

public:

  NSECTTicketManager(g4DistributedRunManager   * runManager, NSECTDetectorConstruction * detector, NSECTPrimaryGeneratorAction * generator,
		     G4int num_positions, G4double events_p_pos, G4double events_p_inc);
  
  int   HandleTicket(g4Ticket * ticket);
  
  int   EndOfTicketAction(const G4String output_name, g4Ticket * the_ticket);
  
  G4int IncrementTicket(g4Ticket * ticket);

  void  ResetTickets();

  g4Ticket * GetNewTicket();

private:
  g4DistributedRunManager       * the_run_manager;
  NSECTDetectorConstruction     * the_detector;
  NSECTPrimaryGeneratorAction   * the_generator;

  G4int    NumberOfPositions;
  G4double NumberOfEventsPerPosition;
  G4double NumberOfEventsPerIncrement;

  G4int current_position;
  G4int current_increment;
};

#endif
