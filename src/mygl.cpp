#define _USE_MATH_DEFINES
#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <math.h>
#include<QMessageBox>
#include <QDir>
#include<time.h>

#include <SOIL.h>

const int MaxReachDistance=8;
MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      gl_camera(), geom_cube(this),Rivers(&scene),center(this),T(this),
      prog_lambert(this), prog_flat(this), prog_new(this),
      grid(this),mousemove(false),game_begin(false),jump_state(false),
      g_velocity(0),external_force_acceleration(-gravity_acceleration),
      character_size(0,0,0)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    geom_cube.destroy();
    center.destroy();
    T.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of Cube
    geom_cube.create();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    prog_new.create(":/glsl/new.vert.glsl", ":/glsl/new.frag.glsl");
    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    prog_lambert.setGeometryColor(glm::vec4(0,1,0,1));
    prog_new.setGeometryColor(glm::vec4(0,1,0,1));

    //prog_new.setTexture();
    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);

    character_size=glm::vec3(0.6,2,0.6);
    for(int i=0;i<25;i++)
        keyboard[i]=false;

    scene.Create();
    Rivers.L_System_River(glm::vec3(70,0,70),10,150,LEFT,STRAIGHT_RIVER);
    Rivers.L_System_River(glm::vec3(0,0,-70),3,100,FORWARD,WINDING_RIVER);

    center.InitializeScreenSize(width(),height());
    T.InitializeScreenSize(width(),height());
    center.create();
    T.create();
    initializeGrid();

    game_begin = true;
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
//    gl_camera = Camera(w, h);
    gl_camera = Camera(w, h, glm::vec3(0, 20, 0),
                       glm::vec3(1, 20, 1), glm::vec3(0,1,0));
    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);
    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function updateGL is called, paintGL is called implicitly.
//DO NOT CONSTRUCT YOUR SCENE GRAPH IN THIS FUNCTION!
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());
    prog_new.setViewProjMatrix(gl_camera.getViewProj());
    prog_new.setViewPos(gl_camera.eye);
    prog_new.setTexture();
    grid.render(prog_new, gl_camera.getViewProj());
    prog_new.deleteTexture();

//    GLDrawScene();

    prog_flat.setViewProjMatrix(glm::mat4(1));
    prog_flat.setModelMatrix(glm::mat4(1));

    glDisable(GL_DEPTH_TEST);
    prog_flat.draw(center);
    prog_flat.draw(T);
    glEnable(GL_DEPTH_TEST);

}

void MyGL::GLDrawScene()
{
//    tuple t(1,2,3);
//    scene.mSceneMap.find(t) != scene.mSceneMap.end();
    std::map<tuple, Block*>::iterator iter;
    for (iter = scene.mSceneMap.begin(); iter != scene.mSceneMap.end(); iter++)
    {
        // iter->first is a tuple
        // std::get<index>(tuple) can get the elements in the tuple
        glm::vec3 trans(std::get<0>(iter->first), std::get<1>(iter->first), std::get<2>(iter->first));
        prog_lambert.setModelMatrix(glm::translate(glm::mat4(), trans));
        prog_lambert.draw(geom_cube);
    }
}

//initialize the grid by the
void MyGL::initializeGrid(){
    int x0 = gl_camera.eye.x ,y0 = gl_camera.eye.y ,z0 = gl_camera.eye.z;
    grid.start_pos = glm::vec3(x0 - 32 * 16, y0 - 32 * 16, z0 - 32 * 16);
    memset(grid.cl, 0, sizeof grid.cl);
    std::map<tuple, Block*>::iterator iter;
    for (iter = scene.mSceneMap.begin(); iter != scene.mSceneMap.end(); iter++)
    {
        // iter->first is a tuple
        // std::get<index>(tuple) can get the elements in the tuple
        int x = std::get<0>(iter->first), y = std::get<1>(iter->first), z = std::get<2>(iter->first);
//        std::cout<<x<<" "<<y<<" "<<z<<" "<<"\n";
        if(x >= grid.start_pos[0] && x <= (grid.start_pos[0] + 64 * 16) &&
                y >= grid.start_pos[1] && y <= (grid.start_pos[1] + 64 * 16) &&
                z >= grid.start_pos[2] && z <= (grid.start_pos[2] + 64 * 16)){
            //grid.set(x,y,z,int(1));
            grid.set(x, y, z, (int)(iter->second->mType));
        }
//        glm::vec3 trans(std::get<0>(iter->first), std::get<1>(iter->first), std::get<2>(iter->first));
//        prog_lambert.setModelMatrix(glm::translate(glm::mat4(), trans));
//        prog_lambert.draw(geom_cube);
    }
}
void MyGL::UpdateWhenNewTerrain(std::map<tuple, Block *> &New_map){
    std::map<tuple, Block*>::iterator iter;
    for (iter = New_map.begin(); iter != New_map.end(); iter++)
    {
        // iter->first is a tuple
        // std::get<index>(tuple) can get the elements in the tuple
        int x = std::get<0>(iter->first), y = std::get<1>(iter->first), z = std::get<2>(iter->first);
//        std::cout<<x<<" "<<y<<" "<<z<<" "<<"\n";
        if(x >= grid.start_pos[0] && x <= (grid.start_pos[0] + 64 * 16) &&
                y >= grid.start_pos[1] && y <= (grid.start_pos[1] + 64 * 16) &&
                z >= grid.start_pos[2] && z <= (grid.start_pos[2] + 64 * 16)){
            grid.set(x,y,z,(int)(iter->second->mType));
        }
    }
}


void MyGL::breakblocks(QPoint screen_pos, int distance_max)
{
    float NDC_X=2*float(screen_pos.x())/float(width())-1;
    float NDC_Y=1-2*float(screen_pos.y())/float(height());
    glm::vec3 p=gl_camera.ref+NDC_X*gl_camera.H+NDC_Y*gl_camera.V;
    std::map<tuple, Block*>::iterator iter;
    for(float i=0;i<distance_max;i+=0.05)
    {
        int x=round(p[0]),y=round(p[1]),z=round(p[2]);
        tuple temp(x,y,z);
        iter=scene.mSceneMap.find(temp);
        if(iter!=scene.mSceneMap.end())
        {
            grid.set(x,y,z,0);
            scene.mSceneMap.erase(iter);
            break;
        }

        p+=(0.05f*glm::normalize(gl_camera.ref-gl_camera.eye));
    }
}

void MyGL::addblocks(QPoint screen_pos, int distance_max)
{
    float NDC_X=2*float(screen_pos.x())/float(width())-1;
    float NDC_Y=1-2*float(screen_pos.y())/float(height());
    glm::vec3 p=gl_camera.ref+NDC_X*gl_camera.H+NDC_Y*gl_camera.V;
    std::map<tuple, Block*>::iterator iter;
    for(float i=0;i<distance_max;i+=0.05)
    {
        int x=round(p[0]),y=round(p[1]),z=round(p[2]);
        tuple temp(x,y,z);

        iter=scene.mSceneMap.find(temp);
        if(iter!=scene.mSceneMap.end())
        {
            glm::vec3 cube_center(x,y,z);
            float distance[6];
            distance[0]=p[0]-(cube_center[0]-0.5f);
            distance[1]=(cube_center[0]+0.5f)-p[0];
            distance[2]=p[2]-(cube_center[2]-0.5f);
            distance[3]=(cube_center[2]+0.5f)-p[2];
            distance[4]=p[1]-(cube_center[1]-0.5f);
            distance[5]=(cube_center[1]+0.5f)-p[1];
            float min=distance[0];
            int min_index=0,i=1;
            for(i=1;i<6;i++)
                if(distance[i]<min)
                {
                    min=distance[i];
                    min_index=i;
                }

            Block* pBlock=NULL;
            tuple tempTuple(0,0,0);
            if(min_index==0)
            {
                pBlock = new Block(glm::ivec3(cube_center[0]-1, cube_center[1], cube_center[2]));
                tempTuple=tuple(cube_center[0]-1, cube_center[1], cube_center[2]);
                scene.mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                grid.set(cube_center[0]-1, cube_center[1], cube_center[2],1);
                break;
            }
            if(min_index==1)
            {
                pBlock = new Block(glm::ivec3(cube_center[0]+1, cube_center[1], cube_center[2]));
                tempTuple=tuple(cube_center[0]+1, cube_center[1], cube_center[2]);
                scene.mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                grid.set(cube_center[0]+1, cube_center[1], cube_center[2],1);
                break;
            }
            if(min_index==2)
            {
                pBlock = new Block(glm::ivec3(cube_center[0], cube_center[1], cube_center[2]-1));
                tempTuple=tuple(cube_center[0], cube_center[1], cube_center[2]-1);
                scene.mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                grid.set(cube_center[0], cube_center[1], cube_center[2]-1,1);
                break;
            }
            if(min_index==3)
            {
                pBlock = new Block(glm::ivec3(cube_center[0], cube_center[1], cube_center[2]+1));
                tempTuple=tuple(cube_center[0], cube_center[1], cube_center[2]+1);
                scene.mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                grid.set(cube_center[0], cube_center[1], cube_center[2]+1,1);

                break;
            }
            if(min_index==4)
            {
                //Can't add a cube below the original one
                break;
            }
            if(min_index==5)
            {
                pBlock = new Block(glm::ivec3(cube_center[0], cube_center[1]+1, cube_center[2]));
                tempTuple=tuple(cube_center[0], cube_center[1]+1, cube_center[2]);
                scene.mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                grid.set(cube_center[0], cube_center[1]+1, cube_center[2],1);
                break;
            }
        }
        p+=(0.05f*glm::normalize(gl_camera.ref-gl_camera.eye));
    }

}

bool MyGL::collision_test(int direction,float step)
{
    glm::vec3 pos1,pos2,pos3,pos4,pos5,pos6,p1,p2;
    glm::vec3 forward_direction=glm::normalize(glm::vec3(gl_camera.look[0],0,gl_camera.look[2])),\
            worldup(0,1,0);
    std::map<tuple, Block*>::iterator iter1,iter2,iter3,iter4,iter5,iter6;
    if(direction==LEFT)
    {
        p1=gl_camera.eye+step*gl_camera.right-0.5f*character_size[0]*gl_camera.right+0.5f*character_size[2]*forward_direction;
        p2=gl_camera.eye+step*gl_camera.right-0.5f*character_size[0]*gl_camera.right-0.5f*character_size[2]*forward_direction;
        pos1=p1+(0.25f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos2=p2+(0.25f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos3=p1-0.25f*character_size[1]*worldup;
        pos4=p2-0.25f*character_size[1]*worldup;
        pos5=p1-(0.75f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos6=p2-(0.75f-BODYEDGE_ERROR)*character_size[1]*worldup;
    }
    if(direction==RIGHT)
    {
        p1=gl_camera.eye+step*gl_camera.right+0.5f*character_size[0]*gl_camera.right+0.5f*character_size[2]*forward_direction;
        p2=gl_camera.eye+step*gl_camera.right+0.5f*character_size[0]*gl_camera.right-0.5f*character_size[2]*forward_direction;
        pos1=p1+(0.25f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos2=p2+(0.25f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos3=p1-0.25f*character_size[1]*worldup;
        pos4=p2-0.25f*character_size[1]*worldup;
        pos5=p1-(0.75f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos6=p2-(0.75f-BODYEDGE_ERROR)*character_size[1]*worldup;
    }
    if(direction==BACK)
    {
        p1=gl_camera.eye+step*forward_direction-0.5f*character_size[2]*forward_direction+0.5f*character_size[0]*gl_camera.right;
        p2=gl_camera.eye+step*forward_direction-0.5f*character_size[2]*forward_direction-0.5f*character_size[0]*gl_camera.right;
        pos1=p1+(0.25f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos2=p2+(0.25f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos3=p1-0.25f*character_size[1]*worldup;
        pos4=p2-0.25f*character_size[1]*worldup;
        pos5=p1-(0.75f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos6=p2-(0.75f-BODYEDGE_ERROR)*character_size[1]*worldup;
    }
    if(direction==FORWARD)
    {
        p1=gl_camera.eye+step*forward_direction+0.5f*character_size[2]*forward_direction+0.5f*character_size[0]*gl_camera.right;
        p2=gl_camera.eye+step*forward_direction+0.5f*character_size[2]*forward_direction-0.5f*character_size[0]*gl_camera.right;;
        pos1=p1+(0.25f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos2=p2+(0.25f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos3=p1-0.25f*character_size[1]*worldup;
        pos4=p2-0.25f*character_size[1]*worldup;
        pos5=p1-(0.75f-BODYEDGE_ERROR)*character_size[1]*worldup;
        pos6=p2-(0.75f-BODYEDGE_ERROR)*character_size[1]*worldup;
    }

    tuple temp1(round(pos1[0]),round(pos1[1]),round(pos1[2]))\
            ,temp2(round(pos2[0]),round(pos2[1]),round(pos2[2]))\
            ,temp3(round(pos3[0]),round(pos3[1]),round(pos3[2]))\
            ,temp4(round(pos4[0]),round(pos4[1]),round(pos4[2]))\
            ,temp5(round(pos5[0]),round(pos5[1]),round(pos5[2]))\
            ,temp6(round(pos6[0]),round(pos6[1]),round(pos6[2]));
    iter1=scene.mSceneMap.find(temp1);
    iter2=scene.mSceneMap.find(temp2);
    iter3=scene.mSceneMap.find(temp3);
    iter4=scene.mSceneMap.find(temp4);
    iter5=scene.mSceneMap.find(temp5);
    iter6=scene.mSceneMap.find(temp6);
    if(iter1!=scene.mSceneMap.end()|| iter2!=scene.mSceneMap.end()||\
            iter3!=scene.mSceneMap.end()||iter4!=scene.mSceneMap.end()||\
            iter5!=scene.mSceneMap.end()||iter6!=scene.mSceneMap.end())
        return true;
    else
        return false;

}
bool MyGL::bottom_test()
{
    if(g_velocity>0)
        return false;
    glm::vec3 forward_direction=glm::normalize(glm::vec3(gl_camera.look[0],0,gl_camera.look[2]));
    glm::vec3 pos1,pos2,pos3,pos4;
    std::map<tuple, Block*>::iterator iter1,iter2,iter3,iter4;
    pos1=gl_camera.eye-glm::vec3(0,(0.75f+BODYEDGE_ERROR)*character_size[1],0)+(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_direction\
            -(0.5f-BLOCKEDGE_ERROR)*character_size[0]*gl_camera.right;
    pos2=gl_camera.eye-glm::vec3(0,(0.75f+BODYEDGE_ERROR)*character_size[1],0)+(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_direction\
            +(0.5f-BLOCKEDGE_ERROR)*character_size[0]*gl_camera.right;
    pos3=gl_camera.eye-glm::vec3(0,(0.75f+BODYEDGE_ERROR)*character_size[1],0)-(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_direction\
            -(0.5f-BLOCKEDGE_ERROR)*character_size[0]*gl_camera.right;
    pos4=gl_camera.eye-glm::vec3(0,(0.75f+BODYEDGE_ERROR)*character_size[1],0)-(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_direction\
            +(0.5f-BLOCKEDGE_ERROR)*character_size[0]*gl_camera.right;
    tuple temp1(round(pos1[0]),round(pos1[1]),round(pos1[2]))\
            ,temp2(round(pos2[0]),round(pos2[1]),round(pos2[2]))\
            ,temp3(round(pos3[0]),round(pos3[1]),round(pos3[2]))\
            ,temp4(round(pos4[0]),round(pos4[1]),round(pos4[2]));
    iter1=scene.mSceneMap.find(temp1);
    iter2=scene.mSceneMap.find(temp2);
    iter3=scene.mSceneMap.find(temp3);
    iter4=scene.mSceneMap.find(temp4);

    if(iter1!=scene.mSceneMap.end())
    {
        gl_camera.TranslateAlongWorldUp(round(pos1[1])+2-gl_camera.eye[1]);
        return true;
    }
    else if(iter2!=scene.mSceneMap.end())
    {
        gl_camera.TranslateAlongWorldUp(round(pos2[1])+2-gl_camera.eye[1]);
        return true;
    }
    else if(iter3!=scene.mSceneMap.end())
    {
        gl_camera.TranslateAlongWorldUp(round(pos3[1])+2-gl_camera.eye[1]);
        return true;
    }
    else if(iter4!=scene.mSceneMap.end())
    {
        gl_camera.TranslateAlongWorldUp(round(pos4[1])+2-gl_camera.eye[1]);
        return true;
    }
    else
        return false;
}
bool MyGL::boundarytest()
{
    if(fabs(gl_camera.eye[0])>500||fabs(gl_camera.eye[1])>500||\
            fabs(gl_camera.eye[2])>500)
        return true;
    return false;

}
void MyGL::Keyevents()
{
    float amount=0.25f;
    if(keyboard[0])
        amount=2.0f;
    if(keyboard[1])             //  Key_Escape
        QApplication::quit();
    if(keyboard[2])             //  Key_Right
        gl_camera.RotateAboutUp(-amount);
    if(keyboard[3])             //Key_Left
        gl_camera.RotateAboutUp(amount);
    if(keyboard[4])             //Key_Up
    {
        //gl_camera.RotateAboutRight(-amount);
        if(gl_camera.cameramode==FLYING_MODE)
            gl_camera.TranslateAlongLook(amount);
        else
        {
            if(collision_test(FORWARD,amount))
            {
                if(!jump_state)
                    gl_camera.TranslateAlongLook(-amount);
            }
            else
                gl_camera.TranslateAlongLook(amount);
        }
    }
    if (keyboard[5])            //Key_Down
    {
        //gl_camera.RotateAboutRight(amount);
        if(gl_camera.cameramode==FLYING_MODE)
            gl_camera.TranslateAlongLook(-amount);
        else
        {
            if(collision_test(BACK,-amount))
            {
                if(!jump_state)
                    gl_camera.TranslateAlongLook(amount);
            }
            else
                gl_camera.TranslateAlongLook(-amount);
        }
    }
    if (keyboard[6])            //Key_1
        gl_camera.fovy += amount;
    if (keyboard[7])            //Key_2
        gl_camera.fovy -= amount;
    if (keyboard[10])           //Key_W
    {
        if(gl_camera.cameramode==FLYING_MODE)
            gl_camera.TranslateAlongLook(amount);
        else
        {
            if(collision_test(FORWARD,amount))
            {
                if(!jump_state)
                    gl_camera.TranslateAlongLook(-amount);
            }
            else
                gl_camera.TranslateAlongLook(amount);
        }
    }
    if (keyboard[11])           //Key_S
    {
        if(gl_camera.cameramode==FLYING_MODE)
            gl_camera.TranslateAlongLook(-amount);
        else
        {
            if(collision_test(BACK,-amount))
            {
                if(!jump_state)
                    gl_camera.TranslateAlongLook(amount);
            }
            else
                gl_camera.TranslateAlongLook(-amount);
        }
    }
    if (keyboard[12])           //Key_D
    {
        if(gl_camera.cameramode==FLYING_MODE)
            gl_camera.TranslateAlongRight(amount);
        else
        {
            if(collision_test(RIGHT,amount))
            {
                if(!jump_state)
                    gl_camera.TranslateAlongRight(-amount);
            }
            else
                gl_camera.TranslateAlongRight(amount);
        }
    }
    if (keyboard[13])           //Key_A
    {
        if(gl_camera.cameramode==FLYING_MODE)
            gl_camera.TranslateAlongRight(-amount);
        else
        {
            if(collision_test(LEFT,-amount))
            {
                if(!jump_state)
                    gl_camera.TranslateAlongRight(amount);
            }
            else
                gl_camera.TranslateAlongRight(-amount);
        }
    }
    if(keyboard[14])              //Key_Q
    {
        if(gl_camera.cameramode==FLYING_MODE)
            gl_camera.TranslateAlongUp(-amount);
    }
    if(keyboard[15])              //Key_E
    {
        if(gl_camera.cameramode==FLYING_MODE)
            gl_camera.TranslateAlongUp(amount);
    }
    if(keyboard[16])              //Key_Space
    {
        if(gl_camera.cameramode==WALKING_MODE)
        {
            if(!jump_state)
            {
                g_velocity=2.0f;
                external_force_acceleration=2.0f;
                jump_state=true;
            }
        }
    }
//    if (keyboard[17])             //Key_R
//        gl_camera = Camera(this->width(), this->height(),\
//                           glm::vec3((scene.mMaxXYZ.x - scene.mMinXYZ.x)/2, (scene.mMaxXYZ.y - scene.mMinXYZ.y)/2 + 1, (scene.mMaxXYZ.z - scene.mMinXYZ.z)/2),
//                           glm::vec3((scene.mMaxXYZ.x - scene.mMinXYZ.x)/2, (scene.mMaxXYZ.y - scene.mMinXYZ.y)/2+1, (scene.mMaxXYZ.z - scene.mMinXYZ.z)/2+1),\
//                           glm::vec3(0,1,0));
    gl_camera.RecomputeAttributes();
    update();
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() & Qt::ShiftModifier){
        keyboard[0]=true;

    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    } else if (e->key() == Qt::Key_Escape) {
        keyboard[1]=true;
    } else if (e->key() == Qt::Key_Right) {
        keyboard[2]=true;
    } else if (e->key() == Qt::Key_Left) {
        keyboard[3]=true;
    } else if (e->key() == Qt::Key_Up) {
        keyboard[4]=true;
    } else if (e->key() == Qt::Key_Down) {
        keyboard[5]=true;
    } else if (e->key() == Qt::Key_1) {
        keyboard[6]=true;
    } else if (e->key() == Qt::Key_2) {
        keyboard[7]=true;
    } else if (e->key() == Qt::Key_G) {
        gl_camera.cameramode=WALKING_MODE;
    } else if (e->key() == Qt::Key_F) {
        if(gl_camera.cameramode!=FLYING_MODE)
        {
            gl_camera.takeoff=true;
            gl_camera.cameramode=FLYING_MODE;
        }
    } else if (e->key() == Qt::Key_W) {
        keyboard[10]=true;
    } else if (e->key() == Qt::Key_S) {
        keyboard[11]=true;
    } else if (e->key() == Qt::Key_D) {
        keyboard[12]=true;
    } else if (e->key() == Qt::Key_A) {
        keyboard[13]=true;
    } else if (e->key() == Qt::Key_Q) {
        keyboard[14]=true;
    } else if (e->key() == Qt::Key_E) {
        keyboard[15]=true;
    } else if(e->key()==Qt::Key_Space){
        keyboard[16]=true;
    } else if (e->key() == Qt::Key_R) {
        keyboard[17]=true;
    }
}
void MyGL::mouseMoveEvent(QMouseEvent *event)
{     
    if(!mousemove)
    {
        mousemove=true;
        mouse_oldpos=event->pos();
        return;
    }
    if(mouse_oldpos.x()==width()-1||mouse_oldpos.x()==0\
            ||mouse_oldpos.y()==height()-1||mouse_oldpos.y()==0)
    {
        QCursor::setPos(width()/2,height()/2);
        mouse_oldpos=QPoint(width()/2,height()/2);
        return;
    }
    QPoint p=event->pos()-mouse_oldpos;
    mouse_oldpos=event->pos();
    float delta_w=2*float(p.x())/float(width())*glm::length(gl_camera.H);
    float delta_h=2*float(p.y())/float(height())*glm::length(gl_camera.V);
    float theta=atan(delta_w/glm::length(gl_camera.ref-gl_camera.eye))*180/M_PI;
    float fai=atan(delta_h/glm::length(gl_camera.ref-gl_camera.eye))*180/M_PI;
    gl_camera.RotateAboutUp(-theta);
    gl_camera.RotateAboutRight(-fai);
    gl_camera.RecomputeAttributes();
    //update();

    //printf("%f %f %d %f\n", gl_camera.ref.x, gl_camera.ref.z, scene.mMinXYZ.z, fabs(gl_camera.ref.z - scene.mMinXYZ.z));
}
void MyGL::keyReleaseEvent(QKeyEvent *e)
{
    if(e->modifiers() & Qt::ShiftModifier){
        keyboard[0]=false;

    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    } else if (e->key() == Qt::Key_Escape) {
        keyboard[1]=false;
    } else if (e->key() == Qt::Key_Right) {
        keyboard[2]=false;
    } else if (e->key() == Qt::Key_Left) {
        keyboard[3]=false;
    } else if (e->key() == Qt::Key_Up) {
        keyboard[4]=false;
    } else if (e->key() == Qt::Key_Down) {
        keyboard[5]=false;
    } else if (e->key() == Qt::Key_1) {
        keyboard[6]=false;
    } else if (e->key() == Qt::Key_2) {
        keyboard[7]=false;
    } else if (e->key() == Qt::Key_G) {
        keyboard[8]=false;
    } else if (e->key() == Qt::Key_F) {
        keyboard[9]=false;
    } else if (e->key() == Qt::Key_W) {
        keyboard[10]=false;
    } else if (e->key() == Qt::Key_S) {
        keyboard[11]=false;
    } else if (e->key() == Qt::Key_D) {
        keyboard[12]=false;
    } else if (e->key() == Qt::Key_A) {
        keyboard[13]=false;
    } else if (e->key() == Qt::Key_Q) {
        keyboard[14]=false;
    } else if (e->key() == Qt::Key_E) {
        keyboard[15]=false;
    } else if(e->key()==Qt::Key_Space){
        keyboard[16]=false;
    } else if (e->key() == Qt::Key_R) {
        keyboard[17]=false;
    }

}

void MyGL::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        breakblocks(QPoint(width()/2,height()/2),MaxReachDistance);
        update();
    }
    if(event->button()==Qt::RightButton)
    {
        addblocks(QPoint(width()/2,height()/2),MaxReachDistance);
        update();
    }

}


void MyGL::timerUpdate()
{
    if(!game_begin)
        return;
    timeCount = (++timeCount) % 150;
    if (timeCount % 150 == 0)
    {
        if (gl_camera.cameramode!=FLYING_MODE)
        {
            if (fabs(gl_camera.eye.x - scene.mMinXYZ.x) < scene.mRefreshDistance)
            {
               // printf("0\n");
                scene.GenerateBlocks(0);
                UpdateWhenNewTerrain(scene.New_map);
            }
            else if (fabs(gl_camera.eye.x - scene.mMaxXYZ.x) < scene.mRefreshDistance)
            {
                //printf("%f %f %f\n", gl_camera.eye.x, scene.mMaxXYZ.x, scene.mRefreshDistance);
                scene.GenerateBlocks(1);
                UpdateWhenNewTerrain(scene.New_map);

            }
            else if (fabs(gl_camera.eye.z - scene.mMinXYZ.z) < scene.mRefreshDistance)
            {
                //printf("2\n");
                scene.GenerateBlocks(2);
                UpdateWhenNewTerrain(scene.New_map);

            }
            else if (fabs(gl_camera.eye.z - scene.mMaxXYZ.z) < scene.mRefreshDistance)
            {
                //printf("3\n");
                scene.GenerateBlocks(3);
                UpdateWhenNewTerrain(scene.New_map);

            }
            //printf("x:%f z:%f\n", gl_camera.eye.x, gl_camera.eye.z);
            //Test whether need to update the superchunk
//            if(gl_camera.eye.x - grid.start_pos[0] > 33 * 16){
//                // +x out of bounds
//        //        std::cout<<"0\n";
//                grid.MoveUpdate(0, scene.mSceneMap);
//                this->update();
//            }
//            else if(gl_camera.eye.x - grid.start_pos[0] < 32 * 16){
//                // -x out of bounds
//        //        std::cout<<"1\n";
//                grid.MoveUpdate(1, scene.mSceneMap);
//                this->update();
//            }
//            else if(gl_camera.eye.y - grid.start_pos[1] > 33 * 16){
//                // +y out of bounds
//        //        std::cout<<"2\n";
//                grid.MoveUpdate(2, scene.mSceneMap);
//                this->update();
//            }
//            else if(gl_camera.eye.y - grid.start_pos[1] < 32 * 16){
//        //        std::cout<<"3\n";
//                // -y out of bounds
//                grid.MoveUpdate(3, scene.mSceneMap);
//                this->update();
//            }
//            else if(gl_camera.eye.z - grid.start_pos[2] > 33 * 16){
//                // +z out of bounds
//        //        std::cout<<"4\n";
//                grid.MoveUpdate(4, scene.mSceneMap);
//                this->update();

//            }
//            else if(gl_camera.eye.z - grid.start_pos[2] < 32 * 16){
//                // -z out of bounds
//        //        std::cout<<"5\n";
//                grid.MoveUpdate(5, scene.mSceneMap);
//                this->update();
//            }
        }
    }
    prog_new.setTime(timeCount);

    if(gl_camera.takeoff)
    {
        if(gl_camera.takeoff_time>1.0f)
        {
            gl_camera.takeoff=false;
            gl_camera.takeoff_time=0;
        }
        else
            gl_camera.TranslateAlongUp(1.0f/20.0f);

        gl_camera.takeoff_time+=1.0f/60.0f;
    }


    Keyevents();
    if(gl_camera.cameramode==FLYING_MODE)
        return;

    if(boundarytest())
    {
        QMessageBox::information(NULL,"Note","Falling out of the boundary!");
        QApplication::quit();
    }
    float distance=g_velocity*(time_step)+0.5*(gravity_acceleration+external_force_acceleration)*time_step*time_step;
    if(distance>1)
        distance=1;
    gl_camera.TranslateAlongWorldUp(distance);
    update();
    if(bottom_test())
    {
        if(g_velocity<=0)
        {
            g_velocity=0;
            external_force_acceleration=-gravity_acceleration;
            if(!keyboard[16])
                jump_state=false;
        }
    }
    else
    {
        g_velocity+=gravity_acceleration*(time_step);
        external_force_acceleration=0;
    }
}


void MyGL::test(){
//    int width, height;
//    unsigned char* image = SOIL_load_image("F:/QT_project/Final_Project_Minicraft/miniminecraft/minecraft_textures_all/minecraft_textures_all.png",
//                                           &width, &height, 0, SOIL_LOAD_RGB);
//    printf("SOIL results: '%s'\n", SOIL_last_result());
}
