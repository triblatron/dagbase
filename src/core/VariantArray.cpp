//
// Created by Tony Horrobin on 27/09/2025.
//

#include "config/config.h"

#include "core/VariantArray.h"

namespace dagbase
{
    void VariantArray::configure(dagbase::ConfigurationElement &config)
    {
        Variant::Index index;
        ConfigurationElement::readConfig<Variant::Index>(config, "index", &Variant::parseIndex, &index);
        if (auto element = config.findElement("items"); element)
        {
            switch (index)
            {
                case Variant::TYPE_INTEGER:
                {
                    ArrayOfInt64 a;
                    a.configure(*element);
                    _value.emplace<decltype(a)>(a);
                    break;
                }
                case Variant::TYPE_DOUBLE:
                {
                    ArrayOfDouble a;
                    a.configure(*element);
                    _value.emplace<decltype(a)>(a);
                    break;
                }
                case Variant::TYPE_BOOL:
                {
                    ArrayOfBool a;
                    a.configure(*element);
                    _value.emplace<decltype(a)>(a);
                    break;
                }
                case Variant::TYPE_STRING:
                {
                    ArrayOfString a;
                    a.configure(*element);
                    _value.emplace<decltype(a)>(a);
                    break;
                }
                case Variant::TYPE_COLOUR:
                {
                    ArrayOfColour a;
                    a.configure(*element);
                    _value.emplace<decltype(a)>(a);
                    break;
                }
                case Variant::TYPE_UINT:
                {
                    ArrayOfUInt32 a;
                    a.configure(*element);
                    _value.emplace<decltype(a)>(a);
                    break;
                }
                default:
                    break;
            }
        }
    }

    Variant VariantArray::find(std::string_view path) const
    {
        Variant retval;

        switch (_value.index())
        {
            case Variant::TYPE_INTEGER:
            {
                auto& a = std::get<ArrayOfInt64>(_value);

                retval = findInternal(path, "items", a);
                if (retval.has_value())
                    return retval;
                break;
            }
            case Variant::TYPE_DOUBLE:
            {
                auto& a = std::get<ArrayOfDouble>(_value);

                retval = findInternal(path, "items", a);
                if (retval.has_value())
                    return retval;
                break;
            }
            case Variant::TYPE_BOOL:
            {
                auto& a = std::get<ArrayOfBool>(_value);

                retval = findInternal(path, "items", a);
                if (retval.has_value())
                    return retval;
                break;
            }
            case Variant::TYPE_STRING:
            {
                auto& a = std::get<ArrayOfString>(_value);

                retval = findInternal(path, "items", a);
                if (retval.has_value())
                    return retval;
                break;
            }
            case Variant::TYPE_COLOUR:
            {
                auto& a = std::get<ArrayOfColour >(_value);

                retval = findInternal(path, "items", a);
                if (retval.has_value())
                    return retval;
                break;
            }
            case Variant::TYPE_UINT:
            {
                auto& a = std::get<ArrayOfUInt32>(_value);

                retval = findInternal(path, "items", a);
                if (retval.has_value())
                    return retval;
                break;
            }
            default:
                break;
        }
        return {};
    }

    void VariantArray::reserve(std::size_t n)
    {
        switch (_value.index())
        {
            case Variant::TYPE_INTEGER:
                std::get<ArrayOfInt64>(_value).a.reserve(n);
                break;
            case Variant::TYPE_DOUBLE:
                std::get<ArrayOfDouble>(_value).a.reserve(n);
                break;
            case Variant::TYPE_STRING:
                std::get<ArrayOfString>(_value).a.reserve(n);
                break;
            case Variant::TYPE_BOOL:
                std::get<ArrayOfBool>(_value).a.reserve(n);
                break;
            case Variant::TYPE_COLOUR:
                std::get<ArrayOfColour>(_value).a.reserve(n);
                break;
            case Variant::TYPE_VEC2:
                std::get<ArrayOfVec2>(_value).a.reserve(n);
                break;
            case Variant::TYPE_UINT:
                std::get<ArrayOfUInt32>(_value).a.reserve(n);
                break;
        }
    }
}