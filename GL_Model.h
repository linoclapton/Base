#pragma once
#include "GL_Program.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "GL_Img_Loader.h"
#include "GL_OBJ.h"
#include <glm/gtc/type_ptr.hpp>
#include <map>

namespace ULY{
    class GL_Model{
    public:
        GL_PG *pg;
        ULY::attrib_t attrib;
        vector<ULY::shape_t> shapes;
        vector<ULY::material_t> materials;
        map<std::string, GLuint> textures;
        float bmin[3], bmax[3];
        glm::mat4 m_project;
        glm::mat4 m_view;
        glm::mat4 m_model;
        GL_Model(){
            pg=NULL;
            m_project = glm::mat4(1.0);
            m_view = glm::mat4(1.0);
            m_model = glm::scale(glm::vec3(0.5,0.5,0.5));
        }
        virtual void render() = 0;
    };

    class GL_Obj_Model : public GL_Model{
    public:

        string path;
        vector<unsigned char *> img;
        vector<pair<int,int>> resolution;
        void init(string path ="",string name ="bunny.obj");
        void render();
    };
}