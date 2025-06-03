//
// Created by tony on 05/10/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/ConfigurationElement.h"
#include "core/Atom.h"

#include <string_view>
#include <cstring>

namespace dagbase
{
	//! Find a key in the last component of a path, using simple equality.
	//! \param[in] path : std::string_view The path that should have only one component left.
	//! \param[in] key : const char* The key to check against.
	//! \param[in] value : Value The value to return if the key is in the path.
    template<typename Value>
	dagbase::ConfigurationElement::ValueType findEndpoint(std::string_view path, const char* key, Value value)
	{
		if (path == key)
		{
			return Variant(value);
		}

		return {};
	}

	//! Find a component in a path and forward to the next object in the chain.
	//! \param[in] path : std::string_view The path with multiple components sepaarated by '.'.
	//! \oaram[in] key : const char* The key that might be the first component of the path.
	//! \param[in] obj : Ref The receiver for the call to find().
	//! \note Type Ref must implement find().
	//! \note Use of string_view makes taking a sub-string more efficient than with a std::string
	//! since it avoids memory allocation.
	//! \note We use std::invoke() to handle pointers and references uniformly.
	template<typename Ref>
	auto findInternal(std::string_view path, const char* key, const Ref& obj) ->
		dagbase::ConfigurationElement::ValueType
	{
		auto pos = path.compare(0, std::strlen(key), key);

		if (pos == 0)
		{
			auto dotPos = path.find('.');
			auto subPos = path.find('[');
			if (subPos != std::string_view::npos && subPos < dotPos)
			{
				return std::invoke(&std::remove_pointer_t<Ref>::find, obj, path.substr(subPos));
			}
			if (dotPos < path.length() - 1)
			{
				return std::invoke(&std::remove_pointer_t<Ref>::find, obj, path.substr(dotPos + 1));
			}
		}

		return {};
	}

	//! Find an element in an array type
	//! \param[in] path : string_view The path that might start with a subscript [n] where n is an integer >= 0.
	//! \note Array must implement InnerType and size().
	//! It is typically std::vector which fulfills these requirements.
	//! \note Array::InnerType must implement find().
	//! \note We use std::invoke() to handle pointers and references uniformly.
	//! \note We can avoid an initial string_view::find() because we are only interested in the first element of path.
	template<typename Array>
	dagbase::ConfigurationElement::ValueType findArray(std::string_view path, const Array& obj)
	{
		if (path.length()>1 && path[0]=='[')
		{
			std::size_t firstIndex = 1;
			std::size_t index = 0;
			char* endPtr = nullptr;
			if (firstIndex < path.length())
			{
				index = strtoull(&path[firstIndex], &endPtr, 10);
			}
			if (index < obj.size())
			{
				auto dotPos = path.find('.');
				if (dotPos < path.length() - 1)
				{
					return std::invoke(&std::remove_pointer_t<typename Array::value_type>::find, obj[index], path.substr(dotPos + 1));
				}
			}
		}

		return {};
	}

    template<typename Array>
	dagbase::ConfigurationElement::ValueType findPrimitiveArray(std::string_view path, const Array& obj)
	{
		if (path.length()>1 && path[0]=='[')
		{
			std::size_t firstIndex = 1;
			std::size_t index = 0;
			char* endPtr = nullptr;
			if (firstIndex < path.length())
			{
				index = strtoull(&path[firstIndex], &endPtr, 10);
			}
			if (index < obj.size() && endPtr && *endPtr==']')
			{
                return obj[index];//std::invoke(&std::remove_pointer_t<typename Array::value_type>::find, obj[index], path.substr(dotPos + 1));
			}
		}

		return {};
	}

	template<typename Map>
	ConfigurationElement::ValueType findMap(std::string_view path, const Map& obj)
    {
    	if (!path.empty())
    	{
    		auto dotPos = path.find('.');
    		if (dotPos < path.length() - 1)
    		{
    			std::string key;
    			key = path.substr(0, dotPos);
    			if (auto it=obj.find(key); it!=obj.end())
    				return std::invoke(&std::remove_pointer_t<typename Map::value_type::second_type>::find, it->second, path.substr(dotPos + 1) );
    		}
    	}

    	return {};
    }

	template<typename Map>
	ConfigurationElement::ValueType findMapFromAtom(std::string_view path, const Map& obj)
    {
    	if (!path.empty())
    	{
    		auto dotPos = path.find('.');
    		if (dotPos < path.length() - 1)
    		{
    			std::string key;
    			key = path.substr(0, dotPos);
                Atom atom = Atom::intern(key);
    			if (auto it=obj.find(atom); it!=obj.end())
    				return std::invoke(&std::remove_pointer_t<typename Map::value_type::second_type>::find, it->second, path.substr(dotPos + 1) );
    		}
    	}

    	return {};
    }
}
