//
// Created by chrtopf on 18.06.23.
//

#ifndef WLED_AUDIOREACT_EFFECTS_H
#define WLED_AUDIOREACT_EFFECTS_H
#include "../Effect.h"
#include "AverageVolumeEffect.h"
#include "RMSMaxVolumeEffect.h"
#include "MaxVolumeEffect.h"


class Effects{
private:
    template<typename T> inline static Effect *Class() { return new T; }
public:
    inline static const vector<pair<string, Effect*(*)()>> EFFECTS = {
            //list all effects here
            {"A rudimentary average volume slider", Class<AverageVolumeEffect>},
            {"True RMS volume slider with maximum volume tip", Class<RMSMaxVolumeEffect>},
            {"A basic maximum volume slider", Class<MaxVolumeEffect>}
    };
};

#endif //WLED_AUDIOREACT_EFFECTS_H
