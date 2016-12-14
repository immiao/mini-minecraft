#include<scene/fireblocks.h>
Fire::Fire()
{

}
void Fire::setpos(glm::vec3 pos)
{
    position=pos;
}

glm::vec3 Fire::getpos()
{
    return position;
}
double Fire::Get_Spreadtime()
{
    return spreadtime;
}
double Fire::Get_Lifetime()
{
    return lifetime;
}

bool Fire::FireBurning()
{
    if(lifetime==0)
        return false;
    else
    {
        if(spreadtime!=-1)
            spreadtime--;
        lifetime--;
        return true;
    }

}
