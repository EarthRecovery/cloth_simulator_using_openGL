#include <iostream>
using namespace std;
#define GLEW_STATIC

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Camera.h"
#include "Shader.h"
#include "stb_image.h"
#include "cloth.cpp"

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void updateTriangleVertices(Cloth& c, unsigned int VBO);

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

const GLint SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;

// camera setting--------------
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// control camera speed--------
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// mouse setting---------------
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX = SCREEN_WIDTH / 2.0;
float lastY = SCREEN_HEIGHT / 2.0;

std::vector<float> vertices;

float px = 0.0f;
float py = 0.0f;
float pz = 0.0f;

bool Line = true;
int main()
{
    // open window--------------
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Learn OpenGL Triangle test", nullptr, nullptr);

    int screenWidth_1, screenHeight_1;
    glfwGetFramebufferSize(window, &screenWidth_1, &screenHeight_1);
    glfwMakeContextCurrent(window);
    //--------------------------

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //load glad
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // mouse setting-------------
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隐藏鼠标
    //--------------------------

    //Imgui setting
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); //创建上下文
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 允许键盘控制
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // 允许游戏手柄控制

    // 设置渲染器后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // set depth test------------
    glEnable(GL_DEPTH_TEST);
    //--------------------------

    //set original vertices
    Cloth c;
    std::vector<glm::vec3> pointList;
    for (int i = 0;i < HEIGHT-1;i++) {
        for (int j = 0;j < LENGTH-1;j++) {
            pointList.push_back(c.CurrentPointArray[i][j].position);
            pointList.push_back(c.CurrentPointArray[i][j+1].position);
            pointList.push_back(c.CurrentPointArray[i+1][j+1].position);
            pointList.push_back(c.CurrentPointArray[i][j].position);
            pointList.push_back(c.CurrentPointArray[i+1][j].position);
            pointList.push_back(c.CurrentPointArray[i + 1][j + 1].position);
        }
    }

    for (auto& point : pointList) {
        vertices.push_back(point.x);
        vertices.push_back(point.y);
        vertices.push_back(point.z);
    }

    // set shader----------------
    Shader lightingShader("colors.vert", "colors.frag");
    //--------------------------

    unsigned int VAO, VBO;
    // 创建并绑定顶点数组对象（VAO）
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建并绑定顶点缓冲对象（VBO）
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int a = sizeof(vertices) * vertices.size();

    // 初次将顶点数据存储到VBO中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    // 配置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error << std::endl;
    }

    // 解绑VBO和VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 在渲染之前启用线框模式
    
    if (Line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glViewport(0, 0, screenWidth_1, screenHeight_1);
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // 创建用户界面
        ImGui::Begin("Control Panel");
        ImGui::SliderFloat("X", &px, -1.0f, 1.0f);
        ImGui::SliderFloat("Y", &py, -1.0f, 1.0f);
        ImGui::SliderFloat("Z", &pz, -1.0f, 1.0f);
        if (ImGui::Button("Update Vertices"))
        {
            c.changePointPosition(px, py, pz);
        }
        ImGui::End();
        
        deltaTime = 0.01f;

        processInput(window);

        lightingShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << error << std::endl;
        }
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (glfwGetTime() > 0.002) {
            updateTriangleVertices(c, VBO);
            glfwSetTime(0.0);
        }

        glfwSwapBuffers(window);
    }
    if(Line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset); // 传入偏移量
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void updateTriangleVertices(Cloth& c, unsigned int VBO) {
    vertices.clear();
    c.run();
    std::vector<glm::vec3> pointList;
    for (int i = 0;i < HEIGHT - 1;i++) {
        for (int j = 0;j < LENGTH - 1;j++) {
            pointList.push_back(c.CurrentPointArray[i][j].position);
            pointList.push_back(c.CurrentPointArray[i][j + 1].position);
            pointList.push_back(c.CurrentPointArray[i + 1][j + 1].position);
            pointList.push_back(c.CurrentPointArray[i][j].position);
            pointList.push_back(c.CurrentPointArray[i + 1][j].position);
            pointList.push_back(c.CurrentPointArray[i + 1][j + 1].position);
        }
    }

    for (auto& point : pointList) {
        vertices.push_back(point.x);
        vertices.push_back(point.y);
        vertices.push_back(point.z);
    }

    // 将新的顶点数据存储到VBO中
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    // 解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}