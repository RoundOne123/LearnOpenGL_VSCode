#include "Chapter13/MultipleLights.h"
#include "camera.h"
#include "shader.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;

// 设置
static unsigned int SCR_WIDTH = 800;
static unsigned int SCR_HEIGHT = 600;

// 函数声明
// 输入
static void processInput(GLFWwindow *window);
// 监听窗口变化
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// 监听光标位置变化
static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// 监听鼠标滚轮变化
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// 加载纹理
static unsigned int loadTexture(char const *path);

// 声明变量
static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
static float deltaTime = 0.0f;
static float lastFrameTime = 0.0f;
static float lastX = 400, lastY = 300; // 初始值为屏幕中心
static bool firstMouse = true;         // 处理 刚进入窗口时的闪动

//static glm::vec3 lightPos(-0.2f, -1.0f, -0.3f); // 平行光方向
//static glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // 点光源位置

// 摄像机
static Camera camera(cameraPos);

int MultipleLightsMain()
{

#pragma region // 一些初始化工作

    // 实例化GLFW窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建一个窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 调用任何OpenGL函数之前初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // 视口 （注意和窗口的区别）
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

#pragma endregion

#pragma region // 添加监听
    // 监听窗口变化
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 监听光标位置变化
    glfwSetCursorPosCallback(window, mouse_callback);
    // 监听鼠标滚轮变化
    glfwSetScrollCallback(window, scroll_callback);
#pragma endregion

#pragma region // 设置相关

    // 线框模式
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 隐藏并捕捉光标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#pragma endregion

#pragma region // 顶点数据

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    unsigned int VAO, VBO, lightVAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定VAO 配置属性
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 绑定lightVAO
    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 解绑VAO VBO 可解绑 可不解绑
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

#pragma endregion

#pragma region // 纹理相关

    unsigned int diffuseMap = loadTexture("./textures/Chapter13-Textures/container2.png");
    unsigned int specularMap = loadTexture("./textures/Chapter13-Textures/container2_specular.png");

#pragma endregion

#pragma region // 着色器相关

    // 着色器
    Shader objectShader("./src/Chapter13-MultipleLights/shader.vs", "./src/Chapter13-MultipleLights/shader.fs");
    Shader lightShader("./src/Chapter13-MultipleLights/lightShader.vs", "./src/Chapter13-MultipleLights/lightShader.fs");

    // 设置Uniform 前先激活shaderProgram
    objectShader.use();
    // 定义那个uniform对应哪个纹理单元
    objectShader.setInt("material.diffuse", 0);
    objectShader.setInt("material.specular", 1);

#pragma endregion

    // 盒子的世界空间坐标
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    // 平行光的方向
    static glm::vec3 dirLightDir(-0.2f, -1.0f, -0.3f); // 平行光方向

    // 顶点光照的位置
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    while (!glfwWindowShouldClose(window))
    {
        // 计算相机位移
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 准备变换矩阵
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // 设置变换矩阵
        objectShader.use();
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);

        // 设置物体颜色 -> 激活纹理单元 绑定纹理 这里就是设置了 物体颜色 的分量
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        objectShader.setFloat("material.shininess", 32.0f);

        // 摄像机位置
        objectShader.setVec3("viewPos", camera.Position);

#pragma region // 设置灯光相关uniform

        // 方向光uniform
        objectShader.setVec3("dirLight.direction", dirLightDir);
        objectShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        objectShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // 点光源uniform
        // for (int i = 0; i < 4; i++)
        // {
        //     objectShader.setVec3("pointLights[i].position", pointLightPositions[i]);
        //     objectShader.setFloat("pointLights[i].constant", 1.0f);
        //     objectShader.setFloat("pointLights[i].linear", 0.09f);
        //     objectShader.setFloat("pointLights[i].quadratic", 0.032f);
        //     objectShader.setVec3("pointLights[i].ambient", 0.05f, 0.05f, 0.05f);
        //     objectShader.setVec3("pointLights[i].diffuse", 0.8f, 0.8f, 0.8f);
        //     objectShader.setVec3("pointLights[i].specular", 1.0f, 1.0f, 1.0f);
        // }
        // 这里不能像上面一样使用For循环 会导致最终结果成黑色的
        // 想不通呀！？
        objectShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        objectShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[0].constant", 1.0f);
        objectShader.setFloat("pointLights[0].linear", 0.09);
        objectShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        objectShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        objectShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[1].constant", 1.0f);
        objectShader.setFloat("pointLights[1].linear", 0.09);
        objectShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        objectShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        objectShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[2].constant", 1.0f);
        objectShader.setFloat("pointLights[2].linear", 0.09);
        objectShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        objectShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        objectShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        objectShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[3].constant", 1.0f);
        objectShader.setFloat("pointLights[3].linear", 0.09);
        objectShader.setFloat("pointLights[3].quadratic", 0.032);

        // 聚光
        objectShader.setVec3("spotLight.position", camera.Position);
        objectShader.setVec3("spotLight.direction", camera.Front);
        // 注意这里传入的是cos值 因为反三角函数的代价太昂贵 直接比较cos值
        objectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        objectShader.setFloat("spotLight.constant", 1.0f);
        objectShader.setFloat("spotLight.linear", 0.09f);
        objectShader.setFloat("spotLight.quadratic", 0.032f);
        objectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        objectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f); // 这里把漫反射强度调高一点
        objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

#pragma endregion

        // 渲染 箱子cube
        glBindVertexArray(VAO);
        for (int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            objectShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 设置灯光的shader
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        // 渲染灯光
        glBindVertexArray(lightVAO);
        for (int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(objectShader.ID);
    glDeleteProgram(lightShader.ID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lightVAO);

    glfwTerminate();

    return 0;
}

// 输入
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        //cameraPos += cameraSpeed * cameraFront;
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        //cameraPos -= cameraSpeed * cameraFront;
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // 讲道理 这里应该使用 glm::cross(cameraUp, -cameraFront) 求 右轴
        // 这里 直接这样结果也是对的但是 不好理解
        //cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        //cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// 窗口大小调整时的回调
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 光标移动时的回调
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的  不能理解？？？
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 监听鼠标滚轮变化
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// 加载纹理
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 这个只要放在glBindTexture之后设置就行
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        cout << "Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }

    return textureID;
}
