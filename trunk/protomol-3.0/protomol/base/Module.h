#ifndef MODULE_H
#define MODULE_H

#include <protomol/debug/Exception.h>

#include <string>
#include <set>

namespace ProtoMol {
  class ProtoMolApp;
  class GenericTopology;
  class ForceFactory;

  class Module {
  public:
    typedef std::set<std::string> module_deps_t;

    Module() {}
    virtual ~Module() {}

    virtual const std::string getName() const = 0;
    virtual int getPriority() const {return 0;}
    virtual const std::string getHelp() const {return "";}
    virtual void getDependencies(module_deps_t &deps) const {}

    virtual void configure(ProtoMolApp *app) {}
    virtual void buildTopology(ProtoMolApp *app)
    {THROW("Not implemented in this module");}
    virtual void registerForces(ProtoMolApp *app) {}

  protected:
    virtual void init(ProtoMolApp *app) = 0;

    friend class ModuleManager;
  };

  struct moduleLess {
    bool operator()(const Module *m1, const Module *m2) {
      if (!m1 || !m2) THROW("null pointer");

      if (m1->getPriority() < m2->getPriority()) return true;
      if (m1->getPriority() > m2->getPriority()) return false;
      return m1->getName() < m2->getName();
    }
  };
};

#endif // MODULE_H