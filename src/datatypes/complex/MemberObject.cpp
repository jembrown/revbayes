/**
 * @file
 * This file contains the partial implementation of MemberObject, an abstract
 * base class for complex objects with member methods and variables.
 *
 * @brief Partial implementation of MemberObject
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-09-08, version 1.0
 *
 * $Id$
 */

#include "ArgumentRule.h"
#include "ConstantNode.h"
#include "Frame.h"
#include "MemberFunction.h"
#include "MemberObject.h"
#include "RbException.h"
#include "RbNames.h"
#include "VectorString.h"
#include "VariableNode.h"


/** Constructor: we set member variables here from member rules */
MemberObject::MemberObject(const MemberRules& memberRules, const MethodTable& methodInits) : RbComplex(), members(), methods(methodInits) {

    /* Fill member table (frame) based on member rules */
    for (MemberRules::const_iterator i=memberRules.begin(); i!=memberRules.end(); i++) {

        std::string name = (*i)->getArgLabel();
        if ((*i)->isReference()) {
            if ((*i)->hasDefault() == false)
                members.addReference(name, (*i)->getArgType(), (*i)->getArgDim());
            else
                members.addReference(name, (*i)->getDefaultReference());
        }
        else {
            if ((*i)->hasDefault() == false)
                members.addVariable(name, (*i)->getArgType(), (*i)->getArgDim());
            else
                members.addVariable(name, (*i)->getDefaultVariable());
        }
    }
}


/** Convert to type: throw an error */
MemberObject* MemberObject::convertTo(const std::string& type, int dim) const {

    std::ostringstream msg;
    msg << "Type conversion of " << getTypeSpec() << " to " << TypeSpec(type, dim) << " not supported (yet)";
    throw RbException(msg);
}


/** Execute member function with preprocessed arguments (repeated evaluation) */
DAGNode* MemberObject::executeMethod(const std::string& name, int funcId) {

    /* Get preprocessed arguments */
    std::vector<VariableSlot> arguments = methods.getProcessedArguments(funcId);

    /* Execute the operation */
    return executeOperation(name, arguments);
}


/** Execute member function (evaluate it once) */
DAGNode* MemberObject::executeMethod(const std::string& name, std::vector<Argument>& args) {

    /* Process the arguments */
    int funcId = methods.processArguments(name, args);

    /* Get the processed arguments */
    const std::vector<VariableSlot>& processedArguments = methods.getProcessedArguments(funcId);

    /* Execute the operation */
    DAGNode* retVal = executeOperation(name, processedArguments);

    /* Delete the processed arguments */
    methods.deleteProcessedArguments(funcId);

    /* Return the result */
    return retVal;
}


/** Get class vector describing type of object */
const VectorString& MemberObject::getClass(void) const {

    static VectorString rbClass = VectorString(MemberObject_name) + RbComplex::getClass();
    return rbClass;
}


/** Get value of a member variable */
const RbObject* MemberObject::getValue(const std::string& name) {

    return members.getValue(name);
}


/** Get value of a member variable (const) */
const RbObject* MemberObject::getValue(const std::string& name) const {

    return members.getValue(name);
}


/** Get a member variable */
const DAGNode* MemberObject::getVariable(const std::string& name) const {

    return members.getVariable(name);
}


/** Get a member variable (non-const, for derived classes) */
DAGNode* MemberObject::getVariable(const std::string& name) {

    return const_cast<DAGNode*>(members.getVariable(name));
}


/** Is convertible to type? Default is false for member objects; override if you want to support type conversion */
bool RbObject::isConvertibleTo(const std::string& type, int dim, bool once) const {

	return false;
}


/** Print value for user */
void MemberObject::printValue(std::ostream& o) const {

    const VariableTable& varTable = members.getVariableTable();
    for (VariableTable::const_iterator i=varTable.begin(); i!=varTable.end(); i++) {
        o << "." << (*i).first << std::endl;
        if ((*i).second.getValue() == NULL)
            o << "NULL";
        else
            (*i).second.getValue()->printValue(o);
        o << std::endl << std::endl;
    }
}


/** Complete info about object */
std::string MemberObject::richInfo(void) const {

    std::ostringstream o;
    o << getType() << ":" << std::endl;
    printValue(o);

    return o.str();
}


/** Set value of a member variable */
void MemberObject::setValue(const std::string& name, RbObject* val) {

    members.setValue(name, val);
}


/** Set a member variable */
void MemberObject::setVariable(const std::string& name, DAGNode* var) {

    members.setVariable(name, var);
}

