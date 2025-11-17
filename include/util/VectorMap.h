#pragma once

#include "core/Variant.h"
#include "util/Searchable.h"
#include "util/Less.h"

#include <vector>
#include <utility>
#include <algorithm>

namespace dagbase
{
	template <typename Key, typename Value, typename Compare=Less<Key,Value>, typename Equals=std::equal_to<Key>>
	class VectorMap
	{
	public:
		typedef std::pair<Key, Value> value_type;
        typedef Key key_type;
        typedef Value mapped_type;
		typedef std::vector<value_type> container;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
        typedef typename container::size_type size_type;
	public:
		void reserve(typename container::size_type n)
		{
			_map.reserve(n);
		}
		
		void resize(typename container::size_type n)
		{
			_map.resize(n);
		}

		std::pair<iterator, bool> insert(const value_type& value)
		{
			auto it = std::lower_bound(_map.begin(), _map.end(), value_type(value.first,Value()), _cmp);
			if (it != _map.end())
			{
				auto equalKey = it->first == value.first;
				
				if (!equalKey)
				{
					auto d = std::distance(_map.begin(), it);
					_map.insert(it, value);
					return std::make_pair(_map.begin()+d, true);
				}
				else
				{
					return std::make_pair(it, false);
				}
			}
			else
			{				
				_map.insert(it, value);

				return std::make_pair(_map.end() - 1, true);
			}
		}
        template<typename... Args>
		std::pair<iterator, bool> emplace(Args&&... args)
		{
            auto temp = std::pair<Key,Value>(std::forward<Args>(args)...);
			auto it = std::lower_bound(_map.begin(), _map.end(), temp, _cmp);
			if (it != _map.end())
			{
				auto equalKey = it->first == temp.first;

				if (!equalKey)
				{
					auto d = std::distance(_map.begin(), it);
					_map.emplace(it, std::move(temp));
					return std::make_pair(_map.begin()+d, true);
				}
				else
				{
					return std::make_pair(it, false);
				}
			}
			else
			{
				_map.emplace(it, std::move(temp));

				return std::make_pair(_map.end() - 1, true);
			}
		}

		iterator find(const key_type& key)
		{
			auto it = std::lower_bound(_map.begin(), _map.end(), value_type(key,Value()), _cmp);
			if (it != _map.end())
			{
				if (_eq(it->first, key))
				{
					return it;
				}
				else
				{
					return end();
				}
			}
			return it;
		}

		const_iterator find(const key_type& key) const
		{
			auto it = std::lower_bound(_map.begin(), _map.end(), value_type(key,Value()), _cmp);
			if (it != _map.end())
			{
				if (_eq(it->first, key))
				{
					return it;
				}
				else
				{
					return end();
				}
			}
			return it;
		}

        iterator begin()
        {
            return _map.begin();
        }

		const_iterator begin() const
		{
			return _map.begin();
		}

		iterator end()
		{
			return _map.end();
		}

		const_iterator end() const
		{
			return _map.end();
		}

        size_type size() const
        {
            return _map.size();
        }

        Variant find(std::string_view path) const
        {
            Variant retval;

            retval = findEndpoint(path, "size", std::uint32_t(size()));
            if (retval.has_value())
                return retval;

            return {};
        }
	private:
		container _map;
		Compare _cmp;
        Equals _eq;
	};
}
