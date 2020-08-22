import cv2
import numpy as np
import math

def rotate(origin, point, angle):
    """
    Rotate a point counterclockwise by a given angle around a given origin.

    The angle should be given in radians.
    """
    ox, oy = origin
    px, py = point

    qx = ox + math.cos(angle) * (px - ox) - math.sin(angle) * (py - oy)
    qy = oy + math.sin(angle) * (px - ox) + math.cos(angle) * (py - oy)
    return qx, qy

point = [[250,240],[490,180]]
shot = [400, 400]

img = np.zeros((480, 640,3), np.uint8)

cv2.circle(img, (point[0][0], point[0][1]), 5, (255, 0, 0))
cv2.circle(img, (point[1][0], point[1][1]), 5, (255, 0, 0))
cv2.circle(img, (shot[0], shot[1]), 5, (255, 0, 0))

cv2.imshow("Hello", img)
cv2.waitKey(0)


p1 = [0,5]
p2 = [10,0]
print math.degrees(math.atan((p1[0] - p2[0]) / (p1[1] - p2[1])))

scale = 2

vector = []

mid = [(point[0][0] + point[1][0]) / 2, (point[0][1] + point[1][1]) / 2]

if point[0][1] < point[1][1]:
    vector = [point[0][0] - point[1][0], point[0][1] - point[1][1]]
else:
    vector = [point[1][0] - point[0][0], point[1][1] - point[0][1]]

#angle = math.atan(vector[0] / vector[1])
angle = (np.arctan2(vector[1], vector[0]))
afx, afy = rotate((mid[0], mid[1]), (point[0][0], point[0][1]), -angle)
afxx, afyy = rotate((mid[0], mid[1]),  (point[1][0], point[1][1]), -angle)

fx, fy = rotate((mid[0], mid[1]), (mid[0] + vector[0], mid[1] + vector[1]), -angle)
fxx, fyy = rotate((mid[0], mid[1]), (mid[0] - vector[0], mid[1] - vector[1]), -angle)

sfxx, sfyy = rotate((mid[0], mid[1]), (shot[0], shot[1]), -angle)

print fx, fy
print angle
print vector
print mid

cv2.circle(img, (mid[0] + vector[0], mid[1] + vector[1]), 5, (255, 0, 0))
cv2.circle(img, (mid[0] - vector[0], mid[1] - vector[1]), 5, (255, 0, 0))
cv2.circle(img, (int(fx), int(fy)), 5, (255, 255, 255))
cv2.circle(img, (int(fxx), int(fyy)), 5, (255, 255, 255))
cv2.circle(img, (int(afx), int(fy)), 5, (255, 255, 255))
cv2.circle(img, (int(afxx), int(fyy)), 5, (255, 255, 255))
cv2.circle(img, (int(sfxx), int(sfyy)), 5, (255, 255, 255))

print "fxx:",fxx,"fx",fx
gun_max_width = fx - fxx
print "gmw:", gun_max_width
gun_pos = sfxx - fxx
print "gp:", gun_pos
final_w = (float(gun_pos) / float(gun_max_width)) * 255


gun_max_height = gun_max_width / (16 + 9) * 9
print gun_max_height
gun_pos_h = sfyy - fy
final_h = (float(gun_pos_h) / float(gun_max_height)) * 255

print final_w, final_h
cv2.imshow("Hello", img)
cv2.waitKey(0)
