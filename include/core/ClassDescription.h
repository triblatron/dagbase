//
// Created by Tony Horrobin on 06/04/2026.
//

#pragma once

#include "config/DagBaseExport.h"

namespace dagbase
{
    class Atom;

    //! Interface to allow an object to describe itself in terms of its fields
    class DAGBASE_API ClassDescription
    {
    public:
        virtual ~ClassDescription() = default;

        virtual void addInt8(const Atom& name) = 0;
        virtual void addInt16(const Atom& name) = 0;
        virtual void addInt32(const Atom& name) = 0;
        virtual void addInt64(const Atom& name) = 0;
        virtual void addFloat(const Atom& name) = 0;
        virtual void addDouble(const Atom& name) = 0;
        virtual void addString(const Atom& name) = 0;
        virtual void addBool(const Atom& name) = 0;
        virtual void addUint8(const Atom& name) = 0;
        virtual void addUint16(const Atom& name) = 0;
        virtual void addUint32(const Atom& name) = 0;
        virtual void addUint64(const Atom& name) = 0;
    };
}