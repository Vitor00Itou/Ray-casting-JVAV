# 🔦 Raycasting com OpenGL (GLUT)

Este é um projeto simples de **ray casting** implementado em **C++ com OpenGL (usando GLUT)** para fins educacionais em disciplinas de Computação Gráfica.

---

## 🚀 Requisitos

- **Compilador:** g++
- **Bibliotecas:** OpenGL + GLUT (ex: freeglut)
- **Ferramentas:** make

### Como instalar dependências (Ubuntu/Debian):

```bash
sudo apt update
sudo apt install build-essential freeglut3-dev
sudo apt install make
```

## 🛠️ Como Compilar e Rodar

Compilar e executar:

```bash
make run
```

(Ou apenas `make` para compilar.)

Limpar compilação:

```bash
make clean
```

Limpar compilação e recompilar:

```bash
make rebuild
```

---

## 🎨 Funcionalidades atuais

- Renderização de cenas 3D usando **ray casting**.
- Suporte a **esferas**, **planos** e **caixas** como objetos ou objetos emissores de luz.
- Fontes de luz **pontuais** com diferentes intensidades e cores.
- Câmera com movimentação básica (usando teclado/mouse).
- Texturização de objetos (mapa de textura simples).

---

## 🧱 Como Instanciar Objetos

Os objetos e luzes são adicionados diretamente na inicialização da cena (`main.cpp` função `setup()`).

### Exemplo de adição de objetos:

```cpp
Sphere* sphere = new Sphere(
    Vec3(0, 0, -5),  // Centro
    1.0f,            // Raio
    Texture("textures/earth.jpg") // Textura
);
scene.objects.push_back(sphere);
```

```cpp
Plane* plane = new Plane(
    Vec3(0, -1, 0),   // Posição
    Vec3(0, 1, 0),    // Normal
    Texture("textures/floor.png") // Textura
);
scene.objects.push_back(plane);
```

### Exemplo de adição de luz:

```cpp
scene.lightSources.push_back(Light(
    Vec3(5, 5, -3), // Posição
    Vec3(1, 1, 1)   // Intensidade RGB (branco)
));
```

---

## 🎮 Controles

- **W / S** — Movimentar para frente/trás
- **A / D** — Movimentar para esquerda/direita
- **Mouse** — Girar a câmera
- **Space** — Subir
- **C** — descer
- **ESC** — Encerrar o programa
- **K** — Ativar/desativar o **ray casting**
- **T** — Ativar/desativar monitoramento de tempo de renderização por frame

---

## 🎯 Trabalhos Futuros

Algumas melhorias planejadas para o projeto:

- [ ] **Adicionar transparência** (materiais translúcidos, tipo vidro).
- [ ] **Reflexividade** (superfícies espelhadas).
- [ ] **Novos tipos de objetos** (cilindros, cones, meshes).
- [ ] **Anti-aliasing** por amostragem múltipla (supersampling).
- [ ] **Sistema de materiais** mais completo (brilho, rugosidade).
- [ ] **Carregamento de cenas via arquivos externos** (ex: `.json` ou `.yaml`).
- [ ] **Calcular a diferença entre rasterização e raycasting** em tempo de renderização.
---

## 📄 Licença

Este projeto é de uso livre para fins educacionais e pessoais.

---

# ✨ Preview

![Esfera luminosa com textura](images/sun_light.png)

![Cubo luminoso cor sólida](images/ciano_cube.png)

