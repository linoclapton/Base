#include "GL_Skybox.h"

namespace ULY{
    GLuint Skybox::loadCubeMapTexture(std::vector<const char*> picFilePathVec, GLint internalFormat,
        GLenum picFormat, GLenum picDataType, int loadChannels)
    {
        GLuint textId;
        glGenTextures(1, &textId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textId);
        GLubyte *imageData = NULL;
        int picWidth, picHeight;
        for (std::vector<const char*>::size_type  i =0; i < picFilePathVec.size(); ++i)
        {
            int channels = 0;
            imageData = SOIL_load_image(picFilePathVec[i], &picWidth, 
                &picHeight, &channels, loadChannels);
            if (imageData == NULL)
            {
                std::cerr << "Error::loadCubeMapTexture could not load texture file:"
                    << picFilePathVec[i] << std::endl;
                return 0;
            }
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, internalFormat, picWidth, picHeight, 0, picFormat, picDataType, imageData);
            SOIL_free_image_data(imageData);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return textId;
    }
}