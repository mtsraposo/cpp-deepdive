#pragma once

class LimitedInt {
public:
    int value;

    LimitedInt(const LimitedInt &other) : value(other.value) {}

    LimitedInt(int v) : value(v) {}

    bool operator<(const LimitedInt &other) const {
        return value < other.value;
    }

    friend std::ostream &operator<<(std::ostream &os, const LimitedInt &obj) {
        os << obj.value;
        return os;
    }
};

class LimitedChar {
public:
    char value;

    LimitedChar(const LimitedChar &v) : value(v.value) {}

    LimitedChar(char v) : value(v) {}

    bool operator==(const LimitedChar &other) const {
        return value == other.value;
    }

    friend std::ostream &operator<<(std::ostream &os, const LimitedChar &obj) {
        os << obj.value;
        return os;
    }
};