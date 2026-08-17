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
    //! \class SlotMap
    //! A class template for a growable slot map
    //! This is backed by an array providing contiguous storage so that
    //! insertion is amortised O(1)
    //! erasure is O(1)
    //! access is O(1)
    //! handles are stable and validated when used to access an element.
    //! \note T must be default-constructible
    //! \note references are invalidated when the underlying array grows
    template<typename T>
    class SlotMap
    {
    public:
        //! \class Ident
        //! An identifier for a slot
        struct Ident
        {
            //! A sentinal value for an index, used to mean there is nothing there.
            //! \note The freeHead takes on this value if there are no free slots.
            static constexpr std::uint32_t INVALID_INDEX = ~0U;

            //! Configure members from a config tree.
            void configure(ConfigurationElement& config)
            {
                ConfigurationElement::readConfig(config, "index", &index);
                ConfigurationElement::readConfig(config, "gen", &gen);
            }

            //! Equality comparison, required to satisfy the compiler.
            bool operator==(const Ident& other) const
            {
                return index == other.index && gen == other.gen;
            }

            //! Output operator, useful for pretty-printing in tests.
            friend
            std::ostream& operator<<(std::ostream& str, const typename SlotMap<T>::Ident& value)
            {
                str << "SlotMap.Ident { index: " << value.index << ", gen: " << value.gen << " }";

                return str;
            }

            //! The index into the slot map backing array.
            std::uint32_t index{INVALID_INDEX};
            //! The generation, used to determine the validity of an identifer against a slot.
            //! \note Zero means a slot is free and the index points to the next free slot.
            std::uint32_t gen{0};
        };

        //! \class Item
        //! Put the item first so we can cast a pointer to T to a pointer to Item safely.
        struct Item
        {
            T item;
            Ident id;
        };

        static_assert(std::is_standard_layout_v<Item>, "Item must be standard-layout");

        //! \constructor Takes a count and allocates space for that many items
        //! but does not run the constructor.
        explicit SlotMap(std::size_t count)
        {
            _data.reserve(count);
        }

        //! Allocate an item, either by growing the array or using a free entry.
        //! \note Runs in amortised constant time because an allocation may be necessary.
        //! \note Invalidates references if an allocation in the underlying array occurs.
        T& alloc()
        {
            if (_freeHead == Ident::INVALID_INDEX)
            {
                _data.emplace_back();
                _data.back().id.index = _data.size()-1;
                _data.back().id.gen = _nextGen;
                ++_size;
                return _data.back().item;
            }
            auto lastIndex = _data[_freeHead].id.index;
            _data[_freeHead].id.gen = _nextGen;
            _data[_freeHead].id.index = _size;
            _freeHead = lastIndex;
            return _data[_size++].item;
        }

        //! Deallocate an item, rendering identifiers to it invalid.
        //! \note This is accomplished by incrementing the generation counter.
        void free(T& value)
        {
            Item* item = (Item*)&value;

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

        //! \return The identifier for a value.
        //! \note Uses the fact that an Item has the T as its first element 
        //! so that a cast between pointers is safe.
        Ident id(T& value)
        {
            Item * item = (Item*)&value;

            return item->id;
        }

        //! \return A reference to an item
        //! \note The identifier must be valid
        T& get(Ident id)
        {
            return _data[id.index].item;
        }

        //! Try to get an item.
        //! \retval A pointer to the item if the identifier is valid, as determined by isValid().
        //! \retval nullptr otherwise.
        T* tryGet(Ident id)
        {
            if (isValid(id))
            {
                return &_data[id.index].item;
            }

            return nullptr;
        }

        //! Determine whether a given identifier is valid
        //! \note The index must be in range and the generation must match the one in the slot.
        bool isValid(Ident id) const
        {
            return id.index < _data.size() && _data[id.index].id.gen == id.gen;
        }

        //! \return The number of elements in the array.
        std::uint32_t size() const
        {
            return _size;
        }

        //! \return An attribute given by the path
        //! Supported attributes:size, nextGen, freeHead
        //! \note Typically used to support assertions in tests.
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
        //! The data array, consisting of the element type and an identifier.
        Array _data;
        //! The number of elements in use.
        std::uint32_t _size{0};
        //! The next generation to be used, indicates a free slot if zero.
        std::uint32_t _nextGen{1};
        //! The start of the free slot list.
        std::uint32_t _freeHead{Ident::INVALID_INDEX};
    };

}
