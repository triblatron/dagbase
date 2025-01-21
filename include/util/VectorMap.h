#pragma once

#include <vector>
#include <utility>
#include <algorithm>

namespace dagbase
{
	template <typename Key, typename Value>
	class VectorMap
	{
	public:
		typedef std::pair<Key, Value> value_type;
		typedef std::vector<value_type> container;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
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
			auto it = std::lower_bound(_map.begin(), _map.end(), value_type(key,Value()), _cmp);
			if (it != _map.end())
			{
				if (it->first == key)
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

		const_iterator find(const Key& key) const
		{
			auto it = std::lower_bound(_map.begin(), _map.end(), value_type(key,Value()), _cmp);
			if (it != _map.end())
			{
				if (it->first == key)
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
	private:
		container _map;
		Compare _cmp;
	};
}
