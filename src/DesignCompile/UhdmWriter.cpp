/*
 Copyright 2019 Alain Dargelas

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/* 
 * File:   UhdmWriter.cpp
 * Author: alain
 * 
 * Created on January 17, 2020, 9:13 PM
 */
#include <map>
#include "headers/uhdm.h"
#include "SourceCompile/SymbolTable.h"
#include "Utils/StringUtils.h"
#include "Library/Library.h"
#include "Design/FileContent.h"
#include "ErrorReporting/Error.h"
#include "ErrorReporting/Location.h"
#include "ErrorReporting/Error.h"
#include "ErrorReporting/ErrorDefinition.h"
#include "ErrorReporting/ErrorContainer.h"
#include "SourceCompile/CompilationUnit.h"
#include "SourceCompile/PreprocessFile.h"
#include "SourceCompile/CompileSourceFile.h"
#include "CommandLine/CommandLineParser.h"
#include "SourceCompile/ParseFile.h"
#include "Testbench/ClassDefinition.h"
#include "SourceCompile/Compiler.h"
#include "DesignCompile/CompileDesign.h"
#include "DesignCompile/ResolveSymbols.h"
#include "DesignCompile/DesignElaboration.h"
#include "DesignCompile/UVMElaboration.h"
#include "DesignCompile/CompilePackage.h"
#include "DesignCompile/CompileModule.h"
#include "DesignCompile/CompileFileContent.h"
#include "DesignCompile/CompileProgram.h"
#include "DesignCompile/CompileClass.h"
#include "DesignCompile/Builtin.h"
#include "DesignCompile/PackageAndRootElaboration.h"
#include "Design/ModuleInstance.h" 
#include "Design/Netlist.h"
#include "Design/Struct.h"
#include "Design/Union.h"
#include "surelog.h"
#include "UhdmWriter.h"
#include "headers/vpi_visitor.h"
#include "headers/Serializer.h"
#include "headers/module.h"
#include "DesignCompile/UhdmChecker.h"
#include "headers/vpi_listener.h"
#include "headers/ElaboratorListener.h"

using namespace SURELOG;
using namespace UHDM;

typedef std::map<ModPort*, modport*> ModPortMap;
typedef std::map<DesignComponent*, BaseClass*> ComponentMap;
typedef std::map<Signal*, BaseClass*> SignalBaseClassMap;
typedef std::map<std::string, Signal*> SignalMap;
typedef std::map<ModuleInstance*, BaseClass*> InstanceMap;
typedef std::map<std::string, BaseClass*> VpiSignalMap;

UhdmWriter::~UhdmWriter()
{
}

unsigned int UhdmWriter::getDataType(VObjectType type) { 
  switch (type) {
  case VObjectType::slIntVec_TypeLogic:
    return vpiLogicVar;
  default:
    return 0;
  }
}

unsigned int UhdmWriter::getVpiOpType(VObjectType type) { 
  switch (type) {
  case VObjectType::slBinOp_Plus: 
    return vpiPlusOp;  
  case VObjectType::slBinOp_Minus: 
    return vpiMinusOp;   
  case VObjectType::slBinOp_Mult: 
    return vpiMultOp;      
  case VObjectType::slBinOp_Div: 
    return vpiDivOp;     
  case VObjectType::slBinOp_Great: 
    return vpiGtOp;          
  case VObjectType::slBinOp_GreatEqual: 
    return vpiGeOp;     
  case VObjectType::slBinOp_Less: 
    return vpiLtOp;     
  case VObjectType::slBinOp_LessEqual: 
    return vpiLeOp;     
  case VObjectType::slBinOp_Equiv: 
    return vpiEqOp;   
  case VObjectType::slBinOp_Not: 
    return vpiNeqOp;    
  case VObjectType::slBinOp_Percent: 
    return vpiModOp;    
  case VObjectType::slBinOp_LogicAnd: 
    return vpiLogAndOp;      
  case VObjectType::slBinOp_LogicOr: 
    return vpiLogOrOp;     
  case VObjectType::slBinOp_BitwAnd: 
    return vpiBitAndOp;
  case VObjectType::slBinOp_BitwOr: 
    return vpiBitOrOp;   
  case VObjectType::slBinOp_BitwXor: 
    return vpiBitXorOp;    
	case VObjectType::slBinOp_ReductXnor1:
	case VObjectType::slBinOp_ReductXnor2: 
		return vpiBitXNorOp;    
  case VObjectType::slBinOp_ShiftLeft: 
    return vpiLShiftOp;    
  case VObjectType::slBinOp_ShiftRight: 
    return vpiRShiftOp;  
  case VObjectType::slIncDec_PlusPlus:
		return vpiPostIncOp;
  case VObjectType::slIncDec_MinusMinus: 
    return vpiPostDecOp; 
  case VObjectType::slConditional_operator: 
    return vpiConditionOp;        
  default:
    return 0;      
  }
}

unsigned int UhdmWriter::getVpiDirection(VObjectType type)
{
  unsigned int direction = vpiNoDirection;
  if (type == VObjectType::slPortDir_Inp ||
      type == VObjectType::slTfPortDir_Inp)
    direction = vpiInput;
  else if (type == VObjectType::slPortDir_Out ||
           type == VObjectType::slTfPortDir_Out)
    direction = vpiOutput;
  else if (type == VObjectType::slPortDir_Inout ||
           type == VObjectType::slTfPortDir_Inout)
    direction = vpiInout;
  else if (type == VObjectType::slTfPortDir_Ref ||
           type == VObjectType::slTfPortDir_ConstRef)
    direction = vpiRef;  
  return direction;
}

unsigned int UhdmWriter::getVpiNetType(VObjectType type)
{
  unsigned int nettype = 0;
  if (type == VObjectType::slNetType_Wire)
    nettype = vpiWire;
  else if (type == VObjectType::slIntVec_TypeReg)
    nettype = vpiReg;
  else if (type == VObjectType::slNetType_Supply0)
    nettype = vpiSupply0;
  else if (type == VObjectType::slNetType_Supply1)
    nettype = vpiSupply1;
  else if (type == VObjectType::slIntVec_TypeLogic)
    nettype = vpiLogicNet;
  // TODO
  return nettype;
}

void writePorts(std::vector<Signal*>& orig_ports, BaseClass* parent, 
        VectorOfport* dest_ports, VectorOfnet* dest_nets,
        Serializer& s, ComponentMap& componentMap,
        ModPortMap& modPortMap, SignalBaseClassMap& signalBaseMap, 
        SignalMap& signalMap, ModuleInstance* instance = nullptr) {
  for (Signal* orig_port : orig_ports ) {
    port* dest_port = s.MakePort();
    signalBaseMap.insert(std::make_pair(orig_port, dest_port));
    signalMap.insert(std::make_pair(orig_port->getName(), orig_port));
    dest_port->VpiName(orig_port->getName());
    unsigned int direction = UhdmWriter::getVpiDirection(orig_port->getDirection());
    dest_port->VpiDirection(direction);    
    dest_port->VpiLineNo(orig_port->getFileContent()->Line(orig_port->getNodeId()));
    dest_port->VpiFile(orig_port->getFileContent()->getFileName());
    dest_port->VpiParent(parent);
    if (ModPort* orig_modport = orig_port->getModPort()) {
      ref_obj* ref = s.MakeRef_obj();
      dest_port->Low_conn(ref);
      std::map<ModPort*, modport*>::iterator itr = modPortMap.find(orig_modport);
      if (itr != modPortMap.end()) {
        ref->Actual_group((*itr).second);
      }
    } else if (ModuleDefinition* orig_interf = orig_port->getInterfaceDef()) {
      ref_obj* ref = s.MakeRef_obj();
      dest_port->Low_conn(ref);
      std::map<DesignComponent*, BaseClass*>::iterator itr = 
                                                componentMap.find(orig_interf);
      if (itr != componentMap.end()) {
        ref->Actual_group((*itr).second);
      }
    }
    dest_ports->push_back(dest_port);
  }
}

void writeDataTypes(DesignComponent::DataTypeMap& datatypeMap,
                    BaseClass* parent, VectorOftypespec* dest_typespecs,
                    Serializer& s) {
  for (std::map<std::string, DataType*>::iterator itr = datatypeMap.begin();
       itr != datatypeMap.end(); itr++) {
    DataType* dtype = (*itr).second;
    while (dtype) {
      TypeDef* typed = dynamic_cast<TypeDef*>(dtype);
      if (typed) {
        DataType* dt = typed->getDataType();
        Enum* en = dynamic_cast<Enum*> (dt);
        if (en) {
          dest_typespecs->push_back(en->getTypespec());
        }
        Struct* st = dynamic_cast<Struct*> (dt);
        if (st) {
          dest_typespecs->push_back(st->getTypespec());
        }
        Union* un = dynamic_cast<Union*> (dt);
        if (un) {
          dest_typespecs->push_back(un->getTypespec());
        }
      }
      dtype = dtype->getDefinition();
    }
  }
}

void writeNets(std::vector<Signal*>& orig_nets, BaseClass* parent, 
        VectorOfnet* dest_nets, Serializer& s, SignalBaseClassMap& signalBaseMap, 
        SignalMap& signalMap, SignalMap& portMap, ModuleInstance* instance = nullptr) {   

  for (auto& orig_net : orig_nets ) {
    net* dest_net = nullptr;
    if (instance) {
      for(net* net : *instance->getNetlist()->nets()) {
        SignalMap::iterator itr = signalMap.find(net->VpiName());
        if (itr == signalMap.end()) {
          if (net->VpiName() == orig_net->getName()) {
            dest_net = net;
            break;
          }
        }
      }
    } else {
      dest_net = s.MakeLogic_net();
    }
    if (dest_net) {
      SignalMap::iterator portItr = portMap.find(orig_net->getName());
      if (portItr != portMap.end()) {
        Signal* sig = (*portItr).second;
        if (sig) {
          SignalBaseClassMap::iterator itr = signalBaseMap.find(sig);
          if (itr != signalBaseMap.end()) {
            port* p = (port*) ((*itr).second);
            if (p->Low_conn() == nullptr) {
              ref_obj* ref = s.MakeRef_obj();          
              ref->Actual_group(dest_net);
              p->Low_conn(ref);
            }
          }
        }
      }
      signalBaseMap.insert(std::make_pair(orig_net, dest_net));
      signalMap.insert(std::make_pair(orig_net->getName(), orig_net));
      dest_net->VpiName(orig_net->getName());
      dest_net->VpiLineNo(orig_net->getFileContent()->Line(orig_net->getNodeId()));
      dest_net->VpiFile(orig_net->getFileContent()->getFileName());
      dest_net->VpiNetType(UhdmWriter::getVpiNetType(orig_net->getType()));
      dest_net->VpiParent(parent);
      dest_nets->push_back(dest_net);
    }
  }
}

void mapLowConns(std::vector<Signal*>& orig_ports, Serializer& s,
        SignalBaseClassMap& signalBaseMap) {
   for (Signal* orig_port : orig_ports ) {
     if (Signal* lowconn = orig_port->getLowConn()) {
       std::map<Signal*, BaseClass*>::iterator itrlow = signalBaseMap.find(lowconn);
       if (itrlow != signalBaseMap.end()) {
         std::map<Signal*, BaseClass*>::iterator itrport = signalBaseMap.find(orig_port);
         if (itrport != signalBaseMap.end()) {
           ref_obj* ref = s.MakeRef_obj();
           ((port*)(*itrport).second)->Low_conn(ref);
           ref->Actual_group((*itrlow).second);
         }
       }
     }
   }
}

void writeClasses(ClassNameClassDefinitionMultiMap& orig_classes, 
        VectorOfclass_defn* dest_classes, Serializer& s, 
        ComponentMap& componentMap) {
  for (auto& orig_class : orig_classes ) {
    ClassDefinition* orig_def = orig_class.second;
    std::map<DesignComponent*, BaseClass*>::iterator itr = 
                componentMap.find(orig_def);
    if (itr != componentMap.end()) {
      dest_classes->push_back((class_defn*) (*itr).second);
    }
  }
}

void writeVariables(DesignComponent::VariableMap& orig_vars, BaseClass* parent,
        VectorOfvariables* dest_vars, Serializer& s, ComponentMap& componentMap) {
  for (auto& orig_var : orig_vars) {
    Variable* var = orig_var.second;
    DataType* dtype = var->getDataType();
    ClassDefinition* classdef = dynamic_cast<ClassDefinition*> (dtype);
    if (classdef) {
      class_var* cvar = s.MakeClass_var();
      cvar->VpiName(var->getName());
      cvar->VpiLineNo(var->getFileContent()->Line(var->getNodeId()));
      cvar->VpiFile(var->getFileContent()->getFileName());
      cvar->VpiParent(parent);
      std::map<DesignComponent*, BaseClass*>::iterator itr =
              componentMap.find(classdef);
      if (itr != componentMap.end()) {
        //TODO: Bind Class type,
        // class_var -> class_typespec -> class_defn
      }
      dest_vars->push_back(cvar);
    }
  }
}

void bindExpr(expr* ex, ComponentMap& componentMap,
        ModPortMap& modPortMap, SignalBaseClassMap& signalBaseMap, 
        SignalMap& signalMap)
{
  if (!ex)
    return;
  switch (ex->UhdmType()) {
  case UHDM_OBJECT_TYPE::uhdmref_obj:
  {
    ref_obj* ref = (ref_obj*) ex;
    const std::string& name = ref->VpiName();
    SignalMap::iterator sigitr = signalMap.find(name);
    if (sigitr != signalMap.end()) {
      Signal* sig = (*sigitr).second;
      SignalBaseClassMap::iterator sigbaseitr = signalBaseMap.find(sig);
      if (sigbaseitr != signalBaseMap.end()) {
        BaseClass* baseclass = (*sigbaseitr).second;
        ref->Actual_group(baseclass);
        
      }
    }
    break;
  }
  default:
    break;
  }
}

void writeContAssigns(std::vector<cont_assign*>* orig_cont_assigns,
        module* parent, Serializer& s, ComponentMap& componentMap,
        ModPortMap& modPortMap, SignalBaseClassMap& signalBaseMap, 
        SignalMap& signalMap) {
  if (orig_cont_assigns == nullptr)
    return;
  for (cont_assign* cassign : *orig_cont_assigns) {
    expr* lexpr = (expr*) cassign->Lhs();
    bindExpr(lexpr, componentMap, modPortMap, signalBaseMap, signalMap);
    expr* rexpr = (expr*) cassign->Rhs();
    bindExpr(rexpr, componentMap, modPortMap, signalBaseMap, signalMap);
  }
}


void writePackage(Package* pack, package* p, Serializer& s, 
        ComponentMap& componentMap) {
  // Typepecs
  VectorOftypespec* typespecs = s.MakeTypespecVec();
  p->Typespecs(typespecs);
  writeDataTypes(pack->getDataTypeMap(), p, typespecs, s);
  // Classes
  ClassNameClassDefinitionMultiMap& orig_classes = pack->getClassDefinitions();
  VectorOfclass_defn* dest_classes = s.MakeClass_defnVec();
  writeClasses(orig_classes, dest_classes, s, componentMap);
  p->Class_defns(dest_classes);
  // Parameters
  if (pack->getParameters())
    p->Parameters(pack->getParameters());
  // Param_assigns
  p->Param_assigns(pack->getParam_assigns());
  // Function and tasks 
  p->Task_funcs(pack->getTask_funcs());
}

void writeModule(ModuleDefinition* mod, module* m, Serializer& s, 
        ComponentMap& componentMap,
        ModPortMap& modPortMap, 
        ModuleInstance* instance = nullptr) {
  SignalBaseClassMap signalBaseMap;
  SignalMap portMap;
  SignalMap netMap;
  // Typepecs
  VectorOftypespec* typespecs = s.MakeTypespecVec();
  m->Typespecs(typespecs);
  writeDataTypes(mod->getDataTypeMap(), m, typespecs, s);
  // Ports
  std::vector<Signal*>& orig_ports = mod->getPorts();
  VectorOfport* dest_ports = s.MakePortVec();
  VectorOfnet* dest_nets = s.MakeNetVec();
  writePorts(orig_ports, m, dest_ports, dest_nets, s, componentMap,
        modPortMap, signalBaseMap, portMap, instance);
  m->Ports(dest_ports);
  // Nets
  std::vector<Signal*> orig_nets = mod->getSignals();
  writeNets(orig_nets, m, dest_nets, s, signalBaseMap, netMap, portMap, instance);
  m->Nets(dest_nets);
  mapLowConns(orig_ports, s, signalBaseMap);
  // Classes
  ClassNameClassDefinitionMultiMap& orig_classes = mod->getClassDefinitions();
  VectorOfclass_defn* dest_classes = s.MakeClass_defnVec();
  writeClasses(orig_classes, dest_classes, s, componentMap);
  m->Class_defns(dest_classes);
  // Variables
  DesignComponent::VariableMap& orig_vars = mod->getVariables();
  VectorOfvariables* dest_vars = s.MakeVariablesVec();
  writeVariables(orig_vars, m, dest_vars, s, componentMap);
  m->Variables(dest_vars);
  // Cont assigns
  std::vector<cont_assign*>* orig_cont_assigns = mod->getContAssigns();
  writeContAssigns(orig_cont_assigns, m, s, componentMap, modPortMap, 
          signalBaseMap, netMap);
  m->Cont_assigns(orig_cont_assigns);
  // Processes
  m->Process(mod->getProcesses());
  // Parameters
  if (mod->getParameters())
    m->Parameters(mod->getParameters());
  // Param_assigns
  m->Param_assigns(mod->getParam_assigns());
  // Function and tasks 
  m->Task_funcs(mod->getTask_funcs());
}

void writeInterface(ModuleDefinition* mod, interface* m, Serializer& s,
        ComponentMap& componentMap,
        ModPortMap& modPortMap, ModuleInstance* instance = nullptr) {
  SignalBaseClassMap signalBaseMap;
  SignalMap portMap;
  SignalMap netMap;
  // Typepecs
  VectorOftypespec* typespecs = s.MakeTypespecVec();
  m->Typespecs(typespecs);
  writeDataTypes(mod->getDataTypeMap(), m, typespecs, s);
  // Ports
  std::vector<Signal*>& orig_ports = mod->getPorts();
  VectorOfport* dest_ports = s.MakePortVec();
  VectorOfnet* dest_nets = s.MakeNetVec();
  writePorts(orig_ports, m, dest_ports, dest_nets, s, componentMap,
        modPortMap, signalBaseMap, portMap, instance);
  m->Ports(dest_ports);
  std::vector<Signal*> orig_nets = mod->getSignals();
  writeNets(orig_nets, m, dest_nets, s, signalBaseMap, netMap, portMap, instance);
  m->Nets(dest_nets);
  // Modports
  ModuleDefinition::ModPortSignalMap& orig_modports = mod->getModPortSignalMap();
  VectorOfmodport* dest_modports = s.MakeModportVec();
  for (auto& orig_modport : orig_modports ) {
    modport* dest_modport = s.MakeModport();
    dest_modport->Interface(m);
    modPortMap.insert(std::make_pair(&orig_modport.second, dest_modport));
    dest_modport->VpiName(orig_modport.first);
    VectorOfio_decl* ios = s.MakeIo_declVec();
    for (auto& sig : orig_modport.second.getPorts()) {
      io_decl* io = s.MakeIo_decl();
      io->VpiName(sig.getName());
      unsigned int direction = UhdmWriter::getVpiDirection(sig.getDirection());
      io->VpiDirection(direction);
      ios->push_back(io);
    }
    dest_modport->Io_decls(ios);
    dest_modports->push_back(dest_modport);
  }
  m->Modports(dest_modports);
  // Function and tasks 
  m->Task_funcs(mod->getTask_funcs());
}

void writeProgram(Program* mod, program* m, Serializer& s,
        ComponentMap& componentMap,
        ModPortMap& modPortMap,
        ModuleInstance* instance = nullptr) {
  SignalBaseClassMap signalBaseMap;
  SignalMap portMap;
  SignalMap netMap;
  // Typepecs
  VectorOftypespec* typespecs = s.MakeTypespecVec();
  m->Typespecs(typespecs);
  writeDataTypes(mod->getDataTypeMap(), m, typespecs, s);
  // Ports
  std::vector<Signal*>& orig_ports = mod->getPorts();
  VectorOfport* dest_ports = s.MakePortVec();
  VectorOfnet* dest_nets = s.MakeNetVec();
  writePorts(orig_ports, m, dest_ports, dest_nets, s, componentMap,
        modPortMap, signalBaseMap, portMap, instance);
  m->Ports(dest_ports);
   // Nets
  std::vector<Signal*>& orig_nets = mod->getSignals();
  writeNets(orig_nets, m, dest_nets, s, signalBaseMap, netMap, portMap, instance);
  m->Nets(dest_nets);
  mapLowConns(orig_ports, s, signalBaseMap);
  // Classes
  ClassNameClassDefinitionMultiMap& orig_classes = mod->getClassDefinitions();
  VectorOfclass_defn* dest_classes = s.MakeClass_defnVec();
  writeClasses(orig_classes, dest_classes, s, componentMap);
  m->Class_defns(dest_classes);
  // Variables
  DesignComponent::VariableMap& orig_vars = mod->getVariables();
  VectorOfvariables* dest_vars = s.MakeVariablesVec();
  writeVariables(orig_vars, m, dest_vars, s, componentMap);
  m->Variables(dest_vars);
  // Processes
  m->Process(mod->getProcesses());
}


bool writeElabProgram(ModuleInstance* instance, program* m) {
  Netlist* netlist = instance->getNetlist();
  m->Ports(netlist->ports());
  if (netlist->ports()) {
    for (auto obj : *netlist->ports()) {
      obj->VpiParent(m);
    }
  }
  m->Nets(netlist->nets());
  if (netlist->nets()) {
    for (auto obj : *netlist->nets()) {
      obj->VpiParent(m);
    }
  }
  m->Gen_scope_arrays(netlist->gen_scopes());
  m->Array_vars(netlist->array_vars());
  return true;
}


bool writeElabGenScope(ModuleInstance* instance, gen_scope* m) {
  Netlist* netlist = instance->getNetlist();
  m->Nets(netlist->nets());
  if (netlist->nets()) {
    for (auto obj : *netlist->nets()) {
      obj->VpiParent(m);
    }
  }
  
  std::vector<gen_scope_array*>* gen_scope_arrays = netlist->gen_scopes();
  if (gen_scope_arrays) {
    for (gen_scope_array* scope_arr : *gen_scope_arrays) {
      for (gen_scope* scope : *scope_arr->Gen_scopes()) {
        m->Cont_assigns(scope->Cont_assigns());
        m->Process(scope->Process());
        if (scope->Parameters())
          m->Parameters(scope->Parameters());     
        m->Param_assigns(scope->Param_assigns());     
      }
    }
  }

  m->Nets(netlist->nets());
  m->Array_vars(netlist->array_vars());
  return true;
}


bool writeElabModule(ModuleInstance* instance, module* m) {
  Netlist* netlist = instance->getNetlist();
  m->Ports(netlist->ports());
  if (netlist->ports()) {
    for (auto obj : *netlist->ports()) {
      obj->VpiParent(m);
    }
  }
  m->Nets(netlist->nets());
  if (netlist->nets()) {
    for (auto obj : *netlist->nets()) {
      obj->VpiParent(m);
    }
  }
  m->Gen_scope_arrays(netlist->gen_scopes());
  if (netlist->gen_scopes()) {
    for (auto obj : *netlist->gen_scopes()) {
      obj->VpiParent(m);
    }
  }
  m->Array_vars(netlist->array_vars());
  return true;
}


bool writeElabInterface(ModuleInstance* instance, interface* m, Serializer& s) {
  Netlist* netlist = instance->getNetlist();
  m->Ports(netlist->ports());
  if (netlist->ports()) {
    for (auto obj : *netlist->ports()) {
      obj->VpiParent(m);
    }
  }
  m->Nets(netlist->nets());
  if (netlist->nets()) {
    for (auto obj : *netlist->nets()) {
      obj->VpiParent(m);
    }
  }
  m->Array_vars(netlist->array_vars());
  ModuleDefinition* mod = (ModuleDefinition*)instance->getDefinition();
  // Modports
  ModuleDefinition::ModPortSignalMap& orig_modports = mod->getModPortSignalMap();
  VectorOfmodport* dest_modports = s.MakeModportVec();
  for (auto& orig_modport : orig_modports ) {
    modport* dest_modport = s.MakeModport();
    dest_modport->Interface(m);
    dest_modport->VpiName(orig_modport.first);
    dest_modport->VpiParent(m);
    VectorOfio_decl* ios = s.MakeIo_declVec();
    for (auto& sig : orig_modport.second.getPorts()) {
      io_decl* io = s.MakeIo_decl();
      io->VpiName(sig.getName());
      unsigned int direction = UhdmWriter::getVpiDirection(sig.getDirection());
      io->VpiDirection(direction);
      io->VpiParent(dest_modport);
      ios->push_back(io);
    }
    dest_modport->Io_decls(ios);
    dest_modports->push_back(dest_modport);
  }
  m->Modports(dest_modports);
  m->Gen_scope_arrays(netlist->gen_scopes());
  return true;
}

void writeInstance(ModuleDefinition* mod, ModuleInstance* instance, any* m, 
        Serializer& s, 
        ComponentMap& componentMap,
        ModPortMap& modPortMap,
        InstanceMap& instanceMap) {
  VectorOfmodule* subModules = nullptr; 
  VectorOfprogram* subPrograms = nullptr;
  VectorOfinterface* subInterfaces = nullptr;
  VectorOfgen_scope_array* subGenScopeArrays = nullptr;
  if (m->UhdmType() == uhdmmodule) {
    writeElabModule(instance, (module*) m);
  } else if (m->UhdmType() == uhdmgen_scope) {
    writeElabGenScope(instance, (gen_scope*) m);
  }

  // Parameters
  for (auto& param : instance->getMappedValues()) {
    const std::string& name = param.first;
    Value* val = param.second.first;
    VectorOfany* params = nullptr;
    if (m->UhdmType() == uhdmmodule)
      params = ((module*)m)->Parameters();
    else if (m->UhdmType() == uhdmgen_scope)
      params = ((gen_scope*)m)->Parameters();
    else if (m->UhdmType() == uhdminterface)
      params = ((interface*)m)->Parameters();  
    if (params == nullptr) {
      params = s.MakeAnyVec();
    }
    parameter* p = s.MakeParameter();
    p->VpiName(name);
    p->VpiValue(val->uhdmValue());
    p->VpiFile(instance->getFileName());
    p->VpiLineNo(param.second.second);
    params->push_back(p);
    if (m->UhdmType() == uhdmmodule)
      ((module*)m)->Parameters(params);
    else if (m->UhdmType() == uhdmgen_scope)
      ((gen_scope*)m)->Parameters(params);
    else if (m->UhdmType() == uhdminterface)
      ((interface*)m)->Parameters(params);  
  }

  for (unsigned int i = 0; i < instance->getNbChildren(); i++) {
    ModuleInstance* child = instance->getChildren(i);
    DesignComponent* childDef = child->getDefinition();
    if (ModuleDefinition* mm = dynamic_cast<ModuleDefinition*> (childDef)) {
      VObjectType insttype = child->getType();
      if (insttype == VObjectType::slModule_instantiation) {
        if (subModules == nullptr)
          subModules = s.MakeModuleVec();
        module* sm = s.MakeModule();
        sm->VpiName(child->getInstanceName());
        sm->VpiDefName(child->getModuleName());
        sm->VpiFullName(child->getFullPathName());
        sm->VpiFile(child->getFileName());
        sm->VpiLineNo(child->getLineNb());
        subModules->push_back(sm);
        if (m->UhdmType() == uhdmmodule) {
          ((module*) m)->Modules(subModules);
          sm->Instance((module*) m);
          sm->Module((module*) m);
          sm->VpiParent(m);
        } else if (m->UhdmType() == uhdmgen_scope) {
          ((gen_scope*) m)->Modules(subModules);
          sm->VpiParent(m);
        }
        writeInstance(mm, child, sm, s, componentMap, modPortMap,instanceMap);
      } else if (insttype == VObjectType::slConditional_generate_construct ||
                 insttype == VObjectType::slLoop_generate_construct ||
                 insttype == VObjectType::slGenerate_block ||
                 insttype == VObjectType::slGenerate_item) {

        if (subGenScopeArrays == nullptr)
          subGenScopeArrays = s.MakeGen_scope_arrayVec();
        gen_scope_array* sm = s.MakeGen_scope_array();
        sm->VpiName(child->getInstanceName());
        sm->VpiFullName(child->getFullPathName());
        sm->VpiFile(child->getFileName());
        sm->VpiLineNo(child->getLineNb());
        subGenScopeArrays->push_back(sm);
        gen_scope* a_gen_scope = s.MakeGen_scope();
        sm->Gen_scopes(s.MakeGen_scopeVec());
        sm->Gen_scopes()->push_back(a_gen_scope);
        a_gen_scope->VpiParent(sm);
        UHDM_OBJECT_TYPE utype = m->UhdmType();
        if (utype == uhdmmodule) {
          ((module*) m)->Gen_scope_arrays(subGenScopeArrays);
          sm->VpiParent(m);
        } else if (utype == uhdmgen_scope) {
          ((gen_scope*)m)->Gen_scope_arrays(subGenScopeArrays);
          sm->VpiParent(m); 
        }
        writeInstance(mm, child, a_gen_scope, s, componentMap, modPortMap,instanceMap);

      } else if (insttype == VObjectType::slInterface_instantiation) {
        if (subInterfaces == nullptr)
          subInterfaces = s.MakeInterfaceVec();
        interface* sm = s.MakeInterface();
        sm->VpiName(child->getInstanceName());
        sm->VpiDefName(child->getModuleName());
        sm->VpiFullName(child->getFullPathName());
        sm->VpiFile(child->getFileName());
        sm->VpiLineNo(child->getLineNb());
        subInterfaces->push_back(sm);
        UHDM_OBJECT_TYPE utype = m->UhdmType();
        if (utype == uhdmmodule) {
          ((module*) m)->Interfaces(subInterfaces);
          sm->Instance((module*) m);
          sm->VpiParent(m);
        } else if (utype == uhdmgen_scope) {
          ((gen_scope*) m)->Interfaces(subInterfaces);
          sm->VpiParent(m);
        }
        writeElabInterface(child, sm, s);
      } else if (insttype == VObjectType::slUdp_instantiation) {
        // TODO
      } else if (insttype == VObjectType::slGate_instantiation) {
        // TODO
      } else {
        // Unknown object type
      }
    } else if (dynamic_cast<Program*> (childDef)) {
      if (subPrograms == nullptr)
        subPrograms = s.MakeProgramVec();
      program* sm = s.MakeProgram();
      sm->VpiName(child->getInstanceName());
      sm->VpiDefName(child->getModuleName());
      sm->VpiFullName(child->getFullPathName());
      sm->VpiFile(child->getFileName());
      sm->VpiLineNo(child->getLineNb());
      subPrograms->push_back(sm);
      UHDM_OBJECT_TYPE utype = m->UhdmType();
      if (utype == uhdmmodule) {
        ((module*) m)->Programs(subPrograms);
        sm->Instance((module*) m);
        sm->VpiParent(m);
      } else if (utype == uhdmgen_scope) {
        ((gen_scope*) m)->Programs(subPrograms);
        sm->VpiParent(m);
      }
      writeElabProgram(child, sm);     
    } else {
      // Undefined module
      if (subModules == nullptr)
        subModules = s.MakeModuleVec();
      module* sm = s.MakeModule();
      sm->VpiName(child->getInstanceName());
      sm->VpiDefName(child->getModuleName());
      sm->VpiFullName(child->getFullPathName());
      sm->VpiFile(child->getFileName());
      sm->VpiLineNo(child->getLineNb());
      subModules->push_back(sm);
      UHDM_OBJECT_TYPE utype = m->UhdmType();
      if (utype == uhdmmodule) { 
        ((module*) m)->Modules(subModules);
        sm->Instance((module*) m);
        sm->Module((module*) m);
        sm->VpiParent(m);
      } else if (utype == uhdmgen_scope) {
        ((gen_scope*) m)->Modules(subModules);
        sm->VpiParent(m);
      }
      writeInstance(mm, child, sm, s, componentMap, modPortMap,instanceMap);
    }
  }
}

vpiHandle UhdmWriter::write(std::string uhdmFile) {
  ComponentMap componentMap;
  ModPortMap modPortMap;
  InstanceMap instanceMap;
  Serializer& s = m_compileDesign->getSerializer();
  vpiHandle designHandle = 0;
  std::vector<vpiHandle> designs;
  if (m_design) {
    design* d = s.MakeDesign();
    designHandle = reinterpret_cast<vpiHandle>(new uhdm_handle(uhdmdesign, d));
    std::string designName = "unnamed";
    auto topLevelModules = m_design->getTopLevelModuleInstances();
    for (auto inst : topLevelModules) {
      designName = inst->getModuleName();
      break;
    }
    d->VpiName(designName);
    designs.push_back(designHandle);
    // ------------------------------- 
    // Non-Elaborated Model
    
    // Packages
    auto packages = m_design->getPackageDefinitions();
    VectorOfpackage* v2 = s.MakePackageVec();
    for (auto packNamePair : packages) {
      Package* pack = packNamePair.second;
      if (pack->getFileContents().size() && 
          pack->getType() == VObjectType::slPackage_declaration) {
        FileContent* fC = pack->getFileContents()[0];
        package* p = s.MakePackage();
        componentMap.insert(std::make_pair(pack, p));
        p->VpiParent(d);
        p->VpiDefName(pack->getName());
        writePackage(pack, p, s, componentMap);
        if (fC) {
          // Builtin package has no file 
          p->VpiFile(fC->getFileName());
          p->VpiLineNo(fC->Line(pack->getNodeIds()[0]));
        }
        v2->push_back(p);      
      }
    }
    d->AllPackages(v2);
    
    // Programs
    auto programs = m_design->getProgramDefinitions();
    VectorOfprogram* uhdm_programs = s.MakeProgramVec();
    for (auto progNamePair : programs) {
      Program* prog = progNamePair.second;
      if (prog->getFileContents().size() && 
          prog->getType() == VObjectType::slProgram_declaration) {
        FileContent* fC = prog->getFileContents()[0];
        program* p = s.MakeProgram();
        componentMap.insert(std::make_pair(prog, p)); 
        p->VpiParent(d);
        p->VpiDefName(prog->getName());    
        p->VpiFile(fC->getFileName());
        p->VpiLineNo(fC->Line(prog->getNodeIds()[0]));
        writeProgram(prog, p, s, componentMap,modPortMap);
        uhdm_programs->push_back(p);      
      }
    }
    d->AllPrograms(uhdm_programs);
    
    // Classes
    auto classes = m_design->getClassDefinitions();
    VectorOfclass_defn* v4 = s.MakeClass_defnVec();
    for (auto classNamePair : classes) {
      ClassDefinition* classDef = classNamePair.second;
      if (classDef->getFileContents().size() && 
          classDef->getType() == VObjectType::slClass_declaration) {
        FileContent* fC = classDef->getFileContents()[0];
        class_defn* c = s.MakeClass_defn();
        componentMap.insert(std::make_pair(classDef, c));  
        c->VpiParent(d);
        c->VpiName(classDef->getName());
        if (fC) {
          // Builtin classes have no file
          c->VpiFile(fC->getFileName());
          c->VpiLineNo(fC->Line(classDef->getNodeIds()[0]));
        }
        v4->push_back(c);      
      }
    }
    d->AllClasses(v4);

    // Interfaces
    auto modules = m_design->getModuleDefinitions();
    VectorOfinterface* uhdm_interfaces = s.MakeInterfaceVec();
    for (auto modNamePair : modules) {
      ModuleDefinition* mod = modNamePair.second;
      if (mod->getFileContents().size() == 0) {
        // Built-in primitive
      } else if (mod->getType() == VObjectType::slInterface_declaration) {
        FileContent* fC = mod->getFileContents()[0];
        interface* m = s.MakeInterface();
        componentMap.insert(std::make_pair(mod, m));
        m->VpiParent(d);
        m->VpiDefName(mod->getName());    
        m->VpiFile(fC->getFileName());
        m->VpiLineNo(fC->Line(mod->getNodeIds()[0]));
        uhdm_interfaces->push_back(m); 
        writeInterface(mod, m, s, componentMap, modPortMap);
      }
    }
    d->AllInterfaces(uhdm_interfaces);
    
    // Modules
    VectorOfmodule* uhdm_modules = s.MakeModuleVec();
    for (auto modNamePair : modules) {
      ModuleDefinition* mod = modNamePair.second;
      if (mod->getFileContents().size() == 0) {
        // Built-in primitive
      } else if (mod->getType() == VObjectType::slModule_declaration) {
        FileContent* fC = mod->getFileContents()[0];
        module* m = s.MakeModule();
        componentMap.insert(std::make_pair(mod, m));
        m->VpiParent(d);
        m->VpiDefName(mod->getName());    
        m->VpiFile(fC->getFileName());
        m->VpiLineNo(fC->Line(mod->getNodeIds()[0]));
        uhdm_modules->push_back(m); 
        writeModule(mod, m, s, componentMap, modPortMap);
      } 
    }
    d->AllModules(uhdm_modules);
    
    // Repair parent relationship
    for (auto classNamePair : classes) {
      ClassDefinition* classDef = classNamePair.second;
      DesignComponent* parent = classDef->getContainer();
      std::map<DesignComponent*, BaseClass*>::iterator itr =
              componentMap.find(parent);
      if (itr != componentMap.end()) {
        std::map<DesignComponent*, BaseClass*>::iterator itr2 =
                componentMap.find(classDef);
        (*itr2).second->VpiParent((*itr).second);
      }
    }
    
    // ------------------------------- 
    // Elaborated Model (Folded)
   
    // Top-level modules
    VectorOfmodule* uhdm_top_modules = s.MakeModuleVec();
    for (ModuleInstance* inst : topLevelModules) {
      DesignComponent* component = inst->getDefinition();
      ModuleDefinition* mod = dynamic_cast<ModuleDefinition*> (component);
      std::map<DesignComponent*, BaseClass*>::iterator itr =
                componentMap.find(mod);
      module* m = s.MakeModule();
      module* def = (module*) (*itr).second;
      m->VpiDefName(def->VpiDefName());
      m->VpiName(def->VpiDefName()); // Top's instance name is module name
      m->VpiFullName(def->VpiDefName()); // Top's full instance name is module name
      m->VpiFile(def->VpiFile());
      m->VpiLineNo(def->VpiLineNo());
      writeInstance(mod, inst, m, s, componentMap, modPortMap, instanceMap);
      uhdm_top_modules->push_back(m); 
    }
    d->TopModules(uhdm_top_modules);
  }
  
  // ----------------------------------
  // Fully elaborated model
  if (m_compileDesign->getCompiler()->getCommandLineParser()->getElabUhdm()) {
      ElaboratorListener* listener = new ElaboratorListener(&s, false);
      listen_designs(designs,listener);
  }

  s.Save(uhdmFile);

  if (m_compileDesign->getCompiler()->getCommandLineParser()->getDebugUhdm()) {
    // Check before restore
    UhdmChecker* uhdmchecker = new UhdmChecker(m_compileDesign, m_design);
    uhdmchecker->check(std::string(uhdmFile) + ".chk");
    delete uhdmchecker;

    std::cout << "====== UHDM =======\n";
    const std::vector<vpiHandle>& restoredDesigns = s.Restore(uhdmFile);
    if (restoredDesigns.size()) {
      designHandle = restoredDesigns[0];
    }
    std::string restored = visit_designs(restoredDesigns);
    std::cout << restored;
    std::cout << "===================\n";

  }
  
  return designHandle;
}
 
