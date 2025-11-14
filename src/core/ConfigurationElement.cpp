//
// Created by tony on 03/08/24.
//

#include "config/config.h"

#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"
#include "core/Atom.h"
#include "util/enums.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

#include <stack>
#include <cstdlib>

namespace dagbase
{
    ConfigurationElement::ConfigurationElement(std::string name)
    :
    _name(std::move(name))
    {
        // Do nothing.
    }
	
    ConfigurationElement *ConfigurationElement::fromString(Lua &lua, const char *str)
    {
        if (str != nullptr)
        {
            lua.eval(str);
        }

        return buildTree(lua);
    }

    ConfigurationElement * ConfigurationElement::fromFile(Lua &lua, const char *filename)
    {
        if (filename != nullptr)
        {
            lua.execute(filename);
        }

        return buildTree(lua);
    }

    ConfigurationElement* ConfigurationElement::findInArray(size_t startIndex, std::string_view path)
    {
        path = path.substr(startIndex);
        size_t index=0;
        for (index = 0; index<path.length() && isdigit(path[index]); ++index);
        char* end = nullptr;
        size_t childIndex = std::strtoul(path.data(), &end, 10);
        if (childIndex<_children.size() && index<path.size() && path[index] == ']')
        {
			auto child = _children[childIndex];
            ++index;
            if (index < path.length()-1)
            {
                path = path.substr(index+1);
                return child->findInChildren(path);
            }
            else
            {
                return _children[childIndex];
            }
        }
        return nullptr;
    }

    ConfigurationElement* ConfigurationElement::findElement(std::string_view path)
    {
        if (path[0] == '$')
        {
            auto root = this;
            while (root != nullptr && root->parent() != nullptr)
            {
                root = root->parent();
            }

            if (root != nullptr && path.find("$.")!=std::string::npos)
            {
                path = path.substr(2);

                return root->findInChildren(path);
            }
            else if (root != nullptr && path.length()>=4 && path[1] == '[')
            {
                path = path.substr(2);
                return findInArray(0, path);
            }
            else if (path == "$")
            {
                return root;
            }
        }
        else
        {
            return findInChildren(path);
        }
        return nullptr;
    }

    ConfigurationElement *ConfigurationElement::findInChildren(std::string_view path)
    {
        if (path.empty())
        {
            return this;
        }

        auto dotPos = path.find('.');
        // Find position of subscript.
        auto subPos = path.find('[');
        if (subPos!=std::string::npos && subPos < dotPos)
        {
            if (subPos == 0)
            {
                return findInArray(1, path);
            }
            else
            {
                size_t index=0;
                for (index=0; index<path.length() && path[index] != ']';++index);
                if (index<path.length() && path[index]==']')
                {
                    auto first = path.substr(0,subPos);
                    auto  rest = path.substr(subPos);

                    for (auto child : _children)
                    {
                        if (child->name() == first)
                        {
                            return child->findInChildren(rest);
                        }

                    }
                }
            }
        }
        else if (dotPos != std::string::npos)
        {
            if (dotPos == 0 && path.length()>1)
            {
                size_t index = 0;
                for (index=0; index < path.length() && path[index] != '.' && path[index] != '['; ++index);
                auto  first = path.substr(1, dotPos);
                auto  rest = path.substr(dotPos, index-dotPos);

                for (auto child : _children)
                {
                    if (child->_name == first)
                    {
                        return child->findInChildren(rest);
                    }
                }
            }
            else
            {
                auto first = path.substr(0,dotPos);
                if (path.length()>dotPos)
                {
                    auto rest = path.substr(dotPos+1);
                    for (auto child : _children)
                    {
                        if (child->_name == first)
                        {
                            return child->findInChildren(rest);
                        }
                    }
                }
            }
        }
        else
        {
            for (auto child : _children)
            {
                if (child->_name == path)
                {
                    return child;
                }
            }
        }

        return nullptr;
    }

    ConfigurationElement *ConfigurationElement::buildTree(Lua& lua)
    {
        using ParentStack = std::stack<ConfigurationElement*>;

        if (lua.tableExists("root"))
        {
            ParentStack parentStack;
            auto parent = new ConfigurationElement("root");
            parentStack.push(parent);
            auto rootTable = lua.tableForName("root");

            TableTraversal trav(lua.get());

            trav([&parentStack](lua_State* lua, size_t level) {
                ConfigurationElement* child = nullptr;

                while (parentStack.size() - 1 > level)
                {
                    parentStack.pop();
                }

                if (lua_isinteger(lua, -2))
                {
                    std::int64_t index = lua_tointeger(lua, -2);
                    std::string name = std::string('[' + std::to_string(index) + ']');
                    
                    if (lua_isinteger(lua, -1))
                    {
                        child = new ConfigurationElement(index, Variant(std::int64_t(lua_tointeger(lua, -1))));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_isnumber(lua, -1))
                    {
                        child = new ConfigurationElement(index, Variant(lua_tonumber(lua, -1)));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_isboolean(lua, -1))
                    {
                        child = new ConfigurationElement(index, Variant(bool(lua_toboolean(lua, -1))));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_isstring(lua, -1))
                    {
                        child = new ConfigurationElement(index, std::string(lua_tostring(lua, -1)));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_isfunction(lua, -1))
                    {
                        child = new ConfigurationElement(index, Variant(new Function(lua)));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_istable(lua, -1))
                    {
                        child = new ConfigurationElement(name);
                        child->setIndex(index);
                        parentStack.top()->addChild(child);
                        parentStack.push(child);
					}
                }
                else if (lua_isstring(lua, -2))
                {
                    std::string name = lua_tostring(lua, -2);
                    if (lua_isinteger(lua, -1))
                    {
                        child = new ConfigurationElement(name, Variant(std::int64_t(lua_tointeger(lua, -1))));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_isnumber(lua, -1))
                    {
                        child = new ConfigurationElement(name, Variant(lua_tonumber(lua, -1)));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_isboolean(lua, -1))
                    {
                        child = new ConfigurationElement(name, Variant(bool(lua_toboolean(lua, -1))));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_isstring(lua, -1))
                    {
                        Vec2 vec;
                        bool isVec2 = Vec2::parse(lua_tostring(lua, -1), &vec);
                        if (isVec2)
                        {
                            child = new ConfigurationElement(name, Variant(vec));
                        }
                        else
                            child = new ConfigurationElement(name, std::string(lua_tostring(lua, -1)));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_isfunction(lua, -1))
                    {
                        auto* func = new Function(lua);
                        //lua_pop(lua, 1);
                        child = new ConfigurationElement(name, Variant(func));
                        parentStack.top()->addChild(child);
                    }
                    else if (lua_istable(lua, -1))
                    {
                        child = new ConfigurationElement(name);
                        parentStack.top()->addChild(child);
                        parentStack.push(child);
                    }
                }
                return 0;
            });
            return parent;
        }
        return nullptr;
    }

    ConfigurationElement::~ConfigurationElement()
    {
        for (auto child : _children)
        {
            delete child;
        }
    }
    
    void ConfigurationElement::eachChild(std::function<bool (ConfigurationElement&)> f)
    {
		for (auto child : _children)
		{
			if (f(*child) == false)
			{
				break;
			}
		}
	}

    void ConfigurationElement::readConfig(ConfigurationElement& config, const char* name, float* value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                *value = static_cast<float>(element->asDouble());
            }
    }

    void ConfigurationElement::readConfig(ConfigurationElement& config, const char* name,
                                          std::string* value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                *value = element->asString();
            }
    }

    void ConfigurationElement::readConfig(ConfigurationElement& config, const char* name,
                                          std::int32_t* value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                *value = element->asInteger();
            }
    }

    void ConfigurationElement::readConfig(ConfigurationElement &config, const char *name, Atom *value)
    {
        if (value)
            if (auto element = config.findElement(name); element)
            {
                *value = Atom::intern(element->asString());
            }
    }

    void ConfigurationElement::readConfig(ConfigurationElement &config, const char *name, std::uint32_t *value)
    {
        if (value)
            if (auto element = config.findElement(name); element)
            {
                *value = std::uint32_t(element->asInteger());
            }
    }

    void ConfigurationElement::readConfig(ConfigurationElement &config, const char *name, double *value)
    {
        if (value)
            if (auto element = config.findElement(name); element)
            {
                *value = element->asDouble();
            }
    }

    void ConfigurationElement::readConfig(ConfigurationElement &config, const char *name, bool *value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                *value = element->asBool();
            }
    }

    const char *ConfigurationElement::relOpToString(ConfigurationElement::RelOp value)
    {
        switch (value)
        {
            ENUM_NAME(RELOP_UNKNOWN)
            ENUM_NAME(RELOP_EQ)
            ENUM_NAME(RELOP_NE)
            ENUM_NAME(RELOP_LT)
            ENUM_NAME(RELOP_LE)
            ENUM_NAME(RELOP_GT)
            ENUM_NAME(RELOP_GE)
        }
        return "<error>";
    }

    ConfigurationElement::RelOp ConfigurationElement::parseRelOp(const char *str)
    {
        TEST_ENUM(RELOP_UNKNOWN, str)
        TEST_ENUM(RELOP_EQ, str)
        TEST_ENUM(RELOP_NE, str)
        TEST_ENUM(RELOP_LT, str)
        TEST_ENUM(RELOP_LE, str)
        TEST_ENUM(RELOP_GT, str)
        TEST_ENUM(RELOP_GE, str)

        return RELOP_UNKNOWN;
    }

    bool ConfigurationElement::operator==(const ConfigurationElement &other) const
    {
        if (_name != other._name)
            return false;

        if (_index != other._index)
            return false;

        if (_value != other._value)
            return false;

        if (_children.size() != other._children.size())
            return false;

        for (std::size_t childIndex=0; childIndex<_children.size(); ++childIndex)
        {
            if (!(*_children[childIndex] == *other._children[childIndex]))
            {
                return false;
            }
        }

        return true;
    }

    OutputStream &ConfigurationElement::write(OutputStream &ostr) const
    {
        ostr.writeHeader("ConfigurationElement");
        ostr.writeField("name");
        ostr.writeString(_name, false);
        ostr.writeField("index");
        ostr.writeInt64(_index);
        ostr.writeField("value");
        _value.write(ostr);
        ostr.writeField("parent");
        if (ostr.writeRef(_parent))
        {
            _parent->write(ostr);
        }
        ostr.writeField("numChildren");
        ostr.writeUInt32(_children.size());
        ostr.writeHeader("childArray");
        for (std::size_t childIndex=0; childIndex<_children.size(); ++childIndex)
        {
            if (ostr.writeRef(_children[childIndex]))
            {
                _children[childIndex]->write(ostr);
            }
        }
        ostr.writeFooter();
        ostr.writeFooter();
        return ostr;
    }

    ConfigurationElement::ConfigurationElement(InputStream &str, Lua& lua)
    {
        str.addObj(this);
        std::string className;
        str.readHeader(&className);

        std::string fieldName;
        str.readField(&fieldName);
        str.readString(&_name, false);
        str.readField(&fieldName);
        str.readInt64(&_index);
        str.readField(&fieldName);
        _value.read(str, &lua);
        InputStream::ObjId parentId{};
        str.readField(&fieldName);
        _parent = str.readRef<ConfigurationElement>(&parentId, lua);
        std::uint32_t numChildren{0};
        str.readField(&fieldName);
        str.readUInt32(&numChildren);
        _children.resize(numChildren);
        str.readHeader(&className);
        for (std::size_t childIndex = 0; childIndex < _children.size(); ++childIndex)
        {
            InputStream::ObjId id{};

            _children[childIndex] = str.readRef<ConfigurationElement>(&id, lua);
        }
        str.readFooter();
        str.readFooter();
    }

    ConfigurationElement *ConfigurationElement::fromStream(Lua& lua, InputStream &str)
    {
        InputStream::ObjId id{};
        return str.readRef<ConfigurationElement>(&id, lua);
    }
}
