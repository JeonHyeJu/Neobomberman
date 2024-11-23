import numpy as np
import matplotlib.pyplot as plt
from sympy import symbols, solve

def main():
    TILE_SIZE = (416, 352)
    HALF_SIZE = (TILE_SIZE[0] / 2, TILE_SIZE[1] / 2)
    X = 128
    Y = 64

    xs = np.arange(-X, X+1, step=1)
    a = X
    b = Y

    plt.figure(figsize=(6,6))
    pts_u = []
    pts_l = []
    for x in xs:
        y = symbols('y', real=True)
        eq = x**2 / a**2 + y**2 / b**2 - 1
        sol = solve(eq)
        for s in sol:
            s = round(s)
            pt = (x, s)
            c = 'r'
            if s > 0:
                pts_u.append(s)
            elif s < 0:
                pts_l.append(s)
                c = 'b'
            else:
                pts_u.append(s)
                pts_l.append(s)
            plt.scatter(pt[0], pt[1], s=5, c=c)

    plt.title("{}, {}".format(len(pts_u), len(pts_l)))
    plt.ylim(-HALF_SIZE[1], HALF_SIZE[1])
    plt.xlim(-HALF_SIZE[0], HALF_SIZE[0])
    plt.grid(True)
    plt.show()
    plt.close()

    pts = pts_u + pts_l[1:-1]     # remove duplicated 0
    with open("ellipse_pts.txt", 'w') as f:
        f.write(str(pts))

def compare():
    with open("ellipse_y_pts.txt", 'r') as f:
        str1 = f.read()
    with open("ellipse_pts.txt", 'r') as f:
        str2 = f.read()

    for i in range(len(str1)):
        if str1[i] != str2[i]:
            print("It's different!")
            break

def linspace():
    PATH = "ellipse_pts.txt"
    arr = []
    with open(PATH, 'r') as f:
        arrStrs = f.read()[1:-1].replace(" ", "").split(",")
        for s in arrStrs:
            arr.append(int(s))

    prevY = arr[0]

    arr_x = np.array([1])
    arr_y = np.array([prevY])

    is_reversed = False
    _size = len(arr)
    for i in range(1, _size):
        curX = i
        curY = arr[i]
        if prevY >= 0 and curY < 0:
            print(curX, _size)
            is_reversed = True
        if abs(curY - prevY) != 1:
            if curY > prevY:    # 0, 8
                _y = np.arange(prevY+1, curY)
            else:   # 0, -8 .. 8, 4
                _y = np.arange(curY+1, prevY)[::-1]
            
            if is_reversed:
                _x = np.full_like(_y, _size - curX)
            else:
                _x = np.full_like(_y, curX)
            arr_x = np.concatenate((arr_x, _x))
            arr_y = np.concatenate((arr_y, _y))
        if curY != prevY:
            prevY = curY

        if is_reversed:
            arr_x = np.append(arr_x, _size - curX)
        else:
            arr_x = np.append(arr_x, curX)
        arr_y = np.append(arr_y, curY)

    print(arr_x.shape, arr_y.shape)

    plt.figure(figsize=(6,4))
    for i in range(arr_y.shape[0]):
        plt.scatter(arr_x[i], arr_y[i], s=5)
    plt.show()
    plt.close()

    # interval
    arr_x_r = np.array([])
    arr_y_r = np.array([])
    for i in range(arr_y.shape[0]):
        if i % 4 == 0:
            arr_x_r = np.append(arr_x_r, arr_x[i])
            arr_y_r = np.append(arr_y_r, arr_y[i])

    print(arr_x_r.shape, arr_y_r.shape)

    max_val = np.max(arr_x_r)
    arr_x_r -= max_val * .5

    max_val = np.max(arr_y_r)
    arr_y_r -= max_val

    plt.figure(figsize=(6,4))
    for i in range(arr_y_r.shape[0]):
        plt.scatter(arr_x_r[i], arr_y_r[i], s=5)
    plt.show()
    plt.close()

    arr_x_r = arr_x_r.astype(int).tolist()
    arr_y_r = arr_y_r.astype(int).tolist()

    with open("ellipse_x_pts.txt", 'w') as f:
        f.write(str(arr_x_r))
    with open("ellipse_y_pts.txt", 'w') as f:
        f.write(str(arr_y_r))

if __name__ == "__main__":
    # main()
    # compare()
    linspace()