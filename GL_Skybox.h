#pragma once
#include "GL_Program.h"
#include "GL_Model.h"
using namespace std;
namespace ULY{
class Skybox : public GL_Model{
public:
    string sky_rt;
    string sky_lt;
    string sky_up;
    string sky_dn;
    string sky_bk;
    string sky_ft;
    Skybox(){
       pg = new GL_PG("skybox.vs","skybox.fs"); 
    }
    void draw(){
       
    }
private:
   /*
    * ����һ��cubeMap
    GL_TEXTURE_CUBE_MAP_POSITIVE_X	�ұ�
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X	���
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y	����
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	�ײ�
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z	����
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	ǰ��
    */
    GLuint loadCubeMapTexture(std::vector<const char*> picFilePathVec, GLint internalFormat = GL_RGB,
     GLenum picFormat = GL_RGB, GLenum picDataType = GL_UNSIGNED_BYTE, int loadChannels = SOIL_LOAD_RGB);
};
}