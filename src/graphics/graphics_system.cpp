#include "graphics_system.h"

#include "debug.h"
#include "memory.h"
#include "graphics_opengl.h"
#include "graphics_program.h"
#include "graphics_shader.h"
#include "graphics_vertex_buffer.h"
#include "graphics_world.h"
#include "vector4.h"
#include <math.h>

namespace gengine
{
namespace graphics
{

#ifdef EMSCRIPTEN
#define PRECISION " highp "
#else
#define PRECISION
#endif

const char vertex_shader_source[] =
    "attribute vec2 position;\n"
    "attribute vec2 texCoords;\n"
    "varying " PRECISION "vec2 v_texCoords;\n"
    "uniform " PRECISION "mat3 projectionMatrix;\n"
    "uniform " PRECISION "mat3 transformMatrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 res = transformMatrix * vec3(position,1.0 ) * projectionMatrix;\n"
    "    v_texCoords = texCoords;\n"
    "    gl_Position = vec4(res,1.0);\n"
    "}";
const char fragment_shader_source[] =
    "varying " PRECISION "vec2 v_texCoords;\n"
    "uniform sampler2D tex0;\n"
    "uniform " PRECISION "vec4 color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(tex0, v_texCoords) * color;\n"
    "}";

#undef PRECISION

void System::init()
{
    geDebugLog("graphics::System::init()");

    Vertex vertices[4];
    ushort indices[6];

    defaultVertexShader.init(GL_VERTEX_SHADER);
    defaultVertexShader.compile(vertex_shader_source);

    defaultFragmentShader.init(GL_FRAGMENT_SHADER);
    defaultFragmentShader.compile(fragment_shader_source);

    defaultProgram.init();
    defaultProgram.attachShader(defaultVertexShader);
    defaultProgram.attachShader(defaultFragmentShader);
    defaultProgram.link();

    vertices[0].x = -0.5f;
    vertices[0].y = 0.5f;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;

    vertices[1].x = 0.5f;
    vertices[1].y = 0.5f;
    vertices[1].u = 1.0f;
    vertices[1].v = 0.0f;

    vertices[2].x = 0.5f;
    vertices[2].y = -0.5f;
    vertices[2].u = 1.0f;
    vertices[2].v = 1.0f;

    vertices[3].x = -0.5f;
    vertices[3].y = -0.5f;
    vertices[3].u = 0.0f;
    vertices[3].v = 1.0f;

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    vertexBufferQuad.init();
    vertexBufferQuad.setData(vertices, 4);

    indexBufferQuad.init();
    indexBufferQuad.setData(indices, 6);

    defaultTexture.init();
    defaultTexture.setFromFile("bird.png");

    defaultCamera.setExtent(Vector2(640.0f, 480.0f));

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);

    {
        World * world = new World();
        world->init();
        worldTable.add(world);

        for(int i=0; i<2560; i++)
        {
            int x,y;
            x = i % 48;
            y = i / 48;
            testSpriteTable[i].setPosition(Vector2(-320.0f + x * 16.0f,-240.0f + y * 16.0f));
            testSpriteTable[i].setExtent(Vector2(16.0f,16.0f));
            testSpriteTable[i].setTexture(defaultTexture);

            world->addSprite(testSpriteTable[i]);
        }

    }
}

void System::finalize()
{
    geDebugLog("graphics::System::finalize()");

    for(World * world : worldTable)
    {
        world->finalize();
    }

    defaultTexture.finalize();
    defaultProgram.finalize();
    defaultFragmentShader.finalize();
    defaultVertexShader.finalize();
    vertexBufferQuad.finalize();
    indexBufferQuad.finalize();
}

void System::update()
{
    for(World * world : worldTable)
    {
        world->update();
    }
}

void System::render()
{
    for(World * world : worldTable)
    {
        world->render();
    }
}

void System::test(const float dt)
{
    static float total = 0;
    total += dt * 3;

    for(int i=0; i<2560; i++)
    {
        testSpriteTable[i].setRotation(i + total);
        testSpriteTable[i].setColorAlpha(0.6f + 0.4f * sinf(total + i));
    }

}

World & System::getWorld(const uint index)
{
    return * worldTable[index];
}

}
}
