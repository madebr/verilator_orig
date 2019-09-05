// -*- mode: C++; c-file-style: "cc-mode" -*-
//*************************************************************************
// DESCRIPTION: Verilator: Emit pybind11 binding code
//
// Code available from: http://www.veripool.org/verilator
//
//*************************************************************************
//
// Copyright 2003-2019 by Wilson Snyder.  This program is free software; you can
// redistribute it and/or modify it under the terms of either the GNU
// Lesser General Public License Version 3 or the Perl Artistic License
// Version 2.0.
//
// Verilator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//*************************************************************************

#include "config_build.h"
#include "verilatedos.h"

#include "V3Global.h"
#include "V3String.h"
#include "V3EmitPy.h"
#include "V3File.h"
#include "V3EmitCBase.h"

#include <memory>

struct ArgDpiInfo {
    enum ArgDpiType {
        UNKNOWN = 0,
        OPENARRHNDL,
        BITCHAR,
        LOGCHAR,
        VLTYPE,
    };
    ArgDpiType type;
    AstBasicDTypeKwd vlType;
    bool unsgnd;
    ArgDpiInfo()
        : type(ArgDpiType::UNKNOWN)
        , vlType(AstBasicDTypeKwd::UNKNOWN)
        , unsgnd(false) {
    }
};


// Adapted from AstVar::dpiArgType
ArgDpiInfo getArgDpiInfo(const AstVar *varp) {
    ArgDpiInfo info;
    info.type = ArgDpiInfo::VLTYPE;
    info.vlType = varp->basicp()->keyword();
    if (varp->isDpiOpenArray()) {
        info.type = ArgDpiInfo::OPENARRHNDL;
    } else if (!varp->basicp()) {
        info.type = ArgDpiInfo::UNKNOWN;
    } else if (varp->basicp()->keyword().isDpiBitVal()) {
        if (varp->widthMin() == 1) {
            info.type = ArgDpiInfo::BITCHAR;
            info.unsgnd = true;
        }
    } else if (varp->basicp()->keyword().isDpiLogicVal()) {
        if (varp->widthMin() == 1) {
            info.type = ArgDpiInfo::LOGCHAR;
            info.unsgnd = true;
        }
    } else {
        info.unsgnd = info.vlType.isDpiUnsignable() && !varp->basicp()->isSigned();
    }
    return info;
}


class PythonVisitor : public EmitCBaseVisitor {
public:
    PythonVisitor()
    : EmitCBaseVisitor()
    , m_topmodp(NULL) {
    }
private:
    typedef std::pair<AstNodeModule*,AstVar*> ModVarPair;
    std::vector<const AstCFunc *> m_dpiFunctions;           // DPI functions
    AstNodeModule *m_topmodp;                               // Top module
    std::vector<ModVarPair> m_modVars;                      // Each public {mod,var}

    virtual void visit(AstCFunc* nodep) {
        if (nodep->dpiExportWrapper() || nodep->dpiImport()) {
            m_dpiFunctions.push_back(nodep);
        }
        iterateChildren(nodep);
    }
    virtual void visit(AstNodeModule* nodep) {
        if (nodep->isTop()) {
            m_topmodp = nodep;
        }
        iterateChildren(nodep);
    }
    virtual void visit(AstNode* nodep) {
        iterateChildren(nodep);
    }

    V3OutCFile* newOutCFile(bool source) {
        string filename = v3Global.opt.prefix() + "_py" + (source ? ".cpp" : ".h");
        string filepath = v3Global.opt.makeDir()+"/"+ filename;
        if (source) {
            AstCFile *astCFile = new AstCFile(NULL, filepath);
            astCFile->source(true);
            astCFile->support(true);
            astCFile->slow(true);
            v3Global.rootp()->addFilesp(astCFile);
        }
        V3OutCFile* ofp = new V3OutCFile(filepath);
        ofp->putsHeader();
        if (source) {
            ofp->puts("// DESCR" "IPTION: Verilator output: Primary design python wrappers internals\n");
        } else {
            ofp->puts("// DESCR" "IPTION: Verilator output: Primary design python wrappers header\n");
        }
        ofp->puts("\n");
        return ofp;
    }

    bool hasDpi() const {
        return (m_dpiFunctions.size() != 0);
    }

    void emitHSource() {
        if (!m_topmodp) {
            v3fatal("% Top module not found");
        }

        puts("#ifndef _"+v3Global.opt.prefix()+"_PY_H_\n");
        puts("#define _"+v3Global.opt.prefix()+"_PY_H_\n");
        puts("\n");

        puts("#include \""+v3Global.opt.prefix()+".h\"\n");
        puts("#include \"verilated_py.h\"\n");
        puts("\n");
        puts("namespace vl_py {\n");

        emitHSourceVerilatedModuleWrapper();

        puts("\n");
        puts("void declare_"+v3Global.opt.prefix()+"(pybind11::module& m);\n");
        puts("}  // namespace vl_py\n");
        puts("\n");

        // finish up h-file
        puts("#endif  // _"+v3Global.opt.prefix()+"_PY_H_\n");
    }

    void emitHSourceVerilatedModuleWrapper () {
        puts("class Py"+v3Global.opt.prefix()+" : public "+v3Global.opt.prefix()+" {\n");
        puts("public:\n");
        if (hasDpi()) {
            puts("static pybind11::object s_dpiMetaClass;\n");
            puts("pybind11::object m_dpiObject;\n");
        }
        puts("Py"+v3Global.opt.prefix()+"(const std::string& name);\n");
        if (hasDpi()) {
            puts("static void setDpiMetaClass(pybind11::object metaClass);\n");
        }
        puts("};\n");
    }

    void emitPyDeclareModule() {
        puts("pybind11::class_<Py"+v3Global.opt.prefix()+", VerilatedModule> pyTopClass(m, \""+v3Global.opt.prefix()+"\", pybind11::module_local{});\n");
        puts("pyTopClass\n");

        m_ofp->indentInc();

        puts(".def(pybind11::init([](const std::string& name) {\n");
        if (hasDpi()) {
            puts("std::string scopeName = name + \".top\";\n");
            puts("svScope prevScope = svGetScopeFromName(scopeName.c_str());\n");
            puts("if (prevScope) {\n");
            puts("throw std::runtime_error(\"The scope \"+scopeName+\" already exists. (choose another name)\");\n");
            puts("}\n");
        }
        puts("return new Py"+v3Global.opt.prefix()+"(name);\n");
        puts("}), pybind11::arg(\"name\") = \""+v3Global.opt.prefix()+"\")\n");

        for (AstNode *nodep = m_topmodp->stmtsp(); nodep; nodep = nodep->nextp()) {
            if (const AstVar* varp = VN_CAST(nodep, Var)) {
                if (varp == NULL) {
                    continue;
                }
                if (varp->isSc()) {
                    v3fatal("Verilator Python does not support SystemC\n");
                }
                if (varp->isIO()) {
                    string funcName;
                    if (varp->isReadOnly()) {
                        funcName = "VL_PY_INPORT";
                    } else if (varp->isInoutish()) {
                        funcName = "VL_PY_INOUTPORT";
                    } else if (varp->isWritable()) {
                        funcName = "VL_PY_OUTPORT";
                    } else {
                        funcName = "VL_PY_INOUTPORT";  // Ignore varp->isAny?
                    }
                    if (varp->width() > 64) {
                        funcName += "_W";
                    }
                    const size_t msb = varp->basicp()->lsb() + varp->basicp()->width();
                    puts(funcName+"(Py"+v3Global.opt.prefix()+", "+nodep->name()+", "+cvtToStr(msb)+", "+cvtToStr(varp->basicp()->lsb())+", "+(varp->isSigned()? "true" : "false")+")\n");
                }

                // Export non const parameters
                if (varp->isParam() && !VN_IS(varp->valuep(), Const)) {
                    puts("VL_PY_PARAM(Py"+v3Global.opt.prefix()+","+nodep->name()+")\n");
                }

                // Export public functions
                const AstCFunc* funcp = VN_CAST(nodep, CFunc);
                if (funcp && !funcp->skipDecl() && !funcp->dpiImport() && funcp->funcPublic()) {
                    if (funcp->ifdef()!="") { puts("#ifdef "+funcp->ifdef()+"\n"); }
                    puts((funcp->isStatic().trueU() ? "VL_PY_FUNC_STATIC(" : "VL_PY_FUNC("));
                    puts("Py"+v3Global.opt.prefix()+", " + funcp->name() + ")\n");
                    if (funcp->ifdef()!="") { puts("#endif  // "+funcp->ifdef()+"\n"); }
                }
            }
        }

        puts("VL_PY_FUNC(Py"+v3Global.opt.prefix()+", eval)\n");
        puts("VL_PY_FUNC(Py"+v3Global.opt.prefix()+", final)\n");

        if (v3Global.opt.trace()) {
            puts("VL_PY_FUNC_TRACE(Py"+v3Global.opt.prefix()+")\n");
        }

        if (v3Global.opt.inhibitSim()) {
            puts("VL_PY_FUNC(Py"+v3Global.opt.prefix()+", inhibitSim)\n");
        }

        if (hasDpi()) {
            puts(".def_static(\"set_dpi_functions\", &Py"+v3Global.opt.prefix()+"::setDpiMetaClass, pybind11::arg(\"dpiMetaClass\"))\n");
        }

        m_ofp->indentDec();
        puts(";\n");
    }

    static bool hasOutputs(const AstCFunc* funcp) {
        if (funcp->rtnType() != AstBasicDTypeKwd::VOID) {
            return true;
        }
        for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
            if (!varp->isNonOutput()) {
                return true;
            }
        }
        return false;
    }

    static string dpiMetaClassName() {
        return "PyDpiMetaClass_"+v3Global.opt.prefix();
    }

    void emitConvertVarCToPython(const AstVar* varp, const string& pyname, const string& cname, bool rawC) {
        const ArgDpiInfo argInfo = getArgDpiInfo(varp);
        switch (argInfo.type) {
        case ArgDpiInfo::OPENARRHNDL:
            v3fatal("% OpenArrayHandler not implemented");
            puts(pyname+" = svOpenArrayHandleWrapper("+cname+");\n");
            break;
        case ArgDpiInfo::BITCHAR:
            puts("svBitVecValWrapper tmp_"+cname+"(1);\n");
            puts("tmp_"+cname+".setBit(0, "+cname+");\n");
            puts(pyname+" = tmp_"+cname+";\n");
            break;
        case ArgDpiInfo::LOGCHAR:
            puts("svLogicVecValWrapper tmp_"+cname+"(1);\n");
            puts("tmp_"+cname+".setLogic(0, "+cname+");\n");
            puts(pyname+" = tmp_"+cname+";\n"); break;
        case ArgDpiInfo::VLTYPE:
            switch (argInfo.vlType) {
            case AstBasicDTypeKwd::BIT:
                if (rawC) {
                    puts(pyname+" = svBitVecValWrapper(reinterpret_cast<const svBitVecVal*>("+cname+"), "+cvtToStr(varp->basicp()->width())+"U);\n");
                } else {
                    puts(pyname+" = "+cname+";\n");
                }
                break;
            case AstBasicDTypeKwd::TIME:
            case AstBasicDTypeKwd::LOGIC:
            case AstBasicDTypeKwd::INTEGER:
                if (rawC) {
                    puts(pyname+" = svLogicVecValWrapper(reinterpret_cast<const svLogicVecVal*>("+cname+"), "+cvtToStr(varp->basicp()->width())+"U);\n");
                } else {
                    puts(pyname+" = "+cname+";\n");
                }
                break;
            case AstBasicDTypeKwd::BYTE:
                puts(pyname+" = static_cast<"+(argInfo.unsgnd?"unsigned":"signed")+" char>("+cname+");\n");
                break;
            case AstBasicDTypeKwd::SHORTINT:
            case AstBasicDTypeKwd::INT:
            case AstBasicDTypeKwd::LONGINT:
            case AstBasicDTypeKwd::DOUBLE:
            case AstBasicDTypeKwd::FLOAT:
            case AstBasicDTypeKwd::STRING:
            case AstBasicDTypeKwd::UINT32:
            case AstBasicDTypeKwd::UINT64:
                puts(pyname+" = "+cname+";\n");
                break;
            case AstBasicDTypeKwd::CHANDLE:
                puts(pyname+" = py_voidp(reinterpret_cast<uintptr_t>("+cname+"));\n");
                break;
            default:
                v3fatal("% Illegal AstBasicDTypeKwd type");
            }
            break;
        default:
            v3fatal("% Unsupported dpi type");
        }
    }

    void emitConvertVarPythonToC(const AstVar* varp, const string& cname, const string& pyname, bool rawC) {
        const ArgDpiInfo argInfo = getArgDpiInfo(varp);
        const string tmp_cname = "tmp_"+cname;
        switch (argInfo.type) {
        case ArgDpiInfo::OPENARRHNDL:
            v3fatal("% OpenArrayHandler not implemented");
            if (rawC) {
                puts(cname+" = pybind11::cast<svOpenArrayHandleWraper>("+pyname+").data();\n");
            } else {
                puts("svOpenArrayHandleWrapper "+cname+" = pybind11::cast<svOpenArrayHandleWrapper>("+pyname+");\n");
            }
            break;
        case ArgDpiInfo::BITCHAR:
            puts("svBitVecValWrapper "+tmp_cname+" = pybind11::cast<svBitVecValWrapper>("+pyname+");\n");
            puts("if ("+tmp_cname+".width() != "+cvtToStr(varp->basicp()->width())+"U) {\n");
            puts("throw std::length_error(\"Result is of wrong size. Must be "+cvtToStr(varp->basicp()->width())+" bits.\");\n");
            puts("}\n");
            if (rawC) {
                puts("*"+varp->name()+" = "+tmp_cname+".getBit(0);\n");
            } else {
                puts("svBit "+cname+" = "+tmp_cname+".getBit(0);\n");
            }
            break;
        case ArgDpiInfo::LOGCHAR:
            puts("svLogicVecValWrapper "+tmp_cname+" = pybind11::cast<svLogicVecValWrapper>("+pyname+");\n");
            puts("if ("+tmp_cname+".width() != "+cvtToStr(varp->basicp()->width())+"U) {\n");
            puts("throw std::runtime_error(\"Result is of wrong size. Must be "+cvtToStr(varp->basicp()->width())+" bits.\");\n");
            puts("}\n");
            if (rawC) {
                puts("*"+cname+" = "+tmp_cname+".getLogic(0);\n");
            } else {
                puts("svLogic "+cname+" = "+tmp_cname+".getLogic(0);\n");
            }
            break;
        case ArgDpiInfo::VLTYPE:
            switch (argInfo.vlType) {
            case AstBasicDTypeKwd::BIT:
                puts("svBitVecValWrapper "+(rawC?tmp_cname:cname)+" = pybind11::cast<svBitVecValWrapper>("+pyname+");\n");
                puts("if ("+(rawC?tmp_cname:cname)+".width() != "+cvtToStr(varp->basicp()->width())+"U) {\n");
                puts("throw std::length_error(\"Result is of wrong size. Must be "+cvtToStr(varp->basicp()->width())+" bits.\");\n");
                puts("}\n");
                if (rawC) {
                    puts(tmp_cname+".copyTo("+cname+");\n");
                }
                break;
            case AstBasicDTypeKwd::TIME:
            case AstBasicDTypeKwd::LOGIC:
            case AstBasicDTypeKwd::INTEGER:
                puts("svLogicVecValWrapper "+(rawC?tmp_cname:cname)+" = pybind11::cast<svLogicVecValWrapper>("+pyname+");\n");
                puts("if ("+(rawC?tmp_cname:cname)+".width() != "+cvtToStr(varp->basicp()->width())+"U) {\n");
                puts("throw std::length_error(\"Result is of wrong size. Must be "+cvtToStr(varp->basicp()->width())+" bits.\");\n");
                puts("}\n");
                if (rawC) {
                    puts(tmp_cname+".copyTo("+cname+");\n");
                }
                break;
            case AstBasicDTypeKwd::BYTE:
                if (rawC) {
                    puts("*");
                } else {
                    puts("char ");
                }
                puts(cname+" = static_cast<"+(argInfo.unsgnd?"unsigned ":"signed ")+"short>(pybind11::cast<pybind11::int_>("+pyname+"));\n");
                break;
            case AstBasicDTypeKwd::SHORTINT:
                if (rawC) {
                    puts("*");
                } else {
                    puts("short ");
                }
                puts(cname+" = static_cast<"+(argInfo.unsgnd?"unsigned ":"signed ")+"short>(pybind11::cast<pybind11::int_>("+pyname+"));\n");
                break;
            case AstBasicDTypeKwd::INT:
                if (rawC) {
                    puts("*");
                } else {
                    puts("int ");
                }
                puts(cname+" = static_cast<"+(argInfo.unsgnd?"unsigned ":"signed ")+"int>(pybind11::cast<pybind11::int_>("+pyname+"));\n");
                break;
            case AstBasicDTypeKwd::LONGINT:
                if (rawC) {
                    puts("*");
                } else {
                    puts("long long ");
                }
                puts(cname+" = static_cast<"+(argInfo.unsgnd?"unsigned ":"signed ")+"long long>(pybind11::cast<pybind11::int_>("+pyname+"));\n");
                break;
            case AstBasicDTypeKwd::DOUBLE:
                if (rawC) {
                    puts("*");
                } else {
                    puts("double ");
                }
                puts(cname+" = static_cast<double>(pybind11::cast<pybind11::float_>("+pyname+"));\n");
                break;
            case AstBasicDTypeKwd::FLOAT:
                if (rawC) {
                    puts("*");
                } else {
                    puts("float ");
                }
                puts(cname+" = static_cast<float>(pybind11::cast<pybind11::float_>("+pyname+"));\n");
                break;
            case AstBasicDTypeKwd::STRING:
                if (rawC) {
                    puts("static std::string static_");
                } else {
                    puts("std::string ");
                }
                puts(cname+" = pybind11::cast<std::string>("+pyname+");\n");
                if (rawC) {
                    puts("*"+cname+" = static_"+cname+".c_str();\n");
                }
                break;
            case AstBasicDTypeKwd::UINT32:
                if (rawC) {
                    puts("*");
                } else {
                    puts("vluint32_t ");
                }
                puts(cname+" = pybind11::cast<vluint32_t>("+pyname+");\n");
                break;
            case AstBasicDTypeKwd::UINT64:
                if (rawC) {
                    puts("*");
                } else {
                    puts("vluint64_t ");
                }
                puts(cname+" = pybind11::cast<vluint64_t>("+pyname+");\n");
                break;
            case AstBasicDTypeKwd::CHANDLE:
                if (!rawC) {
                    puts("void* "+cname+";\n");
                }
                puts("if (("+pyname+".is_none()) || ("+pyname+".attr(\"value\").is_none())) {\n");
                if (rawC) {
                    puts("*");
                }
                puts(cname+" = nullptr;\n");
                puts("} else {\n");
                if (rawC) {
                    puts("*");
                }
                puts(cname+" = reinterpret_cast<void*>(pybind11::cast<uintptr_t>("+pyname+".attr(\"value\")));\n");
                puts("}\n");
                break;
            default:
                v3fatal("% Illegal AstBasicDTypeKwd type");
            }
            break;
        default:
            v3fatal("% Unsupported dpi type");
        }
    }

    void emitDeclareCVariable(const AstVar* varp, const string& cname) {
        const ArgDpiInfo argInfo = getArgDpiInfo(varp);
        switch (argInfo.type) {
        case ArgDpiInfo::OPENARRHNDL:
            puts("svOpenArrayHandleWrapper "+cname+";\n"); break;
        case ArgDpiInfo::BITCHAR:
            puts("svBit "+cname+";\n"); break;
        case ArgDpiInfo::LOGCHAR:
            puts("svLogic "+cname+";\n"); break;
        case ArgDpiInfo::VLTYPE:
            switch (argInfo.vlType) {
            case AstBasicDTypeKwd::BIT:
                puts("svBitVecValWrapper "+cname+"("+cvtToStr(varp->basicp()->width())+");\n"); break;
            case AstBasicDTypeKwd::TIME:
            case AstBasicDTypeKwd::LOGIC:
            case AstBasicDTypeKwd::INTEGER:
                puts("svLogicVecValWrapper "+cname+"("+cvtToStr(varp->basicp()->width())+");\n"); break;
            case AstBasicDTypeKwd::BYTE:
                puts(string(argInfo.unsgnd?"unsigned ":"signed ")+"char "+cname+";\n"); break;
            case AstBasicDTypeKwd::SHORTINT:
                puts(string(argInfo.unsgnd?"unsigned ":"signed ")+"short "+cname+";\n"); break;
            case AstBasicDTypeKwd::INT:
                puts(string(argInfo.unsgnd?"unsigned ":"signed ")+"int "+cname+";\n"); break;
            case AstBasicDTypeKwd::LONGINT:
                puts(string(argInfo.unsgnd?"unsigned ":"signed ")+"long long "+cname+";\n"); break;
            case AstBasicDTypeKwd::DOUBLE:
                puts("double "+cname+";\n"); break;
            case AstBasicDTypeKwd::FLOAT:
                puts("float "+cname+";\n"); break;
            case AstBasicDTypeKwd::STRING:
                puts("const char* "+cname+";\n"); break;
            case AstBasicDTypeKwd::UINT32:
                puts("vluint64_t "+cname+";\n"); break;
            case AstBasicDTypeKwd::UINT64:
                puts("vluint64_t "+cname+";\n"); break;
            case AstBasicDTypeKwd::CHANDLE:
                puts("void* "+cname+";\n"); break;
            default:
                v3fatal("% Illegal AstBasicDTypeKwd type");
            }
            break;
        default:
            v3fatal("% Unsupported dpi type");
        }
    }

    void emitReturnPythonToC(AstBasicDTypeKwd type, string cname, string pyname) {
        string tmp_cname = "tmp_"+cname;
        //string rtnType = funcp->rtnTypeVoid();
        switch (type) {
        case AstBasicDTypeKwd::BIT:
            puts("svBitVecValWrapper "+tmp_cname+" = py::cast<svBitVecValWrapper>("+pyname+");\n");
            puts("if ("+tmp_cname+".width() > 32U) {\n"); // FIXME: should be 1????
            puts("throw std::length_error(\"Result is too big.\");\n");
            puts("}\n");
            puts("svBitVecVal "+cname+";\n");
            puts(tmp_cname+".copyTo(&"+cname+");\n");
            break;
        case AstBasicDTypeKwd::TIME:
        case AstBasicDTypeKwd::LOGIC:
        case AstBasicDTypeKwd::INTEGER:
            puts("svLogicVecValWrapper "+tmp_cname+" = py::cast<svLogicVecValWrapper>("+pyname+");\n");
            puts("svLogicVecVal "+cname+";\n");
            puts(tmp_cname+".copyTo(&"+cname+");\n");
            break;
        case AstBasicDTypeKwd::BYTE:
            puts("char "+cname+" = static_cast<signed char>(py::cast<py::int_>("+pyname+"));\n");
            break;
        case AstBasicDTypeKwd::INT:
            puts("int "+cname+" = static_cast<int>(py::cast<py::int_>("+pyname+"));\n");
            break;
            //puts(rtnType+" function_result = static_cast<"+rtnType+">(py::cast<py::int_>(py_result_tuple["+cvtToStr(tuple_index)+"]));\n"); break;
        case AstBasicDTypeKwd::LONGINT:
            puts("long long "+cname+" = static_cast<long long>(py::cast<py::int_>("+pyname+"));\n");
            break;
            //puts(rtnType+" function_result = static_cast<"+rtnType+">(py::cast<py::int_>(py_result_tuple["+cvtToStr(tuple_index)+"]));\n"); break;
        case AstBasicDTypeKwd::SHORTINT:
            puts("short "+cname+" = static_cast<short>(py::cast<py::int_>("+pyname+"));\n");
            break;
            //puts(rtnType+" function_result = static_cast<"+rtnType+">(py::cast<py::int_>(py_result_tuple["+cvtToStr(tuple_index)+"]));\n"); break;
        case AstBasicDTypeKwd::DOUBLE:
            puts("double "+cname+" = py::cast<double>("+pyname+");\n");
            break;
            //puts(rtnType+" function_result = py::cast<"+rtnType+">(py_result_tuple["+cvtToStr(tuple_index)+"]);\n"); break;
        case AstBasicDTypeKwd::FLOAT:
            puts("float "+cname+" = py::cast<float>("+pyname+");\n");
            break;
            //puts(rtnType+" function_result = py::cast<float>(py_result_tuple["+cvtToStr(tuple_index)+"]);\n"); break;
        case AstBasicDTypeKwd::STRING:
            puts("static std::string "+tmp_cname+" = py::str("+pyname+");\n");
            puts("const char* "+cname+" = "+tmp_cname+".c_str();\n");
            break;
        case AstBasicDTypeKwd::UINT32:
            puts("vluint32_t "+cname+" = py::cast<vluint32_t>("+pyname+");\n");
            break;
        case AstBasicDTypeKwd::UINT64:
            puts("vluint64_t "+cname+" = py::cast<vluint64_t>("+pyname+");\n");
            break;
        case AstBasicDTypeKwd::CHANDLE:
            puts("void* "+cname+";\n");
            puts("if (("+pyname+".is_none()) || ("+pyname+".attr(\"value\").is_none())) {\n");
            puts(cname+" = nullptr;\n");
            puts("} else {\n");
            puts(cname+" = reinterpret_cast<void*>(py::cast<uintptr_t>("+pyname+".attr(\"value\")));\n");
            puts("}\n");
            break;
        default:
            v3fatal("% Illegal AstBasicDTypeKwd type"); break;
        }
    }

    void emitReturnCToPython(AstBasicDTypeKwd type, string pyname, string cname) {
        switch (type) {
        case AstBasicDTypeKwd::BIT:
            // FIXME: how to know a return has 0, 1, 2, 3, ..., 32 bits?
            puts("svBitVecValWrapper "+cname+"_tmp(1);\n");
            puts(cname+"_tmp.setBit(0, "+cname+");\n");
            puts(pyname+" = "+cname+"_tmp;\n"); break;
        case AstBasicDTypeKwd::TIME:
        case AstBasicDTypeKwd::LOGIC:
        case AstBasicDTypeKwd::INTEGER:
            // FIXME: how to know a return has 0, 1, 2, 3, ..., 32 logic bits?
            puts("svLogicVecValWrapper "+cname+"_tmp(1);\n");
            puts(cname+"_tmp.setLogic(0, "+cname+");\n");
            puts(pyname+" = "+cname+"_tmp;\n"); break;
        case AstBasicDTypeKwd::BYTE:
            puts(pyname+" = static_cast<signed char>("+cname+");\n"); break;
        case AstBasicDTypeKwd::INT:
        case AstBasicDTypeKwd::LONGINT:
        case AstBasicDTypeKwd::SHORTINT:
        case AstBasicDTypeKwd::DOUBLE:
        case AstBasicDTypeKwd::FLOAT:
        case AstBasicDTypeKwd::STRING:
        case AstBasicDTypeKwd::UINT32:
        case AstBasicDTypeKwd::UINT64:
            puts(pyname+" = "+cname+";\n"); break;
        case AstBasicDTypeKwd::CHANDLE:
            puts(pyname+" = py_voidp(reinterpret_cast<uintptr_t>("+cname+"));\n"); break;
        case AstBasicDTypeKwd::VOID:
            break;
        default:
            v3fatal("% Illegal AstBasicDTypeKwd type"); break;
        }
    }

    static string pyWrapperReturnType(const AstCFunc* funcp) {
        if (hasOutputs(funcp)) {
            return "pybind11::object";
        } else {
            return "void";
        }
    }

    string inputPyArgsString(const AstCFunc* funcp) {
        string res;
        size_t inputIndex = 0;
        for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
            if (!varp->isNonOutput()) {
                continue;
            }
            if (inputIndex != 0) {
                res += ", ";
            }
            res += "pybind11::object py_" + varp->name();
            inputIndex += 1;
        }
        return res;
    }

    void emitPrefixModuleDPIBaseWrapper() {
        if (!hasDpi()) {
            return;
        }

        puts("\n");
        puts("class "+dpiMetaClassName()+" : public BaseDpiMetaClass {\n");
        puts("public:\n");
        puts("Py"+v3Global.opt.prefix() +"* getInstance() {\n");
        puts("svScopeWrapper scopeWrapper = getCurrentScope();\n");
        puts("Py"+v3Global.opt.prefix()+"* instance = reinterpret_cast<Py"+v3Global.opt.prefix()+"*>(svGetUserData(scopeWrapper.scope, nullptr));\n");
        puts("return instance;\n");
        puts("}\n");
        for (std::vector<const AstCFunc *>::iterator it = m_dpiFunctions.begin(); it != m_dpiFunctions.end(); ++it) {
            const AstCFunc *funcp = *it;
            // FIXME: returning void is not guarantee to returning nothing!!!!
            // FIXME: remove static Vtop:static metaclass
            string pyRtnType = pyWrapperReturnType(funcp);

            string context;
            if (funcp->dpiImport() && (!funcp->context())) {
                context = "static ";
            }
            string dpiType;
            if (funcp->dpiImport()) {
                dpiType = "import";
            } else {
                dpiType = "export";
            }
            puts("// DPI "+dpiType+" declared at "+funcp->fileline()->ascii()+"\n");
            puts(context+pyRtnType+ " "+funcp->name()+"("+inputPyArgsString(funcp)+");\n");
        }
        puts("};\n");
    }

    void emitPyDeclareDPIWrapper() {
        if (!hasDpi()) {
            return;
        }

        puts("\n");
        puts("pybind11::class_<PyDpiMetaClass_"+v3Global.opt.prefix()+", BaseDpiMetaClass, std::shared_ptr<PyDpiMetaClass_"+v3Global.opt.prefix()+">>(pyTopClass, \"DpiMetaClass\", pybind11::module_local{})\n");
        m_ofp->indentInc();
        puts(".def(pybind11::init<>())\n");
        puts(".def(\"get_instance\", &PyDpiMetaClass_"+v3Global.opt.prefix()+"::getInstance)\n");

        for(std::vector<const AstCFunc *>::iterator it = m_dpiFunctions.begin(); it != m_dpiFunctions.end(); ++it) {
            const AstCFunc *funcp = *it;
            string defStr = "def";
            if (funcp->dpiImport() && !funcp->context()) {
                defStr = "def_static";
            }
            puts(("."+defStr+"(\"")+funcp->name()+"\", &PyDpiMetaClass_"+v3Global.opt.prefix()+"::"+funcp->name()+")\n");
        }

        m_ofp->indentDec();
        puts(";\n");
    }

    void emitPyDeclareFunction() {
        puts("\n");
        puts("namespace vl_py {\n");
        puts("\n");
        puts("void declare_"+v3Global.opt.prefix()+"(pybind11::module& m) {\n");

        emitPyDeclareModule();
        emitPyDeclareDPIWrapper();

        puts("\n");
        puts("Py"+v3Global.opt.prefix()+"::s_dpiMetaClass = pybind11::none{};\n");

        puts("}\n");
        puts("}  // namespace vl_py\n");
    }

    void emitCSource() {
        emitCSourceHeader();
        emitPrefixModuleDPIBaseWrapper();
        emitCSourceVerilatedModuleWrapperImpl();
        emitPyDeclareFunction();
        emitCSourceDpi();
    }

    void emitCSourceHeader() {
        puts("#include \""+v3Global.opt.prefix()+"_py.h\"\n");

        if (hasDpi()) {
            puts("\n");
            puts("#include \""+v3Global.opt.prefix()+"__Dpi.h\"\n");
            puts("#include \"svdpi.h\"\n");
        }
        puts("#include \"verilated_py.h\"\n");

        puts("\n");
        puts("#include <pybind11/pybind11.h>\n");

        puts("\n");
        puts("using namespace vl_py;\n");
        puts("namespace py = pybind11;\n");
    }

    void emitCSourceVerilatedModuleWrapperImpl () {
        puts("\n");
        puts("// Py"+v3Global.opt.prefix()+" implementation\n");

        if (hasDpi()) {
            puts("\n");
            puts("py::object Py"+v3Global.opt.prefix()+"::s_dpiMetaClass;\n");
        }
        puts("\n");
        puts("Py"+v3Global.opt.prefix()+"::Py"+v3Global.opt.prefix()+"(const std::string& name)\n");
        puts(": "+v3Global.opt.prefix()+"(name.c_str()) {\n");
        if (hasDpi()) {
            puts("std::string scopeName = name + \".top\";\n");
            puts("svScope scope = svGetScopeFromName(scopeName.c_str());\n");
            puts("if (!scope) {\n");
            puts("throw std::runtime_error(\"Unable to get scope from \"+scopeName);\n");
            puts("}\n");
            puts("if (s_dpiMetaClass.is_none()) {\n");
            puts("m_dpiObject = py::cast(new PyDpiMetaClass_"+v3Global.opt.prefix()+");\n");
            puts("} else {\n");
            puts("m_dpiObject = s_dpiMetaClass();\n");
            puts("}\n");
            puts("int putRes = svPutUserData(scope, nullptr, this);\n");
            puts("if (putRes == -1) {\n");
            puts("throw std::runtime_error(\"Unable to store DPI function wrapper object\");\n");
            puts("}\n");
        }
        puts("}\n");
        puts("\n");
        if (hasDpi()) {
            puts("void Py"+v3Global.opt.prefix()+"::setDpiMetaClass(pybind11::object metaClass) {\n");
            puts("s_dpiMetaClass = metaClass;\n");
            puts("}\n");
        }
    }

    void emitCSourceDpi() {
        if (!hasDpi()) {
            return;
        }

        puts("\n");
        puts("// Start DPI functions\n");

        for(std::vector<const AstCFunc *>::iterator it = m_dpiFunctions.begin(); it != m_dpiFunctions.end(); ++it) {
            const AstCFunc *funcp = *it;
            if (funcp->dpiImport()) {
                emitCSourceDpiImport(funcp);
            } else {
                emitCSourceDpiExport(funcp);
            }
        }
    }

    void emitCSourceDpiImport(const AstCFunc* funcp) {
        puts("\n");
        puts("// DPI import at "+funcp->fileline()->ascii()+"\n");

        puts(pyWrapperReturnType(funcp)+" "+dpiMetaClassName()+"::"+funcp->name()+"("+inputPyArgsString(funcp)+") {\n");
        puts("throw std::runtime_error(\"Missing python implementation for "+funcp->name()+".\");\n");
        puts("}\n");

        puts(funcp->rtnTypeVoid()+" "+funcp->name()+"("+EmitCBaseVisitor::cFuncArgs(funcp).c_str()+") {\n");

        unsigned nbInputs = 0;
        unsigned nbOutputs = 0;

        bool need_chandle = false;
        if (funcp->rtnType() != AstBasicDTypeKwd::VOID) {
            nbOutputs += 1;
        }
        for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
            if (varp->isNonOutput()) {
                nbInputs += 1;
            } else {
                nbOutputs += 1;
            }
            ArgDpiInfo argInfo = getArgDpiInfo(varp);
            if (argInfo.type == ArgDpiInfo::VLTYPE && argInfo.vlType == AstBasicDTypeKwd::CHANDLE) {
                need_chandle = true;
            }
        }

        if (need_chandle) {
            puts("py::object py_voidp = py::module::import(\"ctypes\").attr(\"c_void_p\");\n");
            puts("\n");
        }

        if (nbInputs > 0) {
            puts("// Create arguments tuple\n");
            puts("py::tuple py_arguments("+cvtToStr(nbInputs)+"U);\n");
            unsigned inputCount = 0;

            for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
                if (!varp->isNonOutput()) {
                    continue;
                }
                emitConvertVarCToPython(varp, "py_arguments["+cvtToStr(inputCount)+"]", varp->name(), true);
                inputCount += 1;
            }
            puts("\n");
        }

        puts("// Call python function\n");
        if (funcp->context()) {
            puts("Py"+v3Global.opt.prefix()+"* instance = reinterpret_cast<Py"+v3Global.opt.prefix()+"*>(svGetUserData(svGetScope(), nullptr));\n");
            puts("py::object py_result = instance->m_dpiObject.attr(\""+funcp->name()+"\")(");
        } else {
            puts("py::object py_result = Py"+v3Global.opt.prefix()+"::s_dpiMetaClass.attr(\""+funcp->name()+"\")(");
        }
        if (nbInputs > 0) {
            puts("*py_arguments");
        }
        puts(");\n");

        if (nbOutputs == 0) {
            puts("\n");
            puts("if (!py_result.is_none()) {\n");
            puts("throw std::length_error(\""+funcp->name()+" expects 0 returns.\");\n");
            puts("}\n");
        } else {
            puts("\n");
            puts("// Convert results to tuple\n");
            if (nbOutputs == 0) {
                puts("if (!py_result.is_none()) {\n");
                puts("throw std::length_error(\""+funcp->name()+" expects 0 returns.\");\n");
                puts("}\n");
            } else {
                puts("if (py_result.is_none()) {\n");
                puts("throw std::length_error(\""+funcp->name()+" returned 0 results. Expected "+cvtToStr(nbOutputs)+".\");\n");
                puts("}\n");
                if (nbOutputs == 1) {
                    puts("py::tuple py_result_tuple;\n");
                    puts("if (((!PyTuple_Check(py_result.ptr())) && (!PyList_Check(py_result.ptr())))) {\n");
                    puts("py_result_tuple = py::tuple(1);\n");
                    puts("py_result_tuple[0] = py_result;\n");
                    puts("py_result = py_result_tuple;\n");
                    puts("} else {\n");
                    puts("py_result_tuple = py_result;\n");
                    puts("}\n");
                } else {
                    puts("if (((!PyTuple_Check(py_result.ptr())) && (!PyList_Check(py_result.ptr())))) {\n");
                    puts("throw std::length_error(\""+funcp->name()+" returns wrong # of results. Expected "+cvtToStr(nbOutputs)+".\");\n");
                    puts("}\n");
                    puts("py::tuple py_result_tuple = py_result;\n");
                }
                puts("if (py_result_tuple.size() != "+cvtToStr(nbOutputs)+"U) {\n");
                puts("throw std::length_error(\""+funcp->name()+" returns wrong # of outputs. Expected "+cvtToStr(nbOutputs)+".\");\n");
                puts("}\n");
            }

            unsigned tuple_index = 0;

            puts("\n");
            puts("// Extract results\n");
            if (funcp->rtnType() != AstBasicDTypeKwd::VOID) {
                emitReturnPythonToC(funcp->rtnType(), "function_result", "py_result_tuple["+cvtToStr(tuple_index)+"]");
                tuple_index += 1;
            }

            for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
                if (varp->isNonOutput()) {
                    continue;
                }
                emitConvertVarPythonToC(varp, varp->name(), "py_result_tuple["+cvtToStr(tuple_index)+"]", true);
                tuple_index += 1;
            }
        }

        if (funcp->rtnType() != AstBasicDTypeKwd::VOID) {
            puts("return function_result;\n");
        }
        puts("}\n");
    }

    void emitCSourceDpiExport(const AstCFunc* funcp) {
        puts("\n");
        puts("// DPI export declared at "+funcp->fileline()->ascii()+"\n");
        puts(pyWrapperReturnType(funcp)+" "+dpiMetaClassName()+"::"+funcp->name()+"("+inputPyArgsString(funcp)+") {\n");

        size_t nbInputs = 0;
        size_t nbOutputs = 0;
        bool need_chandle = false;
        if (funcp->rtnType() != AstBasicDTypeKwd::VOID) {
            nbOutputs += 1;
        }
        for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
            if (varp->isNonOutput()) {
                nbInputs += 1;
            } else {
                nbOutputs += 1;
            }
            ArgDpiInfo argInfo = getArgDpiInfo(varp);
            if (argInfo.type == ArgDpiInfo::VLTYPE && argInfo.vlType == AstBasicDTypeKwd::CHANDLE) {
                need_chandle = true;
            }
        }
        if (need_chandle) {
            puts("py::object py_voidp = py::module::import(\"ctypes\").attr(\"c_void_p\");\n");
            puts("\n");
        }

        if ((nbInputs != 0) || (nbOutputs != 0)) {
            puts("// Convert python arguments to C arguments and declare C outputs\n");
            for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
                const ArgDpiInfo argInfo = getArgDpiInfo(varp);
                if (varp->isNonOutput()) {
                    // Input of the function -> convert pybind11::object py_ARG to CTYPE ARG
                    emitConvertVarPythonToC(varp, varp->name(), "py_"+varp->name(), false);
                } else {
                    // Output of the function -> create a CTYPE ARG to which the export can write
                    emitDeclareCVariable(varp, varp->name());
                }
            }
            puts("\n");
        }

        // Call C function with arguments (and eventually get return object)
        puts("// Call DPI exported function\n");
        if (funcp->rtnType() != AstBasicDTypeKwd::VOID) {
            puts(funcp->rtnTypeVoid()+" function_result = ");
        }
        puts("::"+funcp->name()+"(");
        {
            size_t argIndex = 0;
            for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
                const ArgDpiInfo argInfo = getArgDpiInfo(varp);
                if (argIndex != 0) {
                    puts(", ");
                }
                ++argIndex;
                if (varp->isNonOutput()) {
                    switch (argInfo.type) {
                    case ArgDpiInfo::OPENARRHNDL:
                    case ArgDpiInfo::BITCHAR:
                    case ArgDpiInfo::LOGCHAR:
                        puts(varp->name()); break;
                    case ArgDpiInfo::VLTYPE:
                        switch (argInfo.vlType) {
                        case AstBasicDTypeKwd::BIT:
                            puts(varp->name()+".data()"); break;
                        case AstBasicDTypeKwd::TIME:
                        case AstBasicDTypeKwd::LOGIC:
                        case AstBasicDTypeKwd::INTEGER:
                            puts(varp->name()+".data()"); break;
                        case AstBasicDTypeKwd::BYTE:
                        case AstBasicDTypeKwd::SHORTINT:
                        case AstBasicDTypeKwd::INT:
                        case AstBasicDTypeKwd::LONGINT:
                        case AstBasicDTypeKwd::DOUBLE:
                        case AstBasicDTypeKwd::FLOAT:
                        case AstBasicDTypeKwd::UINT32:
                        case AstBasicDTypeKwd::UINT64:
                        case AstBasicDTypeKwd::CHANDLE:
                            puts(varp->name()); break;
                        case AstBasicDTypeKwd::STRING:
                            puts(varp->name()+".c_str()"); break;
                        default:
                            v3fatal("% Illegal AstBasicDTypeKwd type");
                        }
                        break;
                    default:
                        v3fatal("% Unsupported dpi type");
                    }
                } else {
                    switch (argInfo.type) {
                    case ArgDpiInfo::OPENARRHNDL:
                        puts(varp->name()); break;
                    case ArgDpiInfo::BITCHAR:
                    case ArgDpiInfo::LOGCHAR:
                        puts("&"+varp->name()); break;
                    case ArgDpiInfo::VLTYPE:
                        switch (argInfo.vlType) {
                        case AstBasicDTypeKwd::BIT:
                            puts(varp->name()+".data()"); break;
                        case AstBasicDTypeKwd::TIME:
                        case AstBasicDTypeKwd::LOGIC:
                        case AstBasicDTypeKwd::INTEGER:
                            puts(varp->name()+".data()"); break;
                        case AstBasicDTypeKwd::BYTE:
                        case AstBasicDTypeKwd::SHORTINT:
                        case AstBasicDTypeKwd::INT:
                        case AstBasicDTypeKwd::LONGINT:
                        case AstBasicDTypeKwd::DOUBLE:
                        case AstBasicDTypeKwd::FLOAT:
                        case AstBasicDTypeKwd::STRING:
                        case AstBasicDTypeKwd::UINT32:
                        case AstBasicDTypeKwd::UINT64:
                        case AstBasicDTypeKwd::CHANDLE:
                            puts("&"+varp->name()); break;
                        default:
                            v3fatal("% Illegal AstBasicDTypeKwd type");
                        }
                        break;
                    default:
                        v3fatal("% Unsupported dpi type");
                    }
                }
            }
        }
        puts(");\n");

        if (nbOutputs != 0) {
            //Convert return + outputs to python objects and return as tuple
            puts("\n");
            puts("// Convert C outputs to python objects\n");
            puts("py::tuple results("+cvtToStr(nbOutputs)+");\n");
            size_t outputIndex = 0;
            if (funcp->rtnType() != AstBasicDTypeKwd::VOID) {
                emitReturnCToPython(funcp->rtnType(), "results["+cvtToStr(outputIndex)+"]", "function_result");
                outputIndex += 1;
            }
            for (AstVar *varp = (AstVar*) funcp->argsp(); varp; varp = (AstVar*)varp->nextp()) {
                const ArgDpiInfo argInfo = getArgDpiInfo(varp);
                if (varp->isNonOutput()) {
                    continue;
                }
                if (outputIndex != 0) {
                    puts(", ");
                }
                emitConvertVarCToPython(varp, "results["+cvtToStr(outputIndex)+"]", varp->name(), false);
                outputIndex += 1;
            }
            if (nbOutputs == 1) {
                puts("return results[0];\n");
            } else {
                puts("return results;\n");
            }
        }
        puts("}\n");
    }

public:
    void emit() {
        m_ofp = newOutCFile(false/*source*/);
        emitHSource();
        m_ofp = newOutCFile(true/*source*/);
        emitCSource();
    }
};

void V3EmitPy::emitpy() {
    UINFO(2,__FUNCTION__<<": "<<endl);
    PythonVisitor pyVisitor;
    pyVisitor.iterate(v3Global.rootp());
    pyVisitor.emit();
}
