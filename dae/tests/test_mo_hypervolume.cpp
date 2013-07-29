#include <sys/time.h>
#include <sys/resource.h>

#include <iostream>
#include <iomanip>

#include <eo>
#include <moeo>
#include "../src/core/decomposition_mo.h"

#define LOG_FILL ' '
#define FORMAT_LEFT_FILL_WIDTH(width) "\t" << std::left << std::setfill(LOG_FILL) << std::setw(width) 
// #define FORMAT_LEFT_FILL_W_PARAM FORMAT_LEFT_FILL_WIDTH(20)

//#include <limits>

bool AreSame(double a, double b)
{
  //double EPSILON = std::numeric_limits<double>::epsilon();
  double EPSILON = 1.0e-6;
  return fabs(a - b) < EPSILON;
}

int main ( int argc, char* argv[] )
{ 
  //typedef moeoDualRealObjectiveVector<DecompoMOTraits, eoMaximizingDualFitness> DecompoMOObjectives;
  //typedef DecompoMOObjectives ObjectiveVector ;

  //typedef daex::DecompositionMO::ObjectiveVector OVT;
  //typedef daex::DecompositionMO::Fitness Fitness;

  //OVT* ref = new OVT(2.0,false);
  // moeoHyperVolumeMetric<OVT> * m_hypervolume = new moeoHyperVolumeMetric<OVT>(true,*ref); 
  //moeoUnaryMetricStat<daex::DecompositionMO>* stat = new moeoUnaryMetricStat<daex::DecompositionMO>(*m_hypervolume, "HypVol");

  //OVT ref(2.0,false);
  // moeoHyperVolumeMetric<OVT> m_hypervolume(true, ref);

  //  daex::DecompositionMO::ObjectiveVector ref(2.0,false);

  daex::DecompositionMO::ObjectiveVector ref;
  ref[0]= daex::DecompositionMO::Fitness(100.0, false); // sans faire de reserve ?
  ref[1]= daex::DecompositionMO::Fitness(1000.0, false); // sans faire de reserve ?

  moeoHyperVolumeMetric<daex::DecompositionMO::ObjectiveVector> m_hypervolume(false, ref);

  moeoUnaryMetricStat<daex::DecompositionMO> stat(m_hypervolume, "HypVol");
  std::cout << "ref=" << ref << std::endl;

  daex::DecompositionMO::ObjectiveVector OB1, OB2, OB3, OB4, OB5;
  OB1[0]= daex::DecompositionMO::Fitness(8.0, true); // sans faire de reserve ?
  OB1[1]= daex::DecompositionMO::Fitness(120.0, true); // sans faire de reserve ?
  OB2[0]= daex::DecompositionMO::Fitness(12.0, true); // sans faire de reserve ?
  OB2[1]= daex::DecompositionMO::Fitness(100.0, true); // sans faire de reserve ?

  // OB3[0]= daex::DecompositionMO::Fitness(13.0, true); // sans faire de reserve ?
  //OB3[1]= daex::DecompositionMO::Fitness(60.0, true); // sans faire de reserve ?

  //OB4[0]= daex::DecompositionMO::Fitness(13.0, true); // sans faire de reserve ?
  //OB4[1]= daex::DecompositionMO::Fitness(80.0, true); // sans faire de reserve ?

  OB5[0]= daex::DecompositionMO::Fitness(10.0, true); // sans faire de reserve ?
  OB5[1]= daex::DecompositionMO::Fitness(150.0, true); // sans faire de reserve ?

  std::vector <daex::DecompositionMO::ObjectiveVector> front;
  front.push_back(OB1);
  front.push_back(OB2);

  std::cout << "hyp=" << m_hypervolume(front) << std::endl;
  if (AreSame(m_hypervolume(front), 82720.0)) std::cout << "OK" << std::endl;
  else std::cout << "KO" << std::endl;

  front.push_back(OB5);

  std::cout << "hyp=" << m_hypervolume(front) << std::endl;
  if (AreSame(m_hypervolume(front), 82720.0)) std::cout << "OK" << std::endl;
  else std::cout << "KO" << std::endl;
  
 
   /*
  daex::DecompositionMO::ObjectiveVector OB1, OB2, OB3, OB4, OB5;
  OB1[0]= daex::DecompositionMO::Fitness(8.0, true); // sans faire de reserve ?
  OB1[1]= daex::DecompositionMO::Fitness(120.0, true); // sans faire de reserve ?
  OB2[0]= daex::DecompositionMO::Fitness(12.0, true); // sans faire de reserve ?
  OB2[1]= daex::DecompositionMO::Fitness(100.0, true); // sans faire de reserve ?
  OB3[0]= daex::DecompositionMO::Fitness(13.0, true); // sans faire de reserve ?
  OB3[1]= daex::DecompositionMO::Fitness(60.0, true); // sans faire de reserve ?
  OB4[0]= daex::DecompositionMO::Fitness(13.0, true); // sans faire de reserve ?
  OB4[1]= daex::DecompositionMO::Fitness(80.0, true); // sans faire de reserve ?
  OB5[0]= daex::DecompositionMO::Fitness(10.0, true); // sans faire de reserve ?
  OB5[1]= daex::DecompositionMO::Fitness(150.0, true); // sans faire de reserve ?

  std::vector <daex::DecompositionMO::ObjectiveVector> front;
  front.push_back(OB1);
  front.push_back(OB2);

  std::cout << "hyp=" << m_hypervolume(front) << std::endl;
  if (AreSame(m_hypervolume(front), 3.0)) std::cout << "OK" << std::endl;
  else std::cout << "KO" << std::endl;

  front.push_back(OB3);

  std::cout << "hyp=" << m_hypervolume(front) << std::endl;
  if (AreSame(m_hypervolume(front), 3.0+(1.0-2.0/3.0)*(1.0-4.0/5.0))) std::cout << "OK" << std::endl;
  else std::cout << "KO" << std::endl;

  front.push_back(OB4);

  std::cout << std::setprecision(100) << m_hypervolume(front) << std::endl;
  std::cout << std::setprecision(100) << 3.0+(1.0-2.0/3.0)*(1.0-4.0/5.0) << std::endl;
  std::cout << std::setprecision(100) <<  (3.0+(1.0-2.0/3.0)*(1.0-4.0/5.0)) - m_hypervolume(front) << std::endl;
  if (AreSame(m_hypervolume(front), 3.0+(1.0-2.0/3.0)*(1.0-4.0/5.0))) std::cout << "OK" << std::endl;
  else std::cout << "KO" << std::endl;

  front.push_back(OB5);

  std::cout << "hyp=" << m_hypervolume(front) << std::endl;
  if (AreSame(m_hypervolume(front), 3.0+(1.0-2.0/3.0)*(1.0-4.0/5.0))) std::cout << "OK" << std::endl;
  else std::cout << "KO" << std::endl;
  */

}

