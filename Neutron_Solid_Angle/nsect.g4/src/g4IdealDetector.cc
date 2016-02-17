#include "g4IdealDetector.hh"
#include "G4UnitsTable.hh"

g4IdealDetector::g4IdealDetector(G4double r, G4String * name)
{
  zero_it();

  if(name)
    id_name = new G4String(*name);

  if(r <= 0)
    // exception ??
    return;

  id_radius = r;

  // initialize to 1 bin - i.e., all angles in one bin
  SetNumBins(1, 1);
}

g4IdealDetector::~g4IdealDetector()
{

  if(hist_buffer){
    delete[] hist_buffer;
    delete[] hist_array;
  }
  
  std::list<r_info_t*>::iterator info;
  info = range_list.begin();
  
  while(info != range_list.end()){
    
    delete[] (*info)->hist_array;
    delete[] (*info)->hist_buffer;
    //delete[] (*info)->label;
    
    r_info_t * doomed_info = *info;
    info++;
    
    delete doomed_info;
    
  }

  delete id_name;
}

void g4IdealDetector::zero_it()
{
  id_radius     = 0;
  id_name       = 0;
  i_bins        = 0;
  a_bins        = 0;
  hist_array    = 0;
  hist_buffer   = 0;
}

G4int g4IdealDetector::SetNumBins(G4int num_inclination_bins, G4int  num_azimuth_bins)
{
  if(hist_buffer){
    
    delete[] hist_buffer;
    delete[] hist_array;
    
    hist_array  = 0;
    hist_buffer = 0;
  }
  
  if(!range_list.empty()){
    std::list<r_info_t*>::iterator info;
    
    for(info = range_list.begin(); info != range_list.end(); info++){
      delete[] (*info)->hist_array;
      delete[] (*info)->hist_buffer;
      (*info)->hist_array  = 0;
      (*info)->hist_buffer = 0;
    }
  }

  a_bins        = num_azimuth_bins;
  i_bins        = num_inclination_bins;
  
  if((i_bins > 0) && (a_bins > 0)){
    
    if(range_list.empty()){
      // no ranges specified
      if(-1 == allocate_histogram(&hist_buffer, &hist_array))
	return(-1);
    }
    else {
      
      std::list<r_info_t*>::iterator info;
      
      for(info = range_list.begin(); info != range_list.end(); info++){
	if(-1 == allocate_histogram(&(*info)->hist_buffer, &(*info)->hist_array))
	  return(-1);
      }
    }    
  } else
    // a bin is <= 0
    return(-1);
  
  i_bin_size = (M_PI)/i_bins;
  a_bin_size = (M_PI * 2)/a_bins;
  
  return(0);
}

void g4IdealDetector::ZeroHistogram()
{
  
  if(hist_buffer)
    memset(hist_buffer, 0, sizeof(G4long)*i_bins * a_bins);
  
  std::list<r_info_t*>::iterator info;
  
  for(info = range_list.begin(); info != range_list.end(); info++){
    memset((*info)->hist_buffer, 0, sizeof(G4long)*i_bins * a_bins);
  }    
}

void g4IdealDetector::SetEnergyRange(G4double min, G4double max, G4String * label)
{

  if(hist_buffer){
    // delete hist_buffer and hist array and use range_list histograms
    // instead
    delete[] hist_buffer;
    delete[] hist_array;
    
    hist_array  = 0;
    hist_buffer = 0;
  }
  
  r_info_t * r_info = new r_info_t;

  r_info->min_E       = min;
  r_info->max_E       = max;
  r_info->label       = label;
  r_info->hist_array  = 0;
  r_info->hist_buffer = 0;

  if(-1 == allocate_histogram(&(r_info->hist_buffer), &(r_info->hist_array)))
    // error - exception??
    return;

  range_list.push_back(r_info);
}

G4int g4IdealDetector::PrintAngle(const G4Step* fStep)
{
  // should it be detected
  if(NotQualified(fStep))
    return(0);

  // sets member variable cartesian
  if(-1 == find_location_on_sphere(fStep))
    return(-1);

  // sets member variable polar
  if(-1 == find_angle_at_location())
    return(-1);

  // is it in the energy range?
  G4String name = fStep->GetTrack()->GetDefinition()->GetParticleName();
  G4double energy = get_energy(fStep);
  G4double time   = get_time(fStep);

  if(range_list.empty()){
    // all energies printed
    if(id_name)
      G4cout << *id_name << ": ";
    
    G4cout << std::setprecision(6) << G4BestUnit(energy, "Energy") << " " << name;
    G4cout << " ("  << std::setprecision(6) <<  polar.x();
    G4cout << ", "  << std::setprecision(6) <<  polar.y();
    G4cout << ", "  << std::setprecision(6) <<  polar.z();
    G4cout << ") cm-radian ";
    G4cout << " at " << G4BestUnit(time, "Time") << G4endl;
  }
  else {
    // there is a list of energy ranges
    std::list<r_info_t*>::iterator info;
    
    for(info = range_list.begin(); info != range_list.end(); info++){
      
      r_info_t * r_info = *info;
      
      if((energy > r_info->max_E) || (energy < r_info->min_E))
	continue;
      
      if(id_name)
	G4cout << *id_name << ": ";
      
      if(r_info->label)
	G4cout << *r_info->label << ": ";
      
      G4cout << std::setprecision(6) << G4BestUnit(energy, "Energy") << " " << name;
      G4cout << " ("  << std::setprecision(6) <<  polar.x();
      G4cout << ", "  << std::setprecision(6) <<  polar.y();
      G4cout << ", "  << std::setprecision(6) <<  polar.z();
      G4cout << ") cm-radian ";
      G4cout << " at " << G4BestUnit(time, "Time") << G4endl;
    }    
  }
  
  return(0);
}

G4int g4IdealDetector::PrintCoordinates(const G4Step* fStep)
{ 
  // should it be detected
  if(NotQualified(fStep))
    return(0);

  // sets member variable cartesian
  if(-1 == find_location_on_sphere(fStep))
    return(-1);

  // is it in the energy range?
  G4String name = fStep->GetTrack()->GetDefinition()->GetParticleName();
  G4double energy = get_energy(fStep);
  G4double time   = get_time(fStep);
  
  if(range_list.empty()){
    // all energies printed
    if(id_name)
      G4cout << *id_name << ": ";
    
    G4cout << std::setprecision(6) <<  G4BestUnit(energy, "Energy") << " " << name;
    G4cout << " ("  << std::setprecision(6) <<  cartesian.x();
    G4cout << ", "  << std::setprecision(6) <<  cartesian.y();
    G4cout << ", "  << std::setprecision(6) <<  cartesian.z();
    G4cout << ") cm ";
    G4cout << " at " << G4BestUnit(time, "Time") << G4endl;
  } 
  else {
    // there is a list of energy ranges
    std::list<r_info_t*>::iterator info;
    
    for(info = range_list.begin(); info != range_list.end(); info++){
      
      r_info_t * r_info = *info;
      
	if((energy > r_info->max_E) || (energy < r_info->min_E))
	  continue;
      
	if(id_name)
	  G4cout << *id_name << ": ";
    
      if(r_info->label)
	G4cout << *r_info->label << ": ";
	
      G4cout << std::setprecision(6) <<  G4BestUnit(energy, "Energy") << " " << name;
      G4cout << " ("  << std::setprecision(6) <<  cartesian.x();
      G4cout << ", "  << std::setprecision(6) <<  cartesian.y();
      G4cout << ", "  << std::setprecision(6) <<  cartesian.z();
      G4cout << ") cm ";
      G4cout << " at " << G4BestUnit(time, "Time") << G4endl;
    }
  }

  return(0);
}

void g4IdealDetector::PrintHistogram()
{
  if(hist_array){
    
    if(id_name)
      G4cout << *id_name << ": ";
    
    if((i_bins == 1) && ( a_bins == 1)){
      // no spacial binning - just print counts
      G4cout <<  "Total Counts: " << hist_array[0][0] << G4endl;
      
    } else
      print_bins(hist_array, NULL);
  } 

  else {
    // there is a list of energy ranges
    std::list<r_info_t*>::iterator info;
    
    for(info = range_list.begin(); info != range_list.end(); info++){
      
      r_info_t * r_info = *info;
      
      if(id_name)
	G4cout << *id_name << ": ";
      
      if(r_info->label)
	G4cout << *r_info->label << ": ";
      
      if((i_bins == 1) && ( a_bins == 1)){
	// no spacial binning - just print counts
	G4cout <<  "Total Counts: " << r_info->hist_array[0][0] << G4endl;
	
      } else
	print_bins(r_info->hist_array, r_info->label);
      
    }
  }
}

// runs through each azimuth angle at an inclination angle
void g4IdealDetector::print_bins(G4long ** the_hist, G4String * label)
{
  G4cout << "Inclination (theta) bin_size " << std::setprecision(5) <<  std::setw(6) << i_bin_size;
  G4cout << "    Azimuth (phi) bin_size: "<< std::setprecision(5) <<  std::setw(6)  << a_bin_size;
  
  G4cout << G4endl;
  
  for(G4int i = 0; i < i_bins; i++) {
    
    G4long * a_hist = the_hist[i];
    
    for(G4int a = 0; a < a_bins; a++) {
      
      G4double i_radians = i * i_bin_size;
      G4double a_radians = a * a_bin_size;
      
      if(label)
	G4cout << *label << ": ";
      
      G4cout << "("  << std::setprecision(5) <<  std::setw(6) << i_radians;
      G4cout << ", " << std::setprecision(5) <<  std::setw(6) << a_radians;
      G4cout << ") " << a_hist[a] <<  G4endl;
    }
  }
}

// records data in histogram
G4int g4IdealDetector::Record(const G4Step* fStep)
{
  // should it be detected
  if(NotQualified(fStep))
    return(0);

  // sets member variable cartesian
  if(-1 == find_location_on_sphere(fStep))
    return(-1);

  // sets member variable polar
  if(-1 == find_angle_at_location())
    return(-1);

  // is the energy in range
  G4double energy = get_energy(fStep);

  G4double inclination = polar.y();
  G4double azimuth     = polar.z();

  G4int index_i = (int)floor(inclination / i_bin_size);
  G4int index_a = (int)floor(azimuth / a_bin_size);
  
  // range check
  if(index_i > i_bins-1)
    index_i = i_bins-1;
  if(index_a > a_bins-1)
    index_a = a_bins-1;

  if(hist_array){
    // index into histogram
    hist_array[index_i][index_a]++;
  }
  else {
    // there is a list of energy ranges
    std::list<r_info_t*>::iterator info;
    
    for(info = range_list.begin(); info != range_list.end(); info++){
      
      r_info_t * r_info = *info;
      
      if((energy > r_info->max_E) || (energy < r_info->min_E))
	continue;

      r_info->hist_array[index_i][index_a]++;
    }
  }

  return(0);
}

G4int g4IdealDetector::allocate_histogram(G4long ** buffer, G4long *** array)
{
  *buffer   = new G4long[i_bins * a_bins];
  
  if(!*buffer){
    G4cerr << "Memory Exhausted at " << __LINE__ << ":" << __FILE__ << G4endl;
    return(-1);
  }
  
  memset(*buffer, 0, sizeof(G4long)*i_bins * a_bins);

  *array = new G4long * [i_bins];
  
  if(!*array){
    G4cerr << "Memory Exhausted at " << __LINE__ << ":" << __FILE__ << G4endl;
    return(-1);
  }
  
  for(G4int i = 0; i < i_bins; i++)
    (*array)[i] = *buffer + (i * a_bins);

  return(0);
}

G4int g4IdealDetector::find_location_on_sphere(const G4Step * Step)
{
  // given a ray preStep to postStep, find the points it intersects
  // our sphere
  G4ThreeVector start_p =  Step->GetPreStepPoint()->GetPosition(); 
  G4ThreeVector end_p   =  Step->GetPostStepPoint()->GetPosition(); 
  
  G4ThreeVector  intersection_p;

  // initialize to bad values
  cartesian.setX(NAN);
  cartesian.setY(NAN);
  cartesian.setZ(NAN);

  polar.setX(NAN);
  polar.setY(NAN);
  polar.setZ(NAN);

  if(-1 == find_sphere_line_intersect(start_p, end_p, intersection_p))
    // no intersection!!
    return(-1);
      
  //copy the intersection into cartesian
  cartesian = intersection_p;
      
  return(0);
}

//
//http://local.wasp.uwa.edu.au/~pbourke/geometry/sphereline/
//
#define EPS 0.000001
G4int  g4IdealDetector::find_sphere_line_intersect(G4ThreeVector start_p, G4ThreeVector end_p, G4ThreeVector &intersection_p)
{
   G4double a,b,c;
   G4double bb4ac;
   G4double mu1, mu2;

   // initialize as (0,0,0)
   G4ThreeVector delta_p;
   G4ThreeVector center;
   
   delta_p.setX(end_p.x() - start_p.x());
   delta_p.setY(end_p.y() - start_p.y());
   delta_p.setZ(end_p.z() - start_p.z());
   
   a = SQ(delta_p.x()) + SQ(delta_p.y()) + SQ(delta_p.z());

   b = 2 * (delta_p.x() * (start_p.x() - center.x()) + delta_p.y() * (start_p.y() - center.y()) + delta_p.z() * (start_p.z() - center.z()));

   c =  SQ(center.x()) + SQ(center.y()) + SQ(center.z());
   c += SQ(start_p.x()) + SQ(start_p.y()) + SQ(start_p.z());
   c -= 2 * (center.x() * start_p.x() + center.y() * start_p.y() + center.z() * start_p.z());
   c -= id_radius * id_radius;

   bb4ac = SQ(b)  - (4 * a * c);

   // no intersection!!
   if (fabs(a) < EPS || bb4ac < 0) 
      return(-1);
   
   mu1 = (-b + sqrt(bb4ac)) / (2 * a);
   mu2 = (-b - sqrt(bb4ac)) / (2 * a);

   G4ThreeVector p1;
   G4ThreeVector p2;

   p1.setX(start_p.x() + mu1*(end_p.x() - start_p.x()));
   p1.setY(start_p.y() + mu1*(end_p.y() - start_p.y()));
   p1.setZ(start_p.z() + mu1*(end_p.z() - start_p.z()));
   
   p2.setX(start_p.x() + mu2*(end_p.x() - start_p.x()));
   p2.setY(start_p.y() + mu2*(end_p.y() - start_p.y()));
   p2.setZ(start_p.z() + mu2*(end_p.z() - start_p.z()));
   
   // return point closest to end_p
   G4double d1 = distance(p1, end_p);
   G4double d2 = distance(p2, end_p);

   if(d1 > d2)
     // set intersection at p2 point
     intersection_p = p2;
   else
     // set intersection at p1 point
     intersection_p = p1;
   
   return(0);
}

G4double g4IdealDetector::distance(G4ThreeVector p1, G4ThreeVector p2)
{
  G4double d;
  
  d = SQ(p1.x() - p2.x()) + SQ(p1.y() - p2.y()) + SQ(p1.z() - p2.z());
  d = sqrt(d);

  return(d);
}

// convert from cartesian to polar coordinates
G4int g4IdealDetector::find_angle_at_location()
{
  G4double inclination = cartesian.theta();  // theta angle from z-axis
  G4double azimuth     = cartesian.phi();;   // phi angle projected in xy plane

  // limit angle range
  while(azimuth < 0)
    azimuth += 2 * M_PI;

  polar.setX(id_radius);
  polar.setY(inclination);
  polar.setZ(azimuth);

  return(0);
}


G4double g4IdealDetector::get_energy(const G4Step * Step)
{
  return(Step->GetTrack()->GetKineticEnergy());
}

G4double g4IdealDetector::get_time(const G4Step * Step)
{
  // correction for difference between detector crossing time and edge
  // of world crossing time
  G4ThreeVector start_p =  Step->GetPreStepPoint()->GetPosition(); 
  
  // beginning of step to end of step
  G4double step_distance = Step->GetStepLength();
  G4double step_duration = Step->GetDeltaTime();

  // beginning of step to idealdetector
  G4double id_distance = distance(start_p, cartesian);

  G4double delta_duration = step_duration * (id_distance - step_distance)/step_distance;

  double total_time = Step->GetTrack()->GetGlobalTime();

  // correct total time
  total_time += delta_duration;
  
  return(total_time);
}

G4int g4IdealDetector::NotQualified(const G4Step * fStep)
{
  // is there a step?
  if(!fStep)
    return(1);
  
  // is it leaving the world?
  if(fStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume() != 0)
    return(1);
  
  // did it this step begin inside our detector?
  G4ThreeVector start_p =  fStep->GetPreStepPoint()->GetPosition(); 
  G4double start_r = sqrt(SQ(start_p.x()) + SQ(start_p.y()) + SQ(start_p.z()));

  if(start_r >= id_radius)
    // skip this particle - the track begins outside the IdealDetector
    return(1);

  // this particle should be detected
  return(0);


}

