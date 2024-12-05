#ifndef PRIMITIVE_RENDERER_H
#define PRIMITIVE_RENDERER_H

class PrimitiveRenderer {
public:
    static void drawSphere(float radius, int slices, int stacks);
    static void drawCone(float base, float height, int slices, int stacks);
    static void drawTorus(float innerRadius, float outerRadius, int sides, int rings);
    static void drawCube(float size);
    static void drawTeapot(float size);
    static void setColor(float r, float g, float b);
};

#endif // PRIMITIVE_RENDERER_H