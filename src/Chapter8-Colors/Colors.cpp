#include "Chapter8/Colors.h"
#include "camera.h"
#include "shader.h"
//#include "stb_image.h"
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

// 声明变量
static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
static float deltaTime = 0.0f;
static float lastFrameTime = 0.0f;
static float lastX = 400, lastY = 300; // 初始值为屏幕中心
static bool firstMouse = true;         // 处理 刚进入窗口时的闪动

static glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// 摄像机
static Camera camera(cameraPos);

int ColorsMain()
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
    // 这里我们其实只需要顶点数据 不需要纹理左边数据
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    // 为物体（被透光物）设置顶点相关
    unsigned int VAO, VBO, lightVAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定VAO 配置属性
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 绑定lightVAO 配置属性
    // 为灯光设置顶点相关，因为我们经常要修改物体，而不想这些修改影响到灯光 所以单独用个VAO进行设置
    glBindVertexArray(lightVAO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO); // 这里不绑定也行吧 因为上面的VBO并没有解绑
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 这里可以解绑 也可以不解绑
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

#pragma endregion

    // 着色器
    Shader objectShader("./src/Chapter8-Colors/shader.vs", "./src/Chapter8-Colors/shader.fs");
    Shader lightShader("./src/Chapter8-Colors/shader.vs", "./src/Chapter8-Colors/lightShader.fs");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deltaTime 用于计算摄像机移动速度
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;


        // ** 单纯对于这个场景来说 这里的很多变换矩阵的准备以及 设置可以放到while循坏外面 主要这里有个camera的变换 所以才有必要 放在while中 
        // 仅仅是对这个项目来说 因为不会动 **

        // 准备变换矩阵
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(60.0f), glm::vec3(1.0f, 0.3f, 0.5f));

        glm::mat4 view;
        view = camera.GetViewMatrix();

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // 绘制object ----------------------------------------------------
        //设置shader uniform
        objectShader.use();
        objectShader.setMat4("model", model);
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);

        objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        objectShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        // 不使用索引 直接绘制36个顶点
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制light ----------------------------------------------------
        // 变换一下light的位置
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // 缩小

        // 设置shader uniform
        lightShader.use();
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        // 不使用索引 直接绘制36个顶点
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(objectShader.ID);
    glDeleteProgram(lightShader.ID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

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