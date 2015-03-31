#include "model.h"

Model::Model( ModelData data )
{

    //Read in materials
    for(int i = 0; i < data.materials.size(); i++)
    {
        materials.push_back( data.materials[i] );
    }

    setUpBuffers( data );
    
}

void Model::setUpBuffers(ModelData data)
{
    Program::checkGLErrors( "beginning of model init" );

    GLuint vao, vbo, nbo, ibo, tbo;

    int numMeshes = data.numMeshes() ;

    //Lets start with meshes
    for( int i = 0; i < numMeshes; i++)
    {
        Material mat = data.meshes[i].material;
        glUseProgram( mat.shader->getID() );

        unsigned int numVertices = data.meshes[i].positions.size();

        MeshInfo buffers;
        buffers.numVertices = numVertices;
        buffers.numIndices = data.meshes[i].indices.size();

        buffers.name = data.meshes[i].name;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        buffers.vao = vao;

        //Vertices
        glGenBuffers(1, &vbo);
        Program::checkGLErrors("VBO creation");
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        Program::checkGLErrors( "VBO Binding" );
        glBufferData( GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), &data.meshes[i].positions[0], GL_STATIC_DRAW);
        Program::checkGLErrors( "BufferData for vertices" );
        buffers.vertices = vbo;
        glVertexAttribPointer( mat.shader->getAttrib("theV"), 3, GL_FLOAT, 0, 0, 0 );
        Program::checkGLErrors( "Vertex attrib for vertices" );
        //Indices
        glGenBuffers(1, &ibo);
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, data.meshes[i].indices.size() * sizeof(unsigned int), &data.meshes[i].indices[0], GL_STATIC_DRAW);
        buffers.indices = ibo;

        //Normals
        glGenBuffers(1, &nbo);
        glBindBuffer( GL_ARRAY_BUFFER, nbo );
        glBufferData( GL_ARRAY_BUFFER, data.meshes[i].normals.size() * 3 * sizeof(float), &data.meshes[i].normals[0], GL_STATIC_DRAW);
        Program::checkGLErrors( "BufferData for normals" );
        buffers.normals = nbo;
        glVertexAttribPointer( mat.shader->getAttrib("theN"), 3, GL_FLOAT, 0, 0, 0 );
        Program::checkGLErrors( "Vertex attrib for normals" );

        //Texture coords
        glGenBuffers(1, &tbo);
        glBindBuffer( GL_ARRAY_BUFFER, tbo );
        
        if( ! data.meshes[i].hasUVs() )
        {
            //We don't have any texcoords, so lets generate our own.
            float *texCoords = new float[2 * numVertices];

            for (unsigned int k = 0; k < numVertices; ++k) 
            {
                texCoords[k*2] = data.meshes[i].positions[k].x;
                texCoords[k*2+1] = data.meshes[i].positions[k].z;
            }

            glBufferData( GL_ARRAY_BUFFER, numVertices * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);
            delete[] texCoords;

        }
        else 
        {
            glBufferData( GL_ARRAY_BUFFER, data.meshes[i].UVs.size() * sizeof(float), &data.meshes[i].UVs[0], GL_STATIC_DRAW);
        }
        Program::checkGLErrors( "4" );

        buffers.texturecoords = tbo;
        glVertexAttribPointer( mat.shader->getAttrib("tex_in"), 2, GL_FLOAT, 0, 0, 0);
        Program::checkGLErrors( "Vertex attrib for textures" );


        glEnableVertexAttribArray(mat.shader->getAttrib("theV"));
        Program::checkGLErrors( "Enable vertex atrib for vertices" );
        glEnableVertexAttribArray(mat.shader->getAttrib("theN"));
        Program::checkGLErrors( "Enable vertex atrib for normals" );
        glEnableVertexAttribArray(mat.shader->getAttrib("tex_in"));
        Program::checkGLErrors( "Enable vertex atrib for textures" );

        glBindVertexArray( 0 );

        //Setup bounding box buffer
        glm::vec3 min = glm::vec3(FLT_MAX);
        glm::vec3 max = glm::vec3(FLT_MIN);
        glm::vec3 vert, comp;

        for(int j = 0; j < numVertices; j++)
        {
            vert.x = data.meshes[i].positions[j].x;
            vert.y = data.meshes[i].positions[j].y;
            vert.z = data.meshes[i].positions[j].z;

            if( vert.x < min.x )
                min.x = vert.x;
            else if( vert.x > max.x )
                max.x = vert.x;

            if( vert.y < min.y )
                min.y = vert.y;
            else if( vert.y > max.y )
                max.y = vert.y;

            if( vert.z < min.z )
                min.z = vert.z;
            else if( vert.z > max.z )
                max.z = vert.z;

        }

        glm::vec3 cube[8];

        cube[4] = min;
        cube[3] = max;

        cube[1] = max;
        cube[1].y = min.y;

        cube[2] = max;
        cube[2].x = min.x;

        cube[7] = max;
        cube[7].z = min.z;

        cube[6] = min;
        cube[6].y = max.y;

        cube[5] = min;
        cube[5].x = max.x;

        cube[0] = min;
        cube[0].z = max.z;

        GLuint bBoxVbo;
        glGenBuffers( 1, &bBoxVbo );
        glBindBuffer( GL_ARRAY_BUFFER, bBoxVbo );
        //printf("bBoxVbo is %i\n", bBoxVbo);
        glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), cube, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        buffers.boundingBox = bBoxVbo;
        Program::checkGLErrors( "7" );

        //Add material index to storage
        //(so the renderer can load the right material
        //from the model object, for each mesh)
        for(int j = 0; j < materials.size(); j++)
            if(materials[j].name == data.meshes[i].material.name)
                buffers.matIndex = j;


        bufferIDs.push_back( buffers );
    }
    glUseProgram( 0 );
    Program::checkGLErrors( "end of model init" );


}

std::vector<unsigned int> Model::generateFacesVector(aiFace* assimpFaceArray, int numFaces)
{
    // create array with faces
    // have to convert from Assimp format to array
    std::vector<unsigned int> faceArray;

    unsigned int faceIndex = 0;

    for (unsigned int t = 0; t <  numFaces; ++t) {
        const aiFace* face =  &assimpFaceArray[t];

        //memcpy(&faceArray[faceIndex], face->mIndices,3 * sizeof(unsigned int));
        //faceIndex += 3;
        
        faceArray.push_back( face->mIndices[0] );
        faceArray.push_back( face->mIndices[1] );
        faceArray.push_back( face->mIndices[2] );
    }

    return faceArray;

}
