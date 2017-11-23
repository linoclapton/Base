#include "GL_Program.h"

namespace ULY{


    Shader::Shader(string path) {
        read(path);
    }
    void Shader::read(string path){
        ifstream fstream(path, std::ios::in);
        if (fstream.is_open()) {
            std::string Line = "";
            while (getline(fstream, Line))
                 m_shader_source += "\n" + Line;
            fstream.close();
        }
        else {
            cout<<""<<endl;
            return;
        }
    }
    void Shader::compile(GLenum type){
        m_shader = glCreateShader(type);
        GLint Result = GL_FALSE;
        int InfoLogLength;
        // Compile Vertex Shader
        char const * sourcePointer = m_shader_source.c_str();
        glShaderSource(m_shader, 1, &sourcePointer, NULL);
        glCompileShader(m_shader);
        // Check Vertex Shader
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0){
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(m_shader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            cout<<&VertexShaderErrorMessage[0]<<endl;
        }
    }
    void GL_VS::attach(GL_PG *p) {
        if(m_shader!=0)
        glAttachShader(p->m_program, m_shader);
    }

    void GL_PG::link() {
        m_vs->attach(this);
        m_fs->attach(this);
        glLinkProgram(m_program);
        GLint status;
        glGetProgramiv(m_program, GL_LINK_STATUS, &status);
        if (GL_FALSE == status) {
            fprintf(stderr, "Failed to link shader program!\n");
            GLint logLen;
            glGetProgramiv(m_program, GL_INFO_LOG_LENGTH,
                &logLen);
            if (logLen > 0)
            {
                char * log = (char *)malloc(logLen);
                GLsizei written;
                glGetProgramInfoLog(m_program, logLen, &written, log);
            }
        }
        GLint maxLength,nUniforms;
        glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLength);
        glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS,&nUniforms);
        GLchar * name = new GLchar[maxLength];
        GLint written, size, location;
        GLenum type;
        for (int i = 0; i < nUniforms; ++i) {
            glGetActiveUniform(m_program, i, maxLength, &written,&size, &type, name);
            location = glGetUniformLocation(m_program, name);
            m_uniforms[name] = location;
        }
        delete []name;
    }

    void GL_Debugger::printActiveAttirbs(GL_PG *pg) {
        GLint maxLength, nAttribs;
        glGetProgramiv(pg->m_program, GL_ACTIVE_ATTRIBUTES, &nAttribs);
        glGetProgramiv(pg->m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
            &maxLength);
        GLchar * name = (GLchar *)malloc(maxLength);
        GLint written, size, location;
        GLenum type;
        printf(" Index | Name\n");
        printf("------------------------------------------------\n");
        for (int i = 0; i < nAttribs; i++) {
            glGetActiveAttrib(pg->m_program, i, maxLength, &written,
                &size, &type, name);
            location = glGetAttribLocation(pg->m_program, name);
            printf(" %-8d | %-10s | %-2d | %s\n", location, name,size,glEnumToString(type));
        }
        free(name);
    }

    char* GL_Debugger::glEnumToString(GLenum type) {
        switch(type){
        case GL_FLOAT_VEC2:              return "GL_FLOAT_VEC2";
        case GL_FLOAT_VEC3:              return "GL_FLOAT_VEC3";
        case GL_FLOAT_VEC4:				 return "GL_FLOAT_VEC4";
        case GL_FLOAT:                   return "GL_FLOAT";
        case GL_INT:                     return "GL_INT";
        case GL_FLOAT_MAT2:              return "GL_FLOAT_MAT2";
        case GL_FLOAT_MAT3:              return "GL_FLOAT_MAT3";
        case GL_FLOAT_MAT4:				 return "GL_FLOAT_MAT4";
        case GL_SAMPLER_1D:				 return "GL_SAMPLER_1D";
        case GL_SAMPLER_2D:				 return "GL_SAMPLER_2D";
        case GL_SAMPLER_3D:				 return "GL_SAMPLER_3D";
        case GL_SAMPLER_CUBE:            return "GL_SAMPLER_CUBE";
        case GL_SAMPLER_1D_SHADOW:       return "GL_SAMPLER_1D_SHADOW";
        case GL_SAMPLER_2D_SHADOW:		 return "GL_SAMPLER_2D_SHADOW";
        default:                         return "UN_KNOWN_ENUM";
        }
    }

    void GL_Debugger::printActiveUniforms(GL_PG *pg){
        GLint maxLength,nUniforms;
        glGetProgramiv(pg->m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLength);
        glGetProgramiv(pg->m_program, GL_ACTIVE_UNIFORMS,&nUniforms);
        GLchar * name = (GLchar *)malloc(maxLength);
        GLint written, size, location;
        GLenum type;
        printf(" Location | Name | Size | Type\n");
        printf("------------------------------------------------\n");
        for (int i = 0; i < nUniforms; ++i) {
            glGetActiveUniform(pg->m_program, i, maxLength, &written,&size, &type, name);
            location = glGetUniformLocation(pg->m_program, name);
            printf(" %-8d | %-10s | %-2d | %s\n", location, name,size,glEnumToString(type));
        }
        std::cout<<std::endl;
        free(name);
    }

    void GL_Debugger::printGraphicInfo(){
        const GLubyte *renderer = glGetString( GL_RENDERER );
        const GLubyte *vendor = glGetString( GL_VENDOR );
        const GLubyte *version = glGetString( GL_VERSION );
        const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        printf("GL Vendor : %s\n", vendor);
        printf("GL Renderer : %s\n", renderer);
        printf("GL Version (string) : %s\n", version);
        printf("GL Version (integer) : %d.%d\n", major, minor);
        printf("GLSL Version : %s\n", glslVersion);
    }
    void GL_Debugger::printGLExtension(){
        GLint nExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
        for( int i = 0; i < nExtensions; i++ )
            printf("%s\n", glGetStringi( GL_EXTENSIONS, i ) );
    }
}