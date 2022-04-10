//
// Created by Johan Lövgren on 2022-04-09.
//
#include <cstdint>

#ifndef DAUMERGOANT_ANTPROFILE_H
#define DAUMERGOANT_ANTPROFILE_H
class AntProfile {
public:
    AntProfile();
    virtual void HandleTXEvent(uint8_t *txBuffer) = 0;
    void SetChannelNumber(uint8_t number);
private:
    uint8_t ChannelNumber;
};
#endif //DAUMERGOANT_ANTPROFILE_H
