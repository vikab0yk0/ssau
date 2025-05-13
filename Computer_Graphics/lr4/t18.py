import numpy as np
from PIL import Image, ImageOps


SIZE = 2000
img_mat = np.zeros((SIZE, SIZE, 3), dtype=np.uint8)
z_buffer = np.zeros((SIZE, SIZE), dtype=np.float16)
z_buffer[:] = np.inf
angles = [0, -45, 0]
shift = [0, 0, 1]

ax = 8000
ay = 7000
_u0 = _v0 = SIZE // 2


texture_img = ImageOps.flip(Image.open("bunny-atlas.jpg"))
texture = np.array(texture_img)
WT, HT = texture_img.size


def barycentric_coordinates(point0, point1, point2, x, y):
    (x0, y0), (x1, y1), (x2, y2) = point0, point1, point2
    lambda0 = ((x - x2) * (y1 - y2) - (x1 - x2) * (y - y2)) / ((x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2))
    lambda1 = ((x0 - x2) * (y - y2) - (x - x2) * (y0 - y2)) / ((x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2))
    lambda2 = 1.0 - lambda0 - lambda1
    return lambda0, lambda1, lambda2


def normal(point0, point1, point2):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = point0, point1, point2
    v1 = np.array([x1 - x0, y1 - y0, z1 - z0])
    v2 = np.array([x2 - x0, y2 - y0, z2 - z0])
    n = np.cross(v1, v2)
    norm = np.linalg.norm(n)
    if norm < 1e-6:
        return n
    return n / norm


def calc_light(normal_vec):
    l = np.array([0, 0, 1])
    return np.dot(normal_vec, l)


def draw_triangle(point0, point1, point2, norm0, norm1, norm2, uv0, uv1, uv2, image):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = point0, point1, point2

    # Проективное преобразование
    px = [
        ax * x0 / z0 + _u0,
        ax * x1 / z1 + _u0,
        ax * x2 / z2 + _u0
    ]
    py = [
        ay * y0 / z0 + _v0,
        ay * y1 / z1 + _v0,
        ay * y2 / z2 + _v0
    ]

    (u0, v0), (u1, v1), (u2, v2) = uv0, uv1, uv2

    xmin, ymin = max(int(np.floor(min(px))), 0), max(int(np.floor(min(py))), 0)
    xmax, ymax = min(int(np.ceil(max(px))), SIZE-1), min(int(np.ceil(max(py))), SIZE-1)

    for x in range(xmin, xmax + 1):
        for y in range(ymin, ymax + 1):
            l0, l1, l2 = barycentric_coordinates([px[0], py[0]], [px[1], py[1]], [px[2], py[2]], x, y)
            if all(l >= 0 for l in (l0, l1, l2)):
                z = l0 * z0 + l1 * z1 + l2 * z2
                if z < z_buffer[x, y]:
                    interp_normal = l0 * norm0 + l1 * norm1 + l2 * norm2
                    light = calc_light(interp_normal)

                    u = int(WT * (l0 * u0 + l1 * u1 + l2 * u2))
                    v = int(HT * (l0 * v0 + l1 * v1 + l2 * v2))
                    color = texture[v, u]

                    if light < 0:
                        image[int(y), int(x)] = color * (-light)
                        z_buffer[x, y] = z


def twist(point, angles, shift):
    a, b, c = np.radians(angles)
    m1 = np.array([
        [1, 0, 0],
        [0, np.cos(a), np.sin(a)],
        [0, -np.sin(a), np.cos(a)]
    ])
    m2 = np.array([
        [np.cos(b), 0, np.sin(b)],
        [0, 1, 0],
        [-np.sin(b), 0, np.cos(b)]
    ])
    m3 = np.array([
        [np.cos(c), np.sin(c), 0],
        [-np.sin(c), np.cos(c), 0],
        [0, 0, 1],
    ])
    R = np.dot(np.dot(m1, m2), m3)
    return list(np.dot(R, np.array(point)) + np.array(shift))


with open('../model_1.obj') as file:
    v = []
    f = []
    vt = []
    f_vt = []
    for line in file:
        s = line.split()
        if len(s)>0 and s[0] == 'v':
            v.append([float(s[1]), float(s[2]), float(s[3])])
        if len(s)>0 and s[0] == 'vt':
            vt.append([float(s[1]), float(s[2])])
        if len(s)>0 and s[0] == 'f':
            f.append([int(s[1].split('/')[0]), int(s[2].split('/')[0]), int(s[3].split('/')[0])])
            f_vt.append([int(s[1].split('/')[1]), int(s[2].split('/')[1]), int(s[3].split('/')[1])])

# Поворот всех точек однократно
for i in range(len(v)):
    x, y, z = v[i]
    v[i] = twist([x, y, z], angles, shift)

# вычисление нормалей в вершинах
vn_calc = np.zeros((len(v), 3))
for face in f:
    point0 = v[face[0] - 1]
    point1 = v[face[1] - 1]
    point2 = v[face[2] - 1]
    n = normal(point0, point1, point2)

    vn_calc[face[0] - 1] += n
    vn_calc[face[1] - 1] += n
    vn_calc[face[2] - 1] += n

# Нормализация нормалей
for i in range(len(vn_calc)):
    norm = np.linalg.norm(vn_calc[i])
    if norm > 1e-6:
        vn_calc[i] /= norm

for face, face_vt in zip(f, f_vt):
    points = []
    norms = []
    uvs = []
    for j in range(3):
        x, y, z = v[face[j] - 1]
        points.append([x, y, z])
        norms.append(vn_calc[face[j] - 1])
        uvs.append(vt[face_vt[j] - 1])

    triangle_normal = normal(points[0], points[1], points[2])
    if triangle_normal[2] < 0:
        draw_triangle(points[0], points[1], points[2], norms[0], norms[1], norms[2], uvs[0], uvs[1], uvs[2], img_mat)

img = Image.fromarray(img_mat, mode='RGB')
img = ImageOps.flip(img)
img.save('../images/textured_shading.png')
