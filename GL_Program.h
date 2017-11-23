#pragma once
#include <gl/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace ULY {

    class GL_VS;
    class GL_FS;
    using namespace std;
    class GL_PG{
    public:
        GL_VS *m_vs;
        GL_FS *m_fs;
        GLuint m_program;
        map<std::string,GLint> m_uniforms;
        GL_PG(){
            m_program = glCreateProgram();
        }
        GL_PG(GL_VS *vs,GL_FS *fs){
            m_vs = vs;
            m_fs = fs;
            m_program = glCreateProgram();
        }
        void link();
        void use(){
            glUseProgram(m_program);
        }
    };
    using namespace std;
    class Shader{
    public:
        GLuint m_shader;
        string m_shader_source;
        Shader(){}
        Shader(string path);
        void compile(GLenum type);
        void virtual attach(GL_PG *p) = 0;
    private:
        void read(string path);
    };

    class GL_VS : public Shader{
    public:
        GL_VS():Shader(){}
        GL_VS(string path):Shader(path) {
            compile(GL_VERTEX_SHADER);
        }
        void attach(GL_PG *p) ;
    };

    class GL_FS : public Shader{
    public:
        GL_FS():Shader(){}
        GL_FS(string path):Shader(path) {
            compile(GL_FRAGMENT_SHADER);
        }
        void attach(GL_PG *p) {
            if(m_shader!=0)
            glAttachShader(p->m_program, m_shader);
        }
    };

    class GL_Debugger{
    public:
        static void  printActiveAttirbs(GL_PG *pg);
        static void  printActiveUniforms(GL_PG *pg);
        static void  printGraphicInfo();
        static void  printGLExtension();
        static char* glEnumToString(GLenum type);
    };
}