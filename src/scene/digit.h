#ifndef DIGIT
#define DIGIT

class Digit
{
public:
    Digit();
    void setDecade(int num);
    int getDecade();
    void setUnit(int num);
    int getUnit();
private:
    int decade=0;
    int unit=0;

};

#endif // DIGIT

