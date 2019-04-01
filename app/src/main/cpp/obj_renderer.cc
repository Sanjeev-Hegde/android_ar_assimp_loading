/*
 * Copyright 2017 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "obj_renderer.h"
#include "util.h"
#include "model.h"
#include "shader.h"
#include <assimp/Importer.hpp>

namespace hello_ar {
namespace {
const glm::vec4 kLightDirection(0.0f, 1.0f, 0.0f, 0.0f);
constexpr char kVertexShaderFilename[] = "shaders/object.vert";
constexpr char kFragmentShaderFilename[] = "shaders/object.frag";
constexpr char k2VertexShaderFilename[] = "shaders/model_loading.vs";
constexpr char k2FragmentShaderFilename[] = "shaders/model_loading.fs";
}  // namespace

void ObjRenderer::InitializeGlContent(AAssetManager* asset_manager,
                                      const std::string& obj_file_name,
                                      const std::string& png_file_name, std::string apkInternalPath) {
  shader_program_ = util::CreateProgram(kVertexShaderFilename,
                                        kFragmentShaderFilename, asset_manager);

  shader_object = util::CreateProgram(k2VertexShaderFilename,
                                      k2FragmentShaderFilename, asset_manager);
  if (!shader_program_) {
    LOGE("Could not create program.");
  }


  uniform_mvp_mat_ =
      glGetUniformLocation(shader_program_, "u_ModelViewProjection");
  uniform_mv_mat_ = glGetUniformLocation(shader_program_, "u_ModelView");
  uniform_texture_ = glGetUniformLocation(shader_program_, "u_Texture");

  uniform_lighting_param_ =
      glGetUniformLocation(shader_program_, "u_LightingParameters");
  uniform_material_param_ =
      glGetUniformLocation(shader_program_, "u_MaterialParameters");
  uniform_color_correction_param_ =
      glGetUniformLocation(shader_program_, "u_ColorCorrectionParameters");
  uniform_color_ = glGetUniformLocation(shader_program_, "u_ObjColor");

  attri_vertices_ = glGetAttribLocation(shader_program_, "a_Position");
  attri_uvs_ = glGetAttribLocation(shader_program_, "a_TexCoord");
  attri_normals_ = glGetAttribLocation(shader_program_, "a_Normal");

  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_2D, texture_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!util::LoadPngFromAssetManager(GL_TEXTURE_2D, png_file_name)) {
    LOGE("Could not load png texture for planes.");
  }
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  //util::LoadObjFile(obj_file_name, asset_manager, &vertices_, &normals_, &uvs_,
  //                  &indices_);

  //std::string  extracted_asset_filename_path;
  util:: loadFileInInternalStorage("ferris_wheel_AlbedoTransparency.png",asset_manager, extracted_asset_filename_path, apkInternalPath);
    util:: loadFileInInternalStorage("ferris_wheel_Emission.png",asset_manager, extracted_asset_filename_path, apkInternalPath);
    util:: loadFileInInternalStorage("ferris_wheel_MetallicSmoothness.png",asset_manager, extracted_asset_filename_path, apkInternalPath);
    util:: loadFileInInternalStorage("ferris_wheel_Normal.png",asset_manager, extracted_asset_filename_path, apkInternalPath);
    util:: loadFileInInternalStorage("ferris_wheel.fbx",asset_manager, extracted_asset_filename_path, apkInternalPath);
  util::CheckGlError("obj_renderer::InitializeGlContent()");
}

void ObjRenderer::SetMaterialProperty(float ambient, float diffuse,
                                      float specular, float specular_power) {
  ambient_ = ambient;
  diffuse_ = diffuse;
  specular_ = specular;
  specular_power_ = specular_power;
}

void ObjRenderer::Draw(const glm::mat4& projection_mat,
                       const glm::mat4& view_mat, const glm::mat4& model_mat,
                       const float* color_correction4,
                       const float* object_color4) const {



  if (!shader_program_) {
    LOGE("shader_program is null.");
    return;
  }

  glUseProgram(shader_program_);

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(uniform_texture_, 0);
  glBindTexture(GL_TEXTURE_2D, texture_id_);

  glm::mat4 mvp_mat = projection_mat * view_mat * model_mat;
  glm::mat4 mv_mat = view_mat * model_mat;
  glm::vec4 view_light_direction = glm::normalize(mv_mat * kLightDirection);

  glUniform4f(uniform_lighting_param_, view_light_direction[0],
              view_light_direction[1], view_light_direction[2], 1.f);
  glUniform4f(uniform_material_param_, ambient_, diffuse_, specular_,
              specular_power_);
  glUniform4fv(uniform_color_correction_param_, 1, color_correction4);
  glUniform4fv(uniform_color_, 1, object_color4);

  glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(mvp_mat));
  glUniformMatrix4fv(uniform_mv_mat_, 1, GL_FALSE, glm::value_ptr(mv_mat));

  // Note: for simplicity, we are uploading the model each time we draw it.  A
  // real application should use vertex buffers to upload the geometry once.

    //  glEnableVertexAttribArray(attri_vertices_);
    //  glVertexAttribPointer(attri_vertices_, 3, GL_FLOAT, GL_FALSE, 0,
    //                        vertices_.data());
    //
    //  glEnableVertexAttribArray(attri_normals_);
    //  glVertexAttribPointer(attri_normals_, 3, GL_FLOAT, GL_FALSE, 0,
    //                        normals_.data());
    //
    //  glEnableVertexAttribArray(attri_uvs_);
    //  glVertexAttribPointer(attri_uvs_, 2, GL_FLOAT, GL_FALSE, 0, uvs_.data());
    //
    //  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT,
    //                 indices_.data());
    //
    //  glDisableVertexAttribArray(attri_vertices_);
    //  glDisableVertexAttribArray(attri_uvs_);
    //  glDisableVertexAttribArray(attri_normals_);

    Model ourModel(extracted_asset_filename_path);
    LOGI("Modal Loaded");
   // ourModel.Draw(shader_object);


         glUseProgram(shader_object);
          unsigned int numberOfLoadedMeshes = ourModel.meshes.size();
          // render all meshes
          for (unsigned int n = 0; n < numberOfLoadedMeshes; ++n) {

            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;

          //  glBufferData(GL_ARRAY_BUFFER, ourModel.meshes[n].vertices.size() * sizeof(Vertex), &ourModel.meshes[n].vertices[0], GL_STATIC_DRAW);
          //  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ourModel.meshes[n].indices.size() * sizeof(unsigned int), &ourModel.meshes[n].indices[0], GL_STATIC_DRAW);
//              for (unsigned int i = 0; i < ourModel.meshes[n].vertices.size(); ++i) {
//                  glEnableVertexAttribArray(0);
//                  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), & ourModel.meshes[n].vertices[i].Position);
//
//                  // vertex normals
//                  glEnableVertexAttribArray(1);
//                  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), & ourModel.meshes[n].vertices[i].Normal);
//
//                  // vertex texture coords
//                  glEnableVertexAttribArray(2);
//                  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), & ourModel.meshes[n].vertices[i].TexCoords);
//                  // vertex tangent
//                  glEnableVertexAttribArray(3);
//                  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), & ourModel.meshes[n].vertices[i].Tangent);
//                  // vertex bitangent
//                  glEnableVertexAttribArray(4);
//                  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), & ourModel.meshes[n].vertices[i].Bitangent);
//
//              }

              glGenBuffers(1, &ourModel.meshes[n].EBO);
              glGenBuffers(1, &ourModel.meshes[n].VBO);
              glBindBuffer(GL_ARRAY_BUFFER, ourModel.meshes[n].VBO);
              glBufferData(GL_ARRAY_BUFFER, ourModel.meshes[n].vertices.size() * sizeof(Vertex), &ourModel.meshes[n].vertices[0], GL_STATIC_DRAW);
              glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourModel.meshes[n].EBO);
              glBufferData(GL_ELEMENT_ARRAY_BUFFER,  ourModel.meshes[n].indices.size() * sizeof(unsigned int), &ourModel.meshes[n].indices[0], GL_STATIC_DRAW);

              // set the vertex attribute pointers
              // vertex Positions
              glEnableVertexAttribArray(0);
              glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
              // vertex normals
              glEnableVertexAttribArray(1);
              glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
              // vertex texture coords
              glEnableVertexAttribArray(2);
              glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
              // vertex tangent
              glEnableVertexAttribArray(3);
              glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
              // vertex bitangent
              glEnableVertexAttribArray(4);
              glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
              glDrawElements(GL_TRIANGLES, ourModel.meshes[n].indices.size(), GL_UNSIGNED_INT, 0);

              glBindVertexArray(0);
              glDisableVertexAttribArray(0);
              glDisableVertexAttribArray(1);
              glDisableVertexAttribArray(2);
              glDisableVertexAttribArray(3);
              glDisableVertexAttribArray(4);
//              glDisableVertexAttribArray(attri_vertices_);
//              glDisableVertexAttribArray(attri_uvs_);
//              glDisableVertexAttribArray(attri_normals_);


                //        glEnableVertexAttribArray(0);
                //        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ourModel.meshes[n].indices.size() * sizeof(unsigned int), &ourModel.meshes[n].indices[0], GL_STATIC_DRAW);
                //        glBufferData(GL_ARRAY_BUFFER, ourModel.meshes[n].vertices.size() * sizeof(Vertex), &ourModel.meshes[n].vertices[0], GL_STATIC_DRAW);
                //
                //        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
                //        // vertex normals
                //        glEnableVertexAttribArray(1);
                //        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
                //        // vertex texture coords
                //        glEnableVertexAttribArray(2);
                //        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
                //        // vertex tangent
                //        glEnableVertexAttribArray(3);
                //        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
                //        // vertex bitangent
                //        glEnableVertexAttribArray(4);
                //        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
                //        glDrawElements(GL_TRIANGLES, ourModel.meshes[n].vertices.size(), GL_UNSIGNED_INT, 0);
                //        glBindVertexArray(0);
                //
                //        // unbind buffers
                //        glBindBuffer(GL_ARRAY_BUFFER, 0);
                //        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        }


  glUseProgram(0);
  util::CheckGlError("obj_renderer::Draw()");
}

}  // namespace hello_ar
