#include <protomol/output/OutputFinalXYZPos.h>
#include <protomol/config/Configuration.h>
#include <protomol/output/OutputCache.h>
#include <protomol/module/MainModule.h>
#include <protomol/base/StringUtilities.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/io/XYZWriter.h>
#include <protomol/ProtoMolApp.h>
#include <protomol/base/Exception.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;


const string OutputFinalXYZPos::keyword("finXYZPosFile");


OutputFinalXYZPos::OutputFinalXYZPos() : Output(-1), minimalImage(false) {}


OutputFinalXYZPos::OutputFinalXYZPos(const string &filename, bool minimal) :
  Output(-1), filename(filename), minimalImage(minimal) {}


void OutputFinalXYZPos::doFinalize(long step) {
  XYZWriter writer;
  if (!writer.open(filename))
    THROWS("Can't open " << getId() << " '" << filename << "'.");

  const Vector3DBlock *pos =
    (minimalImage ? app->outputCache.getMinimalPositions() : &app->positions);
  writer.setComment("Time : " + toString(app->outputCache.getTime()) +
                    ", step : " + toString(step) +
                    (minimalImage ? ", minimal Image" : "") + ".");

  if (!writer.write(*pos, app->topology->atoms, app->topology->atomTypes))
    THROWS("Could not write " << getId() << " '" << filename << "'.");
}


Output *OutputFinalXYZPos::doMake(const vector<Value> &values) const {
  return new OutputFinalXYZPos(values[0], values[1]);
}


void OutputFinalXYZPos::getParameters(vector<Parameter> &parameter) const {
  parameter.push_back
    (Parameter(getId(), Value(filename, ConstraintValueType::NotEmpty())));
  parameter.push_back
    (Parameter(keyword + "MinimalImage", Value(minimalImage),
               Text("whether the coordinates should be transformed to minimal"
                    " image or not")));
}


bool OutputFinalXYZPos::adjustWithDefaultParameters(vector<Value> &values,
                                                    const Configuration *config)
const {
  if (!checkParameterTypes(values)) return false;

  if (config->valid(InputMinimalImage::keyword) && !values[1].valid())
    values[1] = (*config)[InputMinimalImage::keyword];

  return checkParameters(values);
}
