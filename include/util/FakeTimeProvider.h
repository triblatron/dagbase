//
// Created by Tony Horrobin on 06/09/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "util/TimeProvider.h"

namespace dagbase
{
    class DAGBASE_API FakeTimeProvider : public TimeProvider
    {
    public:
        void setPeriod(double period)
        {
            _period = period;
        }

        double period() const
        {
            return _period;
        }

        void tick() override;

        double provideTime() const override;
    private:
        double _period{0.0};
        double _time{0.0};
    };
}