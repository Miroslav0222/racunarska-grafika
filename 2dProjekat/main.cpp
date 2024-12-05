//Miroslav Blagojevic SV43/2021

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <sstream>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Classes/Taster.h"
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include<string>
#include <ft2build.h>
#include "Classes/Shader.h"
#include FT_FREETYPE_H
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <utility>

static unsigned loadImageToTexture(const char* filePath); //Ucitavanje teksture, izdvojeno u funkciju
auto lastClickTime = std::chrono::steady_clock::now();
const int debounceDelay = 100;
bool canClick() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime);
    return duration.count() > debounceDelay; // Check if the debounce time has passed
}
bool isAFK() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime);
    return duration.count() > 5000; // Check if the debounce time has passed
}
std::pair<float,float> normalizeCoord(GLFWwindow* window,float xpos,float ypos) {
    int width, height;
    float x_normalized, y_normalized;
    glfwGetWindowSize(window, &width, &height);
    ypos = height - ypos;
    std::cout << "HEIGH: " << height << "   WIDTH: " << width;
    x_normalized = 2.0 * (xpos / width) - 1.0;
    y_normalized = 2.0 * (ypos / height) - 1.0;
    return std::make_pair(x_normalized, y_normalized);
}
std::pair<float, float> denormalizeCoord(GLFWwindow* window, float x_normalized, float y_normalized) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // Reverse the normalization formula
    float xpos = (x_normalized + 1.0f) / 2.0f * width;
    float ypos = height - ((y_normalized + 1.0f) / 2.0f * height);
    ypos = height - ypos;

    return std::make_pair(xpos, ypos);
}
std::string getCurrentTime() {
    // Get the current time
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    // Format the time as HH:MM:SS
    std::ostringstream timeStream;
    timeStream << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":"
        << std::setw(2) << std::setfill('0') << localTime->tm_min << ":"
        << std::setw(2) << std::setfill('0') << localTime->tm_sec;

    return timeStream.str();
}
void setDisplay(unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    float vertices[] = {
        //X    Y        R   G   B   A
        0.25, 0.65,       0.0, 0.0, 0.5, 1.0,
        0.25, -0.1,      0.0, 0.0, 0.5, 1.0,
        -0.25, -0.1,     0.0, 0.0, 0.5, 1.0,
        -0.25, 0.65,      0.0, 0.0, 0.5, 1.0
    };
    unsigned int indices[] = {
    0,1,2,
    2,3,0
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void setBrightnessIndicator(unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    float brightnessVertices[] = {
    -0.15, 0.0,     1.0, 1.0, 1.0, 1.0,
    -0.15, -0.05,     1.0, 1.0, 1.0, 1.0,
    -0.2, -0.05,     1.0, 1.0, 1.0, 1.0,
    -0.2, 0.0,     1.0, 1.0, 1.0, 1.0,

    -0.05, 0.05,     1.0, 1.0, 1.0, 1.0,
    -0.05, -0.05,     1.0, 1.0, 1.0, 1.0,
    -0.1, -0.05,     1.0, 1.0, 1.0, 1.0,
    -0.1, 0.05,     1.0, 1.0, 1.0, 1.0,

     0.05, 0.1,     1.0, 1.0, 1.0, 1.0,
     0.05, -0.05,     1.0, 1.0, 1.0, 1.0,
     0.0, -0.05,     1.0, 1.0, 1.0, 1.0,
     0.0, 0.1,     1.0, 1.0, 1.0, 1.0,

     0.15, 0.15,     1.0, 1.0, 1.0, 1.0,
     0.15, -0.05,     1.0, 1.0, 1.0, 1.0,
     0.1, -0.05,     1.0, 1.0, 1.0, 1.0,
     0.1, 0.15,     1.0, 1.0, 1.0, 1.0,
    };
    unsigned int brightnessIndices[] = {
    0,1,2,
    2,3,0,

    4,5,6,
    6,7,4,

    8,9,10,
    10,11,8,

    12,13,14,
    14,15,12
    };


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(brightnessVertices), brightnessVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(brightnessIndices), brightnessIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void setFridge(unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    float vertices[] =
    {   //X    Y        S    T 
        1.0, 1.0,       1.0, 1.0,//prvo tjeme
        1.0, -1.0,       1.0, 0.0, //drugo tjeme
        -1.0, -1.0,     0.0, 0.0, //trece tjeme
        -1.0, 1.0,      0.0, 1.0 //trece tjeme
    };

    unsigned int indices[] = {
       0,1,2,
       2,3,0
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void setStaticGUI(unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    float vertices[] =
    {   //X    Y        R   G   B   A
        0.25, 0.65,       0.0, 0.0, 0.5, 1.0,
        0.25, -0.1,      0.0, 0.0, 0.5, 1.0,
        -0.25, -0.1,     0.0, 0.0, 0.5, 1.0,
        -0.25, 0.65,      0.0, 0.0, 0.5, 1.0,
        //Temepature tasters 4-9
        -0.03, 0.36,      1.0, 1.0, 1.0, 1.0,
        -0.03, 0.31,      1.0, 1.0, 1.0, 1.0,
        -0.1, 0.335,      1.0, 1.0, 1.0, 1.0,
        0.03, 0.36,      1.0, 1.0, 1.0, 1.0,
        0.03, 0.31,      1.0, 1.0, 1.0, 1.0,
        0.1, 0.335,      1.0, 1.0, 1.0, 1.0,
        //Temperature placeholder 10-13
        0.075, 0.435,      1.0, 1.0, 1.0, 0.0,
        0.075, 0.385,      1.0, 1.0, 1.0, 0.0,
        -0.075, 0.385,      1.0, 1.0, 1.0, 0.0,
        -0.075, 0.435,      1.0, 1.0, 1.0, 0.0,
        //Freezer temperature tasters 14-19
        -0.03, 0.21,      1.0, 1.0, 1.0, 1.0,
        -0.03, 0.16,      1.0, 1.0, 1.0, 1.0,
        -0.1, 0.185,      1.0, 1.0, 1.0, 1.0,
        0.03, 0.21,      1.0, 1.0, 1.0, 1.0,
        0.03, 0.16,      1.0, 1.0, 1.0, 1.0,
        0.1, 0.185,      1.0, 1.0, 1.0, 1.0,
        //Freezer temperature placeholder 20-23
        0.075, 0.285,      1.0, 1.0, 1.0, 0.0,
        0.075, 0.235,      1.0, 1.0, 1.0, 0.0,
        -0.075, 0.235,      1.0, 1.0, 1.0, 0.0,
        -0.075, 0.285,      1.0, 1.0, 1.0, 0.0,
        //Change mode taster2 24-27
        0.2, 0.575,      1.0, 1.0, 1.0, 0.0,
        0.2, 0.525,      1.0, 1.0, 1.0, 0.0,
        0.15, 0.525,      1.0, 1.0, 1.0, 0.0,
        0.15, 0.575,      1.0, 1.0, 1.0, 0.0,
        //Timebox 28-31
        0.1, 0.6,      1.0, 1.0, 1.0, 0.0,
        0.1, 0.5,      1.0, 1.0, 1.0, 0.0,
        -0.2, 0.5,      1.0, 1.0, 1.0, 0.0,
        -0.2, 0.6,      1.0, 1.0, 1.0, 0.0,
        //Brigthness tasters 32-37
        0.20, 0.12,      1.0, 1.0, 1.0, 1.0,
        0.23, 0.07,      1.0, 1.0, 1.0, 1.0,
        0.17, 0.07,      1.0, 1.0, 1.0, 1.0,
        0.23, 0.03,      1.0, 1.0, 1.0, 1.0,
        0.20, -0.02,      1.0, 1.0, 1.0, 1.0,
        0.17, 0.03,      1.0, 1.0, 1.0, 1.0


    };

    unsigned int indices[] = {
       0,1,2,
       2,3,0,
       4,5,6,
       7,8,9,
       10,11,12,
       12,13,10,
       14,15,16,
       17,18,19,
       20,21,22,
       22,23,20,
       24,25,26,
       26,27,24,
       28,29,30,
       30,31,28,
       32,33,34,
       35,36,37
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}
void setLok(unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    float vertices[] =
    {   //X    Y        S    T 
    0.5,  0.5,   1.0, 1.0,  // Top-right
    0.5, -0.5,    1.0, 0.0,  // Bottom-right
    -0.5, -0.5,    0.0, 0.0,  // Bottom-left
    -0.5,  0.5,   0.0, 1.0,  // Top-left

    -1.5,  0.5,   1.0, 1.0,  // Top-right
    -1.5, -0.5,    1.0, 0.0,  // Bottom-right
    -2.5, -0.5,    0.0, 0.0,  // Bottom-left
    -2.5,  0.5,   0.0, 1.0  // Top-left
    };

    unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0,
    4,5,6,
    6,7,4
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void setGroceries(unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    float vertices[] =
    {   //X    Y        S    T 
        0.25, 0.65,       1.0, 1.0,//prvo tjeme
        0.25, -0.1,       1.0, 0.0, //drugo tjeme
        -0.25, -0.1,     0.0, 0.0, //trece tjeme
        -0.25, 0.65,      0.0, 1.0, //trece tjeme

        0.25, -0.3,       1.0, 1.0,//prvo tjeme
        0.25, -0.7,       1.0, 0.0, //drugo tjeme
        -0.25, -0.7,     0.0, 0.0, //trece tjeme
        -0.25, -0.3,      0.0, 1.0 //trece tjeme
    };

    unsigned int indices[] = {
       0,1,2,
       2,3,0,
       4,5,6,
       6,7,4
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void setStaticGUIIcons(unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    float vertices[] = {
        0.2, 0.575,     1.0, 1.0,
        0.2, 0.525,     1.0, 0.0,
        0.15, 0.525,    0.0, 0.0,
        0.15, 0.575,    0.0, 1.0,

        0.10, 0.285,    1.0, 1.0,
        0.10, 0.235,    1.0, 0.0, 
        0.05, 0.235,    0.0, 0.0,
        0.05, 0.285,    0.0, 1.0,

        0.10, 0.435,    1.0, 1.0,
        0.10, 0.385,    1.0, 0.0,
        0.05, 0.385,    0.0, 0.0,
        0.05, 0.435,    0.0, 1.0
    };
    unsigned int indices[] = {
     0,1,2,
     2,3,0,
     4,5,6,
     6,7,4,
     8,9,10,
     10,11,8
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void setTint(unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    float vertices[] = {
    0.25, 0.65,       0.0, 0.0, 1.0, 1.0,
    0.25, -0.1,      0.0, 0.0, 1.0, 1.0,
    -0.25, -0.1,     0.0, 0.0, 1.0, 1.0,
    -0.25, 0.65,      0.0, 0.0, 1.0, 1.0,

    0.25, -0.3,       0.0, 0.0, 1.0, 1.0,
    0.25, -0.7,      0.0, 0.0, 1.0, 1.0,
    -0.25, -0.7,     0.0, 0.0, 1.0, 1.0,
    -0.25, 0.-0.3,      0.0, 0.0, 1.0, 1.0,
    };
    unsigned int indices[] = {
        0,1,2,
        2,3,0,
        4,5,6,
        6,7,4
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void setClock(int VAO, int VBO, int EBO) {
    float vertices[] =
    {
        0.1, 0.6,      1.0, 1.0,
        0.1, 0.5,      1.0, 0.0,
        -0.2, 0.5,      0.0, 0.0,
        -0.2, 0.6,      0.0, 1.0,
    };
    unsigned int indices[] = {
      0,1,2,
      2,3,0
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

void renderText(int VAO, int VBO, std::string text, int x, int y,float scale ,float R,float G,float B, std::map<char, Character>& Characters, Shader& textShader) {
    textShader.activate();
    textShader.setVec3("textColor", R, G, B);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

int main(void)
{
    Taster lowerFridgeTemperatureTaster(-0.1, 0.31, -0.03, 0.36);
    Taster higherFridgeTemperatureTaster(0.03, 0.31, 0.1, 0.36);
    Taster lowerFreezerTemperatureTaster(-0.1,0.16,-0.03,0.21);
    Taster higherFreezerTemperatureTaster(0.03, 0.16, 0.1, 0.21);
    Taster changeModeTaster(0.15, 0.525, 0.2, 0.575);
    Taster lowerBrightnessTaster(0.17,-0.02,0.23,0.03);
    Taster higherBrightnessTaster(0.17,0.07,0.23,0.12);

    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (!primaryMonitor) {
        std::cerr << "Failed to get the primary monitor\n";
        glfwTerminate();
        return -1;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    if (!mode) {
        std::cerr << "Failed to get video mode\n";
        glfwTerminate();
        return -1;
    }

    const char wTitle[] = "[Generic Title]";
    unsigned int wWidth = mode->width;
    unsigned int wHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, wTitle, primaryMonitor, NULL);

    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }
    // CHARACTER TEXTURE GENERATION
    std::map<char, Character> Characters;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    unsigned int textures[128];
    glGenTextures(128, textures);
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture = textures[c];
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        //std::cout << "Texture ID for character " << c << ": " << texture << std::endl;
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        std::cout << "Glyph Width: " << face->glyph->bitmap.width
            << ", Height: " << face->glyph->bitmap.rows << std::endl;
        Characters.insert(std::pair<char, Character>(c, character));
    }

    int currentMode = 1;// 1- GUI , 0- Groceries 2- LOK

    //unsigned int unifiedShader = createShader("texture.vert", "texture.frag");
    Shader unifiedShader("Shaders/texture.vert", "Shaders/texture.frag");
    Shader basicShader("Shaders/basic.vert", "Shaders/basic.frag");
    Shader textShader("Shaders/text.vert", "Shaders/text.frag");
    Shader lokShader("Shaders/lok.vert", "Shaders/lok.frag");
    Shader iconsShader("Shaders/texture.vert", "Shaders/icons.frag");
    Shader tintShader("Shaders/basic.vert", "Shaders/tint.frag");

    unsigned int VAO[10];
    glGenVertexArrays(10, VAO);

    unsigned int VBO[10];
    glGenBuffers(10, VBO);

    unsigned int EBO[10];
    glGenBuffers(10, EBO);

    //Text
    glGenVertexArrays(1, &VAO[4]);
    glGenBuffers(1, &VBO[4]);
    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    setFridge(VAO[0], VBO[0], EBO[0]);
    setStaticGUI(VAO[1], VBO[1], EBO[1]);
    setGroceries(VAO[2], VBO[2], EBO[2]);
    setClock(VAO[3], VBO[3], EBO[3]);
    setBrightnessIndicator(VAO[5], VBO[5], EBO[5]);
    setLok(VAO[6], VBO[6], EBO[6]);
    setStaticGUIIcons(VAO[7], VBO[7], EBO[7]);
    setTint(VAO[8], VBO[8], EBO[8]);
    setDisplay(VAO[9], VBO[9], EBO[9]);

    unsigned powerButtonTexture = loadImageToTexture("res/powerbutton.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, powerButtonTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned snowflakeTexture = loadImageToTexture("res/snowflake.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, snowflakeTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned temperatureTexture = loadImageToTexture("res/temperatureIcon.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, temperatureTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned freezerTexture = loadImageToTexture("res/freezer.jpg"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, freezerTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);


    unsigned groceriesTexture = loadImageToTexture("res/groceries.jpg");
    glBindTexture(GL_TEXTURE_2D, groceriesTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned fontTexture = loadImageToTexture("res/square_6x6.png");
    glBindTexture(GL_TEXTURE_2D, fontTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned lokTexture = loadImageToTexture("res/lok1.png");
    glBindTexture(GL_TEXTURE_2D, lokTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //Tekstura
    unsigned checkerTexture = loadImageToTexture("res/fridge2.jpg"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, checkerTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    
    std::chrono::duration<double, std::milli> sleep_duration(1000.0 / 60.0);
    glm::mat4 projection = glm::ortho(0.0f, 1600.0f, 1200.0f, 0.0f);
    textShader.activate();
    textShader.setMat4("projection", projection);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    float fridgeTemperature = 3;
    float freezerTemperature = -20;
    int guiBrightness = 4;

    tintShader.activate();
    tintShader.setFloat("alphaStrength", 0.5);
    basicShader.activate();
    basicShader.setFloat("brightness", 1);
    textShader.activate();
    textShader.setFloat("brightness", 1);
    auto previousTime = std::chrono::steady_clock::now();
    float lokLoadPercentage = 0;
    bool lokLoadPercentageReset = false;
    int lokMode = 1;
    bool lokMovementXIncrease = true;
    bool lokMovementYIncrease = true;

    std::pair<float, float> lokViewportCoordsBottom = denormalizeCoord(window, -0.25, -0.1);
    std::pair<float, float> lokViewportCoordsTop = denormalizeCoord(window, 0.25, 0.65);
    float lokHeight = abs(lokViewportCoordsTop.second - lokViewportCoordsBottom.second);
    float lokWidth = abs(lokViewportCoordsTop.first - lokViewportCoordsBottom.first);

    float lokMovementX = 0.0;
    float lokMovementY = 0.0;
    float lokAlpha = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        auto loopBeginTime = std::chrono::steady_clock::now();

        glfwPollEvents();
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (isAFK()) {
            currentMode = 2;
           
            if (!lokLoadPercentageReset) {
                lokLoadPercentage = 0;
                lokLoadPercentageReset = true;
            }
            basicShader.activate();
            basicShader.setFloat("guiOn", 1);
            //basicShader.setFloat("brightness", 1-lokLoadPercentage);
            lokShader.activate();
            lokShader.setFloat("visibility", lokLoadPercentage);
            float time = glfwGetTime();
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) lokMode = 1;
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) lokMode = 2;
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) lokMode = 3;
            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) lokMode = 4;
            if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) lokMode = 5;
            if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) lokMode = 6;
            if (lokMode == 1) {
                lokShader.setFloat("xMovement", 0.0);
                lokShader.setFloat("yMovement", 0.0);
                lokMovementX = 0; lokMovementY = 0;
                lokShader.setFloat("pulseStrength", 0);
                lokShader.setFloat("alphaStrength", 0);
            }
            else if (lokMode == 2) {
                lokShader.setFloat("pulseStrength", abs(sin(time)));
            }
            else if (lokMode == 3) {
                lokMovementX += 0.01f;
                if (lokMovementX > 2.0) lokMovementX = 0.0;
                lokShader.setFloat("xMovement", lokMovementX);
                lokShader.setFloat("yMovement", 0.0);
            }
            else if (lokMode == 4) {
                lokMovementX = 0.5 * sin(time);
                lokMovementY = 0.5 * cos(time);
                lokShader.setFloat("xMovement", lokMovementX);
                lokShader.setFloat("yMovement", lokMovementY);
            }
            else if (lokMode == 5) {
                lokMovementX = 0.5 * sin(time);
                lokShader.setFloat("xMovement", lokMovementX);
                lokShader.setFloat("yMovement", 0);
                lokShader.setFloat("alphaStrength", abs(lokMovementX)*1.5);
            }
            else if (lokMode == 6) {
                std::cout << "lok x" << lokMovementX << std::endl;
                if (lokMovementXIncrease) lokMovementX += 0.01f;
                else lokMovementX -= 0.01f;

                if (lokMovementYIncrease) lokMovementY += 0.005f;
                else lokMovementY -= 0.005f;

                if (lokMovementX >= 0.5f) lokMovementXIncrease = false;
                else if (lokMovementX <= -0.5f) lokMovementXIncrease = true;

                if (lokMovementY >= 0.65f) lokMovementYIncrease = false;
                else if (lokMovementY <= -0.65f) lokMovementYIncrease = true;

                lokShader.setFloat("xMovement", lokMovementX);
                lokShader.setFloat("yMovement", lokMovementY);
            }
        }
        else {
            float lokMovementX = 0.0;
            float lokMovementY = 0.0;
            lokLoadPercentageReset = false;
            lokShader.activate();
            lokShader.setFloat("visibility", 0);
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && canClick()) { // Clicking on UI
            lastClickTime = std::chrono::steady_clock::now();
            double x_normalized, y_normalized;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            std::tie(x_normalized, y_normalized) = normalizeCoord(window,xpos,ypos);
            std::cout << std::endl;
            std::cout << "Mouse clicked at coordinates: (" << xpos << ", " << ypos << ")" << std::endl;
            std::cout << "Mouse clicked at normalized coordinates: ("<< x_normalized << ", " << y_normalized << ")" << std::endl; 

            if (currentMode == 0 || currentMode == 2) currentMode = 1;
            else if (changeModeTaster.contains(x_normalized, y_normalized) && currentMode == 1) currentMode = 0;
      
            else if (lowerFridgeTemperatureTaster.contains(x_normalized, y_normalized) && currentMode == 1) {
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)  fridgeTemperature -= 5;
                else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) fridgeTemperature -= 1;
                else fridgeTemperature -= 0.1;
           
            }
            else if (higherFridgeTemperatureTaster.contains(x_normalized, y_normalized) && currentMode == 1) {
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)  fridgeTemperature += 5;
                else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) fridgeTemperature += 1;
                else fridgeTemperature += 0.1;
            }
            else if (lowerFreezerTemperatureTaster.contains(x_normalized, y_normalized) && currentMode == 1) {
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)  freezerTemperature -= 5;
                else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) freezerTemperature -= 1;
                else freezerTemperature -= 0.1;           
            }
            else if (higherFreezerTemperatureTaster.contains(x_normalized, y_normalized) && currentMode == 1) {
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)  freezerTemperature += 5;
                else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) freezerTemperature += 1;
                else freezerTemperature += 0.1;
            }
            else if (lowerBrightnessTaster.contains(x_normalized, y_normalized) && currentMode == 1) {
                guiBrightness -= 1;
                std::cout << "-Brightness: " << guiBrightness;
            }
            else if (higherBrightnessTaster.contains(x_normalized, y_normalized) && currentMode == 1) {
                guiBrightness += 1;
                std::cout << "+Brightness: " << guiBrightness;
            }
            else {
                std::cout << "OUTSIDE";
            }
        }
        if (fridgeTemperature < 0) fridgeTemperature = 0;
        if (fridgeTemperature > 7) fridgeTemperature = 7;
        if (freezerTemperature < -40) freezerTemperature = -40;
        if (freezerTemperature > -18) freezerTemperature = -18;
        if (guiBrightness > 4) guiBrightness = 4;
        if (guiBrightness < 0) guiBrightness = 0;

        float realBrightness = 0.25 * guiBrightness;
        if (realBrightness <= 0) realBrightness = 0.1;
        basicShader.activate();
        if (currentMode == 1) {
            basicShader.setFloat("brightness", realBrightness);
        }
        else if (currentMode == 2) {
            basicShader.setFloat("brightness", 0);
        }
        textShader.activate();
        if (currentMode == 1) {
            textShader.setFloat("brightness", realBrightness);
        }
        else if (currentMode == 2) {
            textShader.setFloat("brightness", 0);
        }
        iconsShader.activate();
        if (currentMode == 1) {
            iconsShader.setFloat("brightness", realBrightness);
        }
        else if (currentMode == 2) {
            iconsShader.setFloat("brightness", 0);
        }


        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        //Drawing fridge
        unifiedShader.activate();
        glBindVertexArray(VAO[0]);
        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, checkerTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if (currentMode == 1) {
            basicShader.activate();
            glBindVertexArray(VAO[1]);
            glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_INT, 0);
            //Draw brightness indicator
            unsigned int elementsToDraw = guiBrightness * 6;
            basicShader.activate();
            glBindVertexArray(VAO[5]);
            glDrawElements(GL_TRIANGLES, elementsToDraw, GL_UNSIGNED_INT, 0);
            //Draw GUI icons
            iconsShader.activate();
            glBindVertexArray(VAO[7]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, powerButtonTexture);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, snowflakeTexture);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
            glBindTexture(GL_TEXTURE_2D, temperatureTexture);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));

            //Dawing clock x y scale r g b
            std::string currentTimeString = getCurrentTime();
            renderText(VAO[4], VBO[4], currentTimeString, 675.f, 290.f, 0.9f, 1.0f, 1.0f, 1.0f, Characters, textShader);
            //Drawing fridge temperature
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << fridgeTemperature;
            std::string fridgeTemp = oss.str() + "°C";
            renderText(VAO[4], VBO[4], fridgeTemp, 780.f, 365.f, 0.5f, 1.0f, 1.0f, 1.0f, Characters, textShader);
            //Draw freezer temperature
            oss.str("");
            oss << std::fixed << std::setprecision(1) << freezerTemperature;
            std::string freezerTemp = oss.str() + "°C";
            renderText(VAO[4], VBO[4], freezerTemp, 765.f, 455.f, 0.5f, 1.0f, 1.0f, 1.0f, Characters, textShader);
        }
        else if (currentMode == 0) {
            //Drawing groceries
            unifiedShader.activate();
            glBindVertexArray(VAO[2]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, groceriesTexture);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //DRAW TINT FRIDGE TINT
            tintShader.activate();
            tintShader.setFloat("alphaStrength", 1 - fridgeTemperature / 10 - 0.3);
            glBindVertexArray(VAO[8]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else {
            //Drawing display
            basicShader.activate();
            glBindVertexArray(VAO[9]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glViewport(lokViewportCoordsBottom.first, lokViewportCoordsBottom.second, lokWidth, lokHeight);
            //Drawing lok
            lokShader.activate();
            glBindVertexArray(VAO[6]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, lokTexture);
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
            glViewport(0, 0, wWidth, wHeight);
        }
        //Drawing freezer
        unifiedShader.activate();
        glBindVertexArray(VAO[2]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, freezerTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
        //DRAW FREEZER TINT
        tintShader.activate();
        glBindVertexArray(VAO[8]);
        tintShader.setFloat("alphaStrength", (0.22 - (freezerTemperature + 40) / 100) * 2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
      
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        textShader.activate();
        textShader.setFloat("brightness", 1.0f);
        //renderText(VAO[4], VBO[4], "Miroslav Blagojevic SV43/2021", 1130.f, 1150.f, 0.7f, 0.0f, 1.0f, 0.0f, Characters, textShader);
        renderText(VAO[4], VBO[4], "Miroslav Blagojevic SV43/2021", 0.0f, 30.0f, 0.7f, 0.0f, 1.0f, 0.0f, Characters, textShader);
        glfwSwapBuffers(window);
        glfwPollEvents();

        std::chrono::duration<float, std::milli> loopTime = std::chrono::steady_clock::now() - loopBeginTime;
        float loopTimeSeconds = loopTime.count() / 1000.0f;
        float totalLoopTimeSeconds = loopTimeSeconds + sleep_duration.count()/ 1000.0f;
        lokLoadPercentage += 1.0f/300;
        if (lokLoadPercentage > 1) lokLoadPercentage = 1;


        std::this_thread::sleep_for(sleep_duration-loopTime);
    }


    glDeleteTextures(1, &checkerTexture);
    glDeleteBuffers(1, &VBO[0]);
    glDeleteVertexArrays(1, &VAO[0]);

    glfwTerminate();
    return 0;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}