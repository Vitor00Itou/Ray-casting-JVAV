#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <chrono>
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>

#include "ray_casting_renderer.hpp"
#include "ray_object_renderer.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "box.hpp"
#include "../lib/json.hpp"

// Viewport Dimensions
constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

// Camera state parameters
static float xcam = 0.0f;
static float ycam = 0.0f;
static float zcam = 0.0f;
static float fov = 45.0f;

// Performance timing state
std::chrono::high_resolution_clock::time_point start, end;
static bool IsRayCastingON = false;
static bool monitoringTime = false;

// First-person Camera control state
static float yaw = -90.0f;   // Horizontal angle initialized facing negative Z
static float pitch = 0.0f;    // Vertical angle
int windowWidth = WIDTH;
int windowHeight = HEIGHT;
int centerX = windowWidth / 2;
int centerY = windowHeight / 2;
bool justWarped = false;
bool cameraMode = true;

int maxRecursionDepth = 10;

RayCastingRenderer rayCastingRenderer(WIDTH, HEIGHT);
RayObjectRenderer rayObjectRenderer(WIDTH, HEIGHT);
Scene scene;
Camera camera;

/**
 * @brief Parse Sphere primitive from JSON object descriptor.
 */
Sphere* parseSphere(const nlohmann::json& obj) {
    Vec3 center = Vec3(obj["center"][0], obj["center"][1], obj["center"][2]);
    float radius = obj["radius"];
    Color color = Color(1.0f, 1.0f, 1.0f);
    bool isEmitter = obj.value("emitter", false);
    bool isInert = obj.value("inert", false);
    std::string texture = "";
    float specularShininess = 32.0f;
    float transparency = 0.0f;
    float refractiveIndex = 1.0f;
    float reflectionCoefficient = 0.0f;

    if (obj.contains("color")) {
        color = Color(obj["color"][0], obj["color"][1], obj["color"][2]);
    }
    if (obj.contains("texture")) {
        texture = obj["texture"];
    }
    if (obj.contains("specular")) {
        specularShininess = obj["specular"];
    }
    if (obj.contains("reflection")) {
        reflectionCoefficient = obj["reflection"];
    }
    if (obj.contains("transparency")) {
        transparency = obj["transparency"];
    }
    if (obj.contains("refractiveIndex")) {
        refractiveIndex = obj["refractiveIndex"];
    }

    return new Sphere(center, radius, color, texture.c_str(), isEmitter, specularShininess, reflectionCoefficient, transparency, refractiveIndex, isInert);
}

/**
 * @brief Parse Plane primitive from JSON object descriptor.
 */
Plane* parsePlane(const nlohmann::json& obj) {
    Vec3 point = Vec3(obj["point"][0], obj["point"][1], obj["point"][2]);
    Vec3 normal = Vec3(obj["normal"][0], obj["normal"][1], obj["normal"][2]);
    bool isInert = obj.value("inert", false);
    Color color = Color(1.0f, 1.0f, 1.0f);
    std::string texture = "";
    float specularShininess = 32.0f;
    float transparency = 0.0f;
    float refractiveIndex = 1.0f;
    float reflectionCoefficient = 0.0f;

    if (obj.contains("color")) {
        color = Color(obj["color"][0], obj["color"][1], obj["color"][2]);
    }
    if (obj.contains("texture")) {
        texture = obj["texture"];
    }
    if (obj.contains("specular")) {
        specularShininess = obj["specular"];
    }
    if (obj.contains("transparency")) {
        transparency = obj["transparency"];
    }
    if (obj.contains("refractiveIndex")) {
        refractiveIndex = obj["refractiveIndex"];
    }
    if (obj.contains("reflection")) {
        reflectionCoefficient = obj["reflection"];
    }

    return new Plane(point, normal, color, texture.c_str(), specularShininess, reflectionCoefficient, transparency, refractiveIndex, isInert);
}

/**
 * @brief Parse Box primitive from JSON object descriptor.
 */
Box* parseBox(const nlohmann::json& obj) {
    Vec3 minCorner = Vec3(obj["minCorner"][0], obj["minCorner"][1], obj["minCorner"][2]);
    Vec3 maxCorner = Vec3(obj["maxCorner"][0], obj["maxCorner"][1], obj["maxCorner"][2]);

    Color color = Color(1.0f, 1.0f, 1.0f);
    bool isEmitter = obj.value("emitter", false);
    bool isInert = obj.value("inert", false);
    std::string texture = "";
    float specularShininess = 32.0f;
    float transparency = 0.0f;
    float refractiveIndex = 1.0f;
    float reflectionCoefficient = 0.0f;

    if (obj.contains("color")) {
        color = Color(obj["color"][0], obj["color"][1], obj["color"][2]);
    }
    if (obj.contains("texture")) {
        texture = obj["texture"];
    }
    if (obj.contains("specular")) {
        specularShininess = obj["specular"];
    }
    if (obj.contains("transparency")) {
        transparency = obj["transparency"];
    }
    if (obj.contains("refractiveIndex")) {
        refractiveIndex = obj["refractiveIndex"];
    }
    if (obj.contains("reflection")) {
        reflectionCoefficient = obj["reflection"];
    }

    return new Box(minCorner, maxCorner, color, texture.c_str(), isEmitter, specularShininess, reflectionCoefficient, transparency, refractiveIndex, isInert);
}

/**
 * @brief Parse Point Light from JSON object descriptor.
 */
LightPoint* parseLightPoint(const nlohmann::json& obj) {
    Vec3 position = Vec3(obj["position"][0], obj["position"][1], obj["position"][2]);
    Color color = Color(1.0f, 1.0f, 1.0f);

    if (obj.contains("color")) {
        color = Color(obj["color"][0], obj["color"][1], obj["color"][2]);
    }

    return new LightPoint(position, color);
}

/**
 * @brief Load custom scene file from specified JSON filepath.
 */
void setupScene(const std::string& filename) {
    camera = Camera(
        Vec3(xcam, ycam, zcam),
        Vec3(0, 0, -1),
        Vec3(0, 1, 0),
        fov
    );

    std::ifstream input(filename);
    if (!input.is_open()) {
        std::cerr << "Error opening scene file: " << filename << std::endl;
        return;
    }

    nlohmann::json sceneJson;
    input >> sceneJson;

    for (const auto& obj : sceneJson) {
        std::string type = obj["type"];

        if (type == "sphere") {
            scene.objects.push_back(parseSphere(obj));
        } else if (type == "plane") {
            scene.objects.push_back(parsePlane(obj));
        } else if (type == "box") {
            scene.objects.push_back(parseBox(obj));
        } else if (type == "light_point") {
            scene.objects.push_back(parseLightPoint(obj));
        }
    }
}

/**
 * @brief Setup 3D text scene demo layout.
 */
void setupOutros3D() {
    camera = Camera(
        Vec3(xcam, ycam, zcam),
        Vec3(0, 0, -1),
        Vec3(0, 1, 0),
        fov
    );

    float spacing = 1.2f;
    float boxSizeX = 0.9f;
    float boxSizeY = 0.9f;
    float depth = -11.0f;

    float startX = -20.0f;
    float startY = 5.0f;

    auto addBox = [&](float x, float y) {
        Vec3 minPoint(x, y, depth);
        Vec3 maxPoint(x + boxSizeX, y + boxSizeY, depth - 1.0f);
        scene.objects.push_back(new Box(minPoint, maxPoint, Color(1, 0, 1)));
    };

    // Letter 'O'
    for (int i = 0; i < 5; ++i) {
        addBox(startX + i * spacing, startY);
        addBox(startX + i * spacing, startY - 4 * spacing);
    }
    addBox(startX, startY - spacing);
    addBox(startX, startY - 2 * spacing);
    addBox(startX, startY - 3 * spacing);
    addBox(startX + 4 * spacing, startY - spacing);
    addBox(startX + 4 * spacing, startY - 2 * spacing);
    addBox(startX + 4 * spacing, startY - 3 * spacing);

    startX += 6 * spacing;

    // Letter 'u'
    for (int i = 0; i < 5; ++i) {
        addBox(startX, startY - i * spacing);
        addBox(startX + 3 * spacing, startY - i * spacing);
    }
    for (int i = 1; i < 3; ++i) {
        addBox(startX + i * spacing, startY - 4 * spacing);
    }

    startX += 5 * spacing;

    // Letter 't'
    for (int i = 0; i < 5; ++i) {
        addBox(startX + 1 * spacing, startY - i * spacing);
    }
    for (int i = 0; i < 3; ++i) {
        addBox(startX + i * spacing, startY);
    }

    startX += 5 * spacing;

    // Letter 'r'
    for (int i = 0; i < 5; ++i) {
        addBox(startX, startY - i * spacing);
    }
    addBox(startX + 2 * spacing, startY - 2);
    addBox(startX + 1 * spacing, startY - 3);
    addBox(startX + 2 * spacing, startY - 4);
    addBox(startX + 3 * spacing, startY - 5);
    addBox(startX + 1 * spacing, startY);
    addBox(startX + 2 * spacing, startY);
    addBox(startX + 3 * spacing, startY - spacing);

    startX += 5 * spacing;

    // Letter 'O'
    for (int i = 0; i < 5; ++i) {
        addBox(startX + i * spacing, startY);
        addBox(startX + i * spacing, startY - 4 * spacing);
    }
    addBox(startX, startY - spacing);
    addBox(startX, startY - 2 * spacing);
    addBox(startX, startY - 3 * spacing);
    addBox(startX + 4 * spacing, startY - spacing);
    addBox(startX + 4 * spacing, startY - 2 * spacing);
    addBox(startX + 4 * spacing, startY - 3 * spacing);

    startX += 6 * spacing;

    // Letter 's'
    for (int i = 0; i < 3; ++i) {
        addBox(startX + i * spacing, startY);
        addBox(startX + i * spacing, startY - 2 * spacing);
        addBox(startX + i * spacing, startY - 4 * spacing);
    }
    addBox(startX, startY - spacing);
    addBox(startX + 2 * spacing, startY - 3 * spacing);

    startX += 5 * spacing;

    // Dots '...'
    for (int d = 0; d < 3; ++d) {
        addBox(startX + d * spacing, startY - 4 * spacing);
    }

    startX += 5 * spacing;

    // Parenthesis '('
    for (int i = 0; i < 5; ++i) {
        addBox(startX, startY - i * spacing);
    }
    addBox(startX + 1 * spacing, startY);
    addBox(startX + 1 * spacing, startY - 4 * spacing);

    startX += 3 * spacing;

    // Digit '3'
    addBox(startX, startY);
    addBox(startX + 1 * spacing, startY);
    addBox(startX + 2 * spacing, startY);
    addBox(startX + 2 * spacing, startY - spacing);
    addBox(startX + 1 * spacing, startY - 2 * spacing);
    addBox(startX + 2 * spacing, startY - 3 * spacing);
    addBox(startX, startY - 4 * spacing);
    addBox(startX + 1 * spacing, startY - 4 * spacing);
    addBox(startX + 2 * spacing, startY - 4 * spacing);

    startX += 5 * spacing;

    // Letter 'D'
    for (int i = 0; i < 5; ++i) {
        addBox(startX, startY - i * spacing);
    }
    for (int i = 0; i < 4; ++i) {
        addBox(startX + 3 * spacing, startY - 1 * spacing - i * spacing);
    }
    addBox(startX + 1 * spacing, startY);
    addBox(startX + 2 * spacing, startY);
    addBox(startX + 1 * spacing, startY - 4 * spacing);
    addBox(startX + 2 * spacing, startY - 4 * spacing);

    startX += 5 * spacing;

    // Parenthesis ')'
    for (int i = 0; i < 5; ++i) {
        addBox(startX + 1 * spacing, startY - i * spacing);
    }
    addBox(startX, startY);
    addBox(startX, startY - 4 * spacing);
}

/**
 * @brief Setup default initial scene layout.
 */
void setupSceneDefault() {
    camera = Camera(
        Vec3(xcam, ycam, zcam),
        Vec3(0, 0, -1),
        Vec3(0, 1, 0),
        fov
    );

    scene.objects.push_back(new Sphere(Vec3(-1, 1, 13), 1.0f, "assets/uranus.jpg"));
    scene.objects.push_back(new Sphere(Vec3(-2, 0, -6), 1.0f, "assets/Jupitar.jpg"));
    scene.objects.push_back(new Sphere(Vec3(2, 1, -7), 1.0f, true));
    scene.objects.push_back(new Plane(Vec3(0, -2, 0), Vec3(0, 1, 0)));

    scene.objects.push_back(new Box(Vec3(-1, 2, -2), Vec3(1, 4, 0), Color(1, 1, 1), "assets/Jupitar.jpg", true));
    scene.objects.push_back(new Box(Vec3(-1, 0, 10), Vec3(1, 2, 11), true));
    scene.objects.push_back(new Box(Vec3(-1, 0, -10), Vec3(1, 2, -11), true));

    Box* glassPlane = new Box(Vec3(-3, 0, -1), Vec3(-2, 2, 1));
    glassPlane->transparency = 1.0f;
    glassPlane->refractiveIndex = 1.5f;
    scene.objects.push_back(glassPlane);

    Sphere* glassSphere = new Sphere(Vec3(0, 0, -5), 1.0f, Color(1, 1, 1));
    glassSphere->transparency = 0.1f;
    glassSphere->refractiveIndex = 1.0f;
    scene.objects.push_back(glassSphere);
}

/**
 * @brief Main GLUT display callback routine.
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    start = std::chrono::high_resolution_clock::now();

    if (IsRayCastingON) {
        rayCastingRenderer.render(scene, camera, maxRecursionDepth);
        glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, rayCastingRenderer.getFramebuffer().data());
    } else {
        rayObjectRenderer.render(scene, camera);
        glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, rayObjectRenderer.getFramebuffer().data());
    }

    end = std::chrono::high_resolution_clock::now();
    if (monitoringTime) {
        std::chrono::duration<double, std::milli> duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start);
        std::cout << "Frame render time: " << duration.count() << " ms" << std::endl;
    }

    glutSwapBuffers();
}

/**
 * @brief Standard keyboard input handler.
 */
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC key
            exit(0);
            break;

        case 'w':
        case 'W':
            if (cameraMode) {
                camera.position = camera.position + camera.forward.normalize() * 0.1f;
            } else {
                scene.moveCurrentObjFront();
            }
            break;
        case 's':
        case 'S':
            if (cameraMode) {
                camera.position = camera.position - camera.forward.normalize() * 0.1f;
            } else {
                scene.moveCurrentObjBack();
            }
            break;
        case 'a':
        case 'A': {
            if (cameraMode) {
                Vec3 right = (camera.forward.cross(camera.up)).normalize();
                camera.position = camera.position - right * 0.1f;
            } else {
                scene.moveCurrentObjLeft();
            }
            break;
        }
        case 'd':
        case 'D': {
            if (cameraMode) {
                Vec3 right = (camera.forward.cross(camera.up)).normalize();
                camera.position = camera.position + right * 0.1f;
            } else {
                scene.moveCurrentObjRight();
            }
            break;
        }
        case 'k':
        case 'K':
            IsRayCastingON = !IsRayCastingON;
            std::cout << "Ray Casting mode: " << (IsRayCastingON ? "ENABLED" : "DISABLED") << std::endl;
            break;

        case 't':
        case 'T':
            monitoringTime = !monitoringTime;
            std::cout << "Render timer: " << (monitoringTime ? "ENABLED" : "DISABLED") << std::endl;
            break;

        case ' ': // Space key moves up
            if (cameraMode) {
                camera.position.y += 0.1f;
            } else {
                scene.moveCurrentObjUp();
            }
            break;
        case 'c':
        case 'C': // C key moves down
            if (cameraMode) {
                camera.position.y -= 0.1f;
            } else {
                scene.moveCurrentObjDown();
            }
            break;

        case 'z':
            if (maxRecursionDepth > 1) {
                maxRecursionDepth--;
            }
            std::cout << "Max recursion depth: " << maxRecursionDepth << std::endl;
            break;

        case 'Z':
            maxRecursionDepth++;
            std::cout << "Max recursion depth: " << maxRecursionDepth << std::endl;
            break;

        case 'f':
            if (camera.fov > 5.0f) {
                camera.fov -= 5.0f;
            }
            std::cout << "Current FOV: " << camera.fov << std::endl;
            break;

        case 'F':
            if (camera.fov < 170.0f) {
                camera.fov += 5.0f;
            }
            std::cout << "Current FOV: " << camera.fov << std::endl;
            break;

        case 'x':
        case 'X': {
            cameraMode = !cameraMode;
            std::cout << (cameraMode ? "Mode: CAMERA CONTROL" : "Mode: OBJECT CONTROL") << std::endl;
            break;
        }

        case 'l': {
            const float LUMINOSITY_STEP = 0.05f;
            if (!cameraMode) {
                scene.addToLuminosity(-LUMINOSITY_STEP);
            }
            break;
        }
        case 'L': {
            const float LUMINOSITY_STEP = 0.05f;
            if (!cameraMode) {
                scene.addToLuminosity(LUMINOSITY_STEP);
            }
            break;
        }

        case 'g': {
            const float LUMINOSITY_STEP = 0.05f;
            if (!cameraMode) {
                scene.addToLuminosityG(-LUMINOSITY_STEP);
            }
            break;
        }
        case 'G': {
            const float LUMINOSITY_STEP = 0.05f;
            if (!cameraMode) {
                scene.addToLuminosityG(LUMINOSITY_STEP);
            }
            break;
        }

        case 'b': {
            const float LUMINOSITY_STEP = 0.05f;
            if (!cameraMode) {
                scene.addToLuminosityB(-LUMINOSITY_STEP);
            }
            break;
        }
        case 'B': {
            const float LUMINOSITY_STEP = 0.05f;
            if (!cameraMode) {
                scene.addToLuminosityB(LUMINOSITY_STEP);
            }
            break;
        }

        case 'r': {
            const float LUMINOSITY_STEP = 0.05f;
            if (!cameraMode) {
                scene.addToLuminosityR(-LUMINOSITY_STEP);
            }
            break;
        }
        case 'R': {
            const float LUMINOSITY_STEP = 0.05f;
            if (!cameraMode) {
                scene.addToLuminosityR(LUMINOSITY_STEP);
            }
            break;
        }
    }

    glutPostRedisplay();
}

/**
 * @brief Special key input handler (arrow keys).
 */
void specialKeyboard(int key, int x, int y) {
    if (!cameraMode) {
        switch (key) {
            case GLUT_KEY_LEFT: {
                scene.previousObj();
                std::cout << "Selected object index: " << scene.currentObj << std::endl;
                break;
            }
            case GLUT_KEY_RIGHT: {
                scene.nextObj();
                std::cout << "Selected object index: " << scene.currentObj << std::endl;
                break;
            }
        }
    }
    glutPostRedisplay();
}

inline float radians(float degrees) {
    return degrees * (static_cast<float>(M_PI) / 180.0f);
}

/**
 * @brief First-person Mouse Look callback handler.
 */
void mouseMovement(int x, int y) {
    if (justWarped) {
        justWarped = false;
        return;
    }

    float deltaX = static_cast<float>(x - centerX);
    float deltaY = static_cast<float>(centerY - y);

    float sensitivity = 0.1f;
    yaw += deltaX * sensitivity;
    pitch += deltaY * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    Vec3 direction;
    direction.x = std::cos(radians(yaw)) * std::cos(radians(pitch));
    direction.y = std::sin(radians(pitch));
    direction.z = std::sin(radians(yaw)) * std::cos(radians(pitch));
    camera.forward = direction.normalize();

    justWarped = true;
    glutWarpPointer(centerX, centerY);

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "3d") {
            setupOutros3D();
        } else {
            setupScene(arg);
        }
    } else {
        setupSceneDefault();
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("JVAV Ray-Casting Engine");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glPixelZoom(1, -1);
    glRasterPos2i(0, HEIGHT - 1);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutPassiveMotionFunc(mouseMovement);
    glutWarpPointer(centerX, centerY);

    glutMainLoop();
    return 0;
}

