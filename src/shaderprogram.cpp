#include "shaderprogram.h"
#include <QFile>
#include <QStringBuilder>
#include <iostream>
#include <SOIL.h>

ShaderProgram::ShaderProgram(GLWidget277 *context)
    : vertShader(), fragShader(), prog(),
      attrPos(-1), attrNor(-1), attrCol(-1),
      unifModel(-1), unifModelInvTr(-1), unifViewProj(-1), unifColor(-1), unifTexture(-1), unifTime(-1),unifLightSpaceMatrix(-1),unifBiasMatrix(-1),
      context(context)
{}

void ShaderProgram::create(const char *vertfile, const char *fragfile)
{
    // Allocate space on our GPU for a vertex shader and a fragment shader and a shader program to manage the two
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    prog = context->glCreateProgram();
    // Get the body of text stored in our two .glsl files
    QString qVertSource = qTextFileRead(vertfile);
    QString qFragSource = qTextFileRead(fragfile);

    char* vertSource = new char[qVertSource.size()+1];
    strcpy(vertSource, qVertSource.toStdString().c_str());
    char* fragSource = new char[qFragSource.size()+1];
    strcpy(fragSource, qFragSource.toStdString().c_str());


    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    context->glShaderSource(vertShader, 1, &vertSource, 0);
    context->glShaderSource(fragShader, 1, &fragSource, 0);
    // Tell OpenGL to compile the shader text stored above
    context->glCompileShader(vertShader);
    context->glCompileShader(fragShader);
    // Check if everything compiled OK
    GLint compiled;
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(vertShader);
    }
    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(fragShader);
    }

    // Tell prog that it manages these particular vertex and fragment shaders
    context->glAttachShader(prog, vertShader);
    context->glAttachShader(prog, fragShader);
    context->glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    context->glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(prog);
    }

    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor = context->glGetAttribLocation(prog, "vs_Nor");
    attrCol = context->glGetAttribLocation(prog, "vs_Col");

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifColor      = context->glGetUniformLocation(prog, "u_Color");

    unifTexture    = context->glGetUniformLocation(prog, "u_texture");
    unifNormalMap  = context->glGetUniformLocation(prog, "u_texture_normal_map");
    unifCosinePower= context->glGetUniformLocation(prog, "u_cosine_power_map");
    unifDepthMap   = context->glGetUniformLocation(prog, "u_depth_map");

    unifViewPos    = context->glGetUniformLocation(prog, "u_ViewPos");
    unifTime       = context->glGetUniformLocation(prog, "u_Time");
    unifDaytime    = context->glGetUniformLocation(prog, "u_DayTime");
    unifOpenDNcycle= context->glGetUniformLocation(prog, "u_OpenDNcycle");

    unifLightSpaceMatrix = context->glGetUniformLocation(prog, "u_lightSpaceMatrix");
    unifBiasMatrix = context->glGetUniformLocation(prog, "u_biasMatrix");
    unsigned char* img0 = SOIL_load_image("../miniminecraft/minecraft_textures_all/minecraft_textures_all.png",
                                           &width0, &height0, 0, SOIL_LOAD_RGB);
    image0 = img0;
    unsigned char* img1 = SOIL_load_image("../miniminecraft/minecraft_textures_all/minecraft_normals_all.png",
                                           &width1, &height1, 0, SOIL_LOAD_RGB);
    image1 = img1;
    unsigned char* img2 = SOIL_load_image("../miniminecraft/minecraft_textures_all/minecraft_cosine_power_all.png",
                                           &width2, &height2, 0, SOIL_LOAD_RGB);
    image2 = img2;
}

void ShaderProgram::useMe()
{
    context->glUseProgram(prog);
}

void ShaderProgram::setModelMatrix(const glm::mat4 &model)
{
    useMe();

    if (unifModel != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModel,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &model[0][0]);
    }

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::inverse(glm::transpose(model));
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModelInvTr,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &modelinvtr[0][0]);
    }
}

void ShaderProgram::setViewProjMatrix(const glm::mat4 &vp)
{
    // Tell OpenGL to use this shader program for subsequent function calls
    useMe();

    if(unifViewProj != -1) {
    // Pass a 4x4 matrix into a uniform variable in our shader
                    // Handle to the matrix variable on the GPU
    context->glUniformMatrix4fv(unifViewProj,
                    // How many matrices to pass
                       1,
                    // Transpose the matrix? OpenGL uses column-major, so no.
                       GL_FALSE,
                    // Pointer to the first element of the matrix
                       &vp[0][0]);
    }
}

void ShaderProgram::setGeometryColor(glm::vec4 color)
{
    useMe();

    if(unifColor != -1)
    {
        context->glUniform4fv(unifColor, 1, &color[0]);
    }
}

void ShaderProgram::setViewPos(glm::vec3 pos){
    useMe();

    if(unifViewPos != -1){
        context->glUniform3fv(unifViewPos, 1, &pos[0]);
    }
}

//This function, as its name implies, uses the passed in GL widget
void ShaderProgram::draw(Drawable &d)
{
        useMe();

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

        // Remember, by calling bindPos(), we call
        // glBindBuffer on the Drawable's VBO for vertex position,
        // meaning that glVertexAttribPointer associates vs_Pos
        // (referred to by attrPos) with that VBO
    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();

    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);

    context->printGLErrorLog();
}

char* ShaderProgram::textFileRead(const char* fileName) {
    char* text;

    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

QString ShaderProgram::qTextFileRead(const char *fileName)
{
    QString text;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        text = in.readAll();
        text.append('\0');
    }
    return text;
}

void ShaderProgram::printShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        qDebug() << "ShaderInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

void ShaderProgram::printLinkInfoLog(int prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        qDebug() << "LinkInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }
}

void ShaderProgram::setTexture(GLuint depthMapHandle){
    useMe();
    context->glGenTextures(1, &textureHandle);
    context->glActiveTexture(GL_TEXTURE0);
    context->glBindTexture(GL_TEXTURE_2D, textureHandle);

//    int width, height;
    //We're missing dynamic library right now.
//    unsigned char* image = SOIL_load_image("F:/QT_project/Final_Project_Minicraft/miniminecraft/minecraft_textures_all/minecraft_textures_all.png",
//                                           &width, &height, 0, SOIL_LOAD_RGB);
//    printf("SOIL results: '%s'\n", SOIL_last_result());
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width0, height0, 0, GL_RGB, GL_UNSIGNED_BYTE, image0);
    context->glGenerateMipmap(GL_TEXTURE_2D);
    context->glUniform1i(unifTexture, 0);

    //normal map texture:
    context->glGenTextures(1, &normalmapHandle);
    context->glActiveTexture(GL_TEXTURE1);
    context->glBindTexture(GL_TEXTURE_2D, normalmapHandle);

//    int width1, height1;
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
    context->glGenerateMipmap(GL_TEXTURE_2D);
    context->glUniform1i(unifNormalMap, 1);

    //cosine_power texture:
    context->glGenTextures(1, &cosine_powerHandle);
    context->glActiveTexture(GL_TEXTURE2);
    context->glBindTexture(GL_TEXTURE_2D, cosine_powerHandle);

//    int width2, height2;
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
    context->glGenerateMipmap(GL_TEXTURE_2D);
    context->glUniform1i(unifCosinePower, 2);

    //depth_map:
    context->glActiveTexture(GL_TEXTURE3);
    context->glBindTexture(GL_TEXTURE_2D, depthMapHandle);
    context->glUniform1i(unifDepthMap, 3);
}

void ShaderProgram::deleteTexture(GLuint depthMapHandle){
    useMe();
    context->glDeleteTextures(1, &textureHandle);
    context->glDeleteTextures(1, &normalmapHandle);
    context->glDeleteTextures(1, &cosine_powerHandle);
    context->glDeleteTextures(1, &depthMapHandle);
}

void ShaderProgram::setTime(int timeCount){
    useMe();
    GLint v0 = timeCount;
    context->glUniform1i(unifTime, v0);
}

void ShaderProgram::setShadowTexture(){
    useMe();
    SHADOW_WIDTH = 1920*4;
    SHADOW_HEIGHT = 1080*4;
    //First we'll create a framebuffer object for rendering the depth map
    context->glGenFramebuffers(1, &depthMapFBO);
    context->glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    //Next we create a 2D texture that we'll use as the framebuffer's depth buffer
    context->glGenTextures(1, &depthMap);
    context->glBindTexture(GL_TEXTURE_2D, depthMap);
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 0.0, 0.0, 0.0, 0.0 };
    context->glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //With the generated depth texture we can attach it as the framebuffer's depth buffer
    context->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);

    context->glDrawBuffer(GL_NONE);
    context->glReadBuffer(GL_NONE);

    bool flag = (context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE);
    context->glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void ShaderProgram::setShadowBias_PVmatrix(int Daytime){
         // Send our transformation to the currently bound shader,
         // in the "MVP" uniform
        context->glUniformMatrix4fv(unifLightSpaceMatrix, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
        context->glUniform1i(unifDaytime, Daytime);
}

void ShaderProgram::ComputeLightPVMatrix(int Daytime){
    //    The MVP matrix used to render the scene from the light’s point of view is computed as follows :

    //    The Projection matrix is an orthographic matrix which will encompass everything in the axis-aligned box (-10,10),(-10,10),(-10,20) on the X,Y and Z axes respectively. These values are made so that our entire *visible *scene is always visible ; more on this in the Going Further section.
    //    The View matrix rotates the world so that in camera space, the light direction is -Z (would you like to re-read Tutorial 3 ?)
    //    The Model matrix is whatever you want.
        GLfloat near_plane = -10.0f, far_plane = 200.0f;
        GLfloat width = 70.0f;
        glm::mat4 lightProjection = glm::ortho(-width, width, -width, width, near_plane, far_plane);

        if(Daytime < 625){
            //Day to Night:
            //set the skycolor
            glm::vec3 sky_color(0.37,0.74,1.0);
            //night is 0.1, 0.1, 0.4375
            float deltax = 0.32 / 625.0, deltay = 0.69 / 625.0, deltaz = 0.7825 / 625.0;
            sky_color.x -= deltax * float(Daytime);
            sky_color.y -= deltay * float(Daytime);
            sky_color.z -= deltaz * float(Daytime);
            context->glClearColor(float(sky_color.x), float(sky_color.y), float(sky_color.z), 1.0);

            //Compute the light direction(ViewMatrix):
            int HalfDaytime = Daytime / 2;
            glm::mat4 lightView = glm::lookAt(glm::vec3( 50.0f - 100.0f/312.0f * HalfDaytime, 50.0f,  50.0f - 100.0f/312.0f * HalfDaytime),
                                              glm::vec3( 0.0f, 0.0f,  0.0f),
                                              glm::vec3( 0.0f, 1.0f,  0.0f));
            lightSpaceMatrix = lightProjection * lightView;
        }
        else{
            //Night to Day
            glm::vec3 sky_color(0.05,0.05,0.2175);
            //night is 0.1, 0.1, 0.4375
            float deltax = 0.32 / 625.0, deltay = 0.69 / 625.0, deltaz = 0.7825 / 625.0;
            sky_color.x += deltax * float(Daytime-625);
            sky_color.y += deltay * float(Daytime-625);
            sky_color.z += deltaz * float(Daytime-625);
            context->glClearColor(float(sky_color.x), float(sky_color.y), float(sky_color.z), 1.0);

            //Compute the light direction(ViewMatrix):
            int HalfDaytime = Daytime / 2;
            glm::mat4 lightView = glm::lookAt(glm::vec3( -50.0f + 100.0f/312.0f * (HalfDaytime - 312), 50.0f,  -50.0f + 100.0f/312.0f * (HalfDaytime-312)),
                                              glm::vec3( 0.0f, 0.0f,  0.0f),
                                              glm::vec3( 0.0f, 1.0f,  0.0f));
            lightSpaceMatrix = lightProjection * lightView;
        }
}

void ShaderProgram::setDNcycle(int OpenDNcycle){
    context->glUniform1i(unifOpenDNcycle ,OpenDNcycle);
}
