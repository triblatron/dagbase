//
// Created by Tony Horrobin on 11/10/2026.
//

#pragma once

namespace dagbase
{
    template<typename Tag>
    class Name
    {
    public:
        static constexpr std::uint32_t INVALID_ID = ~0U;
    public:
        Name() = default;

        explicit Name(std::uint32_t id)
        :
        id(id)
        {
        }

        Name(const Name& other) = default;

        Name& operator=(const Name& other)
        {
            if (this != &other)
            {
                id = other.id;
            }
            return *this;
        }

        Name& operator=(std::int64_t id_)
        {
            this->id = id_;

            return *this;
        }

        Name operator++(int)
        {
            Name old = *this;
            id++;

            return old;
        }

        Name operator+(const Name& op2) const
        {
            auto result = Name(id + op2.id);

            return result;
        }

        bool operator<(const Name& op2) const
        {
            return id<op2.id;
        }

        operator std::uint32_t() const
        {
            return id;
        }

        void configure(dagbase::ConfigurationElement& config)
        {
            id = config.asInteger(INVALID_ID);
        }

        bool valid() const
        {
            return id!=INVALID_ID;
        }

        Variant find(std::string_view path) const
        {
            Variant retval;

            retval = findEndpoint(path, "id", id);
            if (retval.has_value())
                return retval;

            return {};
        }

        OutputStream& writeToStream(OutputStream & str) const
        {
            str.writeUInt32(id);

            return str;
        }

        InputStream& readFromStream(InputStream& str)
        {
            str.readUInt32(&id);

            return str;
        }
    private:
        std::uint32_t id{INVALID_ID};
    };
}
