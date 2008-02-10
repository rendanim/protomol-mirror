#include <protomol/modules/OutputModule.h>

#include <protomol/ProtoMolApp.h>
#include <protomol/config/Configuration.h>
#include <protomol/factory/OutputFactory.h>

#include <protomol/output/OutputDCDTrajectory.h>
#include <protomol/output/OutputDCDTrajectoryVel.h>
#include <protomol/output/OutputFinalPDBPos.h>
#include <protomol/output/OutputFinalXYZPos.h>
#include <protomol/output/OutputFinalXYZVel.h>
#include <protomol/output/OutputScreen.h>
#include <protomol/output/OutputXYZTrajectoryForce.h>
#include <protomol/output/OutputXYZTrajectoryPos.h>
#include <protomol/output/OutputXYZTrajectoryVel.h>

using namespace std;
using namespace ProtoMol;

void OutputModule::init(ProtoMolApp *app) {
  OutputFactory &f = app->outputFactory;

  f.registerExemplar(new OutputDCDTrajectory());
  f.registerExemplar(new OutputDCDTrajectoryVel());
  f.registerExemplar(new OutputFinalPDBPos());
  f.registerExemplar(new OutputFinalXYZPos());
  f.registerExemplar(new OutputFinalXYZVel());
  f.registerExemplar(new OutputXYZTrajectoryForce());
  f.registerExemplar(new OutputXYZTrajectoryPos());
  f.registerExemplar(new OutputXYZTrajectoryVel());
}