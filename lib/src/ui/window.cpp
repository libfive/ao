#include <iostream>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "ao/ui/window.hpp"
#include "ao/core/tree.hpp"

#include "ao/gl/core.hpp"
#include "ao/gl/frame.hpp"

////////////////////////////////////////////////////////////////////////////////

bool Window::Show(Tree* tree)
{
    (void)tree;

    auto glfw_window = makeWindow(640, 480, "Hello!");
    if (glfw_window == nullptr)
    {
        return false;
    }
    std::unique_ptr<Window> win(new Window(glfw_window));
    win->run();

    return true;
}

////////////////////////////////////////////////////////////////////////////////

// Callback wrappers
static void _resized(GLFWwindow* window, int w, int h)
{
    static_cast<Window*>(glfwGetWindowUserPointer(window))->resized(w, h);
}

static void _mouseMove(GLFWwindow* window, double x, double y)
{
    static_cast<Window*>(glfwGetWindowUserPointer(window))->mouseMove(x, y);
}

static void _mouseButton(GLFWwindow* window, int b, int a, int m)
{
    static_cast<Window*>(glfwGetWindowUserPointer(window))->mouseButton(b, a, m);
}

////////////////////////////////////////////////////////////////////////////////

Window::Window(GLFWwindow* window)
    : window(window)
{
    glfwGetFramebufferSize(window, &width, &height);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, _resized);
    glfwSetCursorPosCallback(window, _mouseMove);
    glfwSetMouseButtonCallback(window, _mouseButton);
}

void Window::resized(int w, int h)
{
    width = w;
    height = h;
    draw();
}

void Window::mouseMove(double x, double y)
{
    int w;
    int h;
    glfwGetWindowSize(window, &w, &h);

    // Scale coordinates to the range -1, 1
    auto new_pos = glm::vec2((2*x)/w - 1, 1 - (2*y)/h);

    // If we're panning, adjust the center position
    if (drag_mode == WINDOW_DRAG_PAN)
    {
        // Find the starting position in world coordinates
        auto inv = glm::inverse(proj() * view());
        auto diff = inv * glm::vec4(new_pos, 0.0, 1.0) -
                    inv * glm::vec4(mouse_pos, 0.0, 1.0);

        center += glm::vec3(diff.x, diff.y, diff.z);
    }

    mouse_pos = new_pos;

    if (drag_mode != WINDOW_DRAG_NONE)
    {
        draw();
    }
}

void Window::mouseButton(int button, int action, int mods)
{
    (void)mods;

    if (action == GLFW_PRESS)
    {
        if (button == GLFW_MOUSE_BUTTON_1)
        {
            drag_mode = WINDOW_DRAG_PAN;
        }
        else if (button == GLFW_MOUSE_BUTTON_2)
        {
            drag_mode = WINDOW_DRAG_ROTATE;
        }
    }
    else
    {
        drag_mode = WINDOW_DRAG_NONE;
    }
}

////////////////////////////////////////////////////////////////////////////////

glm::mat4 Window::proj() const
{
    if (width > height)
    {
        return glm::scale(glm::vec3(height/float(width), 1.0, 1.0));
    }
    else
    {
        return glm::scale(glm::vec3(1.0, width/float(height), 1.0));
    }
}

glm::mat4 Window::view() const
{
    glm::mat4 m;
    m = glm::scale(m, {scale, scale, scale});
    m = glm::translate(m, center);

    return m;
}

////////////////////////////////////////////////////////////////////////////////

void Window::draw() const
{
    auto m = proj() * view();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    axes.draw(m);

    glfwSwapBuffers(window);
}

void Window::run()
{
    while (!glfwWindowShouldClose(window))
    {
        // Poll for and process events
        glfwPollEvents();

        // Redraw the window
        draw();
    }
}
