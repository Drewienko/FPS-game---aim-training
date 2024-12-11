#ifndef PRIMITIVE_RENDERER_H
#define PRIMITIVE_RENDERER_H

class PrimitiveRenderer {
public:
    // Draw points using vertex and color arrays
    static void drawPoints(float* vertices, float* colors, int count);

    // Draw lines using vertex and color arrays
    static void drawLines(float* vertices, float* colors, int count);

    // Draw triangles using vertex and color arrays
    static void drawTriangles(float* vertices, float* colors, int count);

    // Draw a cube with size, position, and color
    static void drawCube(float size, float x, float y, float z, float* color);
};

#endif // PRIMITIVE_RENDERER_H
