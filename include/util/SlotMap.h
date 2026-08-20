//
// Created by Tony Horrobin on 16/08/2026.
//

#pragma once

#include "core/ConfigurationElement.h"
#include "util/Searchable.h"

#include <cstdint>
#include <vector>
#include <string_view>
#include <iostream>

namespace dagbase
{
    template<typename T>
    class SlotMap
    {
    public:
        struct Ident
        {
            static constexpr std::uint32_t INVALID_INDEX = ~0U;

            void configure(ConfigurationElement& config)
            {
                ConfigurationElement::readConfig(config, "index", &index);
                ConfigurationElement::readConfig(config, "gen", &gen);
            }

            bool operator==(const Ident& other) const
            {
                return index == other.index && gen == other.gen;
            }

            friend
            std::ostream& operator<<(std::ostream& str, const typename SlotMap<T>::Ident& value)
            {
                str << "SlotMap.Ident { index: " << value.index << ", gen: " << value.gen << " }";

                return str;
            }

            std::uint32_t index{INVALID_INDEX};
            std::uint32_t gen{0};
        };

        struct Item
        {
            T item;
            Ident id;

            static_assert(std::is_standard_layout_v<T>, "T must have a standard layout");
            template<typename ...Args>
            Item(Args&&... args)
                :
                item(std::forward<Args>(args)...)
            {
                // Do nothing.
            }
        };
        static_assert(std::is_standard_layout_v<Item>, "Item must have a standard layout");

        explicit SlotMap(std::size_t count)
        {
            _data.reserve(count);
        }

        template<typename... Args>
        T& alloc(Args&&... args)
        {
            if (_freeHead == Ident::INVALID_INDEX)
            {
                _data.emplace_back(std::forward<Args>(args)...);
                _data.back().id.index = _data.size()-1;
                _data.back().id.gen = _nextGen;
                ++_size;
                return _data.back().item;
            }
            auto lastIndex = _data[_freeHead].id.index;
            new (&_data[_size]) Item(std::forward<Args>(args)...);
            _data[_freeHead].id.gen = _nextGen;
            _data[_freeHead].id.index = _size;
            _freeHead = lastIndex;
            return _data[_size++].item;
        }

        void free(T& value)
        {
            Item* item = (Item*)&value;

            item->item.T::~T();

            if (_freeHead == Ident::INVALID_INDEX)
            {
                _freeHead = item->id.index;
                item->id.index = Ident::INVALID_INDEX;
            }
            else
            {
                auto oldFreeHead = _freeHead;
                _freeHead = item->id.index;
                item->id.index = oldFreeHead;

            }

            item->id.gen = 0;
            ++_nextGen;
            --_size;
        }

        Ident id(T& value)
        {
            Item * item = (Item*)&value;

            return item->id;
        }

        T& get(Ident id)
        {
            return _data[id.index].item;
        }

        T* tryGet(Ident id)
        {
            if (isValid(id))
            {
                return &_data[id.index].item;
            }

            return nullptr;
        }

        bool isValid(Ident id) const
        {
            return id.index < _data.size() && _data[id.index].id.gen == id.gen;
        }

        std::uint32_t size() const
        {
            return _size;
        }

        Variant find(std::string_view path) const
        {
            Variant retval;

            retval = findEndpoint(path, "size", size());
            if (retval.has_value())
                return retval;

            retval = findEndpoint(path, "nextGen", _nextGen);
            if (retval.has_value())
                return retval;

            retval = findEndpoint(path, "freeHead", _freeHead);
            if (retval.has_value())
                return retval;

            return {};
        }
    private:
        using Array = std::vector<Item>;
        Array _data;
        std::uint32_t _size{0};
        std::uint32_t _nextGen{1};
        std::uint32_t _freeHead{Ident::INVALID_INDEX};
    };

}
