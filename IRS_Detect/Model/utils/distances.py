import numpy as np

def left_shoulder(C, cart_array):
    reference_joint = cart_array[:, C.l_shoulder, :]
    shape = cart_array.shape
    array = np.zeros((shape[0], shape[1]-1, shape[2]))
    k = 0
    for i in range(shape[1]):
        if i != C.l_shoulder:
            array[:, k, :] = cart_array[:, i, :] - reference_joint
            k += 1
    return array


def right_shoulder(C, cart_array):
    reference_joint = cart_array[:, C.r_shoulder, :]
    shape = cart_array.shape
    array = np.zeros((shape[0], shape[1]-1, shape[2]))
    k = 0
    for i in range(shape[1]):
        if i != C.r_shoulder:
            array[:, k, :] = cart_array[:, i, :] - reference_joint
            k += 1
    return array


def left_hip(C, cart_array):
    reference_joint = cart_array[:, C.l_hip, :]
    shape = cart_array.shape
    array = np.zeros((shape[0], shape[1]-1, shape[2]))
    k = 0
    for i in range(shape[1]):
        if i != C.l_hip:
            array[:, k, :] = cart_array[:, i, :] - reference_joint
            k += 1
    return array


def right_hip(C, cart_array):
    reference_joint = cart_array[:, C.r_hip, :]
    shape = cart_array.shape
    array = np.zeros((shape[0], shape[1]-1, shape[2]))
    k = 0
    for i in range(shape[1]):
        if i != C.r_hip:
            array[:, k, :] = cart_array[:, i, :] - reference_joint
            k += 1
    return array
