//
// Created by Tony Horrobin on 06/04/2025.
//

#pragma once

#include "core/ConfigurationElement.h"

extern void assertComparison(dagbase::ConfigurationElement::ValueType expected, dagbase::ConfigurationElement::ValueType actual, double tolerance, dagbase::ConfigurationElement::RelOp op, const char* path="unspecified");

template<typename Sut>
struct Assertion
{
    std::string path;
    dagbase::Variant value;
    double tolerance{ 0.0 };
    dagbase::ConfigurationElement::RelOp op{ dagbase::ConfigurationElement::RELOP_EQ };
    dagbase::Variant::Index typeIndex{ dagbase::Variant::TYPE_UNKNOWN };

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "path", &path);
        dagbase::ConfigurationElement::readConfig(config, "value", &value);
        dagbase::ConfigurationElement::readConfig<dagbase::Variant::Index>(config, "typeIndex", &dagbase::Variant::parseIndex, &typeIndex);
        if (typeIndex != dagbase::Variant::TYPE_UNKNOWN)
        {
            value = value.cast(typeIndex);
        }
        dagbase::ConfigurationElement::readConfig(config, "tolerance", &tolerance);
        dagbase::ConfigurationElement::readConfig<dagbase::ConfigurationElement::RelOp>(config, "op", &dagbase::ConfigurationElement::parseRelOp, &op);
    }

    void setValue(dagbase::Variant newValue)
    {
        if (typeIndex == dagbase::Variant::TYPE_UNKNOWN)
        {
            value = newValue;
        }
        else
        {
            value = newValue.cast(typeIndex);
        }
    }

    void makeItSo(Sut& sut, const std::string& cmd) const
    {
        auto actual = sut.find(path);
        assertComparison(value, actual, tolerance, op, (path + " " + cmd).c_str());
    }
};
