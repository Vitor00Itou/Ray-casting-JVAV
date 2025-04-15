#include "renderer.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "camera.hpp"

#include <GL/glut.h>
#include <vector>

// Tamanho da janela
constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

static float xcam = 0;
static float ycam = 0;
static float zcam = 0;

Renderer renderer(WIDTH, HEIGHT);
Scene scene;
Camera camera;

void setupScene() {
    // Câmera olhando para o centro da cena
    camera = Camera(
        Vec3(xcam, ycam, zcam),     // posição
        Vec3(0, 0, -1),    // direção
        Vec3(0, 1, 0),     // cima
        90.0f              // fov
    );

    // Luzes zuluzes
    Vec3 lightPos(2, -8, -5);
    scene.lightSources.push_back(Light(
        lightPos,
        1.0
    ));

    // Adiciona esferas à cena
    scene.objects.push_back(Sphere(lightPos, 1.0));
    scene.objects.push_back(Sphere(Vec3(0, 0, -5), 1.0f, Color(1.0, 0.0, 0.0)));
    scene.objects.push_back(Sphere(Vec3(-2, 0, -6), 1.0f));
    scene.objects.push_back(Sphere(Vec3(2, 1, -7), 1.0f));
}

void display() {
    // Renderiza a imagem
    renderer.render(scene, camera);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, renderer.getFramebuffer().data());
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;

        case 'w':
            camera.position.z -= 0.1;
            break;
        case 's':
            camera.position.z += 0.1;
            break;
        case 'a':
            camera.position.x += 0.1;
            break;
        case 'd':
            camera.position.x -= 0.1;
            break;
    }

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

    glutMainLoop();
    return 0;
}
