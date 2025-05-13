#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

const int IMG_WIDTH = 1000;
const int IMG_HEIGHT = 1000;
const float PROJ_COEFF = 2400.0f;
const float FOCAL_OFFSET = 500.0f;

struct Vertex { float x, y, z; };
struct TexCoord { float u, v; };
struct Face { int v_indices[3]; int t_indices[3]; };

enum RotationType { EULER, QUATERNION };

struct ModelTransform {
    float tx, ty, tz;
    float scale;
    RotationType rotation_type;
    float euler_angles[3];
    float quaternion[4];
};

void quaternionMultiply(const float q1[4], const float q2[4], float result[4]) {
    result[0] = q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
    result[1] = q1[0] * q2[1] + q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2];
    result[2] = q1[0] * q2[2] - q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1];
    result[3] = q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0];
}

void quaternionConjugate(const float q[4], float result[4]) {
    result[0] = q[0];
    result[1] = -q[1];
    result[2] = -q[2];
    result[3] = -q[3];
}

void rotateVectorByQuaternion(const float v[3], const float q[4], float rotated_v[3]) {
    float p[4] = { 0, v[0], v[1], v[2] }, q_conj[4], qp[4], rotated_p[4];
    quaternionConjugate(q, q_conj);
    quaternionMultiply(q, p, qp);
    quaternionMultiply(qp, q_conj, rotated_p);
    rotated_v[0] = rotated_p[1];
    rotated_v[1] = rotated_p[2];
    rotated_v[2] = rotated_p[3];
}

void barycentric(float x0, float y0, float x1, float y1, float x2, float y2, float x, float y, float& l0, float& l1, float& l2) {
    float temp = (x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2);
    l0 = ((x - x2) * (y1 - y2) - (x1 - x2) * (y - y2)) / temp;
    l1 = ((x0 - x2) * (y - y2) - (x - x2) * (y0 - y2)) / temp;
    l2 = 1.0f - l0 - l1;
}

void calculateNormal(const Vertex& v0, const Vertex& v1, const Vertex& v2, float& nx, float& ny, float& nz) {
    float i[3] = { v1.y - v2.y, v1.z - v2.z, v1.x - v2.x };
    float j[3] = { v1.y - v0.y, v1.z - v0.z, v1.x - v0.x };
    nx = i[1] * j[2] - i[2] * j[1];
    ny = i[2] * j[0] - i[0] * j[2];
    nz = i[0] * j[1] - i[1] * j[0];
    float length = std::sqrt(nx * nx + ny * ny + nz * nz);
    nx /= length;
    ny /= length;
    nz /= length;
}

void drawTriangle(uint8_t* image_data, float* z_buffer, int img_width, int img_height, const Vertex& v0, const Vertex& v1, const Vertex& v2, const TexCoord& t0, const TexCoord& t1, const TexCoord& t2, const uint8_t* texture_data, int texture_width, int texture_height, const float n0[3], const float n1[3], const float n2[3]) {
    // 3D -> 2D (с масштабом и смещением)
    float x0 = (PROJ_COEFF * v0.x) / v0.z + FOCAL_OFFSET;
    float y0 = (PROJ_COEFF * v0.y) / v0.z + FOCAL_OFFSET;
    float x1 = (PROJ_COEFF * v1.x) / v1.z + FOCAL_OFFSET;
    float y1 = (PROJ_COEFF * v1.y) / v1.z + FOCAL_OFFSET;
    float x2 = (PROJ_COEFF * v2.x) / v2.z + FOCAL_OFFSET;
    float y2 = (PROJ_COEFF * v2.y) / v2.z + FOCAL_OFFSET;

    // границы прямоугольника вокруг треугольника
    int xmin = std::max(0, (int)std::min(std::min(x0, x1), x2));
    int ymin = std::max(0, (int)std::min(std::min(y0, y1), y2));
    int xmax = std::min(img_width - 1, (int)std::max(std::max(x0, x1), x2));
    int ymax = std::min(img_height - 1, (int)std::max(std::max(y0, y1), y2));

    // обход пикселей в прямоугольнике
    for (int x = xmin; x <= xmax; ++x) {
        for (int y = ymin; y <= ymax; ++y) {
            // Барицентрические координаты
            float l0, l1, l2;
            barycentric(x0, y0, x1, y1, x2, y2, (float)x, (float)y, l0, l1, l2);

            if (l0 < 0 || l1 < 0 || l2 < 0)
                continue;

            float z = v0.z * l0 + v1.z * l1 + v2.z * l2;
            int index = y * img_width + x;

            if (z >= z_buffer[index])
                continue;

            float nx = n0[0] * l0 + n1[0] * l1 + n2[0] * l2;
            float ny = n0[1] * l0 + n1[1] * l1 + n2[1] * l2;
            float nz = n0[2] * l0 + n1[2] * l1 + n2[2] * l2;
            float intensity = std::max(0.0f, -nz);

            uint8_t color[3] = { 255, 255, 255 };

            if (texture_data) {
                float tu = (t0.u * l0 + t1.u * l1 + t2.u * l2) * texture_width;
                float tv = (1.0f - (t0.v * l0 + t1.v * l1 + t2.v * l2)) * texture_height;

                int tx = (int)(tu + 0.5f);
                int ty = (int)(tv + 0.5f);

                // проверка границ
                if (tx < 0) tx = 0;
                if (tx >= texture_width) tx = texture_width - 1;
                if (ty < 0) ty = 0;
                if (ty >= texture_height) ty = texture_height - 1;

                const uint8_t* texel = &texture_data[(ty * texture_width + tx) * 3];
                color[0] = (uint8_t)(texel[0] * intensity);
                color[1] = (uint8_t)(texel[1] * intensity);
                color[2] = (uint8_t)(texel[2] * intensity);
            }
            else {
                uint8_t shade = (uint8_t)(255 * intensity);
                color[0] = color[1] = color[2] = shade;
            }

            image_data[index * 3 + 0] = color[0];
            image_data[index * 3 + 1] = color[1];
            image_data[index * 3 + 2] = color[2];
            z_buffer[index] = z;
        }
    }
}

// v вершины vt координаты текстуры f грани
bool parseOBJ(const std::string& file_path, std::vector<Vertex>& vertices, std::vector<TexCoord>& tex_coords, std::vector<Face>& faces) {
    std::ifstream file(file_path);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line.erase(line.find_last_not_of(" \t") + 1);
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vertex v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (prefix == "vt") {
            TexCoord t;
            iss >> t.u >> t.v;
            tex_coords.push_back(t);
        }
        else if (prefix == "f") {
            std::vector<std::string> face_data;
            std::string data;
            while (iss >> data) face_data.push_back(data);

            if (face_data.size() < 3) continue;

            // дробление на треугольники каждой грани
            for (size_t i = 1; i < face_data.size() - 1; ++i) {
                Face f;
                for (int j = 0; j < 3; ++j) {
                    size_t idx = (j == 0) ? 0 : (j == 1 ? i : i + 1);
                    std::replace(face_data[idx].begin(), face_data[idx].end(), '/', ' ');

                    std::istringstream v_iss(face_data[idx]);
                    int vi = 0, ti = 0;
                    v_iss >> vi; v_iss.ignore(); v_iss >> ti;
                    f.v_indices[j] = vi - 1;
                    f.t_indices[j] = ti - 1;
                }
                faces.push_back(f);
            }
        }
    }
    return true;
}

void placeModelOnCanvas(const std::vector<Vertex>& vertices, const std::vector<TexCoord>& tex_coords, const std::vector<Face>& faces,
    uint8_t* image_data, float* z_buffer, int img_width, int img_height,
    const uint8_t* texture_data, int texture_width, int texture_height,
    const ModelTransform& transform) {

    // масшьабирование вершин
    std::vector<Vertex> transformed_vertices = vertices;
    for (size_t i = 0; i < transformed_vertices.size(); ++i) {
        transformed_vertices[i].x *= transform.scale;
        transformed_vertices[i].y *= transform.scale;
        transformed_vertices[i].z *= transform.scale;
    }

    // вращение и перенос
    if (transform.rotation_type == QUATERNION) {
        for (size_t i = 0; i < transformed_vertices.size(); ++i) {
            float in[3] = { transformed_vertices[i].x, transformed_vertices[i].y, transformed_vertices[i].z }, out[3];
            rotateVectorByQuaternion(in, transform.quaternion, out);
            transformed_vertices[i].x = out[0] + transform.tx;
            transformed_vertices[i].y = out[1] + transform.ty;
            transformed_vertices[i].z = out[2] + transform.tz;
        }
    }
    else {
        // матрица вращения для эйлеров углов
        float cx = cos(transform.euler_angles[0]), sx = sin(transform.euler_angles[0]);
        float cy = cos(transform.euler_angles[1]), sy = sin(transform.euler_angles[1]);
        float cz = cos(transform.euler_angles[2]), sz = sin(transform.euler_angles[2]);
        float R[3][3] = {
            { cy * cz, -cy * sz, sy },
            { sx * sy * cz + cx * sz, -sx * sy * sz + cx * cz, -sx * cy },
            { -cx * sy * cz + sx * sz, cx * sy * sz + sx * cz, cx * cy }
        };

        for (size_t i = 0; i < transformed_vertices.size(); ++i) {
            float x = R[0][0] * transformed_vertices[i].x + R[0][1] * transformed_vertices[i].y + R[0][2] * transformed_vertices[i].z + transform.tx;
            float y = R[1][0] * transformed_vertices[i].x + R[1][1] * transformed_vertices[i].y + R[1][2] * transformed_vertices[i].z + transform.ty;
            float z = R[2][0] * transformed_vertices[i].x + R[2][1] * transformed_vertices[i].y + R[2][2] * transformed_vertices[i].z + transform.tz;
            transformed_vertices[i] = { x, y, z };
        }
    }

    // нормали вершин
    std::vector<float> normals(transformed_vertices.size() * 3, 0.0f);
    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& f = faces[i];
        const Vertex& v0 = transformed_vertices[f.v_indices[0]];
        const Vertex& v1 = transformed_vertices[f.v_indices[1]];
        const Vertex& v2 = transformed_vertices[f.v_indices[2]];
        float nx, ny, nz;
        calculateNormal(v0, v1, v2, nx, ny, nz);
        for (int j = 0; j < 3; ++j) {
            normals[f.v_indices[j] * 3 + 0] += nx;
            normals[f.v_indices[j] * 3 + 1] += ny;
            normals[f.v_indices[j] * 3 + 2] += nz;
        }
    }

    // нормализация нормалей
    for (size_t i = 0; i < transformed_vertices.size(); ++i) {
        float& nx = normals[i * 3 + 0], & ny = normals[i * 3 + 1], & nz = normals[i * 3 + 2];
        float len = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (len > 0) {
            nx /= len;
            ny /= len;
            nz /= len;
        }
    }

    // отрисовка граней
    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& f = faces[i];
        const Vertex& v0 = transformed_vertices[f.v_indices[0]];
        const Vertex& v1 = transformed_vertices[f.v_indices[1]];
        const Vertex& v2 = transformed_vertices[f.v_indices[2]];

        const TexCoord& t0 = tex_coords[f.t_indices[0]];
        const TexCoord& t1 = tex_coords[f.t_indices[1]];
        const TexCoord& t2 = tex_coords[f.t_indices[2]];

        float n0[3] = { normals[f.v_indices[0] * 3 + 0], normals[f.v_indices[0] * 3 + 1], normals[f.v_indices[0] * 3 + 2] };
        float n1[3] = { normals[f.v_indices[1] * 3 + 0], normals[f.v_indices[1] * 3 + 1], normals[f.v_indices[1] * 3 + 2] };
        float n2[3] = { normals[f.v_indices[2] * 3 + 0], normals[f.v_indices[2] * 3 + 1], normals[f.v_indices[2] * 3 + 2] };

        drawTriangle(image_data, z_buffer, img_width, img_height, v0, v1, v2, t0, t1, t2, texture_data, texture_width, texture_height, n0, n1, n2);
    }
}


bool loadAndRenderModel(const std::string& obj_path, const std::string& texture_path,uint8_t* image_data, float* z_buffer, int img_width, int img_height,const ModelTransform& transform) {
    std::vector<Vertex> vertices;
    std::vector<TexCoord> tex_coords;
    std::vector<Face> faces;

    if (!parseOBJ(obj_path, vertices, tex_coords, faces)) return false;

    int w, h, ch;
    uint8_t* tex = stbi_load(texture_path.c_str(), &w, &h, &ch, 3);
    if (!tex) return false;

    placeModelOnCanvas(vertices, tex_coords, faces, image_data, z_buffer, img_width, img_height, tex, w, h, transform);
    stbi_image_free(tex);

    return true;
}

int main() {
    uint8_t* img = new uint8_t[IMG_WIDTH * IMG_HEIGHT * 3]();
    float* zbuf = new float[IMG_WIDTH * IMG_HEIGHT];
    std::fill(zbuf, zbuf + IMG_WIDTH * IMG_HEIGHT, std::numeric_limits<float>::infinity());

    ModelTransform bunny = { 0.05f, 0.05f, 10.0f, 0.004f, QUATERNION,
                            {0,0,0},
                            {1.0f, 0.5f, 0.9f, 0.58f} };
    ModelTransform frog = {0.05f, 0.05f, 4.0f, 0.08f, EULER,
                           {M_PI / 3, -M_PI /2, -M_PI / 4},
                           {0,0,0,0} };

    ModelTransform cat = { 0.5f, 0.05f, 6.0f, 0.005f, EULER,
                           {-M_PI / 3, -M_PI / 4 , 0},
                           {0,0,0,0} };

    loadAndRenderModel("models/bunny.obj", "textures/bunny.jpg", img, zbuf, IMG_WIDTH, IMG_HEIGHT, bunny);
    loadAndRenderModel("models/frog.obj", "textures/frog.jpg", img, zbuf, IMG_WIDTH, IMG_HEIGHT, frog);
    loadAndRenderModel("models/cat.obj", "textures/cat.jpg", img, zbuf, IMG_WIDTH, IMG_HEIGHT, cat);

    stbi_flip_vertically_on_write(1);
    stbi_write_png("result.png", IMG_WIDTH, IMG_HEIGHT, 3, img, IMG_WIDTH * 3);
    delete[] img;
    delete[] zbuf;
    return 0;
}
