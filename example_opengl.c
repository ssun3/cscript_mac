#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "CScript OpenGL", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw a triangle
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-0.6f, -0.4f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.6f, -0.4f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f, 0.6f, 0.0f);
        glEnd();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
