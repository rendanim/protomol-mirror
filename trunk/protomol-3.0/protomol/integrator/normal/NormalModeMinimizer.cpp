#include <protomol/integrator/normal/NormalModeMinimizer.h>
#include <protomol/base/Report.h>
#include <protomol/type/ScalarStructure.h>
#include <protomol/type/Vector3DBlock.h>
#include <protomol/force/ForceGroup.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/topology/TopologyUtilities.h>
#include <protomol/base/PMConstants.h>
#include <protomol/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;
//____ NormalModeMinimizer

const string NormalModeMinimizer::keyword("NormalModeMinimizer");

NormalModeMinimizer::NormalModeMinimizer() :
  STSIntegrator(), NormalModeUtilities()
{}

NormalModeMinimizer::NormalModeMinimizer(Real timestep, int firstmode,
                                         int nummode, Real gamma, int seed,
                                         Real temperature,
                                         Real minimlim, bool rforce,
                                         bool rediag, bool simplemin,
                                         ForceGroup *overloadedForces) :
  STSIntegrator(timestep, overloadedForces),
  NormalModeUtilities(firstmode, nummode, gamma, seed, temperature),
  minLim(minimlim), randforce(rforce), reDiag(rediag), simpleMin(simplemin)
{}

NormalModeMinimizer::~NormalModeMinimizer() {
  report.precision(5);
  if (numSteps) 
    report 
      << hint << "[NormalModeMinimizer::~NormalModeMinmizer] Minimizer "
      "average iterations = "
      << (float)avItrs / (float)numSteps << ", force average calcs = "
      << (float)avMinForceCalc / (float)numSteps << endl;
}

void NormalModeMinimizer::initialize(ProtoMolApp *app) {
  STSIntegrator::initialize(app);
  initializeForces();
  myPreviousNormalMode =
    dynamic_cast<NormalModeUtilities *>(myPreviousIntegrator);
  //Check if using complement of prev integrator, then copy all integrator
  // paramiters
  if (firstMode == -1 && numMode == -1) {
    firstMode = myPreviousNormalMode->firstMode; numMode =
      myPreviousNormalMode->numMode;
    myGamma = myPreviousNormalMode->myGamma; mySeed =
      myPreviousNormalMode->mySeed; myTemp = myPreviousNormalMode->myTemp;
  }
  //NM initialization if OK
  //last for complimentary forces, no gen noise
  NormalModeUtilities::initialize((int)app->positions.size(), app->topology,
                                  myForces, COMPLIMENT_FORCES); 
  //
  //initialize minimizer noise vars
  randStp = 0.0;
  //***********************************************************
  //diagnostics
  avItrs = 0;       //average number of minimizer iterations/force calcs
  avMinForceCalc = 0;
  numSteps = 0;     //total steps
}

void NormalModeMinimizer::run(int numTimesteps) {
  if (numTimesteps < 1)
    return;

  //check valid eigenvectors
  if (*Q == 0)
    report << error << "No Eigenvectors for NormalMode integrator." << endr;
  //
  preStepModify();
  //remove last random pertubation
  if (randforce) app->positions.intoWeightedAdd(-randStp, gaussRandCoord1);
  //do minimization with local forces, max loop 100, set subSpace 
  //minimization true
  itrs =
    minimizer(minLim, 100, simpleMin, reDiag, true, &forceCalc, &lastLambda,
              &app->energies, &app->positions,
              app->topology);
  numSteps++;
  avItrs += itrs;
  avMinForceCalc += forceCalc;
  report
    << debug(5) << "[NormalModeMinimizer::run] iterations = " << itrs
    << " average = " << (float)avItrs / (float)numSteps << " force calcs = "
    << forceCalc << " average = " << (float)avMinForceCalc / (float)numSteps
    << endl;
  //add random force, but not if rediagonalizing
  if (randforce && itrs != -1 && lastLambda > 0) {
    //add random force
    if (myPreviousNormalMode->genCompNoise) gaussRandCoord1 =
        myPreviousNormalMode->tempV3DBlk;
    else genProjGauss(&gaussRandCoord1, app->topology);
    //step length
    randStp = sqrt(2 * Constant::BOLTZMANN * myTemp * lastLambda);
    app->positions.intoWeightedAdd(randStp, gaussRandCoord1);
  } else
    randStp = 0.0;
  //
  postStepModify();
}

void NormalModeMinimizer::getParameters(vector<Parameter> &parameters) const {
  STSIntegrator::getParameters(parameters);
  parameters.push_back
    (Parameter("firstmode",
               Value(firstMode, ConstraintValueType::NoConstraints()),
               -1, Text("First mode to use in set")));
  parameters.push_back
    (Parameter("numbermodes",
 Value(numMode, ConstraintValueType::NoConstraints()),
               -1, Text("Number of modes propagated")));
  parameters.push_back
    (Parameter("gamma", Value(myGamma * (1000 * Constant::INV_TIMEFACTOR),
                              ConstraintValueType::NotNegative()),
               80.0, Text("Langevin Gamma")));
  parameters.push_back
    (Parameter("seed", Value(mySeed, ConstraintValueType::NotNegative()),
               1234, Text("Langevin random seed")));
  parameters.push_back
    (Parameter("temperature", Value(myTemp, ConstraintValueType::NotNegative()),
               300.0, Text("Langevin temperature")));
  parameters.push_back
    (Parameter("minimlim", Value(minLim, ConstraintValueType::NotNegative()),
               0.1, Text("Minimizer target PE difference kcal mole^{-1}")));
  parameters.push_back
    (Parameter("randforce",
               Value(randforce, ConstraintValueType::NoConstraints()),
               true, Text("Add random force")));
  parameters.push_back
    (Parameter("rediag", Value(reDiag, ConstraintValueType::NoConstraints()),
               false, Text("Force re-digonalize")));
  parameters.push_back
    (Parameter("simplemin",
               Value(simpleMin, ConstraintValueType::NoConstraints()), true,
               Text("Simple minimizer or exact minima projection.")));
}

STSIntegrator *NormalModeMinimizer::doMake(const vector<Value> &values,
                                           ForceGroup *fg) const {
  return new NormalModeMinimizer(values[0], values[1], values[2], values[3],
                                 values[4], values[5], values[6], values[7],
                                 values[8], values[9],
                                 fg);
}

//******************************************************************************
//****Minimizers virtual force calculation**************************************
//******************************************************************************

void NormalModeMinimizer::utilityCalculateForces() {
  calculateForces();
}
