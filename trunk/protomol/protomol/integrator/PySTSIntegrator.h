#ifndef PYSTSINTEGRATOR_H
#define PYSTSINTEGRATOR_H

#include "STSIntegrator.h"

namespace ProtoMol {

  class PySTSIntegrator : public STSIntegrator {
    
  public:
    PySTSIntegrator() : STSIntegrator() {}
    PySTSIntegrator(Real timestep, ForceGroup *overloadedForces) : 
      STSIntegrator(timestep, overloadedForces) {}
  private:
    virtual STSIntegrator *doMake(const std::vector<Value> &values,
				  ForceGroup *fg) const {}
    
    
  };
  
}




#endif
