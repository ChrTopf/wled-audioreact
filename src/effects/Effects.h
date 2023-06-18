//
// Created by chrtopf on 18.06.23.
//

#ifndef WLED_AUDIOREACT_EFFECTS_H
#define WLED_AUDIOREACT_EFFECTS_H
#include "../Effect.h"

class Effects{
private:
    inline static void printEffect(const int index, const string &text){
        cout << "[" << index << "]" << " " << text << endl;
    }
public:
    static void printEffects();
    static Effect* newEffect(const int index);
};

#endif //WLED_AUDIOREACT_EFFECTS_H
