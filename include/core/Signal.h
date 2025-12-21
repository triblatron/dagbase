//
// Created by Tony Horrobin on 13/12/2025.
//

#ifndef DAGUI_SIGNAL_H
#define DAGUI_SIGNAL_H

#include "util/DummyMutex.h"
#include "util/VectorMultimap.h"

#include <functional>
#include <vector>
#include <type_traits>
#include <mutex>

namespace dagbase
{
    template<typename Signal>
    class ScopedConnection
    {
    public:
        explicit ScopedConnection(Signal* signal, std::size_t index)
            :
        _signal(signal),
        _index(index)
        {
            // Do nothing.
        }

        ~ScopedConnection()
        {
            if (_signal)
            {
                _signal->disconnect(_index);
            }
        }
    private:
        Signal* _signal{nullptr};
        std::size_t _index{~0U};
    };

    template<typename R>
    struct Last
    {
        template<typename InputIterator, typename... Args>
        R operator()(InputIterator first, InputIterator last, Args&&... args)
        {
            R result{};
            for (auto it = first; it != last; ++it)
            {
                result = (it->second)(args...);
            }

            return result;
        }
    };

    template<typename R, typename Combiner=Last<typename std::function<R>::result_type>, typename Mutex=DummyMutex> class Signal;

    template<typename R, typename... Args, typename Combiner, typename Mutex>
    class Signal<R(Args...), Combiner, Mutex>
    {
    public:
        using result_type = R;
        std::size_t connect(std::function<R(Args...)>&& func, int group=0)
        {
            std::scoped_lock<Mutex> guard(_mutex);
            _slots.emplace(group, std::move(func));
            return _slots.size()-1;
        }

        void disconnect(std::size_t index)
        {
            std::scoped_lock<Mutex> guard(_mutex);
            if ( index < _slots.size() )
            {
                _slots.erase(_slots.begin() + index);
            }
        }

        R operator()(Args&&... args)
        {
            std::scoped_lock<Mutex> guard(_mutex);
            if constexpr (std::is_void_v<R>)
            {
                for (auto p : _slots)
                {
                    p.second(std::forward<Args>(args)...);
                }
                return;
            }
            else
            {
                Combiner combiner;
                auto result = combiner(_slots.begin(), _slots.end(), std::forward<Args>(args)...);
                return result;
            }
        }

    private:
        VectorMultimap<int, std::function<R(Args...)>>  _slots;
        Mutex _mutex;
    };
}

#endif //DAGUI_SIGNAL_H
