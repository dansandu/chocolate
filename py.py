import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

vertices = np.array([
    40.0, 10.0, -3.0,
    10.0, 190.0, -1.0,
    190.0, 190.0, -1.0,
    160.0, 10.0, -3.0,
]).reshape((-1, 3))


triangles = np.array([
    0, 1, 2,
    0, 2, 3,
]).reshape((-1, 3))


texture_mapping = np.array([
    0.0, 0.0,
    0.0, 99.0,
    99.0, 99.0,
    99.0, 0.0,
]).reshape((-1, 2))


def interpolate(x, y, triangle):
    a = vertices[triangles[triangle, 0]]
    b = vertices[triangles[triangle, 1]]
    c = vertices[triangles[triangle, 2]]
    n = np.cross(b - a, c - a)
    if n[2] > 1.0e-6:
        z = (n[0] * (x - a[0]) + n[1] * (y - a[1])) / -n[2] + a[2]
    elif b[0] - a[0] > 1.0e-6:
        z = (b[2] - a[2]) * (x - a[0]) / (b[0] - a[0]) + a[2]
    elif b[1] - a[1] > 1.0e-6:
        z = (b[2] - a[2]) * (y - a[1]) / (b[1] - a[1]) + a[2]
    else:
        z = np.min([a[2], b[2], c[2]])
    return np.array([x, y, z])


M_inv = [
    np.linalg.pinv(vertices[triangles[0]].T),
    np.linalg.pinv(vertices[triangles[1]].T)
] 


texture = mpimg.imread('resources/dansandu/chocolate/simple_texture.bmp')
texture = texture / 255.0
# plot = plt.imshow(texture)
# plt.show()

width = 200
height = 200

screen = np.zeros((width, height, 3))

width_begin = 0
height_begin = 0
for x in range(width_begin, width):
    for y in range(height_begin, height):
        def drawTriangle(triangle):
            p = interpolate(float(x), float(y), triangle)
            [u, v, w] = M_inv[triangle] @ p
            if 0.0 <= u <= 1.0 and 0.0 <= v <= 1.0 and 0.0 <= w <= 1.0:
                a = vertices[triangles[triangle, 0]]
                b = vertices[triangles[triangle, 1]]
                c = vertices[triangles[triangle, 2]]
                
                ta = texture_mapping[triangles[triangle, 0]]
                tb = texture_mapping[triangles[triangle, 1]]
                tc = texture_mapping[triangles[triangle, 2]]

                up = u / a[2]
                vp = v / b[2]
                wp = w / c[2]

                tp = np.round((up * ta + vp * tb + wp * tc) / (up + vp + wp)).astype(int)

                assert 0 <= tp[0] <= 99 and 0 <= tp[1] <= 99, f"({tp[0]}, {tp[1]})"

                screen[y, x] = texture[tp[0], tp[1]]
            
        
        drawTriangle(1)
        drawTriangle(0)

plot = plt.imshow(screen)
plt.show()
