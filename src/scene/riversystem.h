#include<direction.h>
#include<scene/scene.h>
#ifndef RIVERSYSTEM
#define RIVERSYSTEM

#define STRAIGHT_RIVER 0
#define WINDING_RIVER 1
#endif // RIVERSYSTEM


class RiverSystem
{
private:
    Scene* scene;
    std::vector<glm::vec3> waterblocks;
public:
    RiverSystem(Scene* s);
    ~RiverSystem();
    void L_System_River(glm::vec3 origin, int width, int length, int direction, int river_type);
    void RiverwidthTranslate(std::vector<glm::vec3>& unit_width,int direction,int step);
    void StepForward(std::vector<glm::vec3>& unit_width,int direction);
    void ToWater(glm::vec3 pos);
    void CarveBank(std::vector<glm::vec3> unit_width, int river_direction);
    void CarveBankOrigin(std::vector<glm::vec3> origin,int riverdirection);
    void CarveBankTerminal(std::vector<glm::vec3> terminal,int riverdirection);
    std::vector<glm::vec3> GetRiverBlocks();
};


