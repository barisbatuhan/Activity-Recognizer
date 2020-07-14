import numpy as np
from math import atan
     
joint_n = 18 # the number of joints
joint_d = 3 # the dimension of joints
clc_num = 18 # the number of class
l_shoulder = 5 # 5 place number
r_shoulder = 2 # 2 place number
l_hip = 11 # 11 place number
r_hip = 8 # 8 place number

def input_preperation(joint):
    ls_r, ls_ro, ls_z=cart_to_cyl(left_shoulder(joint))
    rs_r, rs_ro, rs_z=cart_to_cyl(right_shoulder(joint))
    lh_r, lh_ro, lh_z=cart_to_cyl(left_hip(joint))
    rh_r, rh_ro, rh_z=cart_to_cyl(right_hip(joint))
    return [ls_r, ls_ro, ls_z, rs_r, rs_ro, rs_z, lh_r, lh_ro, lh_z, rh_r, rh_ro, rh_z]

def find_r(cart_array):
    return np.sqrt(cart_array[:,:,0]**2+cart_array[:,:,1]**2) 

def find_ro(cart_array):
    ro = np.zeros(cart_array.shape[:-1])
    for i in range(cart_array.shape[0]):
        for j in range(cart_array.shape[1]):
            if cart_array[i,j,0] != 0 and type(atan(cart_array[i,j,1]/cart_array[i,j,0])) == float:
                ro[i,j] = atan(cart_array[i,j,1]/cart_array[i,j,0]) 
    return ro  

def find_z(cart_array):
    return cart_array[:,:,2]

def cart_to_cyl(joint_coordinate):
    r_matrix = find_r(joint_coordinate)
    ro_matrix = find_ro(joint_coordinate)
    z_matrix = find_z(joint_coordinate)
    return r_matrix, ro_matrix, z_matrix

def left_shoulder(cart_array):
    reference_joint = cart_array[:, l_shoulder, :]
    shape = cart_array.shape
    array = np.zeros((shape[0], shape[1]-1, shape[2]))
    k = 0
    for i in range(shape[1]):
        if i != l_shoulder:
            array[:, k, :] = cart_array[:, i, :] - reference_joint
            k += 1
    return array


def right_shoulder(cart_array):
    reference_joint = cart_array[:, r_shoulder, :]
    shape = cart_array.shape
    array = np.zeros((shape[0], shape[1]-1, shape[2]))
    k = 0
    for i in range(shape[1]):
        if i != r_shoulder:
            array[:, k, :] = cart_array[:, i, :] - reference_joint
            k += 1
    return array


def left_hip(cart_array):
    reference_joint = cart_array[:, l_hip, :]
    shape = cart_array.shape
    array = np.zeros((shape[0], shape[1]-1, shape[2]))
    k = 0
    for i in range(shape[1]):
        if i != l_hip:
            array[:, k, :] = cart_array[:, i, :] - reference_joint
            k += 1
    return array


def right_hip(cart_array):
    reference_joint = cart_array[:, r_hip, :]
    shape = cart_array.shape
    array = np.zeros((shape[0], shape[1]-1, shape[2]))
    k = 0
    for i in range(shape[1]):
        if i != r_hip:
            array[:, k, :] = cart_array[:, i, :] - reference_joint
            k += 1
    return array
