//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/DagBaseExport.h"

#include "core/Variant.h"

namespace dagbase
{
    class DAGBASE_API PropertyFormat
    {
    public:
        virtual ~PropertyFormat() = default;

        virtual std::string name() const = 0;

        virtual Variant parse(const Variant& value) = 0;

        virtual bool validate(const Variant& value) const = 0;

        virtual Variant toSceneGraph() const = 0;
    };
}
