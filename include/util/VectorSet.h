//
// Created by Tony Horrobin on 14/09/2025.
//

#pragma once

#include "core/Variant.h"
#include "util/Searchable.h"

#include <string_view>
#include <vector>

namespace dagbase
{
    template<typename Value, typename Compare=std::less<Value>>
    class VectorSet
    {
    public:
        using value_type = Value;
        using container = std::vector<Value>;
        using iterator = typename container::iterator;
        using const_iterator = typename container::const_iterator;
        using size_type = typename container::size_type;
    public:
        void reserve(size_type n)
        {
            _set.reserve(n);
        }

        void resize(size_type n)
        {
            _set.resize(n);
        }

        size_type size() const
        {
            return _set.size();
        }

        iterator begin()
        {
            return _set.begin();
        }

        const_iterator begin() const
        {
            return _set.begin();
        }

        iterator end()
        {
            return _set.end();
        }

        const_iterator end() const
        {
            return _set.end();
        }

        std::pair<iterator, bool> insert(const value_type& value)
        {
            auto it = std::lower_bound(_set.begin(), _set.end(), value, _cmp);
            if (it != _set.end())
            {
                auto equalKey = *it==value;

                if (!equalKey)
                {
                    auto d = std::distance(_set.begin(), it);
                    _set.insert(it, value);
                    return std::make_pair(_set.begin()+d, true);
                }
                else
                {
                    return std::make_pair(it, false);
                }
            }
            else
            {
                _set.insert(it, value);

                return std::make_pair(_set.end() - 1, true);
            }
        }

        template<typename... Args>
        std::pair<iterator, bool> emplace(Args&&... args)
        {
            Value value(std::forward<Args>(args)...);
            auto it = std::lower_bound(_set.begin(), _set.end(), value, _cmp);
            if (it != _set.end())
            {
                auto equalKey = *it==value;

                if (!equalKey)
                {
                    auto d = std::distance(_set.begin(), it);
                    _set.emplace(it, std::move(value));
                    return std::make_pair(_set.begin()+d, true);
                }
                else
                {
                    return std::make_pair(it, false);
                }
            }
            else
            {
                _set.emplace(it, std::move(value));

                return std::make_pair(_set.end() - 1, true);
            }
        }


        const_iterator find(const Value& value) const
        {
            auto it = std::lower_bound(_set.begin(), _set.end(), value_type(value), _cmp);
            if (it != _set.end())
            {
                if (*it == value)
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

        Variant find(std::string_view path) const
        {
            Variant retval;

            retval = findEndpoint(path, "size", std::uint32_t(size()));
            if (retval.has_value())
                return retval;

            return {};
        }
    private:
        container _set;
        Compare _cmp;
    };
}