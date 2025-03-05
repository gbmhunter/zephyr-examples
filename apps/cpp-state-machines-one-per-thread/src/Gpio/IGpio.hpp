#pragma once

class IGpio {
    public:
        virtual void setState(bool state) = 0;
        virtual bool getState() = 0;
};