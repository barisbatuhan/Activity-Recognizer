import numpy as np
from math import atan

from IRS_Detect.Model.utils.distances import *

def input_preperation(C, joint):
  ls_r, ls_ro, ls_z=cart_to_cyl(left_shoulder(C, joint))
  rs_r, rs_ro, rs_z=cart_to_cyl(right_shoulder(C, joint))
  lh_r, lh_ro, lh_z=cart_to_cyl(left_hip(C, joint))
  rh_r, rh_ro, rh_z=cart_to_cyl(right_hip(C, joint))
  return [ls_r, ls_ro, ls_z, rs_r, rs_ro, rs_z, lh_r, lh_ro, lh_z, rh_r, rh_ro, rh_z]

def find_r(cart_array):
    return np.sqrt(cart_array[:,:,0]**2+cart_array[:,:,1]**2) 

def find_ro(cart_array):
    ro = np.zeros(cart_array.shape[:-1])
    for i in range(cart_array.shape[0]):
        for j in range(cart_array.shape[1]):
            if type(atan(cart_array[i,j,1]/cart_array[i,j,0])) == float:
                ro[i,j] = atan(cart_array[i,j,1]/cart_array[i,j,0]) 
    return ro  

def find_z(cart_array):
    return cart_array[:,:,2]

def cart_to_cyl(joint_coordinate):
    r_matrix = find_r(joint_coordinate)
    ro_matrix = find_ro(joint_coordinate)
    z_matrix = find_z(joint_coordinate)
    return r_matrix, ro_matrix, z_matrix