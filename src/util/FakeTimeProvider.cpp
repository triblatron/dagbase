//
// Created by Tony Horrobin on 06/09/2025.
//

#include "config/config.h"

#include "util/FakeTimeProvider.h"

namespace dagbase
{
    void FakeTimeProvider::tick()
    {
        _time += _period;
    }

    double FakeTimeProvider::provideTime() const
    {
        return _time;
    }
}
