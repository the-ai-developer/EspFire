#pragma once

class EnvState {
public:
    void update(bool tempHigh, bool humHigh, bool airBad);
    bool abnormal() const;
    bool changed() const;
    const char* reason() const;

private:
    bool abnormal_ = false;
    bool changed_ = false;
    const char* reason_ = "normal";
};
