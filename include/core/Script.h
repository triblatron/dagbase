//
// Created by Tony Horrobin on 11/10/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"

#include <string_view>

namespace dagbase
{
    class InputStream;
    class Lua;
    class Node;
    class OutputStream;

    class DAGBASE_API Script
    {
    public:
        virtual ~Script() = default;

        virtual void configure(dagbase::ConfigurationElement& config) = 0;

        virtual void step(Node* self) = 0;

        virtual bool isFinished() const = 0;

        virtual void reset() = 0;

        virtual void setMainFunction(const std::string&) = 0;

        virtual Script* clone() const = 0;

        virtual Variant find(std::string_view path) const = 0;

        virtual OutputStream& writeToStream(OutputStream& stream) const = 0;

        virtual InputStream& readFromStream(InputStream& stream, Lua& lua) = 0;
    };
}