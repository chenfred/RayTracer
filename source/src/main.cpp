#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "glm/fwd.hpp"
#include "material/diffuse_material.hpp"
#include "renderer/debug_renderer.hpp"
#include "renderer/direct_shading_renderer.hpp"
#include "renderer/simple_rt_renderer.hpp"
#include "shape/cube.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "shape/shape.hpp"
#include "shape/sphere.hpp"
#include "shape/triangle.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void test_renderer();

static const size_t WIDTH = 1920;
static const size_t HEIGHT = 1080;

int main() {
    test_renderer();
    return 0;
}

std::vector<Triangle> generate_triangles(float edge) {
    float halfEdge = edge / 2.0f;
    std::vector<Triangle> triangles;

    // glm::vec3 norm{0, 0, 1};
    // triangles.emplace_back(glm::vec3{halfEdge, halfEdge, 0},
    //                        glm::vec3{-halfEdge, halfEdge, 0},
    //                        glm::vec3{halfEdge, -halfEdge, 0});
    // return triangles;

    std::array<glm::vec3, 8> vertices{
        glm::vec3(-halfEdge, -halfEdge, -halfEdge),
        glm::vec3(halfEdge, -halfEdge, -halfEdge),
        glm::vec3(halfEdge, halfEdge, -halfEdge),
        glm::vec3(-halfEdge, halfEdge, -halfEdge),
        glm::vec3(-halfEdge, -halfEdge, halfEdge),
        glm::vec3(halfEdge, -halfEdge, halfEdge),
        glm::vec3(halfEdge, halfEdge, halfEdge),
        glm::vec3(-halfEdge, halfEdge, halfEdge)};

    // 定义正方体的6个面，每个面由2个三角形组成
    // 每个三角形的顶点按逆时针顺序排列，以确保法线朝外
    std::array<std::array<int, 3>, 12> faceTriangles = {{
        {{0, 3, 2}}, {{0, 2, 1}}, // -Z face
        {{4, 5, 6}},
        {{4, 6, 7}}, // +Z face
        {{0, 1, 5}},
        {{0, 5, 4}}, // -Y face
        {{2, 3, 7}},
        {{2, 7, 6}}, // +Y face
        {{0, 7, 3}},
        {{0, 4, 7}}, // -X face
        {{1, 2, 6}},
        {{1, 6, 5}} // +X face
    }};

    triangles.reserve(faceTriangles.size());
    for (const auto &face : faceTriangles) {
        triangles.emplace_back(vertices[face[0]], vertices[face[1]], vertices[face[2]]);
    }

    return triangles;
}

void test_renderer() {
    Film film{WIDTH, HEIGHT};
    glm::vec3 light_pos{0, 2, 0};
    glm::vec3 light_intensity{5};

    // shapes
    Sphere lightSphere{0.1, {0, 0, 0}, new DiffuseMaterial(glm::vec3{1.0}, glm::vec3{0.5})};
    Sphere sphere{1, {0, 0, 0}, new DiffuseMaterial(glm::vec3{0.3, 0.3, 0.7})};
    Cube planeCube{1, new DiffuseMaterial(glm::vec3{0.5})};
    Plane plane{{0, 0, 0}, {0, 1, 0}, new DiffuseMaterial({glm::vec3{0.1}})};
    Model model{"resources/models/simple_dragon.obj", new DiffuseMaterial(glm::vec3{0.3})};
    // Model model{"resources/models/dragon_87k.obj", new DiffuseMaterial(glm::vec3{0.3})};

    // Scene
    Scene scene;
    // scene.addShape(lightSphere, {0, 0.5, 0});
    // scene.addShape(model, {0.75, 0, 0}, glm::vec3{1}, {0, -90, 0});
    scene.addShape(sphere, {0, 0, 0.25}, glm::vec3{0.35});
    scene.addShape(planeCube, {0,-0.5,0},glm::vec3{8,0.25,8}, glm::vec3{0});
    // scene.addShape(plane, {0, -0.5, 0});

    // Triangle tri{{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}};
    // Mesh mesh{{tri}, new DiffuseMaterial(glm::vec3{0.5})};
    // scene.addShape(mesh);

    // std::vector<Triangle> triangles = generate_triangles(0.5);
    // Mesh cubeMesh{triangles, new DiffuseMaterial(glm::vec3{1.0})};
    // scene.addShape(cubeMesh,glm::vec3{0}, glm::vec3{1}, {30,0,0});

    // Camera
    Camera camera(film, {0, 0, 1}, {0, 0, 0}, 90);

    // Renderer
    DirectShadingRenderer directShadingRenderer(camera, scene, light_pos, light_intensity);
    SimpleRayTracingRenderer simpleRTRenderer(camera, scene);
    DebugInstanceRenderer instRen(camera, scene);
    DebugNormalRenderer normalRen(camera, scene);
    DebugPositionRenderer posRen(camera, scene);
    DebugDepthRenderer depthRen(camera, scene);
    DebugLightDirRenderer lightDirRen(camera, scene);

    // Go!
    Renderer &renderer{directShadingRenderer};
    renderer.render(1, "./results/scene.png");

    // Debug Go!
    instRen.render("./results/instances.png");
    normalRen.render("./results/normal.png");
    depthRen.render("./results/depth.png");
}