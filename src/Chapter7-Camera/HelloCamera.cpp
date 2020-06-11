#include "Chapter7/HelloCamera.h"
#include "camera.h"
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

        为什么Camera的构造方法一定要传个参数？
            注意下面的调用的区别
            Camera camera; // 在栈里创建对象，操作系统进行内存的分配和管理。调用用 .
            Camera camera(cameraPos);  // 在栈里创建对象，操作系统进行内存的分配和管理
            Camera *camera = new Camera(); // 堆中分配，由管理者进行内存的分配和管理，用完必须delete()，否则可能造成内存泄漏。调用要用->
            Camera camera();   // 声明返回值为Camera类型的无参函数，函数名叫camera

        Camera的构造函数的冒号是什么意思？
            这里应该不一定非要用冒号，构造函数加冒号的用法参考下面的链接
            参考：https://blog.csdn.net/kaixinbingju/article/details/9094289

        YAW的默认值为什么是-90°方向是怎么计算的？
            首先我们用于计算的公式是，cameraFront的公式为：
                front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
                front.y = sin(glm::radians(Pitch));
                front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            如果忽略Pitch的影响，我们不难推测Yaw为x轴正方向顺时针转动方向的夹角，
            因为我们不仅要保证front分量的值符合三角函数，还要保证分量的方向也符合三角函数。
            （当然我们可以用更复杂的计算方式（手动处理正负），然后用不指向x正方向的Yaw，可为什么要给自己找麻烦呢？(●'◡'●)）
            所以，Yaw为0的时候其实是与x轴正方向重合。
            又因为我们的Cube相对摄像机在z轴的负方向，所以我们的Yaw要设置为-90°，以正对cube。

        再看一下camera.h使用后的变化，主要理解view、projection矩阵怎么就这样算了？
*/

// 设置
static unsigned int SCR_WIDTH = 800;
static unsigned int SCR_HEIGHT = 600;

// 函数声明
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window);
// 监听光标位置变化
static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// 监听鼠标滚轮变化
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

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

// 使用摄像机类
static Camera camera(cameraPos); // 在栈里创建对象

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
        //view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        view = camera.GetViewMatrix();

        // 投影矩阵 -> 透视投影
        glm::mat4 projection; // = glm::mat4(1.0f);   // 这里可初始化 可不初始化
        // -> 45 -> fov 支持视角缩放
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

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

    // float sensitivity = 0.05f;
    // xoffset *= sensitivity;
    // yoffset *= sensitivity;

    // // 这里为什么是 += yaw、pitch是什么一直存在的值 还是 一次鼠标移动产生的值 ？？？
    // yaw += xoffset;
    // pitch += yoffset;

    // if (pitch > 89.0f)
    //     pitch = 89.0f;
    // if (pitch < -89.0f)
    //     pitch = -89.0f;

    // glm::vec3 front;
    // front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    // front.y = sin(glm::radians(pitch));
    // front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    // cameraFront = glm::normalize(front);

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 监听鼠标滚轮变化
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // if (fov >= 1.0f && fov <= 45.0f)
    //     fov -= yoffset; // 滚轮x 是哪个方向？
    // if (fov <= 1.0f)
    //     fov = 1.0f;
    // if (fov >= 45.0f)
    //     fov = 45.0f;
    camera.ProcessMouseScroll(yoffset);
}