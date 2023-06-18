//
// Created by chrtopf on 18.06.23.
//

#include "Effects.h"
#include "AverageVolumeEffect.h"
#include "RMSMaxVolumeEffect.h"
#include "MaxVolumeEffect.h"


void Effects::printEffects() {
    printEffect(0, "A rudimentary average volume slider.");
    printEffect(1, "True RMS volume slider with maximum volume tip.");
    printEffect(2, "A basic maximum volume slider.");
    //printEffect(n, "Your description");
}

Effect *Effects::newEffect(const int index) {
    switch (index) {
        case 0:
            return new AverageVolumeEffect();
        case 1:
            return new RMSMaxVolumeEffect();
        case 2:
            return new MaxVolumeEffect();
        //case n:
        //return new ...();
        default:
            return nullptr;
    }
}
