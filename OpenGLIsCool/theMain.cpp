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
#include <vector>

#include "cShaderManager.h"
#include "cVAOManager.h"
#include "cMeshObject.h"

//Camera
glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, +100.0f);
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

//The objects we are drawing go in here
std::vector<cMeshObject*> g_pVecObjects;

//Create a global cShaderManager pointer
cShaderManager* g_pShaderManager = 0;   //0 == NULL

cVAOManager * g_pVAOManager = 0;

bool g_isWireFrame = false;

int g_selectedObject = 0;

const std::string IN_OUT_FILE = "ObjecsInfo.txt";

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


static void SaveToFile()
{
    std::ofstream file(IN_OUT_FILE.c_str());

    for (std::vector<cMeshObject*>::iterator it_object = ::g_pVecObjects.begin();
        it_object != ::g_pVecObjects.end(); it_object++)
    {
        file << (*it_object)->meshName << std::endl;

        file << (*it_object)->position.x << std::endl;
        file << (*it_object)->position.y << std::endl;
        file << (*it_object)->position.z << std::endl;

        file << (*it_object)->colourRGBA.r << std::endl;
        file << (*it_object)->colourRGBA.g << std::endl;
        file << (*it_object)->colourRGBA.b << std::endl;
        file << (*it_object)->colourRGBA.a << std::endl;

        file << (*it_object)->orientation.x << std::endl;
        file << (*it_object)->orientation.y << std::endl;
        file << (*it_object)->orientation.z << std::endl;

        file << (*it_object)->scale << std::endl;

        file << (*it_object)->isWireFrame << std::endl;

        if (it_object != ::g_pVecObjects.end() - 1)
        {
            file << "end_of_object" << std::endl;
        }
    }
    file << "end_of_file" << std::endl;
    file.close();
    std::cout << "Information saved..." << std::endl;
}

static void ReadFromFile()
{
    std::ifstream file(IN_OUT_FILE.c_str());

    if (!file.is_open()) {
        //There was something wrong
        std::cout << "Unable to load the file" << std::endl;
        return;
    }

    bool kepReading = true;
    std::string tempString;
    std::string meshName;					
    bool isWireFrame;
    float tempFloat;

    //Load information for existing objects
    if (::g_pVecObjects.size() != 0)
    {
        std::vector<cMeshObject*>::iterator it_object = ::g_pVecObjects.begin();
        do
        {
            file >> meshName;
            (*it_object)->meshName = meshName;

            file >> tempFloat;
            (*it_object)->position.x = tempFloat;
            file >> tempFloat;
            (*it_object)->position.y = tempFloat;
            file >> tempFloat;
            (*it_object)->position.z = tempFloat;

            file >> tempFloat;
            (*it_object)->colourRGBA.r = tempFloat;
            file >> tempFloat;
            (*it_object)->colourRGBA.g = tempFloat;
            file >> tempFloat;
            (*it_object)->colourRGBA.b = tempFloat;
            file >> tempFloat;
            (*it_object)->colourRGBA.a = tempFloat;

            file >> tempFloat;
            (*it_object)->orientation.x = tempFloat;
            file >> tempFloat;
            (*it_object)->orientation.y = tempFloat;
            file >> tempFloat;
            (*it_object)->orientation.z = tempFloat;

            file >> tempFloat;
            (*it_object)->scale = tempFloat;

            file >> isWireFrame;
            (*it_object)->isWireFrame = isWireFrame;

            file >> tempString;
            if (tempString == "end_of_file")
            {
                kepReading = false;
                std::cout << "Information loaded..." << std::endl;
            }

            //Iterate to the next object
            it_object++;

            //If the number of saved objects greater than the number of existing objects
            //Create new objets
            if (it_object == ::g_pVecObjects.end())
            {
                cMeshObject* pObject = new cMeshObject();
                ::g_pVecObjects.push_back(pObject);
                it_object = ::g_pVecObjects.end() - 1;
            }
        } while (kepReading);
    }
    //There is no existing object, create loaded objects
    else
    {
        do
        {
            cMeshObject* pObject = new cMeshObject();

            file >> meshName;
            pObject->meshName = meshName;

            file >> tempFloat;
            pObject->position.x = tempFloat;
            file >> tempFloat;
            pObject->position.y = tempFloat;
            file >> tempFloat;
            pObject->position.z = tempFloat;

            file >> tempFloat;
            pObject->colourRGBA.r = tempFloat;
            file >> tempFloat;
            pObject->colourRGBA.g = tempFloat;
            file >> tempFloat;
            pObject->colourRGBA.b = tempFloat;
            file >> tempFloat;
            pObject->colourRGBA.a = tempFloat;

            file >> tempFloat;
            pObject->orientation.x = tempFloat;
            file >> tempFloat;
            pObject->orientation.y = tempFloat;
            file >> tempFloat;
            pObject->orientation.z = tempFloat;

            file >> tempFloat;
            pObject->scale = tempFloat;

            file >> isWireFrame;
            pObject->isWireFrame = isWireFrame;
            ::g_pVecObjects.push_back(pObject);

            file >> tempString;
            if (tempString == "end_of_file") 
            {
                kepReading = false;
                std::cout << "All objects loaded..." << std::endl;
            }
        } while (kepReading);
    }
}


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}




static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float const CAMERA_SPEED = 0.5f;
    float const SCALE_LEVEL = 0.001f;
    float const ROTATE_LEVEL = 1.0f;
    float const TRANSLATE_LEVEL = 0.5f;

    if (action != GLFW_RELEASE) 
    {
        switch (key)
        {
        /******************* SELECT OBJECT **********************/
        case GLFW_KEY_V:
            ::g_selectedObject++;
            ::g_selectedObject = ::g_selectedObject == ::g_pVecObjects.size() ? 0 : ::g_selectedObject;
            std::cout << "Selected object: " << ::g_selectedObject + 1 << std::endl;
            break;
        case GLFW_KEY_C:
            ::g_selectedObject--;
            ::g_selectedObject = ::g_selectedObject == -1 ? ::g_pVecObjects.size() - 1 : ::g_selectedObject;
            std::cout << "Selected object: " << ::g_selectedObject + 1 << std::endl;
            break;

        /*************** SCALE SELECTED OBJECT ******************/
        case GLFW_KEY_M: ::g_pVecObjects[::g_selectedObject]->scale += SCALE_LEVEL; break;    //Scale up
        case GLFW_KEY_N: ::g_pVecObjects[::g_selectedObject]->scale -= SCALE_LEVEL; break;     //Scale down
        
        /************* TRANSLATE SELECTED OBJECT ****************/
        case GLFW_KEY_UP: ::g_pVecObjects[::g_selectedObject]->position.y += TRANSLATE_LEVEL; break;   //Move up
        case GLFW_KEY_DOWN: ::g_pVecObjects[::g_selectedObject]->position.y -= TRANSLATE_LEVEL; break;   //Move down
        case GLFW_KEY_LEFT: ::g_pVecObjects[::g_selectedObject]->position.x -= TRANSLATE_LEVEL; break;   //Move left
        case GLFW_KEY_RIGHT: ::g_pVecObjects[::g_selectedObject]->position.x += TRANSLATE_LEVEL; break;   //Move right
        
        /************* ROTATE SELECTED OBJECT ****************/
        case GLFW_KEY_J: ::g_pVecObjects[::g_selectedObject]->orientation.z += glm::radians(ROTATE_LEVEL); break; //Rotate z+
        case GLFW_KEY_L: ::g_pVecObjects[::g_selectedObject]->orientation.z -= glm::radians(ROTATE_LEVEL); break; //Rotate z-
        case GLFW_KEY_I: ::g_pVecObjects[::g_selectedObject]->orientation.x -= glm::radians(ROTATE_LEVEL); break; //Rotate x-
        case GLFW_KEY_K: ::g_pVecObjects[::g_selectedObject]->orientation.x += glm::radians(ROTATE_LEVEL); break; //Rotate x+
        case GLFW_KEY_U: ::g_pVecObjects[::g_selectedObject]->orientation.y += glm::radians(ROTATE_LEVEL); break; //Rotate y+
        case GLFW_KEY_O: ::g_pVecObjects[::g_selectedObject]->orientation.y -= glm::radians(ROTATE_LEVEL); break; //Rotate y-

        /************* CAMERA CONTROLLER ****************/
        case GLFW_KEY_A: ::g_cameraEye.x -= CAMERA_SPEED; break;   //Move left
        case GLFW_KEY_D: ::g_cameraEye.x += CAMERA_SPEED; break;   //Move right
        case GLFW_KEY_W: ::g_cameraEye.z += CAMERA_SPEED; break;   //Move forward
        case GLFW_KEY_S: ::g_cameraEye.z -= CAMERA_SPEED; break;   //Move back
        case GLFW_KEY_Q: ::g_cameraEye.y += CAMERA_SPEED; break;   //Move up
        case GLFW_KEY_E: ::g_cameraEye.y -= CAMERA_SPEED; break;   //Move down

        /************* SWITCH SOLID/ WIREFRAME ****************/
        case GLFW_KEY_9:    //Switch all objects
            for (std::vector<cMeshObject*>::iterator it_object = ::g_pVecObjects.begin();
                it_object != ::g_pVecObjects.end(); it_object++)
            {
                (*it_object)->isWireFrame = !(*it_object)->isWireFrame ? true : false;
            }
            break;    
        case GLFW_KEY_0:    //Switch selected object
            ::g_pVecObjects[::g_selectedObject]->isWireFrame = !::g_pVecObjects[::g_selectedObject]->isWireFrame ? true : false;
            break;   


        /************* SAVE OBJECTS TO FILE ****************/
        case GLFW_KEY_LEFT_BRACKET: SaveToFile(); break;

        /************* READ OBJECTS TO FILE ****************/
        case GLFW_KEY_RIGHT_BRACKET: ReadFromFile(); break;


        default:
            break;
        }
    }

    //if (action != GLFW_RELEASE)
    //{     
    //    /******************* SELECT OBJECT **********************/
    //    if (key == GLFW_KEY_V) 
    //    {
    //        ::g_selectedObject++;
    //        ::g_selectedObject = ::g_selectedObject == ::g_pVecObjects.size() ? 0 : ::g_selectedObject;
    //        std::cout << "Selected object: " << ::g_selectedObject + 1 << std::endl;
    //    }
//
    //    if (key == GLFW_KEY_C)
    //    {
    //        ::g_selectedObject--;
    //        ::g_selectedObject = ::g_selectedObject == -1 ? ::g_pVecObjects.size() - 1 : ::g_selectedObject;
    //        std::cout << "Selected object: " << ::g_selectedObject + 1 << std::endl;
    //    }
//
    //    /*************** SCALE SELECTED OBJECT ******************/
    //    if (key == GLFW_KEY_M) {    //scale up
    //        ::g_pVecObjects[::g_selectedObject]->scale += SCALE_LEVEL;
    //    }
//
    //    if (key == GLFW_KEY_N) {    //scale down
    //        ::g_pVecObjects[::g_selectedObject]->scale -= SCALE_LEVEL;
    //    }
//
    //    /********************************************************/
//
    //    /************* TRANSLATE SELECTED OBJECT ****************/
    //    if (key == GLFW_KEY_UP) {    //move up
    //        ::g_pVecObjects[::g_selectedObject]->position.y += TRANSLATE_LEVEL;
    //    }
//
    //    if (key == GLFW_KEY_DOWN) {    //move down
    //        ::g_pVecObjects[::g_selectedObject]->position.y -= TRANSLATE_LEVEL;
    //    }
//
    //    if (key == GLFW_KEY_LEFT) {    //move left
    //        ::g_pVecObjects[::g_selectedObject]->position.x -= TRANSLATE_LEVEL;
    //    }
//
    //    if (key == GLFW_KEY_RIGHT) {    //move right
    //        ::g_pVecObjects[::g_selectedObject]->position.x += TRANSLATE_LEVEL;
    //    }
//
    //    /********************************************************/
//
    //    /************* ROTATE SELECTED OBJECT ****************/
    //    if (key == GLFW_KEY_J) {    //rotate left
    //        ::g_pVecObjects[::g_selectedObject]->orientation.z += glm::radians(ROTATE_LEVEL);
    //    }
//
    //    if (key == GLFW_KEY_L) {    //rotate right
    //        ::g_pVecObjects[::g_selectedObject]->orientation.z -= glm::radians(ROTATE_LEVEL);
    //    }
//
    //    if (key == GLFW_KEY_I) {    //rotate up
    //        ::g_pVecObjects[::g_selectedObject]->orientation.x += glm::radians(ROTATE_LEVEL);
    //    }
//
    //    if (key == GLFW_KEY_K) {    //rotate down
    //        ::g_pVecObjects[::g_selectedObject]->orientation.x -= glm::radians(ROTATE_LEVEL);
    //    }
    //    /*****************************************************/
//
//
    //    if (key == GLFW_KEY_A) {    //Move left
    //        ::g_cameraEye.x -= CAMERA_SPEED;
    //    }
//
    //    if (key == GLFW_KEY_D) {    //Move right
    //        ::g_cameraEye.x += CAMERA_SPEED;
    //    }
//
    //    if (key == GLFW_KEY_W) {    //Move foward
    //        ::g_cameraEye.z += CAMERA_SPEED;
    //    }
//
    //    if (key == GLFW_KEY_S) {    //Move backward
    //        ::g_cameraEye.z -= CAMERA_SPEED;
    //    }
//
    //    if (key == GLFW_KEY_Q) {    //Move up
    //        ::g_cameraEye.y += CAMERA_SPEED;
    //    }
//
    //    if (key == GLFW_KEY_E) {    //Move down
    //        ::g_cameraEye.y -= CAMERA_SPEED;
    //    }
//
    //    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //        glfwSetWindowShouldClose(window, GLFW_TRUE);
//
    //    // Switches from wireframe to solid (glPolygonMode)
    //    if (key == GLFW_KEY_9 && action == GLFW_PRESS) { ::g_isWireFrame = true; }
    //    if (key == GLFW_KEY_0 && action == GLFW_PRESS) { ::g_isWireFrame = false; }
//
    //    /*std::cout << "cam: " <<
    //        ::g_cameraEye.x << " " <<
    //        ::g_cameraEye.y << " " <<
    //        ::g_cameraEye.z << std::endl;*/
//
    //    return;
    //}
}


int main(void)
{
    std::cout << "Selected object: " << ::g_selectedObject + 1 << std::endl;
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

    window = glfwCreateWindow(1024, 800, "Example", NULL, NULL);
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
    
    //sModelDrawInfo mdiBunny;
    //if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/bun_zipper_res4_xyz_colour.ply", mdiBunny, program)) 
    //{
    //    std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
    //}
//
    //sModelDrawInfo mdiPlane;
    //if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/su47_xyz_rgba.ply.ply", mdiPlane, program))
    //{
    //    std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
    //}
//
    //sModelDrawInfo mdiArena;
    //if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/free_arena_ASCII_xyz_rgba.ply",
    //    mdiArena, program))
    //{
    //    std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
    //}
//
    //{// Load the bunny, too
    //    sModelDrawInfo mdiRabbit;
    //    ::g_pVAOManager->LoadModelIntoVAO("assets/models/bun_zipper_res4_xyz_colour.ply",
    //        mdiRabbit, program);
    //}
    
    // Load the space shuttle, too
    sModelDrawInfo mdiSpaceShuttle;
    if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/SpaceShuttleOrbiter_xyz_rgba.ply",
        mdiSpaceShuttle, program)) 
    {
        std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
    }
    
    
    /****************************************************************************************************************/



    /************************************************ MESS OBJECTS ***************************************************/
    //cMeshObject* bunny = new cMeshObject();
    //bunny->meshName = "assets/models/bun_zipper_res4_xyz_colour.ply";
    //bunny->position.x = -5.0f;
    //bunny->scale = 10.0f;
    //::g_pVecObjects.push_back(bunny);
//
    //cMeshObject* pShuttle01 = new cMeshObject();
    //pShuttle01->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    //pShuttle01->position.x = -10.0f;
    //pShuttle01->scale = 1.0f / 100.0f;    // 100th of it's normal size 0.001
    //::g_pVecObjects.push_back(pShuttle01);
//
    //cMeshObject* pShuttle02 = new cMeshObject();
    //pShuttle02->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    //pShuttle02->position.x = +10.0f;
    //pShuttle02->scale = 1.0f / 100.0f;    // 100th of it's normal size
    //pShuttle02->orientation.z = glm::radians(135.0f);
    //::g_pVecObjects.push_back(pShuttle02);
//
    //cMeshObject* pBunny = new cMeshObject();
    //pBunny->meshName = "assets/models/bun_zipper_res4_xyz_colour.ply";
    //pBunny->position.y = +10.0f;
    //pBunny->scale = 25.0f;
    //::g_pVecObjects.push_back(pBunny);
//
    //cMeshObject* pArena = new cMeshObject();
    //pArena->meshName = "assets/models/free_arena_ASCII_xyz_rgba.ply";
    //pArena->position.y = -20.0f;
    //pArena->scale = 1.0f;
    //::g_pVecObjects.push_back(pArena);

    cMeshObject* pShuttle01 = new cMeshObject();
    pShuttle01->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle01->position.x = +30.0f;
    pShuttle01->position.y = +10.0f;
    pShuttle01->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle01->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle01);

    cMeshObject* pShuttle02 = new cMeshObject();
    pShuttle02->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle02->position.x = +15.0f;
    pShuttle02->position.y = +10.0f;
    pShuttle02->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle02->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle02);

    cMeshObject* pShuttle03 = new cMeshObject();
    pShuttle03->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle03->position.x = +0.0f;
    pShuttle03->position.y = +10.0f;
    pShuttle03->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle03->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle03);

    cMeshObject* pShuttle04 = new cMeshObject();
    pShuttle04->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle04->position.x = -15.0f;
    pShuttle04->position.y = +10.0f;
    pShuttle04->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle04->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle04);

    cMeshObject* pShuttle05 = new cMeshObject();
    pShuttle05->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle05->position.x = -30.0f;
    pShuttle05->position.y = +10.0f;
    pShuttle05->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle05->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle05);

    cMeshObject* pShuttle06 = new cMeshObject();
    pShuttle06->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle06->position.x = +30.0f;
    pShuttle06->position.y = -10.0f;
    pShuttle06->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle06->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle06);

    cMeshObject* pShuttle07 = new cMeshObject();
    pShuttle07->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle07->position.x = +15.0f;
    pShuttle07->position.y = -10.0f;
    pShuttle07->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle07->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle07);

    cMeshObject* pShuttle08 = new cMeshObject();
    pShuttle08->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle08->position.x = +0.0f;
    pShuttle08->position.y = -10.0f;
    pShuttle08->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle08->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle08);

    cMeshObject* pShuttle09 = new cMeshObject();
    pShuttle09->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle09->position.x = -15.0f;
    pShuttle09->position.y = -10.0f;
    pShuttle09->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle09->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle09);

    cMeshObject* pShuttle10 = new cMeshObject();
    pShuttle10->meshName = "assets/models/SpaceShuttleOrbiter_xyz_rgba.ply";
    pShuttle10->position.x = -30.0f;
    pShuttle10->position.y = -10.0f;
    pShuttle10->scale = 1.0f / 100.0f;    // 100th of it's normal size
    pShuttle10->orientation.z = glm::radians(0.0f);
    ::g_pVecObjects.push_back(pShuttle10);

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
        glm::mat4 matModel, p, v, mvp;
//        glm::mat4 matModel, p, v, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);



        ////mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        //glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
        //    (float)glfwGetTime(),
        //    glm::vec3(0.0f, 0.0, 1.0f));

        //m = m * rotateZ;

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        p = glm::perspective(0.6f,
            ratio,
            0.1f,
            10000.0f);

        v = glm::mat4(1.0f);

        /*glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);*/

        v = glm::lookAt(::g_cameraEye,
                        ::g_cameraTarget,
                        ::g_upVector);


        /******************************************** DRAW ALL OBJECTS HERE ****************************************/
        for (std::vector<cMeshObject*>::iterator it_curMesh = ::g_pVecObjects.begin();
            it_curMesh != ::g_pVecObjects.end(); it_curMesh++)
        {
            cMeshObject* pCurMesh = *it_curMesh;

            //         mat4x4_identity(m);
            matModel = glm::mat4(1.0f);

            /******************************** MODEL TRANSLATION *************************************/
            // Place the object in the world at 'this' location
            glm::mat4 matTranslation
                = glm::translate(glm::mat4(1.0f),
                    glm::vec3(pCurMesh->position.x,
                        pCurMesh->position.y,
                        pCurMesh->position.z));
            matModel = matModel * matTranslation;


            //mat4x4_rotate_Z(m, m, );
            //*************************************
            // ROTATE around Z
            glm::mat4 matRotateZ = glm::rotate( glm::mat4(1.0f),
                                                pCurMesh->orientation.z, // (float) glfwGetTime(), 
                                                glm::vec3(0.0f, 0.0f, 1.0f));
            matModel = matModel * matRotateZ;
            //*************************************

            //*************************************
            // ROTATE around Y
            glm::mat4 matRotateY = glm::rotate( glm::mat4(1.0f),
                                                pCurMesh->orientation.y, // (float) glfwGetTime(), 
                                                glm::vec3(0.0f, 1.0f, 0.0f));
            matModel = matModel * matRotateY;
            //*************************************

            //*************************************
            // ROTATE around X
            glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
                                            pCurMesh->orientation.x, // (float) glfwGetTime(), 
                                            glm::vec3(1.0f, 0.0, 0.0f));
            matModel = matModel * rotateX;
            //*************************************


            // Set up a scaling matrix
            glm::mat4 matScale = glm::mat4(1.0f);

            float theScale = pCurMesh->scale;		// 1.0f;		
            matScale = glm::scale(glm::mat4(1.0f),
            glm::vec3(theScale, theScale, theScale));


            // Apply (multiply) the scaling matrix to 
            // the existing "model" (or "world") matrix
            matModel = matModel * matScale;

            /******************************** MODEL TRANSLATION *************************************/
           
            
            //mat4x4_mul(mvp, p, m);
            mvp = p * v * matModel;

            glUseProgram(program);





            //Enable/ disable wireframe
            if (pCurMesh->isWireFrame)
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
            if (::g_pVAOManager->FindDrawInfoByModelName(pCurMesh->meshName, mdiModelToDraw))
            {
                glBindVertexArray(mdiModelToDraw.VAO_ID);
                glDrawElements(GL_TRIANGLES,
                    mdiModelToDraw.numberOfIndices,
                    GL_UNSIGNED_INT,    //How big the index values are
                    0);                 //Starting index for this model
                glBindVertexArray(0);
            }

        }



        glfwSwapBuffers(window);   //Present all the things we drawed on the screen
        glfwPollEvents();          //Read keyboard, mouse,...
    }

    delete ::g_pShaderManager;

    //delete bunny;
    //delete plane;
    //delete[] ::g_pVertexBuffer;

    //delete pointer to objects
    for (std::vector<cMeshObject*>::iterator it_object = ::g_pVecObjects.begin();
        it_object != ::g_pVecObjects.end(); it_object++)
    {
        delete (*it_object);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
