import cv2
import numpy as np

try:
    from openpose import *
except:
    raise Exception('Error: OpenPose lib could not be found. Enable BUILD_PYTHON in Cmake and add lib path')

def getPoints(img):
    keypoints, output_image = openpose.forward(img, True)
    print (keypoints)
    return keypoints, output_image

def getAngle(vx, vy):
    lx = np.sqrt(x.dot(x))
    ly = np.sqrt(y.dot(y))

    cos = x.dot(y) / (lx * ly)
    rad = np.arccos(cos)

    angle = rad * 180 / np.pi
    return angle

def checkEqual(a, b, RS=0.1):
    if a*(1+RS) > b and a(1-RS) < b:
        return True
    return False

def backStep(step, val, min=0):
    v = step - val
    if v < min: v = min
    return v

def checkPose(img, type, step = None, interval=500, RS=0.1, timeout=5000, param=None):
    points,_ = getPoints(img)
    
    if step is None:
        step = 0
    # only consider one person
    points = points[:1]

    for person in points:
        xs = person[0::3]
        ys = person[1::3]
        ns = []

        # convert points to vector
        for i in range(1, 15):
            ns.append(np.array([xs[i] - xs[i-1], ys[i] - ys[i - 1]))
        
        # check type first
        if type == 1:
            if step < 15000:
                checkAngle = getAngle(ns[7], ns[9])
                if checkEqual(90.0, checkAngle, RS):
                    step += interval
                    return True, "Seems ok", 0, step
                else:
                    step = backStep(step, interval * 2, 0)
                    return False, "Angle is not 90 degree, try again", 1, step
            elif: step < 15000 + 5000:
                checkAngle = getAngle(ns[7], ns[9]):
                if checkEqual(90.0, checkAngle, RS):
                    step += interval
                    return True, "Seems ok", 0, step
                elif:
                    step = backStep(step, interval*2, 15000)
            elif: step < 15000 + 5000 + timeout:
                step += interval
                return True, "Let's try again", 0, step
            else:
                step = 0
                return True, "Let's try again", 0, step
        elif type == 2:
            pass
        elif type == 3:
            pass
        else:
            print('unknow pose type')
            return True, "Unknow pose type" + type.toString(), -1, step