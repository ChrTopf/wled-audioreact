//
// Created by chrtopf on 18.06.23.
//

#ifndef WLED_AUDIOREACT_EFFECTS_H
#define WLED_AUDIOREACT_EFFECTS_H
#include "../Effect.h"
#include "AverageVolumeEffect.h"
#include "RMSMaxVolumeEffect.h"
#include "MaxVolumeEffect.h"
#include "DFTRockStrobo.h"
#include "DFTFrequencySpectrum.h"
#include "DFTLowFrequencySpectrum.h"
#include "DFTElectronicStrobo.h"
#include "RMSMaxVolumeStrobo.h"

class Effects{
private:
    template<typename T> inline static Effect *Class() { return new T; }
public:
    inline static const std::vector<std::pair<std::string, Effect*(*)()>> EFFECTS = {
            //list all effects here
            {"A rudimentary average volume slider", Class<AverageVolumeEffect>},
            {"True RMS volume slider with maximum volume tip", Class<RMSMaxVolumeEffect>},
            {"A basic maximum volume slider", Class<MaxVolumeEffect>},
            {"Stroboscopic white light reacting to the frequencies of rock music", Class<DFTRockStrobo>},
            {"Frequency spectrum analyzer", Class<DFTFrequencySpectrum>},
            {"Frequency spectrum analyzer for low frequencies", Class<DFTLowFrequencySpectrum>},
            {"Stroboscopic white light reacting to the frequencies of electronic music", Class<DFTElectronicStrobo>},
            {"Stroboscopic white light reacting to RMS peak volume.", Class<RMSMaxVolumeStrobo>}
    };
};

#endif //WLED_AUDIOREACT_EFFECTS_H
