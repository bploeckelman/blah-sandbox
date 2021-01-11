#pragma once
#include <blah.h>
#include "../world.h"
#include <functional>

using namespace Blah;

namespace Zen {

    class Timer : public Component {
    private:
        float m_duration = 0;

    public:
        Timer() = default;
        Timer(float duration, const std::function<void(Timer* self)>& on_end = nullptr);

        void start(float duration);

        std::function<void(Timer* self)> on_end;

        void update() override;
    };

}

