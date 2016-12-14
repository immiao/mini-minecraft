#ifndef MYGL_H
#define MYGL_H

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cube.h>
#include "camera.h"
#include <direction.h>
#include <scene/scene.h>
#include<scene/riversystem.h>
#include <scene/screen_center.h>
#include <scene/screen_triangles.h>
#include<scene/fireblocks.h>
#include<scene/digit.h>
#include<scene/itemlist.h>
#include<QSound>
#include <QSoundEffect>
#include <chunk.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <scene/skybox.h>
#include <scene/particle.h>

#define BODYEDGE_ERROR 0.05f
#define BLOCKEDGE_ERROR 0.02f
#define PICKMODE 0
#define COLLECTMODE 1
const float gravity_acceleration=-1.5f;
const float time_step=1.0f/15.0f;
class MyGL
    : public GLWidget277
{
private:
    Cube geom_cube;// The instance of a unit cube we can use to render any cube. Should NOT be used in final version of your project.
    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram prog_new;
    ShaderProgram prog_shadow;

    ShaderProgram prog_skybox;
    ShaderProgram prog_particle;
    Particle rainParticle;

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera gl_camera;
    Scene scene;
    RiverSystem Rivers;
    superchunk grid;
    Screen_Center center;
    Screen_Triangles T;

    //ItemList
    std::vector<ItemList*> ItemMenu;
    std::vector<ItemList*> Item;
    std::vector<ItemList*> ItemNumber;
    //Special blocks
    std::vector<glm::vec3> waterblocks;
    std::vector<Fire> Fireblocks;

    //Sound
    QSound StepSound;
    QSound WaterStepSound;
    QSound Ground;
    QSound WaterGround;
    QSoundEffect RiverSound;
    QSoundEffect FireSound;


    // skybox
    Skybox skybox;
    Camera gl_skyboxCamera;

    QPoint mouse_oldpos;
    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;


    glm::vec3 character_size;//used to define the size of character on the 3 directions
    BLOCK_TYPE Step_type;
    BLOCK_TYPE block_type;
    BLOCK_TYPE block_type_Origin;

    Digit Resource_Digit_Num[15];
    int Resource_Num[15];
    int Fetch_Mode;

    bool game_begin;
    bool mousemove;
    bool jump_state;
    bool keyboard[25];

    float external_force_acceleration;
    float g_velocity;

    int timeCount = 0;
    int Firecount=0;
    int Daytime = 0;
    int OpenDNcycle = 0;

    float fTime = 0.f;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void GLDrawScene();
    void initializeGrid();
    void UpdateWhenNewTerrain(std::map<tuple, Block *> &New_map);


    void breakblocks(QPoint screen_pos,int distance_max,int Mode);
    void addblocks(QPoint screen_pos,int distance_max,BLOCK_TYPE type);

    void CurrentItemChange();
    void FireSpread();
    void playvoice();
    void Keyevents();

    bool boundarytest();
    bool collision_test(int direction,float step);
    bool bottom_test();

    double WaterDistanceClose();
    double FireDistanceClose();


protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    /// Slot that gets called ~60 times per second
    virtual void timerUpdate();
};


#endif // MYGL_H
