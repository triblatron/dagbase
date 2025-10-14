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
        T* readRef(const char*baseClassName, ClassLibrary& nodeLib)
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
                else
                {
                    return dynamic_cast<T*>(nodeLib.instantiate(baseClassName, *this));
                }
            }
            else
            {
                return nullptr;
            }
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
                else
                {
                    return nullptr;
                }
            }
            else
            {
                return nullptr;
            }
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

        virtual InputStream& readString(std::string* value, bool quoted);

        virtual InputStream& readHeader(std::string* className)
        {
            // Do nothing.
            return *this;
        }

        virtual InputStream& readField(std::string* fieldName)
        {
            // Do nothing.
            return *this;
        }

        virtual InputStream& readUInt8(std::uint8_t* value)
        {
            return read(value);
        }

        virtual InputStream& readUInt32(std::uint32_t* value)
        {
            return read(value);
        }

        virtual InputStream& readInt64(int64_t* value)
        {
            return read(value);
        }

        virtual InputStream& readBool(bool* value)
        {
            return read(value);
        }

        virtual InputStream& readDouble(double* value)
        {
            return read(value);
        }

        virtual InputStream& read(dagbase::Variant* value)
        {
            return *this;
        }

        virtual InputStream& readFooter()
        {
            // Do nothing.
            return *this;
        }
    private:
        using PtrArray = std::vector<Ref>;
        PtrArray _ptrLookup;
        ObjId _lastReadId{0};
    };
}
