#ifndef MYGL_H
#define MYGL_H

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cube.h>
#include "camera.h"
#include <scene/scene.h>
#include <scene/screen_center.h>
#include <scene/screen_triangles.h>
#include <chunk.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#define LEFT 0
#define RIGHT 1
#define FORWARD 2
#define BACK 3


const float gravity_acceleration=-0.98f;
const float time_step=1.0f/15.0f;
class MyGL
    : public GLWidget277
{
private:
    Cube geom_cube;// The instance of a unit cube we can use to render any cube. Should NOT be used in final version of your project.
    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram prog_new;
    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera gl_camera;
    Scene scene;
    superchunk grid;
    Screen_Center center;
    Screen_Triangles T;
    QPoint mouse_oldpos;
    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;

    glm::vec3 character_size;//used to define the size of character on the 3 directions

    bool game_begin;
    bool mousemove;
    bool jump_state;
    bool keyboard[25];

    float external_force_acceleration;
    float g_velocity;




public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void GLDrawScene();
    void initializeGrid();
    void UpdateWhenNewTerrain(std::map<tuple, Block *> New_map);

    void breakblocks(QPoint screen_pos,int distance_max);
    void addblocks(QPoint screen_pos,int distance_max);

    void Keyevents();

    bool boundarytest();
    bool collision_test(int direction,float step);
    bool bottom_test();


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
