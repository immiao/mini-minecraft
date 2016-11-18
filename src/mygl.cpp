#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      geom_cube(this),
      prog_lambert(this), prog_flat(this), prog_new(this),
      grid(this),
      gl_camera()
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
    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);

    scene.Create();
    initializeGrid();

}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
//    gl_camera = Camera(w, h);
    gl_camera = Camera(w, h, glm::vec3((scene.mMaxXYZ.x - scene.mMinXYZ.x)/2, (scene.mMaxXYZ.y - scene.mMinXYZ.y)/2 + 2, (scene.mMaxXYZ.z - scene.mMinXYZ.z)/2),
                       glm::vec3((scene.mMaxXYZ.x - scene.mMinXYZ.x)/2, (scene.mMaxXYZ.y - scene.mMinXYZ.y)/2+2, (scene.mMaxXYZ.z - scene.mMinXYZ.z)/2+1), glm::vec3(0,1,0));
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

    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());
    prog_new.setViewProjMatrix(gl_camera.getViewProj());
    grid.render(prog_new);

    //GLDrawScene();
}

void MyGL::GLDrawScene()
{
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
            grid.set(x,y,z,int(1));
        }
//        glm::vec3 trans(std::get<0>(iter->first), std::get<1>(iter->first), std::get<2>(iter->first));
//        prog_lambert.setModelMatrix(glm::translate(glm::mat4(), trans));
//        prog_lambert.draw(geom_cube);
    }
}

void MyGL::keyPressEvent(QKeyEvent *e)
{

    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        gl_camera = Camera(this->width(), this->height());
    }
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things

    //printf("%f %f %d %f\n", gl_camera.ref.x, gl_camera.ref.z, scene.mMinXYZ.z, fabs(gl_camera.ref.z - scene.mMinXYZ.z));
    if (fabs(gl_camera.ref.x - scene.mMinXYZ.x) < scene.mRefreshDistance)
    {
        //printf("0\n");
        scene.GenerateBlocks(0);
        initializeGrid();
    }
    else if (fabs(gl_camera.ref.x - scene.mMaxXYZ.x) < scene.mRefreshDistance)
    {
        //printf("1\n");
        scene.GenerateBlocks(1);
        initializeGrid();

    }
    else if (fabs(gl_camera.ref.z - scene.mMinXYZ.z) < scene.mRefreshDistance)
    {
        //printf("2\n");
        scene.GenerateBlocks(2);
        initializeGrid();

    }
    else if (fabs(gl_camera.ref.z - scene.mMaxXYZ.z) < scene.mRefreshDistance)
    {
        //printf("3\n");
        scene.GenerateBlocks(3);
        initializeGrid();

    }
    //Test whether need to update the superchunk
    if(gl_camera.eye.x - grid.start_pos[0] > 33 * 16){
        // +x out of bounds
        std::cout<<"0\n";
        grid.MoveUpdate(0, scene.mSceneMap);
        this->update();
    }
    else if(gl_camera.eye.x - grid.start_pos[0] < 32 * 16){
        // -x out of bounds
        std::cout<<"1\n";
        grid.MoveUpdate(1, scene.mSceneMap);
        this->update();
    }
    else if(gl_camera.eye.y - grid.start_pos[1] > 33 * 16){
        // +y out of bounds
        std::cout<<"2\n";
        grid.MoveUpdate(2, scene.mSceneMap);
        this->update();
    }
    else if(gl_camera.eye.y - grid.start_pos[1] < 32 * 16){
        std::cout<<"3\n";
        // -y out of bounds
        grid.MoveUpdate(3, scene.mSceneMap);
        this->update();
    }
    else if(gl_camera.eye.z - grid.start_pos[2] > 33 * 16){
        // +z out of bounds
        std::cout<<"4\n";
        grid.MoveUpdate(4, scene.mSceneMap);
        this->update();

    }
    else if(gl_camera.eye.z - grid.start_pos[2] < 32 * 16){
        // -z out of bounds
        std::cout<<"5\n";
        grid.MoveUpdate(5, scene.mSceneMap);
        this->update();

    }
}

void MyGL::timerUpdate()
{
}
