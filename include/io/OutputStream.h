//
// Created by tony on 07/03/24.
//

#pragma  once

#include "config/DagBaseExport.h"

#include "io/Stream.h"

#include <map>
#include <string>

#include "core/ConfigurationElement.h"

namespace dagbase
{
    //! \class OutputStream
    //! Supports writing primitive types, strings and pointers.
    //! \note Pointers are encoded as integer identifiers allocated in an LUT as they are encountered.
    class DAGBASE_API OutputStream : public Stream
    {
    public:
        //! Write a buffer of bytes to the stream.
        virtual OutputStream& writeBuf(const value_type* buf, std::size_t len) = 0;

        //! Type-safe version of writeRef() that takes a typed pointer.
        //! \retval true if the pointer has not yet been encountered.
        //! \retval false otherwise.
        //! \note The pointer may be nullptr and the method returns false.
        template<typename T>
        bool writeRef(T* ptr)
        {
            if (ptr != nullptr)
            {
                if (auto it=_idLookup.find(ptr); it!=_idLookup.end())
                {
                    writeUInt32(it->second);
                    return false;
                }
                else
                {
                    std::size_t id = _idLookup.size() + 1;
                    _idLookup.insert(PtrToIdMap::value_type(ptr, id));
                    writeUInt32(id);
                    return true;
                }
            }
            else
            {
                writeUInt32(ObjId(0));
                return false;
            }
        }

        //! Type-safe writing of a primitive value.
        //! \note Delegates to writeBuf().
        template<typename T>
        OutputStream& write(T value)
        {
            return writeBuf(reinterpret_cast<const value_type*>(&value), sizeof(T));
        }

        virtual OutputStream& write(Variant value)
        {
            if (value.has_value())
                return write(value.value());

            return *this;
        }

        virtual OutputStream& writeUInt32(std::uint32_t value) = 0;

        //! Write a string which requires special encoding because it is not a primitive type.
        virtual OutputStream& writeString(std::string const& value, bool quoted);

        virtual OutputStream& writeDouble(double value) = 0;

        virtual OutputStream& writeInt64(std::int64_t value) = 0;

        virtual OutputStream& writeBool(bool value) = 0;

        virtual OutputStream& writeHeader(std::string_view className)
        {
            // Do nothing.
            return *this;
        }

        virtual OutputStream& writeFooter()
        {
            // Do nothing.
            return *this;
        }

        virtual OutputStream& writeField(std::string_view name)
        {
            // Do nothing.
            return *this;
        }
    private:
        typedef std::map<void*, ObjId> PtrToIdMap;
        PtrToIdMap _idLookup;
    };
}
