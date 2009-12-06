/**
 * @file
 * This file contains the implementation of RbString, which is
 * a REvBayes wrapper around a regular string.
 *
 * @brief Implementation of RbString
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date$
 * @author The REvBayes development core team
 * @license GPL version 3
 *
 * $Id$
 */
 
#include "RbException.h"
#include "RbNames.h"
#include "RbString.h"
#include "StringVector.h"


/** Constructor from string */
RbString::RbString(const std::string& v) 
    : RbComplex(), value(v) {
}


/** Copy constructor */
RbString::RbString(const RbString& s) :
    RbComplex(), value(s.value) {
}


/** Clone function */
RbObject* RbString::clone() const {

	return (RbObject*)(new RbString(*this));
}


/** Equals comparison */
bool RbString::equals(const RbObject* obj) const {

    const RbString* p = dynamic_cast<const RbString*>(obj);
    if (p == NULL)
        return false;

	return (p->value == value);
}


/** Get class vector describing type of object */
const StringVector& RbString::getClass() const {

    static StringVector rbClass = StringVector(RbNames::RbString::name) + RbComplex::getClass();
    return rbClass;
}


/** Print value */
void RbString::printValue(std::ostream& o) const {

	o << value << std::endl;
}


/** Complete info on object */
std::string RbString::toString() const {

	return "RbString; value = '" + value + "'";
}


/** Type conversion to string */
RbString::operator std::string() const {

    return value;
}


/** String concatenation with operator+  */
RbString RbString::operator+(const RbString& s) const {

    return RbString(value+s.value);
}


