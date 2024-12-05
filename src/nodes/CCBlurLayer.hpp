// this was TAKEN from eclipse, not DOGGO
#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct Shader {
    GLuint vertex = 0;
    GLuint fragment = 0;
    GLuint program = 0;

    geode::Result<std::string> compile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    geode::Result<std::string> link();
    void cleanup();
};

struct RenderTexture {
    GLuint fbo = 0;
    GLuint tex = 0;
    GLuint rbo = 0;

    void setup(GLsizei width, GLsizei height);
    void resize(GLsizei width, GLsizei height) const;
    void cleanup();
};

struct Blur {
    RenderTexture ppRt0;
    RenderTexture ppRt1;
    GLuint ppVao = 0;
    GLuint ppVbo = 0;
    Shader ppShader;
    GLint ppShaderFast = 0;
    GLint ppShaderFirst = 0;
    GLint ppShaderRadius = 0;

    float blurTimer = 0.f;
    float blurProgress = 0.f;
};

class CCBlurLayer : public CCLayerColor {
    public:
        bool visiting = false;
        float blurStrength = 1;

        ~CCBlurLayer();
        bool init();
        void visit();
        void draw();

        GLuint compileShader(GLenum type, const char* source);
        GLuint loadShaderProgram(std::filesystem::path vertShaderPath, std::filesystem::path fragShaderPath);

        static CCBlurLayer* create();
};
