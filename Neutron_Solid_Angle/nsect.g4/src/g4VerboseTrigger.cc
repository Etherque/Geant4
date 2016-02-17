#include "g4VerboseTrigger.hh"

g4VerboseTrigger::g4VerboseTrigger()
{
  Clear();
}

g4VerboseTrigger::~g4VerboseTrigger()
{ 
  Clear();
}

void g4VerboseTrigger::AddStep(const G4Step * fStep)
{
  StoredTrackInfo.push_back(new g4VerboseInfo(fStep));
}

void g4VerboseTrigger::Print()
{
  if(!g4VerboseTrigger::Trigger)
    return;

  G4int print_banner =  0;
  G4int track_id     = -1;
  G4int parent_id    = -1;

  for(unsigned int i = 0; i < StoredTrackInfo.size(); i++){
    
    if((track_id != StoredTrackInfo[i]->GetTrackID()) ||
       (parent_id !=StoredTrackInfo[i]->GetParentID())){
      
      // this is the first step of a track
      // create the init step
      PrintInit(StoredTrackInfo[i]);

      // for each new track, print a banner
      track_id  = StoredTrackInfo[i]->GetTrackID();
      parent_id = StoredTrackInfo[i]->GetParentID();
    }
    
    StoredTrackInfo[i]->DumpTrack(print_banner);
  }
}

void g4VerboseTrigger::PrintInit(g4VerboseInfo * step_1)
{ 
  // this is a hack to replicate the initial step which is not
  // reported to the UserStepping Action (!)

  int           print_banner = 1;
  g4VerboseInfo step_0(step_1);

  if((step_1->GetParentID() == 0) && ( step_1->GetStepNumber() == 1 )){
    // XXX WRONG! - This should print the coord of the particle gun XXX
    G4ThreeVector pos(0,0,0);
    step_0.SetPosition(pos);
  }

  step_0.SetTLength(0);

  if(step_1->GetStepNumber() > 1){

    // there is a previous partial track of this particle printed
    // already - get the track length!

    G4int track_id = step_1->GetTrackID();
    G4int step_number  = step_1->GetStepNumber() - 1;

    // find track_id/step_num in the list and get trach length
    for(unsigned int i = 0; i < StoredTrackInfo.size(); i++){
      if((track_id == StoredTrackInfo[i]->GetTrackID()) && 
	 (step_number == StoredTrackInfo[i]->GetStepNumber())){
	
	step_0.SetTLength(StoredTrackInfo[i]->GetTLength());
	step_0.SetPosition(StoredTrackInfo[i]->GetPosition());

      }
    }
  }
  
  step_0.SetProcess("initStep");
  step_0.SetStepNumber(step_1->GetStepNumber() - 1);
  step_0.SetTEnergy(0);
  step_0.SetSLength(0);
  step_0.SetVolume("World");

  step_0.DumpTrack(print_banner);
  
}

void g4VerboseTrigger::Clear()
{
  vector<g4VerboseInfo*>::iterator it;
  
  for(it = StoredTrackInfo.begin(); it < StoredTrackInfo.end(); it++) {
    delete (*it);// delete the g4VerboseInfo
    *it = 0; 
  } 
  
  StoredTrackInfo.clear();
  
  g4VerboseTrigger::Trigger = 0;
}

//
//
// g4VerboseInfo
//
//

g4VerboseInfo::g4VerboseInfo(const g4VerboseInfo * other_info)
{
  step_number    = other_info->step_number;
  position       = other_info->position;
  k_energy       = other_info->k_energy;
  t_energy       = other_info->t_energy;
  s_length       = other_info->s_length;
  t_length       = other_info->t_length;

  n_vol_pt       = other_info->n_vol_pt;
  volume_name    = other_info->volume_name;

  particle_name  = other_info->particle_name;
  track_id       = other_info->track_id;
  parent_id      = other_info->parent_id;

  process        = other_info->process;

}

g4VerboseInfo::g4VerboseInfo(const G4Step *fStep)
{
  G4Track * fTrack  = fStep->GetTrack();
              
  step_number    = fTrack->GetCurrentStepNumber();
  position       = fTrack->GetPosition();
  k_energy       = fTrack->GetKineticEnergy();
  t_energy       = fStep->GetTotalEnergyDeposit();
  s_length       = fStep->GetStepLength();
  t_length       = fTrack->GetTrackLength();

  n_vol_pt       = fTrack->GetNextVolume();

  if(n_vol_pt)
    volume_name    = fTrack->GetVolume()->GetName();
  else
    volume_name    = "";

  particle_name  = fTrack->GetDefinition()->GetParticleName();
  track_id       = fTrack->GetTrackID();
  parent_id      = fTrack->GetParentID();

  if(fStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL){
    process = fStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  } else {
    process = "UserLimit";
  }

}

g4VerboseInfo::~g4VerboseInfo()
{;}

void g4VerboseInfo::DumpTrack(G4int Banner)
{
  if(Banner){
    TrackBanner();
  }

  //G4int prec = G4cout.precision(3);
  G4cout.precision(5);
  
  G4cout << std::setw(5) << step_number << " "
	 << std::setw(8) << G4BestUnit(position.x(),"Length") 
	 << std::setw(8) << G4BestUnit(position.y(),"Length")
	 << std::setw(8) << G4BestUnit(position.z(),"Length")
	 << std::setw(8) << G4BestUnit(k_energy,"Energy")
	 << std::setw(8) << G4BestUnit(t_energy,"Energy")
	 << std::setw(8) << G4BestUnit(s_length,"Length")
	 << std::setw(8) << G4BestUnit(t_length,"Length")
	 << "  ";
  
  // if( fStepStatus != fWorldBoundary){ 
  if( n_vol_pt != 0 ) { 
    G4cout << std::setw(10) << volume_name;
  } else {
    G4cout << std::setw(10) << "OutOfWorld";
  }
  
  G4cout << "  " <<  std::setw(10) << process;
  
  G4cout << G4endl;
  
}

void g4VerboseInfo::TrackBanner()
{
  G4cout << G4endl;
  G4cout << "*******************************************************"
	 << "**************************************************"
	 << G4endl;
  G4cout << "* Triggered Information: "
	 << "  Particle = " << particle_name
	 << ","
	 << "   Track ID = " << track_id
	 << ","
	 << "   Parent ID = " << parent_id
	 << G4endl;
  G4cout << "*******************************************************"
	 << "**************************************************"
	 << G4endl;
  //G4cout << G4endl;
  
  G4cout << G4endl;    
  G4cout << std::setw( 5) << "#Step#"     << " "
	 << std::setw( 6) << "X"          << "    "
	 << std::setw( 6) << "Y"          << "    "  
	 << std::setw( 6) << "Z"          << "    "
	 << std::setw( 9) << "KineE"      << " "
	 << std::setw( 9) << "dEStep"     << " "  
	 << std::setw(10) << "StepLeng"     
	 << std::setw(10) << "TrakLeng" 
	 << std::setw(10) << "Volume"    << "  "
	 << std::setw(10) << "Process"   << G4endl;	          
}
