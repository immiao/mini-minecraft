#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <glwidget277.h>
#include <la.h>
#include <glm/glm.hpp>

#include "drawable.h"
#include <scene/particle.h>

class ShaderProgram
{
public:
    GLuint vertShader; // A handle for the vertex shader stored in this shader program
    GLuint fragShader; // A handle for the fragment shader stored in this shader program
    GLuint geomShader;
    GLuint prog;       // A handle for the linked shader program stored in this class

    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader
    int attrUv;

    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifColor; // A handle for the "uniform" vec4 representing color of geometry in the vertex shader

    int unifTextureFlag;
    int unifFlatTexture;

    int unifTexture;
    int unifNormalMap;
    int unifCosinePower;
    int unifDepthMap;

    int unifViewPos;
    int unifTime;
    int unifDaytime;
    int unifOpenDNcycle;
//  shadow mapping
    int unifLightSpaceMatrix;
    int unifBiasMatrix;


    GLuint textureHandle;
    GLuint normalmapHandle;
    GLuint cosine_powerHandle;

    //Shadow mapping
    GLuint depthMapFBO;
    GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    GLuint depthMap;

    //texture = 0, normal_map = 1;
    int width0, height0, width1, height1, width2, height2,width3, height3;
    unsigned char *image0, *image1, *image2,*image3;
    glm::mat4 lightSpaceMatrix;

    // skybox
    unsigned char *up, *down, *front, *back, *left, *right;
    int w0,w1,w2,w3,w4,w5;
    int h0,h1,h2,h3,h4,h5;
    GLuint m_skyboxTexture;
    int unifSkyboxTexture;
    int unifSkyColorFactor;

    // particle
    int unifCameraPos;

public:
    ShaderProgram(GLWidget277* context);
    // Sets up the requisite GL data and shaders from the given .glsl files
    void create(const char *vertfile, const char *fragfile);
    void create(const char *vertfile, const char *geomfile, const char* fragfile);

    // Tells our OpenGL context to use this shader to draw things
    void useMe();
    // Pass the given model matrix to this shader on the GPU
    void setModelMatrix(const glm::mat4 &model);
    // Pass the given Projection * View matrix to this shader on the GPU
    void setViewProjMatrix(const glm::mat4 &vp);
    // Pass the given color to this shader on the GPU
    void setGeometryColor(glm::vec4 color);
    //
    void setViewPos(glm::vec3 pos);
    // Draw the given object to our screen using this ShaderProgram's shaders
    void draw(Drawable &d);
    void drawParticle(Particle &d);
    // Utility function used in create()
    char* textFileRead(const char*);
    // Utility function that prints any shader compilation errors to the console
    void printShaderInfoLog(int shader);
    // Utility function that prints any shader linking errors to the console
    void printLinkInfoLog(int prog);

    void setTextureFlag(float Flag);
    void setFlatTexture(int mode);

    void setTexture(GLuint depthMapHandle);
    void deleteTexture(GLuint depthMapHandle);

    void setTime(int timeCount);

    //Shadow mapping
    void setShadowTexture();
    void setShadowBias_PVmatrix(int Daytime);
    float ComputeLightPVMatrix(int Daytime);
    void setDNcycle(int OpenDNcycle);
    QString qTextFileRead(const char*);

    // skybox
    void initSkyBox();
    void setSkyboxTexture();
    void setSkyColorFactor(float factor);

    // particle
    void setCameraPos(glm::vec3 pos, float t);

public:
    GLWidget277* context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};


#endif // SHADERPROGRAM_H
