#include "ModelObject.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ModelObject::ModelObject(const std::string& path) {
    loadModel(path);
}

void ModelObject::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        Mesh mesh = processMesh(scene->mMeshes[i]);
        setupMesh(mesh);
        meshes.push_back(mesh);
    }
}

ModelObject::Mesh ModelObject::processMesh(aiMesh* mesh) {
    Mesh out;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z);

        vertex.normal = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z) : glm::vec3(0.0f);

        vertex.texCoord = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x,
            mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);

        out.vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            out.indices.push_back(face.mIndices[j]);
        }
    }

    return out;
}


void ModelObject::setupMesh(Mesh& mesh) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


void ModelObject::draw(GLuint shaderProgram,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    for (const Mesh& mesh : meshes) {
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}


void ModelObject::setPosition(const glm::vec3& pos) {
    position = pos;
}

void ModelObject::setScale(const glm::vec3& scl) {
    scaleVec = scl;
}

void ModelObject::translate(const glm::vec3& direction) {
    position += direction;
}

void ModelObject::rotate(float angle, const glm::vec3& axis) {
    rotationAngle = angle;
    rotationAxis = axis;
}

glm::mat4 ModelObject::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
    model = glm::scale(model, scaleVec);
    return model;
}

void ModelObject::rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) {
    position -= point;
    rotate(angle, axis);
    position += point;
}

void ModelObject::scale(float sx, float sy) {
    scaleVec.x *= sx;
    scaleVec.y *= sy;
}

