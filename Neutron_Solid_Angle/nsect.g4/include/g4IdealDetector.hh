#ifndef g4IdealDetector_h
#define g4IdealDetector_h 1

// Examines particles leaving the world.  Particle steps are projected
// onto a sphere of given radius and then reported in cartesian
// coordinates (x, y, z) in cm or spherical coordinates (r, t, p) in
// cm and radians

// The radius r is in cm The inclination or polar angle t (theta)
// ranges from 0 to PI measured from the positive Z axis.  The azimuth
// or projection angle p (phi) ranges from -PI to PI measured from the
// positive X axis.  For purposes of printing and binning, the azimuth
// angle is adjusted to range from 0 to 2*PI.

// If SetNumBins is called, binning is created from 0 to 2*PI for
// azimuth (phi) and and 0 to PI for inclination angles (theta).  A
// histogram is generated which can be subsequently printed with
// PrintHistogram.  The PrintHistogram prints the start of the bin (in
// radians) as (theta, phi) followed by the number of counts.

// If energy range is set, particles outside the range are disgarded.
// Otherwise all energies are printed or counted.

#include "G4Step.hh"

#include <math.h>
#include <list>

#define SQ(x) (x)*(x)

struct range_info {
  G4double     min_E;
  G4double     max_E;
  G4String   * label;
  G4long     * hist_buffer;
  G4long    ** hist_array;
};
typedef struct range_info r_info_t;

class g4IdealDetector {

public:
  g4IdealDetector(G4double radius, G4String * detector_name = 0);

  ~g4IdealDetector();

  // alocates histograms - returns -1 on memory overflow
  // inclination (theta) bins are 0 to PI
  // azimuth (phi) bins are 0 to 2*PI
  G4int SetNumBins(G4int num_inclination_bins, G4int num_azimuth_bins);

  // may be called multiple times to set various ranges
  void SetEnergyRange(G4double min, G4double max, G4String * label = 0);

  // print per/step
  G4int  PrintAngle(const G4Step* fStep);
  G4int  PrintCoordinates(const G4Step* fStep);

  // record in histogram
  G4int  Record(const G4Step* fStep);
  void PrintHistogram();
  void ZeroHistogram();

private:
  G4double   id_radius;
  G4String * id_name;

  // location on sphere
  G4ThreeVector cartesian;
  G4ThreeVector polar;

  // energy range(s)
  std::list<r_info_t*>  range_list;

  // master histogram - used if no energy ranges are set
  G4long ** hist_array;
  G4long  * hist_buffer;

  // number of bins
  G4int     i_bins;
  G4int     a_bins;

  // size of bins
  G4double  i_bin_size;
  G4double  a_bin_size;

  // private methods
  void     zero_it();
  G4int    NotQualified(const G4Step * Step);
  G4int    allocate_histogram(G4long ** buffer, G4long *** array);
  void     print_bins(G4long ** the_hist, G4String * label);
  G4int    find_location_on_sphere(const G4Step * Step);
  G4int    find_angle_at_location();
  G4int    find_sphere_line_intersect(G4ThreeVector start_p, G4ThreeVector end_p, G4ThreeVector &intersection_p);
  G4double distance(G4ThreeVector p1, G4ThreeVector p2);  
  G4double get_energy(const G4Step * Step);
  G4double get_time(const G4Step * Step);

};

#endif
