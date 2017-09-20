#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SOIL2/SOIL2.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "Model.h"

const GLint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Camera
Camera  camera(glm::vec3( 0.0f, 0.0f, 3.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 dirLightDir(-0.2f, -1.0f, -0.3f);
glm::vec3 pointLightPos[] = {
    glm::vec3(  0.7f,  0.2f,  2.0f      ),
    glm::vec3(  2.3f, -3.3f, -4.0f      ),
    glm::vec3(  -4.0f,  2.0f, -12.0f    ),
    glm::vec3(  0.0f,  0.0f, -3.0f      )
};

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main( )
{
    // Init GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
    }
    
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    // glfwSetScrollCallback( window, ScrollCallback );
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // enable alpha support
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_DEPTH_TEST );
    
    // Shader myShader ("resources/shaders/core.vert", "resources/shaders/core.frag");
    Shader lightingShader( "resources/shaders/lighting.vert", "resources/shaders/lighting.frag" );
    Shader lampShader( "resources/shaders/lamp.vert", "resources/shaders/lamp.frag" );
    
    GLfloat vertices[] = {
        // Positions            // Normals              // Texture Coords
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
        
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
        
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
        
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
    };
    
    glm::vec3 cubePositions[] = {
        glm::vec3(  0.0f,   0.0f,   0.0f),
        glm::vec3(  2.0f,   5.0f,   -15.0f),
        glm::vec3(  -1.5f,  -2.2f,  -2.5f),
        glm::vec3(  -3.8f,  -2.0f,  -12.3f),
        glm::vec3(  2.4f,   -0.4f,  -3.5f),
        glm::vec3(  -1.7f,  3.0f,   -7.5f),
        glm::vec3(  1.3f,   -2.0f,  -2.5f),
        glm::vec3(  1.5f,   2.0f,   -2.5f),
        glm::vec3(  1.5f,   0.2f,   -1.5f),
        glm::vec3(  -1.3f,  1.0f,   -1.5f)
    };
    
    GLuint VBO, EBO;
    glGenBuffers (1, &VBO);
    // glGenBuffers (1, &EBO);
    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), indices, GL_STATIC_DRAW);
    
    GLuint VAO;
    glGenVertexArrays (1, &VAO);
    glBindVertexArray (VAO);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray (0);
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof( GLfloat )));
    glEnableVertexAttribArray (1);
    glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof( GLfloat )));
    glEnableVertexAttribArray (2);
    
    GLuint lightVAO;
    glGenVertexArrays (1, &lightVAO);
    glBindVertexArray (lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), ( GLvoid * )0 );
    glEnableVertexAttribArray( 0 );
    
    glBindVertexArray (0);
    
    // Load Texture
    GLuint diffuseMap, specularMap;
    glGenTextures( 1, &diffuseMap );
    glGenTextures( 1, &specularMap );
    
    unsigned char *image;
    int width, height;
    
    image = SOIL_load_image( "resources/images/container2.png", &width, &height, 0, SOIL_LOAD_RGB );
    glBindTexture( GL_TEXTURE_2D, diffuseMap);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    
    image = SOIL_load_image( "resources/images/container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB );
    glBindTexture( GL_TEXTURE_2D, specularMap);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    
    glBindTexture( GL_TEXTURE_2D, 0);
    
    // Set texture units
    lightingShader.Use( );
    glUniform1i( glGetUniformLocation( lightingShader.Program, "material.diffuse" ),  10 );
    glUniform1i( glGetUniformLocation( lightingShader.Program, "material.specular" ), 11 );
    
    glActiveTexture( GL_TEXTURE10 );
    glBindTexture( GL_TEXTURE_2D, diffuseMap );
    glActiveTexture( GL_TEXTURE11 );
    glBindTexture( GL_TEXTURE_2D, specularMap );
    
    glm::mat4 projection;
    // 3D camera
    projection = glm::perspective( camera.GetZoom(), ( GLfloat )SCREEN_WIDTH / ( GLfloat )SCREEN_HEIGHT, 0.1f, 100.0f );
    // 2D camera
    // projection = glm::ortho(0.0f, ( GLfloat )SCREEN_WIDTH, 0.0f, ( GLfloat )SCREEN_HEIGHT, 0.1f, 1000.0f);
    
    // Load models
    Shader modelShader( "resources/shaders/model.vert", "resources/shaders/model.frag" );
    Model loadedModel( "resources/models/nanosuit.obj" );
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Game loop
    while ( !glfwWindowShouldClose( window ) )
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Check and call events
        glfwPollEvents( );
        DoMovement( );
        
        // Render
        // Clear the colorbuffer
        glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // rander boxes
        lightingShader.Use();
        GLint viewPosLoc = glGetUniformLocation( lightingShader.Program, "viewPos" );
        glUniform3f( viewPosLoc, camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z );
        glUniform1f(glGetUniformLocation( lightingShader.Program, "material.shininess" ), 32.0f );
        
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.direction" ), dirLightDir.x, dirLightDir.y, dirLightDir.z);
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirlight.ambient" ), 0.2f, 0.2f, 0.2f);
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirlight.diffuse" ), 0.8f, 0.8f, 0.8f);
        
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].position" ), pointLightPos[0].x, pointLightPos[0].y, pointLightPos[0].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].position" ), pointLightPos[1].x, pointLightPos[1].y, pointLightPos[1].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].position" ), pointLightPos[2].x, pointLightPos[2].y, pointLightPos[2].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].quadratic" ), 0.032f );

        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].position" ), pointLightPos[3].x, pointLightPos[3].y, pointLightPos[3].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.position" ), camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.direction" ), camera.GetFront( ).x, camera.GetFront( ).y, camera.GetFront( ).z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.ambient" ), 0.0f, 0.0f, 0.0f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.diffuse" ), 1.0f, 1.0f, 1.0f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.quadratic" ), 0.032f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.cutOff" ), glm::cos( glm::radians( 12.5f ) ) );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.outerCutOff" ), glm::cos( glm::radians( 15.0f ) ) );
        
        // Create transformations
        glm::mat4 model, view;
        // model = glm::rotate( model, ( GLfloat)glfwGetTime( ) * 1.0f, glm::vec3( 0.5f, 1.0f, 0.0f ) );
        view = camera.GetViewMatrix ();
        
        // Get their uniform location
        GLint modelLoc = glGetUniformLocation( lightingShader.Program, "model" );
        GLint viewLoc = glGetUniformLocation( lightingShader.Program, "view" );
        GLint projLoc = glGetUniformLocation( lightingShader.Program, "projection" );
        // Pass them to the shaders
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glBindVertexArray (VAO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        for ( GLuint i = 0; i < 10; i++ )
        {
            model = glm::mat4( );
            model = glm::translate( model, cubePositions[i] );
            model = glm::rotate( model, 20.0f * i, glm::vec3( 1.0f, 0.3f, 0.5f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        glBindVertexArray (0);

        // render lamp
        lampShader.Use( );
        modelLoc = glGetUniformLocation( lampShader.Program, "model" );
        viewLoc = glGetUniformLocation( lampShader.Program, "view" );
        projLoc = glGetUniformLocation( lampShader.Program, "projection" );
        
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glBindVertexArray( lightVAO );
        for (int i = 0; i < 4; i++) {
            model = glm::mat4();
            model = glm::translate( model, pointLightPos[i] );
            model = glm::scale( model, glm::vec3( 0.2f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        glBindVertexArray( 0 );
        
        // Draw the loaded model
        modelShader.Use();
        viewPosLoc = glGetUniformLocation( modelShader.Program, "viewPos" );
        glUniform3f( viewPosLoc, camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z );
        
        glUniform3f( glGetUniformLocation( modelShader.Program, "dirLight.direction" ), dirLightDir.x, dirLightDir.y, dirLightDir.z);
        glUniform3f( glGetUniformLocation( modelShader.Program, "dirlight.ambient" ), 0.2f, 0.2f, 0.2f);
        glUniform3f( glGetUniformLocation( modelShader.Program, "dirlight.diffuse" ), 0.8f, 0.8f, 0.8f);
        
        glUniform3f( glGetUniformLocation( modelShader.Program, "pointLight.position" ), pointLightPos[0].x, pointLightPos[0].y, pointLightPos[0].z );
        glUniform3f( glGetUniformLocation( modelShader.Program, "pointLight.ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( modelShader.Program, "pointLight.diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( modelShader.Program, "pointLight.specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( modelShader.Program, "pointLight.constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( modelShader.Program, "pointLight.linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( modelShader.Program, "pointLight.quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( modelShader.Program, "spotLight.position" ), camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z );
        glUniform3f( glGetUniformLocation( modelShader.Program, "spotLight.direction" ), camera.GetFront( ).x, camera.GetFront( ).y, camera.GetFront( ).z );
        glUniform3f( glGetUniformLocation( modelShader.Program, "spotLight.ambient" ), 0.0f, 0.0f, 0.0f );
        glUniform3f( glGetUniformLocation( modelShader.Program, "spotLight.diffuse" ), 1.0f, 1.0f, 1.0f );
        glUniform3f( glGetUniformLocation( modelShader.Program, "spotLight.specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( modelShader.Program, "spotLight.constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( modelShader.Program, "spotLight.linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( modelShader.Program, "spotLight.quadratic" ), 0.032f );
        glUniform1f( glGetUniformLocation( modelShader.Program, "spotLight.cutOff" ), glm::cos( glm::radians( 12.5f ) ) );
        glUniform1f( glGetUniformLocation( modelShader.Program, "spotLight.outerCutOff" ), glm::cos( glm::radians( 15.0f ) ) );
        
        model = glm::mat4();
        model = glm::translate( model, glm::vec3( 2.0f, -1.75f, 1.0f ) );
        model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) );
        glUniformMatrix4fv( glGetUniformLocation( modelShader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        glUniformMatrix4fv( glGetUniformLocation( modelShader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( glGetUniformLocation( modelShader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
        loadedModel.Draw(modelShader);
        
        // Swap the screen buffers
        glfwSwapBuffers( window );
    }
    
    glDeleteVertexArrays (1, &VAO);
    glDeleteVertexArrays( 1, &lightVAO );
    glDeleteBuffers (1, &VBO);
    
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate( );
    
    return EXIT_SUCCESS;
}

// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    
    if( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    
    if( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    
    if( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}


void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset )
{
    camera.ProcessMouseScroll( yOffset );
}
