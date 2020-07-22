import numpy as np
import math
import random
import pandas as pd
import os
import matplotlib.pyplot as plt
import cv2
import glob
from tqdm import tqdm
import pickle

import scipy.ndimage.interpolation as inter
from scipy.signal import medfilt 
from scipy.spatial.distance import cdist

frame_l = 50 # the length of frames
joint_n = 18 # the number of joints
joint_d = 3 # the dimension of joints
clc_num = 18 # the number of class
feat_d = 153
filters = 64

# Temple resizing function
def zoom(p,target_l=32,joints_num=18,joints_dim=3):
    l = p.shape[0]
    p_new = np.empty([target_l,joints_num,joints_dim]) 
    for m in range(joints_num):
        for n in range(joints_dim):
            p_new[:,m,n] = medfilt(p_new[:,m,n],3)
            p_new[:,m,n] = inter.zoom(p[:,m,n],target_l/l)[:target_l]         
    return p_new

# Calculate JCD feature
def norm_scale(x):
    if(np.mean(x) == 0):
        return (x-np.mean(x))
    else:
        return (x-np.mean(x))/np.mean(x)
  
def get_CG(p):
    M = []
    iu = np.triu_indices(joint_n,1,joint_n)
    for f in range(frame_l): 
        d_m = cdist(p[f],p[f],'euclidean')       
        d_m = d_m[iu] 
        M.append(d_m)
    M = np.stack(M) 
    M = norm_scale(M)
    return M