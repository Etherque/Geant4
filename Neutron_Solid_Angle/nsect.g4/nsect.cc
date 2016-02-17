#include "NSECTDetectorConstruction.hh"
#include "NSECTPrimaryGeneratorAction.hh"
#include "NSECTRunAction.hh"
#include "NSECTEventAction.hh"
#include "NSECTSteppingAction.hh"
#include "NSECTVisManager.hh"
#include "NSECTTicketManager.hh"

#include "G4VisExecutive.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4VisManager.hh"
#include "G4PhysListFactory.hh"

#include "QGSP_BIC_HP.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BERT.hh"

#include "g4VerboseTrigger.hh"
#include "g4IdealDetector.hh"
#include "g4DistributedRunManager.hh"

#include <unistd.h>
using namespace std;

struct _input {
  G4int    parallel;             // flag to indicate run mode
  G4int    number_of_positions;  // 
  G4double events_per_position;  // total number of events at a position
  G4double events_per_increment; // events per process (increment of events_per_position)
  G4int    use_restart;          // flag 
  char   * host;                 // master host name
  G4int    port;
  G4int    verbose;
};
typedef struct _input Input_t;

G4int   parse_command_line(int ac, char ** av, Input_t * the_input);
void    usage(char * name);
G4int g4VerboseTrigger::Trigger = 0;
G4int GlobalTrigger;

int main(int argc,char** argv) 
{
  Input_t the_input;

  // read cmd line input
  if(-1 == parse_command_line(argc, argv, &the_input)){
    G4cerr << "Failed parsing command line" << G4endl;
    G4cerr << "See -h for usage" << G4endl;
    return(-1);
  }
  
 // G4VSteppingVerbose::SetInstance(new NSECTSteppingVerbose);
  g4VerboseTrigger * t_verbose = new g4VerboseTrigger();
  GlobalTrigger = 0;
  g4DistributedRunManager * RunManager;

  if(the_input.parallel == 1){
    // Use Distributed Run manager

    if(!the_input.host){
      // master
      RunManager = new g4DistributedRunManager(the_input.use_restart, 
					       the_input.port);      
    } else {
      // client
      RunManager = new g4DistributedRunManager(the_input.host, the_input.port);
    }
  
  } else {
    // Default run manager - sets rng seed
    RunManager = new g4DistributedRunManager;
  }

  if(RunManager->GetStatus() != 1){
    G4cerr << "Failed to start RunManager.  Exiting." << G4endl;
    return(-1);
  }

  g4IdealDetector * IdealDetector = new  g4IdealDetector(1*m, new G4String("GammaDetector"));
  IdealDetector->SetEnergyRange(846*keV, 848*keV, new G4String("Iron"));
  IdealDetector->SetEnergyRange(961*keV,963*keV, new G4String("Copper"));
  //  IdealDetector->SetNumBins(4, 8);

  // User Initialization classes (mandatory)
  NSECTDetectorConstruction* NSECTdetector = new NSECTDetectorConstruction;

  RunManager->SetUserInitialization(NSECTdetector);

   G4PhysListFactory *physList_Factory = new G4PhysListFactory(); 
   G4VUserPhysicsList* physics_list = physList_Factory->GetReferencePhysList("FTFP_BERT_HP");//FTFP_BERT
  //G4VUserPhysicsList* physics_list = new QGSP_BIC_HP();
 
  RunManager->SetUserInitialization(physics_list);

  
  NSECTPrimaryGeneratorAction * NSECTgenerator = new NSECTPrimaryGeneratorAction();

  // UserAction classes
  RunManager->SetUserAction(NSECTgenerator);
  RunManager->SetUserAction(new NSECTRunAction(IdealDetector));  
  RunManager->SetUserAction(new NSECTEventAction());
  RunManager->SetUserAction(new NSECTSteppingAction(t_verbose));
  
  // Initialize G4 kernel
  RunManager->Initialize();  

  ///////////////////////////////////////////////////////////
  // Non Distributed Modes
  ///////////////////////////////////////////////////////////

  if(!the_input.parallel){
    
    //get the pointer to the User Interface manager 
    G4UImanager * UI = G4UImanager::GetUIpointer();  
    
    if(argc==1)
      // Define (G)UI terminal for interactive mode  
      { 
	// visualize
	G4VisManager* visManager = new NSECTVisManager;
	visManager->Initialize();

	// G4UIterminal is a (dumb) terminal.
	G4UIsession * session = 0;
	
	session = new G4UIterminal(new G4UItcsh);      
	
	UI->ApplyCommand("/control/execute vis.mac");    
	session->SessionStart();

	delete session;
	delete visManager;
      }
    else
      // Batch mode
      { 
	G4String command = "/control/execute ";
	G4String fileName = argv[1];
	UI->ApplyCommand(command+fileName);

	//IdealDetector->PrintHistogram();
      }
    
    delete IdealDetector;
    delete RunManager;

    return(0);
  }

  // ELSE
  ///////////////////////////////////////////////////////////
  // Distributed Mode
  ///////////////////////////////////////////////////////////

  RunManager->SetVerbose(the_input.verbose);

  NSECTTicketManager * TicketManager = new NSECTTicketManager(RunManager, 
							      NSECTdetector, 
							      NSECTgenerator,
							      the_input.number_of_positions,
							      the_input.events_per_position,
							      the_input.events_per_increment);

  RunManager->SetUserTicketManager(TicketManager);

  RunManager->RunTickets();

  // Job termination
  delete TicketManager;
  delete IdealDetector;
  delete RunManager;
  
  return 0;
}

#define ARGS "H:pn:i:x:vrh?"

int parse_command_line(int ac, char ** av, Input_t * the_input)
{
  int        opt;

  memset(the_input, 0, sizeof(struct _input));

  // default is one position
  the_input->number_of_positions = 1;

  // input flags
  while((opt = getopt(ac, av, ARGS)) != -1){
    switch (opt) {
      
    case 'p':
      the_input->number_of_positions = NSECT_NUM_POSITIONS;
      the_input->parallel = 1;
      break;     

    case 'n':
      if(1 != sscanf(optarg, "%lf", &the_input->events_per_position)){
	G4cerr << "Error scanning -n option" << G4endl;
	return(-1);
      }      
      the_input->parallel = 1;
      break;     

    case 'i':
      if(1 != sscanf(optarg, "%lf", &the_input->events_per_increment)){
	G4cerr << "Error scanning -i option" << G4endl;
	return(-1);
      }      
      the_input->parallel = 1;
      break;     

    case 'x':
      if(1 != sscanf(optarg, "%d", &the_input->port)){
	G4cerr << "Error scanning -x option" << G4endl;
	return(-1);
      } 
      the_input->parallel = 1;
      break;     

    case 'H':
      the_input->host = new char[strlen(optarg) + 1];
      if(!the_input->host){
	G4cerr << "memory exhausted " << __FILE__ << ":" << __LINE__ << G4endl;
	return(-1);
      }
      strcpy(the_input->host, optarg);
      the_input->parallel = 1;
      break;     

    case 'r':
      the_input->use_restart = 1;
      break;     

    case 'v':
      the_input->verbose = 1;
      break;     

    case '?':
    case 'h':
      usage(av[0]);
      exit(0);
      break;     

    default:
      G4cerr << "Unrecognized flag: '" << opt << "'" << G4endl;
      return(-1);
    }    
  }

  if((!the_input->host) && (the_input->parallel)){
    // master
    if(the_input->events_per_position <= 0){
      G4cerr << "Events per position (set with -n) must be > 0" << G4endl;
      return(-1);
    }
    
    if(the_input->events_per_increment > the_input->events_per_position){
      G4cerr << "Events per increment (set with -i) may not be more than total events per position (set with -n)" << G4endl;
      return(-1);
    }

    if(the_input->events_per_increment == 0){
      // no increments used -also handled in g4DistributedRunManager
      the_input->events_per_increment = the_input->events_per_position;
    }

    else if(the_input->events_per_increment < 0){
      G4cerr << "Events per increment (set with -i) must be >= 0" << G4endl;
      return(-1);
    }
  }

  return(0);
}
  
void usage(char * name)
{
  G4cerr << "Usage: " << name << " (run with visuaization - expects vis.mac)" << G4endl;
  G4cerr << "- or - " << G4endl;
  G4cerr << "Usage: " << name << " run.mac (run batch)" << G4endl;
  G4cerr << "- or - " << G4endl;
  G4cerr << "Usage: " << name << " -n N [ -p -i n -x port -r -v  ] (run once as master)" << G4endl;
  G4cerr << "\t-n N:    total number of events per position" << G4endl;
  G4cerr << "\t-p:      use multiple positions <default is to use one position only>" << G4endl;
  G4cerr << "\t-i n:    incremental number of events per process" << G4endl;
  G4cerr << "\t-x port: use specified port rather than default" << G4endl;
  G4cerr << "\t-r:      rerun using a 'position_restart' file" << G4endl;
  G4cerr << "\t-v:      verbose" << G4endl;
  G4cerr << "- or - " << G4endl;
  G4cerr << "Usage: " << name << " -H  master_host [-x port] (run many as slaves)" << G4endl;
}  


