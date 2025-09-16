//
// Created by tony on 03/08/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"

extern "C" {
#include <lua.h>
}

#include <string>
#include <variant>
#include <cstdint>
#include <vector>
#include <optional>
#include <functional>
#include <iosfwd>

struct lua_State;

namespace dagbase
{
    class Atom;
    class Lua;

    class DAGBASE_API ConfigurationElement
    {
    public:
//        enum Type
//        {
//            TYPE_BOOL,
//            TYPE_INTEGER,
//            TYPE_DOUBLE,
//            TYPE_STRING
//        };
        using ValueType = Variant;//std::optional<std::variant<bool, std::int64_t, double, std::string>>;
        enum RelOp
        {
            RELOP_UNKNOWN,
            RELOP_EQ,
            RELOP_NE,
            RELOP_LT,
            RELOP_LE,
            RELOP_GT,
            RELOP_GE
        };
    public:
        explicit ConfigurationElement(std::string name);
		
        ConfigurationElement(std::string name, ValueType value)
        :
        _name(std::move(name)),
        _value(value)
        {
            // Do nothing.
        }

        ConfigurationElement(std::int64_t index, ValueType value)
        :
        _index(index),
        _value(value)
        {
            // Do nothing.
        }

        ~ConfigurationElement();

        static ConfigurationElement *fromString(Lua &lua, const char *str);

        static ConfigurationElement *fromFile(Lua &lua, const char *filename);

		void setIndex(std::int64_t index)
		{
			_index = index;
		}
		
        const std::string& name() const
        {
            return _name;
        }

        ValueType value() const
        {
            return _value;
        }

        void addChild(ConfigurationElement* child)
        {
            if (child != nullptr)
            {
                child->setParent(this);
                _children.emplace_back(child);
            }
        }

        ConfigurationElement* parent()
        {
            return _parent;
        }

        std::int64_t asInteger(int64_t defaultValue=0) const
        {
            return _value.asInteger(defaultValue);
//            if (_value.has_value() && _value->index() == TYPE_INTEGER)
//            {
//                return std::get<TYPE_INTEGER>(_value.value());
//            }
//            else
//            {
//                return defaultValue;
//            }
        }

        double asDouble(double defaultValue=0.0) const
        {
            return _value.asDouble(defaultValue);
//            if (_value.has_value() && _value->index()==TYPE_DOUBLE)
//            {
//                return std::get<TYPE_DOUBLE>(_value.value());
//            }
//            else
//            {
//                return defaultValue;
//            }
        }

        bool asBool(bool defaultValue=false) const
        {
            return _value.asBool(defaultValue);
//            if (_value.has_value() && _value->index()==TYPE_BOOL)
//            {
//                return std::get<TYPE_BOOL>(_value.value());
//            }
//            else
//            {
//                return defaultValue;
//            }
        }

        std::string asString(std::string defaultValue="") const
        {
            return _value.asString(defaultValue);
//            if (_value.has_value() && _value->index()==TYPE_STRING)
//            {
//                return std::get<TYPE_STRING>(_value.value());
//            }
//            else
//            {
//                return defaultValue;
//            }
        }

        template <typename T>
        T as() const
		{
            return _value.as<T>();
//		    if (_value.has_value())
//		    {
//		        return std::get<T>(_value.value());
//		    }
//		    else
//		    {
//		        return T();
//		    }
		}

        ConfigurationElement* findElement(std::string_view path);
        
        std::size_t numChildren() const
        {
            return _children.size();
        }

        ConfigurationElement* child(std::size_t index)
        {
            return _children[index];
        }

        const ConfigurationElement* child(std::size_t index) const
        {
            return _children[index];
        }

        void eachChild(std::function<bool (ConfigurationElement&)> f);

        static void readConfig(ConfigurationElement& config, const char* name, double* value);
        static void readConfig(ConfigurationElement& config, const char* name, float* value);
        static void readConfig(ConfigurationElement& config, const char* name, bool* value);
        static void readConfig(ConfigurationElement& config, const char* name, std::string* value);
        static void readConfig(ConfigurationElement& config, const char* name, std::int32_t* value);
        static void readConfig(ConfigurationElement& config, const char* name, std::uint32_t* value);
        static void readConfig(ConfigurationElement& config, const char* name, Atom* value);
        template <typename Enum>
        static void readConfig(ConfigurationElement& config, const char* name, std::function<Enum(const char*)> parseEnum, Enum* value)
        {
            if (value)
                if (auto element=config.findElement(name); element)
                {
                    *value = parseEnum(element->asString().c_str());
                }
        }
        template<typename Obj>
        static void readConfig(ConfigurationElement& config, const char* name, Obj* value)
        {
            if (value)
                if (auto element=config.findElement(name); element)
                {
                    value->configure(*element);
                }
        }
        template<typename Associative>
        static void readConfigSet(ConfigurationElement& config, const char* name, Associative* value)
        {
            if (value)
                if (auto element=config.findElement(name); element)
                {
                    value->reserve(element->numChildren());
                    element->eachChild([&value](ConfigurationElement& child) {
                        typename Associative::value_type item;

                        item.configure(child);

                        value->emplace(item);

                        return true;
                    });
                }
        }

        template<typename Map>
        static void readConfigMap(ConfigurationElement& config, const char* name, Map* value)
        {
            if (value)
                if (auto element=config.findElement(name); element)
                {
                    value->reserve(element->numChildren());
                    element->eachChild([&value](ConfigurationElement& child) {
                        typename Map::key_type key;

                        key.configure(child);

                        typename Map::value_type::second_type mapped;

                        mapped.configure(child);

                        value->emplace(key, mapped);

                        return true;
                    });
                }
        }
    private:
        void setParent(ConfigurationElement* parent)
        {
            if (parent!=nullptr)
            {
                _parent = parent;
            }
        }
        ConfigurationElement* findInChildren(std::string_view path);
        ConfigurationElement* findInArray(size_t index, std::string_view path);
        ValueType _value;
        using Children = std::vector<ConfigurationElement*>;
        Children _children;
        std::string _name;
        std::int64_t _index{0};
        ConfigurationElement* _parent{nullptr};
        static ConfigurationElement* buildTree(Lua& lua);
    };

}

std::ostream DAGBASE_API & operator<<(std::ostream& str, dagbase::ConfigurationElement::ValueType value);
