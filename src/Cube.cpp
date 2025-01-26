#include "Cube.h"
#include "PrimitiveRenderer.h"

Cube::Cube(float size, float x, float y, float z, const float* color) {
    vertices = {
        // Positions          // Texture Coords   // Normals
        // Front face
        x - size, y - size, z + size,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
        x + size, y - size, z + size,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
        x + size, y + size, z + size,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
        x - size, y + size, z + size,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,
        // Back face
        x - size, y - size, z - size,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
        x + size, y - size, z - size,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
        x + size, y + size, z - size,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
        x - size, y + size, z - size,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,
        // Left face
        x - size, y - size, z - size,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        x - size, y - size, z + size,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        x - size, y + size, z + size,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        x - size, y + size, z - size,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        // Right face
        x + size, y - size, z - size,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
        x + size, y - size, z + size,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
        x + size, y + size, z + size,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
        x + size, y + size, z - size,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
        // Top face
        x - size, y + size, z - size,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
        x + size, y + size, z - size,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
        x + size, y + size, z + size,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
        x - size, y + size, z + size,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
        // Bottom face
        x - size, y - size, z - size,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
        x + size, y - size, z - size,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
        x + size, y - size, z + size,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
        x - size, y - size, z + size,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f
    };

    indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Back face
        4, 6, 5,
        6, 4, 7,
        // Left face
        8, 9, 10,
        10, 11, 8,
        // Right face
        12, 14, 13,
        14, 12, 15,
        // Top face
        16, 18, 17,
        18, 16, 19,
        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    for (int i = 0; i < 6; ++i) {
        textures[i] = 0; // Initialize with no texture by default
    }

    setupBuffers();
}

void Cube::setupBuffers() {
    // Generate VAO, VBO, and EBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // Normals
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Cube::draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    // Pass matrices to shader
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Bind textures for each face
    for (int side = 0; side < 6; ++side) {
        if (textures[side] != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[side]);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        }
    }

    // Render the cube
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}



void Cube::setTextureForSide(int side, GLuint textureID) {
    if (side >= 0 && side < 6) {
        textures[side] = textureID;
    }
}

void Cube::translate(const glm::vec3& direction) {
    for (size_t i = 0; i < vertices.size(); i += 8) { // Stride of 8
        vertices[i] += direction.x;     // Update X
        vertices[i + 1] += direction.y; // Update Y
        vertices[i + 2] += direction.z; // Update Z
    }

    // Update GPU buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::rotate(float angle, const glm::vec3& axis) {
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    for (size_t i = 0; i < vertices.size(); i += 8) { // Stride of 8
        glm::vec4 vertex(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f); // Position
        vertex = rotationMatrix * vertex;
        vertices[i] = vertex.x;
        vertices[i + 1] = vertex.y;
        vertices[i + 2] = vertex.z;

        // Update normal
        glm::vec4 normal(vertices[i + 5], vertices[i + 6], vertices[i + 7], 0.0f); // Normal (w = 0.0)
        normal = rotationMatrix * normal;
        vertices[i + 5] = normal.x;
        vertices[i + 6] = normal.y;
        vertices[i + 7] = normal.z;
    }

    // Update GPU buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) {
    this->translate(-point);

    this->rotate(angle, axis);

    this->translate(point);
}

void Cube::scale(float sx, float sy) {
    glm::vec3 center(0.0f);
    size_t vertexCount = vertices.size() / 8; // Divide by stride (8 floats per vertex)

    // Calculate center of the cube
    for (size_t i = 0; i < vertices.size(); i += 8) {
        center.x += vertices[i];     // X
        center.y += vertices[i + 1]; // Y
        center.z += vertices[i + 2]; // Z
    }
    center /= static_cast<float>(vertexCount);

    // Scale positions
    for (size_t i = 0; i < vertices.size(); i += 8) {
        vertices[i] = center.x + (vertices[i] - center.x) * sx;     // Scale X
        vertices[i + 1] = center.y + (vertices[i + 1] - center.y) * sy; // Scale Y
        vertices[i + 2] = center.z; // Z remains unchanged

        // Normalize normals (scaling distorts them)
        glm::vec3 normal(vertices[i + 5], vertices[i + 6], vertices[i + 7]);
        normal = glm::normalize(normal);
        vertices[i + 5] = normal.x;
        vertices[i + 6] = normal.y;
        vertices[i + 7] = normal.z;
    }

    // Update GPU buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

