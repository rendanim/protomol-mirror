#include <protomol/integrator/MTSIntegrator.h>
#include <protomol/types/ScalarStructure.h>
#include <protomol/types/Vector3DBlock.h>
#include <protomol/force/ForceGroup.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/base/PMConstants.h>
#include <protomol/base/Exception.h>

using namespace std;
using namespace ProtoMol;

//____ MTSIntegrator

MTSIntegrator::MTSIntegrator() :
  StandardIntegrator(), myNextIntegrator(NULL), myCycleLength(0) {}

MTSIntegrator::MTSIntegrator(int cycles, ForceGroup *overloadedForces,
                             StandardIntegrator *nextIntegrator) :
  StandardIntegrator(overloadedForces), myNextIntegrator(nextIntegrator),
  myCycleLength(cycles) {
  // The plan for this constructor:
  //  We make sure that we get a non-zero pointer to ForceGroup object.
  //  In some cases (for test purpose) we may not have any forces to evaluate,
  //  but the force evaluation is still call ...

  myNextIntegrator->myPreviousIntegrator = this;
}

MTSIntegrator::~MTSIntegrator() {
  delete myNextIntegrator;
}

void MTSIntegrator::doDriftOrNextIntegrator() {
  preDriftOrNextModify();
  myNextIntegrator->run(myCycleLength);
  postDriftOrNextModify();
}

void MTSIntegrator::initialize(GenericTopology *topo, Vector3DBlock *positions,
                               Vector3DBlock *velocities,
                               ScalarStructure *energies) {
  myNextIntegrator->initialize(topo, positions, velocities, energies);
  StandardIntegrator::initialize(topo, positions, velocities, energies);
}

void MTSIntegrator::getParameters(vector<Parameter> &parameters) const {
  parameters.push_back(Parameter("cyclelength",
      Value(myCycleLength, ConstraintValueType::Positive())));
}

MTSIntegrator *MTSIntegrator::make(const vector<Value> &values, ForceGroup *fg,
                                   StandardIntegrator *nextIntegrator) const {
  assertParameters(values);

  return adjustAlias(doMake(values, fg, nextIntegrator));
}

