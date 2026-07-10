#pragma once

#include "core/NodeLibrary.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"
#include "Transfer.h"
#include "core/Types.h"
#include "util/enums.h"

#include <atomic>
#include <cassert>
#include <iostream>
#include <type_traits>

namespace dagbase
{
    class DebugPrinter;

	template<typename T>
	class TypedPort : public dagbase::Port
	{
	public:
		static_assert(std::is_convertible_v<T, std::string> || std::is_convertible_v<T, std::int64_t> || std::is_convertible_v<T, bool> || std::is_convertible_v<T, double>);
        TypedPort(dagbase::PortID id, std::string name, dagbase::PortType::Type type, dagbase::PortDirection::Direction dir, T value, dagbase::Node* parent = nullptr, std::uint32_t flags=0x0)
        :
        Port(id, parent, std::move(name), type, dir),
        _value(value)
        {
            setOwnMetaPort(true);
        }

        TypedPort(dagbase::PortID id, dagbase::Node* parent, std::string name, PortType::Type type, PortDirection::Direction direction, T value, PortFlags flags=FLAGS_NONE)
			:
			Port(id, parent, name, type, direction, flags),
			_value(value)
		{
			// Do nothing.
		}

        TypedPort(const TypedPort& other, dagbase::CloningFacility& facility, dagbase::CopyOp copyOp, dagbase::KeyGenerator* keyGen)
        :
        Port(other, facility, copyOp, keyGen)
        {
            _value = other._value;
        }

        explicit TypedPort(dagbase::InputStream& str, dagbase::NodeLibrary& nodeLib, dagbase::Lua& lua)
        {
        	std::string className;
        	std::string fieldName;
        	str.readHeader(&className);
        	Port::readFromStream(str, nodeLib, lua);
        	str.readField(&fieldName);
        	dagbase::ConfigurationElement::ValueType configValue(_value);
            str.read(lua, &configValue);
        	if (configValue.has_value())
        	{
        		_value = std::get<T>(configValue.value().value());
        	}
        	str.readFooter();
        }

        void edit(ImGuiContext* context) override
        {
            editType(std::to_string(id()).c_str(), &_value);
        }

        dagbase::OutputStream& writeToStream(dagbase::OutputStream& str, dagbase::NodeLibrary& nodeLib, dagbase::Lua& lua) const override
        {
            std::string className;

            switch(type())
            {
                case dagbase::PortType::TYPE_INT64:
                    className = "TypedPort<int64_t>";
                    break;
                case dagbase::PortType::TYPE_DOUBLE:
                    className = "TypedPort<double>";
                    break;
                case dagbase::PortType::TYPE_STRING:
                    className = "TypedPort<string>";
                    break;
                case dagbase::PortType::TYPE_BOOL:
                    className = "TypedPort<bool>";
                    break;
                default:
                    assert(false);
                    break;
            }
        	str.writeField("className");
        	str.writeString(className, true);
        	str.writeHeader(className);
            Port::writeToStream(str, nodeLib, lua);
        	str.writeField("value");
        	str.write(dagbase::ConfigurationElement::ValueType(_value));
        	str.writeFooter();

            return str;
        }

        TypedPort* clone(dagbase::CloningFacility& facility, dagbase::CopyOp copyOp, dagbase::KeyGenerator* keyGen) const override
        {
            return new TypedPort(*this, facility, copyOp, keyGen);
        }

		void setValue(T value)
		{
			_value = value;
		}

		T value() const
		{
			return _value;
		}

        dagbase::Transfer* connectTo(dagbase::Port& dest) override
        {
			if (dir() == dagbase::PortDirection::DIR_OUT && dest.dir() == dagbase::PortDirection::DIR_IN && isCompatibleWith(dest))
			{
				auto transfer = new dagbase::TypedTransfer(&_value);
				dest.setDestination(transfer);

				addOutgoingConnection(&dest);
				dest.addIncomingConnection(this);

				return transfer;
			}

			return nullptr;
        }

		dagbase::Transfer* setDestination(dagbase::Transfer* transfer) override
		{
			auto typedTransfer = dynamic_cast<dagbase::TypedTransfer<T>*>(transfer);

			if (typedTransfer != nullptr)
			{
				typedTransfer->setDest(&_value);
			}

			return transfer;
		}

        void accept(dagbase::ValueVisitor& visitor) const override
        {
            visitor.setValue(_value);
        }

        void accept(dagbase::SetValueVisitor& visitor) override
        {
            _value = visitor.value().operator T();
        }

        [[nodiscard]]bool equals(const Port& other) const override
        {
            if (!Port::operator==(other))
            {
                return false;
            }

            const auto & typed = dynamic_cast<const TypedPort<T>&>(other);

            if (_value != typed._value)
            {
                return false;
            }

            return true;
        }

        void debug(dagbase::DebugPrinter& printer) const override;

        [[nodiscard]]const char* className() const override
        {
            return PortType::className(type());
        }

        std::ostream& toLua(std::ostream& str) override;

		Variant find(std::string_view path) const override
		{
			Variant retval = Port::find(path);

			if (retval.has_value())
				return retval;

			retval = findEndpoint(path, "value", _value);
			if (retval.has_value())
				return retval;

			return {};
		}

	private:
		T _value;
	};

    template<typename T>
    std::ostream &TypedPort<T>::toLua(std::ostream &str)
    {
        Port::toLua(str);
        if (type() == PortType::TYPE_STRING)
            str << "value = \"" << _value << "\",";
        else
            str << std::boolalpha << "value = " << _value;

        return str;
    }

    template<typename T>
    void TypedPort<T>::debug(dagbase::DebugPrinter& printer) const
    {
        Port::debug(printer);
    }
}
