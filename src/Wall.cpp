#include "Wall.h"

Wall::Wall(float width, float height, float x, float y, float z, GLuint texture) {
    vertices = {
        x,          y,          z,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
        x + width,  y,          z,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
        x + width,  y + height, z,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
        x,          y + height, z,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f
    };

    indices = {
        0, 1, 2,
        2, 3, 0
    };

    this->textureID = texture;

    setupBuffers();
}

void Wall::setupBuffers() {
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); 
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

void Wall::draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}

void Wall::translate(const glm::vec3& direction) {
    for (size_t i = 0; i < vertices.size(); i += 8) {
        vertices[i] += direction.x; 
        vertices[i + 1] += direction.y; 
        vertices[i + 2] += direction.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Wall::rotate(float angle, const glm::vec3& axis) {
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    for (size_t i = 0; i < vertices.size(); i += 8) {
        glm::vec4 vertex(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
        vertex = rotationMatrix * vertex;
        vertices[i] = vertex.x;
        vertices[i + 1] = vertex.y;
        vertices[i + 2] = vertex.z;

        glm::vec4 normal(vertices[i + 5], vertices[i + 6], vertices[i + 7], 0.0f);
        normal = rotationMatrix * normal;
        vertices[i + 5] = normal.x;
        vertices[i + 6] = normal.y;
        vertices[i + 7] = normal.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Wall::scale(float sx, float sy) {
    glm::vec3 center(0.0f);
    size_t vertexCount = vertices.size() / 8;

    for (size_t i = 0; i < vertices.size(); i += 8) {
        center.x += vertices[i];
        center.y += vertices[i + 1];
        center.z += vertices[i + 2];
    }
    center /= static_cast<float>(vertexCount);

    for (size_t i = 0; i < vertices.size(); i += 8) {
        vertices[i] = center.x + (vertices[i] - center.x) * sx;
        vertices[i + 1] = center.y + (vertices[i + 1] - center.y) * sy;

        glm::vec3 normal(vertices[i + 5], vertices[i + 6], vertices[i + 7]);
        normal = glm::normalize(normal);
        vertices[i + 5] = normal.x;
        vertices[i + 6] = normal.y;
        vertices[i + 7] = normal.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Wall::rotatePoint(float angle, const glm::vec3& axis, const glm::vec3& point) {
    translate(-point);
    rotate(angle, axis);
    translate(point);
}

void Wall::rotateAround(float angle, const glm::vec3& axis) {
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    for (size_t i = 0; i < vertices.size(); i += 8) {
        center.x += vertices[i]; 
        center.y += vertices[i + 1];
        center.z += vertices[i + 2];
    }
    center /= static_cast<float>(vertices.size() / 8);

    rotatePoint(angle,axis,center);
}

glm::vec3 Wall::getMinBounds() const {
    glm::vec3 min(FLT_MAX);
    for (size_t i = 0; i < vertices.size(); i += 8) {
        min.x = std::min(min.x, vertices[i]);
        min.y = std::min(min.y, vertices[i + 1]);
        min.z = std::min(min.z, vertices[i + 2]);
    }
    return min;
}

glm::vec3 Wall::getMaxBounds() const {
    glm::vec3 max(-FLT_MAX);
    for (size_t i = 0; i < vertices.size(); i += 8) {
        max.x = std::max(max.x, vertices[i]);
        max.y = std::max(max.y, vertices[i + 1]);
        max.z = std::max(max.z, vertices[i + 2]);
    }
    return max;
}

