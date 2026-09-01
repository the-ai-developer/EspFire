#include "core/fire_state.h"

void FireState::update(bool flameFire) {
    SystemState next = flameFire ? SystemState::Fire : SystemState::Safe;
    changed_ = (next != state_);
    state_ = next;
}

SystemState FireState::state() const { return state_; }
bool FireState::changed() const { return changed_; }
bool FireState::isFire() const { return state_ == SystemState::Fire; }
