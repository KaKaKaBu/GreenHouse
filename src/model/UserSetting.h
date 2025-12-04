#ifndef USERSETTING_H
#define USERSETTING_H
struct Threshold
{
    uint8_t fanOffThreshold=0;
    uint8_t fanOnThreshold=0;
    uint8_t lampOffThreshold=0;
    uint8_t lampONThreshold=0;
    uint8_t DumpOffThreshold=0;
    uint8_t DumpOnThreshold=0;
};
#endif // USERSETTING_H
