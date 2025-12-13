//
// Created by Tony Horrobin on 13/12/2025.
//

#ifndef DAGUI_SIGNAL_H
#define DAGUI_SIGNAL_H

#include <functional>

namespace dagbase
{
    template<typename Signature>
    class Signal
    {
    public:
        void connect(std::function<Signature>&& func)
        {
            _func = std::move(func);
        }

        template<typename... Args>
        typename std::function<Signature>::result_type operator()(Args&... args)
        {
            return _func(std::forward<Args>(args)...);
        }

    private:
        std::function<Signature> _func;
    };
}

#endif //DAGUI_SIGNAL_H
