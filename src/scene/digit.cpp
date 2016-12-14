#include<scene/digit.h>
Digit::Digit(){}
void Digit::setUnit(int num)
{
    unit=num;
}
void Digit::setDecade(int num)
{
    decade=num;
}
int Digit::getDecade()
{
    return decade;
}
int Digit::getUnit()
{
    return unit;
}
