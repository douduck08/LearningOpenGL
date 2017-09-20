#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/types.h>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id;
    std::string type;
    aiString path;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    
    Mesh (std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->setupMesh();
    }
    
    void Draw( Shader shader )
    {
        GLuint diffuseNum = 1;
        GLuint specularNum = 1;
        for( GLuint i = 0; i < this->textures.size( ); i++ )
        {
            // Retrieve texture number (the N in diffuse_textureN)
            std::stringstream ss;
            std::string number;
            std::string name = this->textures[i].type;
            
            if( name == "texture_diffuse" )
            {
                ss << diffuseNum++; // Transfer GLuint to stream
            }
            else if( name == "texture_specular" )
            {
                ss << specularNum++; // Transfer GLuint to stream
            }
            
            number = ss.str( );
            // Now set the sampler to the correct texture unit
            glUniform1i( glGetUniformLocation( shader.Program, ( name + number ).c_str( ) ), i );
            glActiveTexture( GL_TEXTURE0 + i ); // Active proper texture unit before binding
            glBindTexture( GL_TEXTURE_2D, this->textures[i].id );
        }
        
        glUniform1f( glGetUniformLocation( shader.Program, "material.shininess" ), 16.0f );
        
        // Draw mesh
        glBindVertexArray( this->VAO );
        glDrawElements( GL_TRIANGLES, this->indices.size( ), GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );
        
        // Set everything to defaults
        for ( GLuint i = 0; i < this->textures.size( ); i++ )
        {
            glActiveTexture( GL_TEXTURE0 + i );
            glBindTexture( GL_TEXTURE_2D, 0 );
        }
    }
    
private:
    GLuint VAO, VBO, EBO;
    
    void setupMesh()
    {
        // Create buffers/arrays
        glGenVertexArrays( 1, &this->VAO );
        glGenBuffers( 1, &this->VBO );
        glGenBuffers( 1, &this->EBO );
        
        glBindVertexArray(this->VAO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBindBuffer( GL_ARRAY_BUFFER, this->VBO );
        glBufferData( GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW );
        
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->EBO );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW );
        
        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid * )0 );
        // Vertex Normals
        glEnableVertexAttribArray( 1 );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid * )offsetof(Vertex, Normal) );
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid * )offsetof(Vertex, TexCoords) );
        
        glBindVertexArray(0);
    }
};
