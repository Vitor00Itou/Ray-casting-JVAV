#include <iostream>
#include <chrono>
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <fstream>

#include "ray_casting_renderer.hpp"
#include "ray_object_renderer.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "box.hpp"
#include "../lib/json.hpp"

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
bool cameraMode = true;

int maxRecursionDepth = 10;

RayCastingRenderer rayCastingRenderer(WIDTH, HEIGHT);
RayObjectRenderer rayObjectRenderer(WIDTH, HEIGHT);
Scene scene;
Camera camera;

Sphere* parseSphere(const nlohmann::json& obj) {
    Vec3 center = Vec3(obj["center"][0], obj["center"][1], obj["center"][2]);
    float radius = obj["radius"];
    Color color = Color(1.0f, 1.0f, 1.0f); // Padrão: branco
    bool isEmitter = obj.value("emitter", false);
    std::string texture = ""; // Sem textura por padrão
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

    if (obj.contains("refractiveIndex")) {
        transparency = obj["transparency"];
    }

    if (obj.contains("refractiveIndex")) {
        transparency = obj["transparency"];
    }

    return new Sphere(center, radius, color, texture.c_str(), isEmitter, specularShininess, reflectionCoefficient, transparency, refractiveIndex);
}

Plane* parsePlane(const nlohmann::json& obj) {
    Vec3 point = Vec3(obj["point"][0], obj["point"][1], obj["point"][2]);
    Vec3 normal = Vec3(obj["normal"][0], obj["normal"][1], obj["normal"][2]);
    Color color = Color(1.0f, 1.0f, 1.0f); // Cor padrão: branco
    std::string texture = ""; // Sem textura por padrão
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


    return new Plane(point, normal, color, texture.c_str(), specularShininess, reflectionCoefficient, transparency, refractiveIndex);
}

Box* parseBox(const nlohmann::json& obj) {
    // Extrair os cantos mínimo e máximo do Box
    Vec3 minCorner = Vec3(obj["minCorner"][0], obj["minCorner"][1], obj["minCorner"][2]);
    Vec3 maxCorner = Vec3(obj["maxCorner"][0], obj["maxCorner"][1], obj["maxCorner"][2]);
    
    // Propriedades adicionais
    Color color = Color(1.0f, 1.0f, 1.0f); // Cor padrão: branco
    bool isEmitter = obj.value("emitter", false);
    std::string texture = ""; // Sem textura por padrão
    float specularShininess = 32.0f;
    float transparency = 0.0f;
    float refractiveIndex = 1.0f;
    float reflectionCoefficient = 0.0f;

    // Verificar e atribuir a cor, se fornecida
    if (obj.contains("color")) {
        color = Color(obj["color"][0], obj["color"][1], obj["color"][2]);
    }

    // Verificar e atribuir a textura, se fornecida
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

    return new Box(minCorner, maxCorner, color, texture.c_str(), isEmitter, specularShininess, reflectionCoefficient, transparency, refractiveIndex);


}

void setupScene(const std::string& filename) {
    // Câmera olhando para o centro da cena
    camera = Camera(
        Vec3(xcam, ycam, zcam),     // posição
        Vec3(0, 0, -1),    // direção
        Vec3(0, 1, 0),     // cima
        fov              // fov
    );

    std::ifstream input(filename);
    if (!input.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de cena: " << filename << std::endl;
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
        }
    }
}


void setupSceneW() {
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
            if (cameraMode) {
                //Faz a camera andar na direção que a camera aponta
                camera.position = camera.position + camera.forward.normalize()*0.1;
            } else {
                scene.moveCurrentObjFront();
            }
            break;
        case 's':
            if (cameraMode) {
                camera.position = camera.position - camera.forward.normalize()*0.1;
            } else {
                scene.moveCurrentObjBack();
            }
            break;
        case 'a':{
            if (cameraMode) {
                Vec3 right = (camera.forward.cross(camera.up)).normalize();  // Acha a direta da camera
                camera.position = camera.position - right * 0.1f; // Move para a esquerda
            } else {
                scene.moveCurrentObjLeft();
            }
            break;
        }
        case 'd':{
            if (cameraMode) {
                Vec3 right = (camera.forward.cross(camera.up)).normalize();  // Acha a direta da camera
                camera.position = camera.position + right * 0.1f; // Move para a direita
            } else {
                scene.moveCurrentObjRight();
            }
            break;
        }
        case 'k':
            IsRayCastingON = !IsRayCastingON;
            break;

        case 't':
            monitoringTime = !monitoringTime;
            break;

        case ' ':  // Espaço sobe
            if (cameraMode) {
                camera.position.y += 0.1f;
            } else {
                scene.moveCurrentObjUp();
            }
            break;
        case 'c':  // C desce
            if (cameraMode) {
                camera.position.y -= 0.1f;
            } else {
                scene.moveCurrentObjDown();
            }
            break;

        case 'z':
            if (maxRecursionDepth)
            {
                maxRecursionDepth--;
            }
            
            break;

        case 'Z':
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

        case 'x': {
            cameraMode = !cameraMode;
            std::vector<std::string> modes = {"OBJECT MODE ON", "CAMERA MODE ON"};
            std::cout << modes[cameraMode] << std::endl;
            break;
        }

        case 'l': {
            const float LUMINOSITY_INCREASE = 0.05;
            if (!cameraMode) {
                scene.addToLuminosity(-LUMINOSITY_INCREASE);
            }
            break;
        }

        case 'L': {
            const float LUMINOSITY_INCREASE = 0.05;
            if (!cameraMode) {
                scene.addToLuminosity(LUMINOSITY_INCREASE);
            }
            break;
        }

        case 'g': {
            const float LUMINOSITY_INCREASE = 0.05;
            if (!cameraMode) {
                scene.addToLuminosityG(LUMINOSITY_INCREASE);
            }
            break;
        }

        case 'G': {
            const float LUMINOSITY_INCREASE = 0.05;
            if (!cameraMode) {
                scene.addToLuminosityG( - LUMINOSITY_INCREASE);
            }
            break;
        }
        case 'b': {
            const float LUMINOSITY_INCREASE = 0.05;
            if (!cameraMode) {
                scene.addToLuminosityB(LUMINOSITY_INCREASE);
            }
            break;
        }

        case 'B': {
            const float LUMINOSITY_INCREASE = 0.05;
            if (!cameraMode) {
                scene.addToLuminosityB( - LUMINOSITY_INCREASE);
            }
            break;
        }

        case 'r': {
            const float LUMINOSITY_INCREASE = 0.05;
            if (!cameraMode) {
                scene.addToLuminosityR(LUMINOSITY_INCREASE);
            }
            break;
        }

        case 'R': {
            const float LUMINOSITY_INCREASE = 0.05;
            if (!cameraMode) {
                scene.addToLuminosityR( - LUMINOSITY_INCREASE);
            }
            break;
        }
        
    }

    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {
    if (!cameraMode) {
        switch (key) {
            case GLUT_KEY_LEFT: {
                scene.previousObj();
                std::cout << "Objeto atual: " << scene.currentObj << "\n";
                break;
            }
            case GLUT_KEY_RIGHT: {
                scene.nextObj();
                std::cout << "Objeto atual: " << scene.currentObj << "\n";
                break;
            }
        }
        // luminosidade (talvez rgb separadamente)
        // transparencia
        // refração (em objetos transparentes)
        // reflexao
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
    setupSceneW();
    //setupScene("worlds/caixas.json");

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
    glutSpecialFunc(specialKeyboard);
    glutSetCursor(GLUT_CURSOR_NONE); // Esconde o cursor
    glutPassiveMotionFunc(mouseMovement); // Configura o callback do mouse
    glutWarpPointer(centerX, centerY); 

    glutMainLoop();
    return 0;
}
