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
    template<typename R>
    struct Last
    {
        template<typename InputIterator, typename... Args>
        R operator()(InputIterator first, InputIterator last, Args&&... args)
        {
            R result;
            for (auto it = first; it != last; ++it)
            {
                result = (*it)(args...);
            }

            return result;
        }
    };

    template<class> class Signal;

    template<class R, class... Args>
    class Signal<R(Args...)>
    {
    public:
        using result_type = R;
        void connect(std::function<R(Args...)>&& func)
        {
            _slots.emplace_back(std::move(func));
        }

        template<typename Combiner=Last<R>>
        R operator()(Args&&... args)
        {
            if constexpr (std::is_void_v<R>)
            {
                for (auto func : _slots)
                {
                    func(std::forward<Args>(args)...);
                }
            }
            else
            {
                Combiner combiner;
                auto result = combiner(_slots.begin(), _slots.end(), std::forward<Args>(args)...);
                // R result{};
                // for (auto func : _slots) {
                //     result = func(std::forward<Args>(args)...);
                // }
                return result;
            }
        }

    private:
        std::vector<std::function<R(Args...)>>  _slots;
    };
}

#endif //DAGUI_SIGNAL_H
