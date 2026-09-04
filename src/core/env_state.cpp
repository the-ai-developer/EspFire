#include "core/env_state.h"

void EnvState::update(bool tempHigh, bool humHigh, bool airBad) {
    bool next = tempHigh || humHigh || airBad;
    changed_ = (next != abnormal_);
    abnormal_ = next;
    if (tempHigh) reason_ = "high-temp";
    else if (humHigh) reason_ = "high-humidity";
    else if (airBad) reason_ = "air-quality";
    else reason_ = "normal";
}

bool EnvState::abnormal() const { return abnormal_; }
bool EnvState::changed() const { return changed_; }
const char* EnvState::reason() const { return reason_; }
