#pragma once

#include "config/DagBaseExport.h"

#include <vector>
#include <cstdlib>

namespace dagbase
{
    class MetaCoroutine;
    class MetaField;
    class MetaOperation;

#define DAGBASE_PROPERTY(type, name, member, initialValue, setter, getter) \
    public: \
        void setter(type value)\
        {\
            member = value;\
        }                                                    \
                                                             \
        type getter() const                                  \
        {                                                    \
            return member;                                   \
        }                                                    \
    private:                                                 \
        type member{initialValue};

    class DAGBASE_API MetaClass
    {
    public:
        typedef std::vector<MetaField *> MetaFieldArray;
        typedef std::vector<MetaOperation *> MetaOperationArray;
        typedef std::vector<MetaCoroutine *> MetaCoroutineArray;
    public:
        void addField( MetaField * field )
        {
            _fields.emplace_back(field);
        }

        MetaField * fieldForIndex(size_t i)
        {
            return _fields[i];
        }

        void addOperation( MetaOperation * op )
        {
            _ops.emplace_back(op);
        }

        MetaOperation * opForIndex(size_t i)
        {
            return _ops[i];
        }

        void addCoroutine( MetaCoroutine * coroutine )
        {
            _coroutines.emplace_back(coroutine);
        }

        MetaCoroutine * coroutineForIndex(size_t i)
        {
            return _coroutines[i];
        }

        //! Expects the arguments to be on the stack.
        void invoke( size_t index );
    private:
        MetaFieldArray _fields;
        MetaOperationArray _ops;
        MetaCoroutineArray _coroutines;
    };
}
