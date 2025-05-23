#ifndef MODELOBJECT_H
#define MODELOBJECT_H

#include "DrawableObject.h"
#include "TransformableObject.h"
#include "BitMapHandler.h"
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class ModelObject : public DrawableObject, public TransformableObject {
public:
    ModelObject(const std::string& path);
    void draw(GLuint shaderProgram,
        const glm::mat4& model,
        const glm::mat4& view,
        const glm::mat4& projection) override;
    void setPosition(const glm::vec3& pos);
    void setScale(const glm::vec3& scale);
    void translate(const glm::vec3& direction) override;
    void rotate(float angle, const glm::vec3& axis) override;
    void rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) override;
    void scale(float sx, float sy) override;
    glm::mat4 getModelMatrix() const;

protected:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    glm::vec3 position{ 0.0f };
    glm::vec3 scaleVec{ 1.0f };
    glm::vec3 rotationAxis{ 0.0f, 1.0f, 0.0f };
    float rotationAngle{ 0.0f };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint VAO=0, VBO=0, EBO=0;
        GLuint textureID = 0;
    };

    std::vector<Mesh> meshes;

    void loadModel(const std::string& path);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void setupMesh(Mesh& mesh);
};

#endif //MODELOBJECT_H
