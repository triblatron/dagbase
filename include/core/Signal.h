//
// Created by Tony Horrobin on 13/12/2025.
//

#ifndef DAGUI_SIGNAL_H
#define DAGUI_SIGNAL_H

#include <functional>
#include <vector>
#include <type_traits>

namespace dagbase
{
    template<typename Signature>
    class Signal
    {
    public:
        void connect(std::function<Signature>&& func)
        {
            _slots.emplace_back(std::move(func));
        }

        template<typename... Args>
        typename std::function<Signature>::result_type operator()(Args&&... args)
        {
            if constexpr (std::is_void_v<typename std::function<Signature>::result_type>)
            {
                for (auto func : _slots)
                {
                    func(std::forward<Args>(args)...);
                }
            }
            else
            {
                typename std::function<Signature>::result_type result{};
                for (auto func : _slots) {
                    result = func(args...);
                }
                return result;
            }
        }

    private:
        std::vector<std::function<Signature>>  _slots;
    };
}

#endif //DAGUI_SIGNAL_H
