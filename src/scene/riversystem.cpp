#include<scene/riversystem.h>
#include<time.h>
#include<iostream>

RiverSystem::RiverSystem(Scene*s):scene(s){}
RiverSystem::~RiverSystem()
{
    scene=NULL;
}

void RiverSystem::L_System_River(glm::vec3 origin, int width,int length,int direction,int river_type )
{
    int branchRandomMax,branchRandomMin,TestPoint;
    float branchlengthscale;
    float branchwidthscale;
    std::vector<glm::vec3> unit_width;
    unit_width.resize(width);

    int MainDirection=direction;
    int StepForChange=0;
    int StepForBranch=0;
    int StepForChange_FLAG=0;
    int StepForBranch_FLAG=8;
    int StepForWind=0;
    if(river_type==STRAIGHT_RIVER)
    {
        StepForWind=1;
        StepForChange_FLAG=10;
        branchRandomMax=8;
        branchRandomMin=0;
        TestPoint=2;
        branchlengthscale=0.1;
        branchwidthscale=0.2;
    }
    else if(river_type==WINDING_RIVER)
    {
        StepForWind=2;
        StepForChange_FLAG=5;
        branchRandomMax=6;
        branchRandomMin=0;
        TestPoint=2;
        branchlengthscale=0.5;
        branchwidthscale=1;

    }
    else
        return;



    if(MainDirection==LEFT||MainDirection==RIGHT)
    {
        for(int j=0;j<width;j++)
        {
            unit_width[j][0]=origin[0];
            unit_width[j][2]=origin[2]+j;
        }
        CarveBankOrigin(unit_width,MainDirection);
        for(int j=0;j<length;j++)
        {

            CarveBank(unit_width,MainDirection);
            StepForward(unit_width,MainDirection);
            if(StepForBranch==StepForBranch_FLAG)
            {
                StepForBranch=0;
                int branchflag=rand()%(branchRandomMax-branchRandomMin+1)+branchRandomMin;
                if(branchflag<TestPoint)
                {
                    int branchlength=length*branchlengthscale+rand()%6;
                    int branchwidth=width*branchwidthscale+rand()%2;
                    if(branchlength>0&&branchwidth>0)
                    {
                        int Random_Branch_Direction=-1;
                        while(Random_Branch_Direction!=FORWARD&&Random_Branch_Direction!=BACK)
                            Random_Branch_Direction=Random_Direction();

                        if(Random_Branch_Direction==FORWARD)
                            L_System_River(unit_width[width-1],branchwidth,branchlength,\
                                    Random_Branch_Direction,river_type);
                        else if(Random_Branch_Direction==BACK)
                            L_System_River(unit_width[0],branchwidth,branchlength,\
                                    Random_Branch_Direction,river_type);
                    }
                }
            }
            if(StepForChange>StepForChange_FLAG)
            {
                StepForChange=0;
                int ChangeDirection=-1;
                while(ChangeDirection!=FORWARD&&ChangeDirection!=BACK)
                    ChangeDirection=Random_Direction();
                RiverwidthTranslate(unit_width,ChangeDirection,StepForWind);
            }
            StepForChange++;
            StepForBranch++;
        }
        CarveBankTerminal(unit_width,MainDirection);
    }
    else if(MainDirection==FORWARD||MainDirection==BACK)
    {
        for(int j=0;j<width;j++)
        {
            unit_width[j][0]=origin[0]+j;
            unit_width[j][2]=origin[2];

        }
        CarveBankOrigin(unit_width,MainDirection);
        for(int j=0;j<length;j++)
        {
            CarveBank(unit_width,MainDirection);
            StepForward(unit_width,MainDirection);
            if(StepForBranch==StepForBranch_FLAG)
            {
                StepForBranch=0;
                int branchflag=rand()%(branchRandomMax-branchRandomMin+1)+branchRandomMin;
                if(branchflag<TestPoint)
                {
                    int branchlength=length*branchlengthscale+rand()%6;
                    int branchwidth=width*branchwidthscale+rand()%2;
                    if(branchlength>0&&branchwidth>0)
                    {
                        int Random_Branch_Direction=-1;
                        while(Random_Branch_Direction!=LEFT&&Random_Branch_Direction!=RIGHT)
                            Random_Branch_Direction=Random_Direction();

                        if(Random_Branch_Direction==RIGHT)
                            L_System_River(unit_width[width-1],branchwidth,branchlength,\
                                    Random_Branch_Direction,river_type);
                        else if(Random_Branch_Direction==LEFT)
                            L_System_River(unit_width[0],branchwidth,branchlength,\
                                    Random_Branch_Direction,river_type);
                    }

                }
            }
            if(StepForChange>StepForChange_FLAG)
            {
                StepForChange=0;
                int ChangeDirection=-1;
                while(ChangeDirection!=RIGHT&&ChangeDirection!=LEFT)
                    ChangeDirection=Random_Direction();
                RiverwidthTranslate(unit_width,ChangeDirection,StepForWind);
            }
            StepForChange++;
            StepForBranch++;
        }
        CarveBankTerminal(unit_width,MainDirection);
    }
}

//void RiverSystem::SpliteBranch(glm::vec3 origin,int width,int length, int direction)
//{
//    std::vector<glm::vec3> unit_width;
//    unit_width.resize(width);
//    if(direction==LEFT||direction==RIGHT)



//}


void RiverSystem::ToWater(glm::vec3 pos)
{
    std::map<tuple, Block*>::iterator iter;
    tuple tupletemp(pos[0],pos[1],pos[2]);
    iter=scene->mSceneMap.find(tupletemp);
    if(iter!=scene->mSceneMap.end())
    {
        iter->second->mType=WATER;
    }

    std::get<1>(tupletemp)++;
    iter=scene->mSceneMap.find(tupletemp);
    while(iter!=scene->mSceneMap.end())
    {
        scene->mSceneMap.erase(iter);
        std::get<1>(tupletemp)++;
        iter=scene->mSceneMap.find(tupletemp);
    }
    waterblocks.push_back(pos);
}



void RiverSystem::RiverwidthTranslate(std::vector<glm::vec3> &unit_width, int direction, int step)
{
    if(direction==FORWARD)
    {
        for(int i=0;i<unit_width.size();i++)
            unit_width[i][2]+=step;
    }
    else if(direction==BACK)
    {
        for(int i=0;i<unit_width.size();i++)
            unit_width[i][2]-=step;
    }
    else if(direction==LEFT)
    {
        for(int i=0;i<unit_width.size();i++)
            unit_width[i][0]-=step;
    }
    else if(direction==RIGHT)
    {
        for(int i=0;i<unit_width.size();i++)
            unit_width[i][0]+=step;
    }
}
void RiverSystem::StepForward(std::vector<glm::vec3>& unit_width,int direction)
{
    if(direction==LEFT)
    {
        for(int i=0;i<unit_width.size();i++)
        {
            ToWater((unit_width[i]));
            unit_width[i][0]--;
        }
    }
    else if(direction==RIGHT)
    {
        for(int i=0;i<unit_width.size();i++)
        {
            ToWater((unit_width[i]));
            unit_width[i][0]++;
        }
    }
    else if(direction==FORWARD)
    {
        for(int i=0;i<unit_width.size();i++)
        {
            ToWater((unit_width[i]));
            unit_width[i][2]++;
        }
    }
    else if(direction==BACK)
    {
        for(int i=0;i<unit_width.size();i++)
        {
            ToWater((unit_width[i]));
            unit_width[i][2]--;
        }
    }
}
void RiverSystem::CarveBank(std::vector<glm::vec3> unit_width,int river_direction)
{
    int riverwidth=unit_width.size();
    int heightstep=1;
    tuple tupletemp1_1(0,0,0),tupletemp1_2(0,0,0),tupletemp2_1(0,0,0),tupletemp2_2(0,0,0);
    std::map<tuple, Block*>::iterator iter1_1,iter1_2,iter2_1,iter2_2;
    if(river_direction==LEFT||river_direction==RIGHT)
    {        
        tupletemp1_1=tuple(unit_width[0][0],unit_width[0][1]+1,unit_width[0][2]-1);
        tupletemp1_2=tuple(unit_width[0][0],unit_width[0][1]+1+heightstep,unit_width[0][2]-2);
        tupletemp2_1=tuple(unit_width[riverwidth-1][0],unit_width[riverwidth-1][1]+1,unit_width[riverwidth-1][2]+1);
        tupletemp2_2=tuple(unit_width[riverwidth-1][0],unit_width[riverwidth-1][1]+1+heightstep,unit_width[riverwidth-1][2]+2);
    }
    else if(river_direction==FORWARD||river_direction==BACK)
    {
        tupletemp1_1=tuple(unit_width[0][0]-1,unit_width[0][1]+1,unit_width[0][2]);
        tupletemp1_2=tuple(unit_width[0][0]-2,unit_width[0][1]+1+heightstep,unit_width[0][2]);
        tupletemp2_1=tuple(unit_width[riverwidth-1][0]+1,unit_width[riverwidth-1][1]+1,unit_width[riverwidth-1][2]);
        tupletemp2_2=tuple(unit_width[riverwidth-1][0]+2,unit_width[riverwidth-1][1]+1+heightstep,unit_width[riverwidth-1][2]);
    }
    iter1_1=scene->mSceneMap.find(tupletemp1_1);
    iter1_2=scene->mSceneMap.find(tupletemp1_2);
    iter2_1=scene->mSceneMap.find(tupletemp2_1);
    iter2_2=scene->mSceneMap.find(tupletemp2_2);

    while(iter1_2!=scene->mSceneMap.end())
    {
        if(iter1_1!=scene->mSceneMap.end())
        {
            tuple tupletemp3(std::get<0>(tupletemp1_1),std::get<1>(tupletemp1_1)-1,std::get<2>(tupletemp1_1));
            scene->mSceneMap[tupletemp3]->mType=GRASS;
        }
        while(iter1_1!=scene->mSceneMap.end())
        {
            scene->mSceneMap.erase(iter1_1);
            std::get<1>(tupletemp1_1)++;
            iter1_1=scene->mSceneMap.find(tupletemp1_1);
        }

        iter1_1=iter1_2;
        tupletemp1_1=tupletemp1_2;
        if(heightstep>0)
            heightstep=0;
        else
            heightstep=1;
        std::get<1>(tupletemp1_2)+=heightstep;
        if(river_direction==LEFT||river_direction==RIGHT)
            std::get<2>(tupletemp1_2)--;
        else if(river_direction==FORWARD||river_direction==BACK)
            std::get<0>(tupletemp1_2)--;
        iter1_2=scene->mSceneMap.find(tupletemp1_2);
    }
    heightstep=1;
    while(iter2_2!=scene->mSceneMap.end())
    {
        if(iter2_1!=scene->mSceneMap.end())
        {
            tuple tupletemp3(std::get<0>(tupletemp2_1),std::get<1>(tupletemp2_1)-1,std::get<2>(tupletemp2_1));
            scene->mSceneMap[tupletemp3]->mType=GRASS;
        }
        while(iter2_1!=scene->mSceneMap.end())
        {
            scene->mSceneMap.erase(iter2_1);
            std::get<1>(tupletemp2_1)++;
            iter2_1=scene->mSceneMap.find(tupletemp2_1);
        }

        iter2_1=iter2_2;
        tupletemp2_1=tupletemp2_2;
        if(heightstep>0)
            heightstep=0;
        else
            heightstep=1;
        std::get<1>(tupletemp2_2)+=heightstep;
        if(river_direction==LEFT||river_direction==RIGHT)
            std::get<2>(tupletemp2_2)++;
        else if(river_direction==FORWARD||river_direction==BACK)
            std::get<0>(tupletemp2_2)++;
        iter2_2=scene->mSceneMap.find(tupletemp2_2);
    }
}
void RiverSystem::CarveBankOrigin(std::vector<glm::vec3> origin,int riverdirection)
{
    std::vector<glm::vec3> originborder;
    int heightstep=1;
    int step=0;
    originborder.resize(origin.size());
    tuple tupletemp1(0,0,0),tupletemp2(0,0,0),tupletemp_prime1(0,0,0),tupletemp_prime2(0,0,0);
    std::map<tuple, Block*>::iterator iter1,iter2,iter_prime;

    for(int i=0;i<origin.size();i++)
    {
        step=0;
        heightstep=1;
        if(riverdirection==LEFT)
        {
            tupletemp1=tuple(origin[i][0]+1,origin[i][1]+1,origin[i][2]);
            tupletemp2=tuple(origin[i][0]+2,origin[i][1]+heightstep+1,origin[i][2]);
        }
        else if(riverdirection==RIGHT)
        {
            tupletemp1=tuple(origin[i][0]-1,origin[i][1]+1,origin[i][2]);
            tupletemp2=tuple(origin[i][0]-2,origin[i][1]+heightstep+1,origin[i][2]);
        }
        else if(riverdirection==FORWARD)
        {
            tupletemp1=tuple(origin[i][0],origin[i][1]+1,origin[i][2]-1);
            tupletemp2=tuple(origin[i][0],origin[i][1]+heightstep+1,origin[i][2]-2);
        }
        else if(riverdirection==BACK)
        {
            tupletemp1=tuple(origin[i][0],origin[i][1]+1,origin[i][2]+1);
            tupletemp2=tuple(origin[i][0],origin[i][1]+heightstep+1,origin[i][2]+2);
        }

        iter1=scene->mSceneMap.find(tupletemp1);
        iter2=scene->mSceneMap.find(tupletemp2);

        while(iter2!=scene->mSceneMap.end())
        {
            step++;
            if(i==0)
            {
                if(riverdirection==LEFT||riverdirection==RIGHT)
                    tupletemp_prime1=tuple(std::get<0>(tupletemp1),std::get<1>(tupletemp1),std::get<2>(tupletemp1)-1);
                else if(riverdirection==FORWARD||riverdirection==BACK)
                    tupletemp_prime1=tuple(std::get<0>(tupletemp1)-1,std::get<1>(tupletemp1),std::get<2>(tupletemp1));
                for(int i=0;i<step;i++)
                {
                    tupletemp_prime2=tupletemp_prime1;
                    iter_prime=scene->mSceneMap.find(tupletemp_prime2);
                    if(iter_prime!=scene->mSceneMap.end())
                    {
                        tuple tupletemp3(std::get<0>(tupletemp_prime2),\
                                         std::get<1>(tupletemp_prime2)-1,\
                                         std::get<2>(tupletemp_prime2));
                        scene->mSceneMap[tupletemp3]->mType=GRASS;
                    }
                    while(iter_prime!=scene->mSceneMap.end())
                    {
                        scene->mSceneMap.erase(iter_prime);
                        std::get<1>(tupletemp_prime2)++;
                        iter_prime=scene->mSceneMap.find(tupletemp_prime2);
                    }
                    if(riverdirection==LEFT||riverdirection==RIGHT)
                        std::get<2>(tupletemp_prime1)--;
                    else if(riverdirection==FORWARD||riverdirection==BACK)
                        std::get<0>(tupletemp_prime1)--;
                }
                if(riverdirection==LEFT)
                {
                    std::get<2>(tupletemp_prime1)++;
                    std::get<0>(tupletemp_prime1)--;
                }
                else if(riverdirection==RIGHT)
                {
                    std::get<2>(tupletemp_prime1)++;
                    std::get<0>(tupletemp_prime1)++;
                }
                else if(riverdirection==BACK)
                {
                    std::get<0>(tupletemp_prime1)++;
                    std::get<2>(tupletemp_prime1)--;
                }
                else if(riverdirection==FORWARD)
                {
                    std::get<0>(tupletemp_prime1)++;
                    std::get<2>(tupletemp_prime1)++;
                }
                for(int i=0;i<step-1;i++)
                {
                    tupletemp_prime2=tupletemp_prime1;
                    iter_prime=scene->mSceneMap.find(tupletemp_prime2);
                    if(iter_prime!=scene->mSceneMap.end())
                    {
                        tuple tupletemp3(std::get<0>(tupletemp_prime2),\
                                         std::get<1>(tupletemp_prime2)-1,\
                                         std::get<2>(tupletemp_prime2));
                        scene->mSceneMap[tupletemp3]->mType=GRASS;
                    }
                    while(iter_prime!=scene->mSceneMap.end())
                    {
                        scene->mSceneMap.erase(iter_prime);
                        std::get<1>(tupletemp_prime2)++;
                        iter_prime=scene->mSceneMap.find(tupletemp_prime2);
                    }
                    if(riverdirection==LEFT)
                        std::get<0>(tupletemp_prime1)--;
                    else if(riverdirection==RIGHT)
                        std::get<0>(tupletemp_prime1)++;
                    else if(riverdirection==FORWARD)
                        std::get<2>(tupletemp_prime1)++;
                    else if(riverdirection==BACK)
                        std::get<2>(tupletemp_prime1)--;
                }
            }
            else if(i==origin.size()-1)
            {
                if(riverdirection==LEFT||riverdirection==RIGHT)
                    tupletemp_prime1=tuple(std::get<0>(tupletemp1),std::get<1>(tupletemp1),std::get<2>(tupletemp1)+1);
                else if(riverdirection==FORWARD||riverdirection==BACK)
                    tupletemp_prime1=tuple(std::get<0>(tupletemp1)+1,std::get<1>(tupletemp1),std::get<2>(tupletemp1));
                for(int i=0;i<step;i++)
                {
                    tupletemp_prime2=tupletemp_prime1;
                    iter_prime=scene->mSceneMap.find(tupletemp_prime2);
                    if(iter_prime!=scene->mSceneMap.end())
                    {
                        tuple tupletemp3(std::get<0>(tupletemp_prime2),std::get<1>(tupletemp_prime2)-1,std::get<2>(tupletemp_prime2));
                        scene->mSceneMap[tupletemp3]->mType=GRASS;
                    }
                    while(iter_prime!=scene->mSceneMap.end())
                    {
                        scene->mSceneMap.erase(iter_prime);
                        std::get<1>(tupletemp_prime2)++;
                        iter_prime=scene->mSceneMap.find(tupletemp_prime2);
                    }
                    if(riverdirection==LEFT||riverdirection==RIGHT)
                        std::get<2>(tupletemp_prime1)++;
                    else if(riverdirection==FORWARD||riverdirection==BACK)
                        std::get<0>(tupletemp_prime1)++;
                }
                if(riverdirection==LEFT)
                {
                    std::get<2>(tupletemp_prime1)--;
                    std::get<0>(tupletemp_prime1)--;
                }
                else if(riverdirection==RIGHT)
                {
                    std::get<2>(tupletemp_prime1)--;
                    std::get<0>(tupletemp_prime1)++;
                }
                else if(riverdirection==BACK)
                {
                    std::get<0>(tupletemp_prime1)--;
                    std::get<2>(tupletemp_prime1)--;
                }
                else if(riverdirection==FORWARD)
                {
                    std::get<0>(tupletemp_prime1)--;
                    std::get<2>(tupletemp_prime1)++;
                }
                for(int i=0;i<step-1;i++)
                {
                    tupletemp_prime2=tupletemp_prime1;
                    iter_prime=scene->mSceneMap.find(tupletemp_prime2);
                    if(iter_prime!=scene->mSceneMap.end())
                    {
                        tuple tupletemp3(std::get<0>(tupletemp_prime2),\
                                         std::get<1>(tupletemp_prime2)-1,\
                                         std::get<2>(tupletemp_prime2));
                        scene->mSceneMap[tupletemp3]->mType=GRASS;
                    }
                    while(iter_prime!=scene->mSceneMap.end())
                    {
                        scene->mSceneMap.erase(iter_prime);
                        std::get<1>(tupletemp_prime2)++;
                        iter_prime=scene->mSceneMap.find(tupletemp_prime2);
                    }
                    if(riverdirection==LEFT)
                        std::get<0>(tupletemp_prime1)--;
                    else if(riverdirection==RIGHT)
                        std::get<0>(tupletemp_prime1)++;
                    else if(riverdirection==FORWARD)
                        std::get<2>(tupletemp_prime1)++;
                    else if(riverdirection==BACK)
                        std::get<2>(tupletemp_prime1)--;
                }
            }
            if(iter1!=scene->mSceneMap.end())
            {
                tuple tupletemp3(std::get<0>(tupletemp1),std::get<1>(tupletemp1)-1,std::get<2>(tupletemp1));
                scene->mSceneMap[tupletemp3]->mType=GRASS;
            }
            while(iter1!=scene->mSceneMap.end())
            {
                scene->mSceneMap.erase(iter1);
                std::get<1>(tupletemp1)++;
                iter1=scene->mSceneMap.find(tupletemp1);
            }
            iter1=iter2;
            tupletemp1=tupletemp2;
            if(heightstep>0)
                heightstep=0;
            else
                heightstep=1;
            if(riverdirection==LEFT)
                std::get<0>(tupletemp2)++;
            else if(riverdirection==RIGHT)
                std::get<0>(tupletemp2)--;
            else if(riverdirection==FORWARD)
                std::get<2>(tupletemp2)--;
            else if(riverdirection==BACK)
                std::get<2>(tupletemp2)++;
            std::get<1>(tupletemp2)+=heightstep;
            iter2=scene->mSceneMap.find(tupletemp2);
        }
    }
}
void RiverSystem::CarveBankTerminal(std::vector<glm::vec3> terminal,int riverdirection)
{
    if(riverdirection==LEFT)
    {
        for(int i=0;i<terminal.size();i++)
            terminal[i][0]++;
        CarveBankOrigin(terminal,RIGHT);
    }
    else if(riverdirection==RIGHT)
    {
        for(int i=0;i<terminal.size();i++)
            terminal[i][0]--;
        CarveBankOrigin(terminal,LEFT);
    }
    else if(riverdirection==FORWARD)
    {
        for(int i=0;i<terminal.size();i++)
            terminal[i][2]--;
        CarveBankOrigin(terminal,BACK);
    }
    else if(riverdirection==BACK)
    {
        for(int i=0;i<terminal.size();i++)
            terminal[i][2]++;
        CarveBankOrigin(terminal,FORWARD);
    }
}
std::vector<glm::vec3> RiverSystem::GetRiverBlocks()
{
    return waterblocks;
}

