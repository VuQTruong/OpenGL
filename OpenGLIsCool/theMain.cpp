#include "globalOpenGLStuff.h"

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <fstream>
#include <string>

#include "cShaderManager.h"
#include "cVAOManager.h"

glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, -1.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);

//static const struct sVertex
//{
//    float x, y, z;
//    float r, g, b;
//};
//
//int g_numOfVertices = 0;
//sVertex* g_pVertexBuffer = 0; //Set the buffer to null

//sVertext vertices[6] =
//{
//    { -0.6f, -0.4f, 0.0f, 1.f, 0.f, 0.f },
//    {  0.6f, -0.4f, 0.0f, 0.f, 1.f, 0.f },
//    {  0.0f,  0.6f, 0.0f, 0.f, 0.f, 1.f },
//    { -0.6f,  0.4f, 0.0f, 1.f, 0.f, 0.f },
//    {  0.6f,  0.4f, 0.0f, 0.f, 1.f, 0.f },
//    {  0.0f,  1.6f, 0.0f, 1.f, 0.f, 0.f }
//};

//static const char* vertex_shader_text =
//"#version 110\n"
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"
//"attribute vec3 vPos;\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_Position = MVP * vec4(vPos, 1.0);\n"
//"    color = vCol;\n"
//"}\n";

//static const char* fragment_shader_text =
//"#version 110\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_FragColor = vec4(color, 1.0);\n"
//"}\n";

//Create a global cShaderManager pointer
cShaderManager* g_pShaderManager = 0;   //0 == NULL

cVAOManager * g_pVAOManager = 0;

bool g_isWireFrame = false;

//static void ProcessingGraphicFile(std::string fileName) 
//{
//    std::ifstream file(fileName.c_str());
//
//    if (!file.is_open()) {
//        //There was something wrong
//        std::cout << "Unable to load the file" << std::endl;
//        return;
//    }
//
//    
//    std::string temp;
//    //Looking for the word "vertex"
//    while (true) {
//        file >> temp;
//        if (temp == "vertex") {
//            break;
//        }
//    }
//    
//    file >> ::g_numOfVertices;
//    std::cout << ::g_numOfVertices << std::endl;
//
//    //Allocate memory for pVertexBuffer 
//    ::g_pVertexBuffer = new sVertex[::g_numOfVertices];
//
//    //Looking for the word "face"
//    while (true) {
//        file >> temp;
//        if (temp == "face") {
//            break;
//        }
//    }
//    int numOfTriangles = 0;
//    file >> numOfTriangles;
//    std::cout << numOfTriangles << std::endl;
//
//    //Looking fro the word "end_header"
//    while (true) {
//        file >> temp;
//        if (temp == "end_header") {
//            break;
//        }
//    }
//
//    float x, y, z, r, g, b, a;
//
//    for (int index = 0; index < ::g_numOfVertices; index++) {
//        file >> x >> y >> z >> r >> g >> b >> a;
//
//        ::g_pVertexBuffer[index].x = x;
//        ::g_pVertexBuffer[index].y = y;
//        ::g_pVertexBuffer[index].z = z;
//        ::g_pVertexBuffer[index].r = r / 255.0f;
//        ::g_pVertexBuffer[index].g = g / 255.0f;
//        ::g_pVertexBuffer[index].b = b / 255.0f;
//    }
//
//    return;
//}













static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}















static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float const CAMERA_SPEED = 0.01f;
    if (key == GLFW_KEY_A) {    //Move left
        ::g_cameraEye.x -= CAMERA_SPEED;
    }

    if (key == GLFW_KEY_D) {    //Move right
        ::g_cameraEye.x += CAMERA_SPEED;
    }

    if (key == GLFW_KEY_W) {    //Move foward
        ::g_cameraEye.z += CAMERA_SPEED;
    }

    if (key == GLFW_KEY_S) {    //Move backward
        ::g_cameraEye.z -= CAMERA_SPEED;
    }

    if (key == GLFW_KEY_Q) {    //Move up
        ::g_cameraEye.y -= CAMERA_SPEED;
    }

    if (key == GLFW_KEY_E) {    //Move down
        ::g_cameraEye.y += CAMERA_SPEED;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    // Switches from wireframe to solid (glPolygonMode)
    if (key == GLFW_KEY_9 && action == GLFW_PRESS) { ::g_isWireFrame = true; }
    if (key == GLFW_KEY_0 && action == GLFW_PRESS) { ::g_isWireFrame = false; }

    std::cout << "cam: " << 
        ::g_cameraEye.x << " " <<
        ::g_cameraEye.y << " " <<
        ::g_cameraEye.z << std::endl;

    return;
}















int main(void)
{
    //ProcessingGraphicFile("assets/models/bun_zipper_res4_xyz_colour.ply");

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(600, 800, "Polygon example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //// NOTE: OpenGL error checks have been omitted for brevity
    //glGenBuffers(1, &vertex_buffer);
    //glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //unsigned int sizeOfVertBufferInBytes = sizeof(sVertex) * ::g_numOfVertices;
    //glBufferData(GL_ARRAY_BUFFER, 
    //             sizeOfVertBufferInBytes,
    //             ::g_pVertexBuffer, 
    //             GL_STATIC_DRAW);

    /*vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);*/








    /************************************************ SHADER MANAGER ***************************************************/
    //Since we create g_pShaderManager as a global variable, we have to put the global scope (::) in front of it
    ::g_pShaderManager = new cShaderManager();

    cShaderManager::cShader vertShader;
    cShaderManager::cShader fragShader;

    vertShader.fileName = "simpleVertex.glsl";
    fragShader.fileName = "simpleFragment.glsl";

    //Create the program
    if (!::g_pShaderManager->createProgramFromFile("SimpleShaderProg", vertShader, fragShader)) {
        //There was a problem
        std::cout << "ERROR: Can't make shader program because: " << std::endl;
        std::cout << ::g_pShaderManager->getLastError() << std::endl;

        //Exit the program and clean up code
        return -1;
    }
    
    //Get the id of the program and put it in "program"
    program = ::g_pShaderManager->getIDFromFriendlyName("SimpleShaderProg");
    /*******************************************************************************************************************/
    












    
    
    


    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    /************************************************ VAO MANAGER ***************************************************/
    /*************************************** LOADING MODELS INTO VAO MANAGER ****************************************/

    ::g_pVAOManager = new cVAOManager();
    
    sModelDrawInfo mdiBunny;

    if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/bun_zipper_res4_xyz_colour.ply", mdiBunny, program)) 
    {
        std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
    }
    
    /****************************************************************************************************************/

    //glEnableVertexAttribArray(vpos_location);
    //glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)0);

    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)(sizeof(float) * 2));

    //glEnableVertexAttribArray(vpos_location);
    //glVertexAttribPointer(  vpos_location,
    //                        3, GL_FLOAT, GL_FALSE,
    //                        sizeof(sVertex),               //Stride
    //                        (void*)offsetof(sVertex, x));


    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(  vcol_location,
    //                        3, GL_FLOAT, GL_FALSE,
    //                        sizeof(sVertex),
    //                        (void*)offsetof(sVertex, r));


    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        //       mat4x4 m, p, mvp;
        glm::mat4 m, p, v, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);

        //         mat4x4_identity(m);
        m = glm::mat4(1.0f);

        ////mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        //glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
        //    (float)glfwGetTime(),
        //    glm::vec3(0.0f, 0.0, 1.0f));

        //m = m * rotateZ;

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        p = glm::perspective(0.6f,
            ratio,
            0.1f,
            1000.0f);

        v = glm::mat4(1.0f);

        /*glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);*/

        v = glm::lookAt(::g_cameraEye,
                        ::g_cameraTarget,
                        ::g_upVector);

        //mat4x4_mul(mvp, p, m);
        mvp = p * v * m;


        glUseProgram(program);










        //Enable/ disable wireframe
        if (::g_isWireFrame)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }













        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawArrays(GL_TRIANGLES, 0, ::g_numOfVertices);

        sModelDrawInfo mdiModelToDraw;
        if (::g_pVAOManager->FindDrawInfoByModelName("assets/models/bun_zipper_res4_xyz_colour.ply", mdiModelToDraw))
        {
            glBindVertexArray(mdiModelToDraw.VAO_ID);
            glDrawElements( GL_TRIANGLES,
                            mdiModelToDraw.numberOfIndices,
                            GL_UNSIGNED_INT,    //How big the index values are
                            0);                 //Starting index for this model
            glBindVertexArray(0);
        }










        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete ::g_pShaderManager;
    //delete[] ::g_pVertexBuffer;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
