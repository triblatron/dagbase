#pragma once

#include <vector>
#include <utility>
#include <algorithm>

namespace dagui
{
	template <typename Key, typename Value>
	class VectorMap
	{
	public:
		typedef std::pair<Key, Value> value_type;
		typedef std::vector<value_type> container;
		typedef typename container::iterator iterator;
		struct Compare
		{
			bool operator()(const value_type& op1, const value_type& op2) const
			{
				return op1.first < op2.first;
			}
		};
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
					_map.emplace(it, value);
					return std::make_pair(_map.begin()+d, true);
				}
				else
				{
					return std::make_pair(it, false);
				}
			}
			else
			{				
				_map.emplace(it, value);

				return std::make_pair(_map.end() - 1, true);
			}
		}

		iterator find(const Key& key)
		{
			return std::lower_bound(_map.begin(), _map.end(), value_type(key,Value()), _cmp);
		}

        iterator begin()
        {
            return _map.begin();
        }

		iterator end()
		{
			return _map.end();
		}
	private:
		container _map;
		Compare _cmp;
	};
}
