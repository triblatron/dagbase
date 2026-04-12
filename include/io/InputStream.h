//
// Created by tony on 09/03/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include "Stream.h"
#include <vector>
#include <string>

#include "core/ConfigurationElement.h"

namespace dagbase
{
    class Lua;

    //! Input stream that supports reading arbitrary objects encoded as integer identifiers
    //! Explicitly supports reading Nodes and Ports
    class DAGBASE_API InputStream : public Stream
    {
    public:
        virtual InputStream& readBuf(value_type* buf, std::size_t len) = 0;

        template<typename T>
        InputStream& read(T* value)
        {
            return readBuf(reinterpret_cast<value_type*>(value), sizeof(T));
        }

        //virtual void* readRef(ObjId* id) = 0;

        template<typename T, typename ClassLibrary>
        T* readRef(const char*baseClassName, ClassLibrary& nodeLib, dagbase::Lua& lua)
        {
            ObjId id;
            readUInt32(&id);

            if (id != 0)
            {
                _lastReadId = id;
                if (_lastReadId-1<_ptrLookup.size())
                {
                    return static_cast<T*>(_ptrLookup[_lastReadId-1]);
                }
                return dynamic_cast<T*>(nodeLib.instantiate(baseClassName, *this, lua));
            }
            return nullptr;
        }

        Ref readRef(ObjId* id)
        {
            readUInt32(id);

            if (*id != 0)
            {
                _lastReadId = *id;
                if (_lastReadId-1<_ptrLookup.size())
                {
                    return (_ptrLookup[_lastReadId-1]);
                }
                return nullptr;
            }
            return nullptr;
        }

        template<typename T>
        T* readRef(ObjId* id)
        {
            readUInt32(id);

            if (*id != 0)
            {
                _lastReadId = *id;
                if (_lastReadId-1<_ptrLookup.size())
                {
                    return static_cast<T*>(_ptrLookup[_lastReadId-1]);
                }
                else
                {
                    return new T(*this);
                }
            }
            else
            {
                return nullptr;
            }
        }

        template<typename T>
        T* readRef(ObjId* id, Lua& lua)
        {
            readUInt32(id);

            if (*id != 0)
            {
                _lastReadId = *id;
                if (_lastReadId-1<_ptrLookup.size())
                {
                    return static_cast<T*>(_ptrLookup[_lastReadId-1]);
                }
                else
                {
                    return new T(*this, lua);
                }
            }
            else
            {
                return nullptr;
            }
        }

        // template<typename BaseClass, typename ClassLibrary>
        // BaseClass* readPort(const char* baseClassName, ClassLibrary& nodeLib)
        // {
        //     ObjId id{0};
        //     Ref ref = readRef(&id);
        //     if (id != 0)
        //     {
        //         if (ref!=nullptr)
        //         {
        //             return static_cast<BaseClass*>(ref);
        //         }
        //         else
        //         {
        //             return nodeLib.instantiate(baseClassName, *this);
        //         }
        //     }
        //     else
        //     {
        //         return nullptr;
        //     }
        //}

        void addObj(void* ref)
        {
            if (_lastReadId>_ptrLookup.size())
            {
                _ptrLookup.resize(_lastReadId);
            }
            _ptrLookup[_lastReadId-1] = ref;
        }

        virtual InputStream& readString(std::string* value, bool quoted) = 0;

        virtual InputStream& readHeader(std::string* className) = 0;

        virtual InputStream& readField(std::string* fieldName) = 0;

        virtual InputStream& readUInt8(std::uint8_t* value) = 0;

        virtual InputStream& readInt8(std::int8_t* value) = 0;

        virtual InputStream& readUInt16(std::uint16_t* value) = 0;

        virtual InputStream& readInt16(std::int16_t* value) = 0;

        virtual InputStream& readUInt32(std::uint32_t* value) = 0;

        virtual InputStream& readInt32(std::int32_t* value) = 0;

        virtual InputStream& readUInt64(std::uint64_t* value) = 0;

        virtual InputStream& readInt64(int64_t* value) = 0;

        virtual InputStream& readBool(bool* value) = 0;

        virtual InputStream& readFloat(float* value) = 0;

        virtual InputStream& readDouble(double* value) = 0;

        virtual InputStream& read(Lua& lua, dagbase::Variant* value) = 0;

        virtual InputStream& readFooter() = 0;
    private:
        using PtrArray = std::vector<Ref>;
        PtrArray _ptrLookup;
        ObjId _lastReadId{0};
    };
}
