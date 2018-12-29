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
                if checkEqual(60.0, checkAngle, RS):
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
            if step < 15000:
                checkAngle1 = getAngle(ns[7], ns[9])
                checkAngle2 = getAngle(ns[9], ns[10])
                if checkEqual(90.0, checkAngle1, RS) and checkEqual(0, checkAngle2, RS):
                    step += interval
                    return True, "Semms ok", 0, step
                else:
                    step = backStep(step, interval*2, 0)
                    return False, "Angles are not ok", 0, step
            elif step < 15000 + timout:
                step += interval
                return True, "Let's try again", 0, step
        elif type == 3 and param is not None:
            correctFlag = True
            vectorIndex = [
                [0, 7],
                [1, 7],
                [2, 3],
                [7, 9],
                [9, 10]]
            if step < param['stay']:
                for i in range(0,4):
                    if correctFlag == False:
                        break
                    pair = vectorIndex[i]
                    checkAngle = getAngle(ns[pair[0], ns[pair[1]])
                    if checkEqual(param['angle'][i], checkAngle, RS):
                        continue
                    else:
                        step = backStep(step, interval*2, 0)
                        correctFlag = False
                if correctFlag == True:
                    step += interval
                    return True, "Semms ok", 0, step
                else:
                    step = backStep(step, interval*2, 0)
                    return False, "Angles are not ok", 0, step
            elif step < param['stay'] + timeout:
                step += interval
                return True, "Let's try again", 0, step
        else:
            print('unknow pose type')
            return True, "Unknow pose type" + type.toString(), -1, step