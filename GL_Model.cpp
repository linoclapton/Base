#include "GL_Model.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "GL_OBJ.h"
#include "ULYMath.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include <windows.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
#include <mmsystem.h>

namespace ULY{

    static std::string GetBaseDir(const std::string &filepath) {
        if (filepath.find_last_of("/\\") != std::string::npos)
            return filepath.substr(0, filepath.find_last_of("/\\"));
        return "";
    }

    static bool FileExists(const std::string &abs_filename) {
        bool ret;
        FILE *fp = fopen(abs_filename.c_str(), "rb");
        if (fp) {
            ret = true;
            fclose(fp);
        } else {
            ret = false;
        }

        return ret;
    }

    static void CheckErrors(std::string desc) {
        GLenum e = glGetError();
        if (e != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", desc.c_str(), e, e);
        }
    }

    static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
        float v10[3];
        v10[0] = v1[0] - v0[0];
        v10[1] = v1[1] - v0[1];
        v10[2] = v1[2] - v0[2];

        float v20[3];
        v20[0] = v2[0] - v0[0];
        v20[1] = v2[1] - v0[1];
        v20[2] = v2[2] - v0[2];

        N[0] = v20[1] * v10[2] - v20[2] * v10[1];
        N[1] = v20[2] * v10[0] - v20[0] * v10[2];
        N[2] = v20[0] * v10[1] - v20[1] * v10[0];

        float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
        if (len2 > 0.0f) {
            float len = sqrtf(len2);

            N[0] /= len;
            N[1] /= len;
        }
    }

    static bool LoadObjAndConvert(float bmin[3], float bmax[3],
        std::vector<DrawObject>* drawObjects,
        std::vector<ULY::material_t>& materials,
        std::map<std::string, GLuint>& textures,
        const char* filename) {
            ULY::attrib_t attrib;
            std::vector<ULY::shape_t> shapes;

            std::string base_dir = GetBaseDir(filename);
            if (base_dir.empty()) {
                base_dir = ".";
            }
#ifdef _WIN32
            base_dir += "/";
#else
            base_dir += "/";
#endif

            std::string err;
            bool ret =
                ULY::LoadObj(&attrib, &shapes, &materials, &err, filename, base_dir.c_str());
            if (!err.empty()) {
                std::cerr << err << std::endl;
            }


            ULY::normalizeAttrib(&attrib);
            if (!ret) {
                std::cerr << "Failed to load " << filename << std::endl;
                return false;
            }


            printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
            printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
            printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
            printf("# of materials = %d\n", (int)materials.size());
            printf("# of shapes    = %d\n", (int)shapes.size());

            // Append `default` material
            materials.push_back(ULY::material_t());

            for (size_t i = 0; i < materials.size(); i++) {
                printf("material[%d].diffuse_texname = %s\n", int(i), materials[i].diffuse_texname.c_str());
            }

            // Load diffuse textures
            {
                for (size_t m = 0; m < materials.size(); m++) {
                    ULY::material_t* mp = &materials[m];

                    if (mp->diffuse_texname.length() > 0) {
                        // Only load the texture if it is not already loaded
                        if (textures.find(mp->diffuse_texname) == textures.end()) {
                            GLuint texture_id;
                            int w, h;
                            int comp;

                            std::string texture_filename = mp->diffuse_texname;
                            if (!FileExists(texture_filename)) {
                                // Append base dir.
                                texture_filename = base_dir + mp->diffuse_texname;
                                if (!FileExists(texture_filename)) {
                                    std::cerr << "Unable to find file: " << mp->diffuse_texname << std::endl;
                                    exit(1);
                                }
                            }

                            unsigned char* image = Image::load(texture_filename.c_str(), &w, &h, &comp);
                            if (!image) {
                                std::cerr << "Unable to load texture: " << texture_filename << std::endl;
                                exit(1);
                            }
                            std::cout << "Loaded texture: " << texture_filename << ", w = " << w << ", h = " << h << ", comp = " << comp << std::endl;

                            glBindTexture(GL_TEXTURE_2D, 0);
                            glGenTextures(1, &texture_id);
                            glBindTexture(GL_TEXTURE_2D, texture_id);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                            if (comp == 3) {
                                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                            }
                            else if (comp == 4) {
                                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                            } else {
                                assert(0); // TODO
                            }
                            glBindTexture(GL_TEXTURE_2D, 0);
                            SOIL_free_image_data(image);
                            textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
                        }
                    }
                }
            }

            bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
            bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

            {
                for (size_t s = 0; s < shapes.size(); s++) {
                    DrawObject o;
                    std::vector<float> buffer;  // pos(3float), normal(3float), color(3float)
                    for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
                        ULY::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
                        ULY::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
                        ULY::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

                        int current_material_id = shapes[s].mesh.material_ids[f];

                        if ((current_material_id < 0) || (current_material_id >= static_cast<int>(materials.size()))) {
                            // Invaid material ID. Use default material.
                            current_material_id = materials.size() - 1; // Default material is added to the last item in `materials`.
                        }
                        //if (current_material_id >= materials.size()) {
                        //    std::cerr << "Invalid material index: " << current_material_id << std::endl;
                        //}
                        //
                        float diffuse[3];
                        for (size_t i = 0; i < 3; i++) {
                            diffuse[i] = materials[current_material_id].diffuse[i];
                        }
                        float tc[3][2];
                        if (attrib.texcoords.size() > 0) {
                            assert(attrib.texcoords.size() > 2 * idx0.texcoord_index + 1);
                            assert(attrib.texcoords.size() > 2 * idx1.texcoord_index + 1);
                            assert(attrib.texcoords.size() > 2 * idx2.texcoord_index + 1);
                            // Flip Y coord.
                            tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
                            tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
                            tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
                            tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
                            tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
                            tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
                        } else {
                            tc[0][0] = 0.0f;
                            tc[0][1] = 0.0f;
                            tc[1][0] = 0.0f;
                            tc[1][1] = 0.0f;
                            tc[2][0] = 0.0f;
                            tc[2][1] = 0.0f;
                        }

                        float v[3][3];
                        for (int k = 0; k < 3; k++) {
                            int f0 = idx0.vertex_index;
                            int f1 = idx1.vertex_index;
                            int f2 = idx2.vertex_index;
                            assert(f0 >= 0);
                            assert(f1 >= 0);
                            assert(f2 >= 0);

                            v[0][k] = attrib.vertices[3 * f0 + k];
                            v[1][k] = attrib.vertices[3 * f1 + k];
                            v[2][k] = attrib.vertices[3 * f2 + k];
                            bmin[k] = std::min(v[0][k], bmin[k]);
                            bmin[k] = std::min(v[1][k], bmin[k]);
                            bmin[k] = std::min(v[2][k], bmin[k]);
                            bmax[k] = std::max(v[0][k], bmax[k]);
                            bmax[k] = std::max(v[1][k], bmax[k]);
                            bmax[k] = std::max(v[2][k], bmax[k]);
                        }

                        float n[3][3];
                        if (attrib.normals.size() > 0) {
                            int f0 = idx0.normal_index;
                            int f1 = idx1.normal_index;
                            int f2 = idx2.normal_index;
                            assert(f0 >= 0);
                            assert(f1 >= 0);
                            assert(f2 >= 0);
                            for (int k = 0; k < 3; k++) {
                                n[0][k] = attrib.normals[3 * f0 + k];
                                n[1][k] = attrib.normals[3 * f1 + k];
                                n[2][k] = attrib.normals[3 * f2 + k];
                            }
                        } else {
                            // compute geometric normal
                            CalcNormal(n[0], v[0], v[1], v[2]);
                            n[1][0] = n[0][0];
                            n[1][1] = n[0][1];
                            n[1][2] = n[0][2];
                            n[2][0] = n[0][0];
                            n[2][1] = n[0][1];
                            n[2][2] = n[0][2];
                        }

                        for (int k = 0; k < 3; k++) {
                            buffer.push_back(v[k][0]);
                            buffer.push_back(v[k][1]);
                            buffer.push_back(v[k][2]);
                            buffer.push_back(n[k][0]);
                            buffer.push_back(n[k][1]);
                            buffer.push_back(n[k][2]);
                            // Combine normal and diffuse to get color.
                            float normal_factor = 0.2;
                            float diffuse_factor = 1 - normal_factor;
                            float c[3] = {
                                n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
                                n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
                                n[k][2] * normal_factor + diffuse[2] * diffuse_factor
                            };
                            float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
                            if (len2 > 0.0f) {
                                float len = sqrtf(len2);

                                c[0] /= len;
                                c[1] /= len;
                                c[2] /= len;
                            }
                            buffer.push_back(c[0] * 0.5 + 0.5);
                            buffer.push_back(c[1] * 0.5 + 0.5);
                            buffer.push_back(c[2] * 0.5 + 0.5);

                            buffer.push_back(tc[k][0]);
                            buffer.push_back(tc[k][1]);
                        }
                    }

                    o.vb_id = 0;
                    o.numTriangles = 0;

                    // OpenGL viewer does not support texturing with per-face material.
                    if (shapes[s].mesh.material_ids.size() > 0 && shapes[s].mesh.material_ids.size() > s) {
                        o.material_id = shapes[s].mesh.material_ids[0]; // use the material ID of the first face.
                    } else {
                        o.material_id = materials.size() - 1; // = ID for default material.
                    }
                    printf("shape[%d] material_id %d\n", int(s), int(o.material_id));

                    if (buffer.size() > 0) {
                        glGenBuffers(1, &o.vb_id);
                        glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);
                        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0),
                            GL_STATIC_DRAW);
                        o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) / 3; // 3:vtx, 3:normal, 3:col, 2:texcoord

                        printf("shape[%d] # of triangles = %d\n", static_cast<int>(s),
                            o.numTriangles);
                    }

                    drawObjects->push_back(o);
                }
            }

            printf("bmin = %f, %f, %f\n", bmin[0], bmin[1], bmin[2]);
            printf("bmax = %f, %f, %f\n", bmax[0], bmax[1], bmax[2]);

            return true;
    }




    static void Draw(const std::vector<DrawObject>& drawObjects, std::vector<ULY::material_t>& materials, std::map<std::string, GLuint>& textures) {
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 1.0);
        GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
        for (size_t i = 0; i < drawObjects.size(); i++) {
            DrawObject o = drawObjects[i];
            if (o.vb_id < 1) {
                continue;
            }

            glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glBindTexture(GL_TEXTURE_2D, 0);
            if ((o.material_id < materials.size())) {
                std::string diffuse_texname = materials[o.material_id].diffuse_texname;
                if (textures.find(diffuse_texname) != textures.end()) {
                    glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
                }
            }
            glVertexPointer(3, GL_FLOAT, stride, (const void*)0);
            glNormalPointer(GL_FLOAT, stride, (const void*)(sizeof(float) * 3));
            glColorPointer(3, GL_FLOAT, stride, (const void*)(sizeof(float) * 6));
            glTexCoordPointer(2, GL_FLOAT, stride, (const void*)(sizeof(float) * 9));

            glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
            CheckErrors("drawarrays");
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // draw wireframe
        /*glDisable(GL_POLYGON_OFFSET_FILL);
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);

        glColor3f(0.0f, 0.0f, 0.4f);
        for (size_t i = 0; i < drawObjects.size(); i++) {
            DrawObject o = drawObjects[i];
            if (o.vb_id < 1) {
                continue;
            }

            glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glVertexPointer(3, GL_FLOAT, stride, (const void*)0);
            glNormalPointer(GL_FLOAT, stride, (const void*)(sizeof(float) * 3));
            glColorPointer(3, GL_FLOAT, stride, (const void*)(sizeof(float) * 6));
            glTexCoordPointer(2, GL_FLOAT, stride, (const void*)(sizeof(float) * 9));

            glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
            CheckErrors("drawarrays");
        }*/
    }
    void GL_Obj_Model::init(string path,string name){
        string err;
        if (false == LoadObjAndConvert(bmin, bmax, &gDrawObjects, materials, textures, (path+name).c_str())) {
            return;
        }
        float maxExtent = 0.5f * (bmax[0] - bmin[0]);
        if (maxExtent < 0.5f * (bmax[1] - bmin[1])) {
            maxExtent = 0.5f * (bmax[1] - bmin[1]);
        }
        if (maxExtent < 0.5f * (bmax[2] - bmin[2])) {
            maxExtent = 0.5f * (bmax[2] - bmin[2]);
        }
        pg = new GL_PG();
        /*bool ret = ULY::LoadObj(&attrib, &shapes, &materials, &err,(path+name).c_str(),path.c_str());
        this->path = path;
        for(int i = 0 ; i < materials.size() ; ++i){
            if(materials[i].bump_texname!="") {
                int w,h;
                unsigned char* m = Image::load(path+materials[i].bump_texname,&w,&h);
                img.push_back(m);
                resolution.push_back(make_pair(w,h));
            }else if(materials[i].diffuse_texname != ""){
                int w,h;
                unsigned char* m = Image::load(path+materials[i].diffuse_texname ,&w,&h);
                img.push_back(m);
                resolution.push_back(make_pair(w,h));
            }else if(materials[i].emissive_texname != ""){
                int w,h;
                unsigned char* m = Image::load(path+materials[i].emissive_texname ,&w,&h);
                img.push_back(m);
                resolution.push_back(make_pair(w,h));
            }
        }*/
        //ULY::normalizeAttrib(&attrib);
    }

    void GL_Obj_Model::render(){
        if(pg){
            // Fit to -1, 1
            //pg->use();
            Draw(gDrawObjects, materials, textures);
        }else{
            for(int i = 0 ; i < shapes.size() ; ++i){
                if(img.size()!=0 && img[i]){
                    GLuint texture;
                    glEnable(GL_TEXTURE_2D);
                    glGenTextures(1,&texture);
                    glBindTexture(GL_TEXTURE_2D,texture);
                    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,resolution[i].first,resolution[i].second,0,GL_RGB,GL_UNSIGNED_BYTE,img[i]);
                    // Set texture filtering parameters
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                }
                //glGenerateMipmap(GL_TEXTURE_2D);
                glCullFace(GL_BACK);
                //glCullFace(GL_FRONT);
                //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                size_t index_offset = 0;
                for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
                    int fv = shapes[i].mesh.num_face_vertices[f];

                    // Loop over vertices in the face.
                    glBegin(GL_POLYGON);
                    for (size_t v = 0; v < fv; v++) {
                        // access to vertex
                        index_t idx = shapes[i].mesh.indices[index_offset + v];
                        real_t vx = attrib.vertices[3*idx.vertex_index+0];
                        real_t vy = attrib.vertices[3*idx.vertex_index+1];
                        real_t vz = attrib.vertices[3*idx.vertex_index+2];
                        glVertex3f(vx,vy,vz);
                        if(idx.normal_index>-1){
                            real_t nx = attrib.normals[3*idx.normal_index+0];
                            real_t ny = attrib.normals[3*idx.normal_index+1];
                            real_t nz = attrib.normals[3*idx.normal_index+2];
                            glNormal3f(nx,ny,nz);
                        }
                        if(attrib.texcoords.size()>0){
                            real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                            real_t ty = 1.0-attrib.texcoords[2*idx.texcoord_index+1];
                            glTexCoord2f(tx,ty);
                        }
                        // Optional: vertex colors
                        if(attrib.colors.size()>0){
                         real_t red = attrib.colors[3*idx.vertex_index+0];
                         real_t green = attrib.colors[3*idx.vertex_index+1];
                         real_t blue = attrib.colors[3*idx.vertex_index+2];
                         glColor3f(red,green,blue);
                        }
                    }
                    glEnd();
                    index_offset += fv;

                    // per-face material
                    //shapes[i].mesh.material_ids[f];
                }
            }
        }
    }
}
