#include "ray_casting_renderer.hpp"
#include "ray_object_renderer.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "box.hpp"
#include <iostream>
#include <chrono>

#include <GL/glut.h>
#include <vector>
#include <cmath>

// Tamanho da janela
constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

static float xcam = 0;
static float ycam = 0;
static float zcam = 0;
static float fov = 45;

std::chrono::high_resolution_clock::time_point start, end;
static bool IsRayCastingON = false;
static bool monitoringTime = false;

// Variáveis para movimentação da camera
static float yaw = -90.0f;   // Ângulo horizontal (inicia virado para -Z)
static float pitch = 0.0f;    // Ângulo vertical
int windowWidth = WIDTH;
int windowHeight = HEIGHT;
int centerX = windowWidth / 2;
int centerY = windowHeight / 2;
bool justWarped = false;

int maxRecursionDepth = 10;


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
        fov              // fov
    );

    // Luzes zuluzes
    Vec3 lightPos(2, 7, -5);
    // scene.objects.push_back(new LightPoint(lightPos, Color(0,0,1)));
    // scene.objects.push_back(new LightPoint(lightPos + Vec3(0, 0, -5), Color(0,1,0)));
    // scene.objects.push_back(new LightPoint(lightPos + Vec3(0, 0, 5), Color(1,0,0)));

    // Adiciona esferas à cena
    // scene.objects.push_back(new Sphere(Vec3(2, 2, -5), 1.0));
    //scene.objects.push_back(new Sphere(Vec3(2, 2, -5), 1.0f, Color(1, 1, 1), "assets/sol.jpg", true));
    //scene.objects.push_back(new Sphere(Vec3(2, 2, 5), 1.0f, Color(1, 1, 1), "assets/sol.jpg", true));
    //scene.objects.push_back(new Sphere(Vec3(0, -3, -5), 1.0f, "assets/uranus.jpg"));
    scene.objects.push_back(new Sphere(Vec3(-1, 1, 13), 1.0f, "assets/uranus.jpg"));
    //scene.objects.push_back(new Sphere(Vec3(0, 0, -5), 1.0f, "assets/earth albedo.jpg"));
    scene.objects.push_back(new Sphere(Vec3(-2, 0, -6), 1.0f, "assets/Jupitar.jpg"));
    //scene.objects.push_back(new Sphere(Vec3(2, 1, -7), 1.0f, "assets/uranus.jpg"));
    scene.objects.push_back(new Sphere(Vec3(2, 1, -7), 1.0f, true));

    // Adiciona planos à cena
    //scene.objects.push_back(new Plane(Vec3(0,1,0), Vec3(0,1,0), "assets/earth albedo.jpg"));
    //scene.objects.push_back(new Plane(Vec3(0,-2,0), Vec3(0,1,0),"assets/earth albedo.jpg"));
    scene.objects.push_back(new Plane(Vec3(0,-2,0), Vec3(0,1,0)));


    // Adicionar paralelipipedos à cena
    scene.objects.push_back(new Box(Vec3(-1, 2, -2), Vec3(1, 4, 0), Color(1, 1, 1), "assets/Jupitar.jpg", true));
    //scene.objects.push_back(new Box(Vec3(-1, 0, 6), Vec3(1, 2, 8), "assets/Jupitar.jpg"));
    scene.objects.push_back(new Box(Vec3(-1, 0, 10), Vec3(1, 2, 11), true));
    scene.objects.push_back(new Box(Vec3(-1, 0, -10), Vec3(1, 2, -11), true));


    Box* glassPlane = new Box(Vec3(-3, 0, -1), Vec3(-2, 2, 1));
    glassPlane->transparency = 1.0f;
    glassPlane->refractiveIndex = 1.5f;
    
    scene.objects.push_back(glassPlane);

    Sphere* glassSphere = new Sphere(Vec3(0,0,-5), 1.0f, Color(1,1,1));
    glassSphere->transparency = 0.1f;
    glassSphere->refractiveIndex = 1.0f;
    scene.objects.push_back(glassSphere);

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    start = std::chrono::high_resolution_clock::now();
    // Renderiza a imagem
    if (IsRayCastingON)
    {
        rayCastingRenderer.render(scene, camera, maxRecursionDepth);
        glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, rayCastingRenderer.getFramebuffer().data());
    }else{
        rayObjectRenderer.render(scene, camera);
        glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, rayObjectRenderer.getFramebuffer().data());
    }

    end = std::chrono::high_resolution_clock::now();
    if (monitoringTime) {
        std::chrono::duration<double, std::milli> duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start);
        std::cout << "Tempo decorrido: " << duration.count() << " ms" << std::endl;
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
            camera.position = camera.position - right * 0.1f; // Move para a esquerda
            break;
        }
        case 'd':{
            Vec3 right = (camera.forward.cross(camera.up)).normalize();  // Acha a direta da camera
            camera.position = camera.position + right * 0.1f; // Move para a direita
            break;
        }
        case 'k':
            IsRayCastingON = !IsRayCastingON;
            break;

        case 't':
            monitoringTime = !monitoringTime;
            break;

        case ' ':  // Espaço sobe
            camera.position.y += 0.1f;
            break;
        case 'c':  // Espaço sobe
            camera.position.y -= 0.1f;
            break;

        case 'r':
            if (maxRecursionDepth)
            {
                maxRecursionDepth--;
            }
            
            break;

        case 'R':
            maxRecursionDepth++;
            break;  

        case 'f':
            if (camera.fov)
            {
                camera.fov = camera.fov - 5;
            }
            std::cout <<"Fov atual: "<< camera.fov  << std::endl;
            break;

        case 'F':
            camera.fov = camera.fov + 5;
            std::cout <<"Fov atual: "<< camera.fov  << std::endl;
            break;

        
    }

    glutPostRedisplay();
}

inline float radians(float degrees) {
    return degrees * (M_PI / 180.0f);
}



void mouseMovement(int x, int y) {
    if (justWarped) {
        justWarped = false;
        return;
    }

    float deltaX = x - centerX;
    float deltaY = centerY - y; // Inverte o Y (cima positivo)

    float sensitivity = 0.1f;
    yaw += deltaX * sensitivity;
    pitch += deltaY * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    Vec3 direction;
    direction.x = cos(radians(yaw)) * cos(radians(pitch));
    direction.y = sin(radians(pitch));
    direction.z = sin(radians(yaw)) * cos(radians(pitch));
    camera.forward = direction.normalize();

    // Agora sim, centraliza o mouse
    justWarped = true;
    glutWarpPointer(centerX, centerY);

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
    glutSetCursor(GLUT_CURSOR_NONE); // Esconde o cursor
    glutPassiveMotionFunc(mouseMovement); // Configura o callback do mouse
    glutWarpPointer(centerX, centerY); 

    glutMainLoop();
    return 0;
}
