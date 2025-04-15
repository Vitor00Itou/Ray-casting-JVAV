#include "ray_casting_renderer.hpp"
#include "ray_object_renderer.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "camera.hpp"

#include <GL/glut.h>
#include <vector>
#include <cmath>

// Tamanho da janela
constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

static float xcam = 0;
static float ycam = 0;
static float zcam = 0;

static bool IsRayCastingON = false;


// Ângulos da câmera (rotação horizontal e vertical)
static float yaw = -90.0f;   // Ângulo horizontal (inicia virado para -Z)
static float pitch = 0.0f;    // Ângulo vertical
static float lastX = WIDTH / 2.0f; // Posição anterior do mouse (X)
static float lastY = HEIGHT / 2.0f; // Posição anterior do mouse (Y)
static bool firstMouse = true; // Flag para o primeiro movimento


RayCastingRenderer rayCastingRenderer(WIDTH, HEIGHT);
RayObjectRenderer rayObjectRenderer(WIDTH, HEIGHT);
Scene scene;
Camera camera;

void setupScene() {
    // Câmera olhando para o centro da cena
    camera = Camera(
        Vec3(xcam, ycam, zcam),     // posição
        Vec3(0, 0, -1),    // direção
        Vec3(0, 1, 0),     // cima
        60.0f              // fov
    );

    // Luzes zuluzes
    Vec3 lightPos(2, -8, -5);
    scene.lightSources.push_back(Light(
        lightPos,
        1.0
    ));

    // Adiciona esferas à cena
    //scene.objects.push_back(Sphere(lightPos, 1.0));
    scene.objects.push_back(Sphere(Vec3(0, 3, -5), 1.0f, Color(1.0, 0.0, 0.0)));
    scene.objects.push_back(Sphere(Vec3(0, 0, -5), 1.0f, Color(1.0, 0.0, 0.0)));
    scene.objects.push_back(Sphere(Vec3(-2, 0, -6), 1.0f, Color(0.0, 1.0, 0.0)));
    scene.objects.push_back(Sphere(Vec3(2, 1, -7), 1.0f, Color(0.0, 1.0, 1.0)));
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Renderiza a imagem
    if (IsRayCastingON)
    {
        rayCastingRenderer.render(scene, camera);
        glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, rayCastingRenderer.getFramebuffer().data());
    }else{
        rayObjectRenderer.render(scene, camera);
        glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, rayObjectRenderer.getFramebuffer().data());
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;

        case 'w':
            //Faz a camera andar na direção que a camera aponta
            camera.position = camera.position + camera.forward.normalize()*0.1;
            break;
        case 's':
            camera.position = camera.position - camera.forward.normalize()*0.1;
            break;
        case 'a':{
            Vec3 right = (camera.forward.cross(camera.up)).normalize();  // Acha a direta da camera
            camera.position = camera.position + right * 0.1f; // Move para a esquerda
            break;
        }
        case 'd':{
            Vec3 right = (camera.forward.cross(camera.up)).normalize();  // Acha a direta da camera
            camera.position = camera.position - right * 0.1f; // Move para a direita
            break;
        }
        case 'k':
            IsRayCastingON = !IsRayCastingON;
            break;
            
    }

    glutPostRedisplay();
}

inline float radians(float degrees) {
    return degrees * (M_PI / 180.0f);
}


void mouseMovement(int x, int y) {
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float sensitivity = 0.1f; // Ajuste a sensibilidade
    float deltaX = (x - lastX) * sensitivity;
    float deltaY = (lastY - y) * sensitivity; // Invertido (Y cresce para baixo)

    lastX = x;
    lastY = y;

    // Atualiza ângulos
    yaw += deltaX;
    pitch += deltaY;

    // Limita o pitch para evitar "gimbal lock"
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Calcula a nova direção da câmera
    Vec3 direction;
    direction.x = cos(radians(yaw)) * cos(radians(pitch));
    direction.y = sin(radians(pitch));
    direction.z = sin(radians(yaw)) * cos(radians(pitch));
    camera.forward = direction.normalize();

    // glutWarpPointer(WIDTH / 2, HEIGHT / 2); // Centraliza o cursor
    // lastX = WIDTH / 2.0f;
    // lastY = HEIGHT / 2.0f;

    glutPostRedisplay();
}


int main(int argc, char** argv) {
    setupScene();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Ray Casting Renderer");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glPixelZoom(1, -1);
    glRasterPos2i(0, HEIGHT - 1);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    //glutSetCursor(GLUT_CURSOR_NONE); // Esconde o cursor
    glutPassiveMotionFunc(mouseMovement); // Configura o callback do mouse

    glutMainLoop();
    return 0;
}
