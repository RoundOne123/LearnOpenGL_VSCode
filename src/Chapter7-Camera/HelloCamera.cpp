#include "Chapter7/HelloCamera.h"
#include "shader.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

/*
    本节疑问：
        顶点变换变换着怎么就抽象出摄像机的概念了？
        为什么计算摄像机y轴位移时要反过来相减？
        为什么我的工程进去视角不对，很歪？
        鼠标滚轮xoffset是啥？yoffset是怎么变化的？怎么影响的fov？
        怎么实现四元数摄像机？
        怎么实现本节中用到的摄像机类？
*/

// 设置
static unsigned int SCR_WIDTH = 800;
static unsigned int SCR_HEIGHT = 600;

// 函数声明
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window);
// 监听光标位置变化
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// 监听鼠标滚轮变化
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// 摄像机相关
static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 这里的意思就是 一直看向z轴的负方向
static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

static float deltaTime = 0.0f;
static float lastFrameTime = 0.0f;

// 计算 摄像机视角移动相关
static float lastX = 400, lastY = 300; // 初始值为屏幕中心
static float yaw, pitch;               // 摄像机视角
static bool firstMouse = true;         // 处理 刚进入窗口时的闪动
static float fov = 45;                 // 摄像机视角 filed of view

int HelloCameraMain()
{

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 处理着色器
    Shader ourShader("./src/Chapter7-Camera/shader.vs", "./src/Chapter7-Camera/shader.fs");

#pragma region // 处理顶点数据相关
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

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

#pragma endregion

#pragma region // 纹理相关

    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);

    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载纹理数据
    int width, height, nrChannels;
    // 反转纹理y轴
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("./textures/Chapter6-Textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture1" << endl;
    }

    stbi_image_free(data);

    // 第二张纹理
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("./textures/Chapter6-Textures/awesomeface.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture2" << endl;
    }

    stbi_image_free(data);

    // 设置Uniform 前先激活shaderProgram
    ourShader.use();
    // 定义那个uniform对应哪个纹理单元
    ourShader.setInt("textrue1", 0);
    ourShader.setInt("texture2", 1);

#pragma endregion

    // 模型位置
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

    // 线框模式
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 隐藏并捕捉光标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 监听光标位置变化
    glfwSetCursorPosCallback(window, mouse_callback);
    // 监听鼠标滚轮变化
    glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window))
    {

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // 开始绘制
        glBindVertexArray(VAO);

        // // 观察矩阵  -> 可以相当于摄像机的位置位于z为-3的位置
        // // 这里明显是对顶点的运算呀，为什么是-3.0呢
        // // 因为OpenGL是右手坐标系 这里就是把cube往屏幕里面位移！
        // glm::mat4 view = glm::mat4(1.0f); // 这里必须初始化
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // // =>> 通过LookAt创建 观察矩阵 view
        // float radius = 10.0f;
        // float camX = sin(glfwGetTime()) * radius;
        // float camZ = cos(glfwGetTime()) * radius;
        // glm::mat4 view = glm::mat4(1.0f);
        // view = glm::lookAt(glm::vec3(camX, 0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        // 用于计算移动速度
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;

        // =>> 自由移动
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // 投影矩阵 -> 透视投影
        glm::mat4 projection; // = glm::mat4(1.0f);   // 这里可初始化 可不初始化
        // -> 45 -> fov 支持视角缩放
        projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // 设置Uniform
        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        // ↓↓↓ 更多的cube
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            // 绘制10遍 10个cube
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(ourShader.ID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// 窗口大小调整时的回调
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 输入
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // 讲道理 这里应该使用 glm::cross(cameraUp, -cameraFront) 求 右轴
        // 这里 直接这样结果也是对的但是 不好理解
        cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
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

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // 这里为什么是 += yaw、pitch是什么一直存在的值 还是 一次鼠标移动产生的值 ？？？
    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// 监听鼠标滚轮变化
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset; // 滚轮x 是哪个方向？
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}