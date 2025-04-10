//
// Created by Tony Horrobin on 06/04/2025.
//

#pragma once

#include "core/ConfigurationElement.h"

extern void assertComparison(dagbase::ConfigurationElement::ValueType expected, dagbase::ConfigurationElement::ValueType actual, double tolerance, dagbase::ConfigurationElement::RelOp op);