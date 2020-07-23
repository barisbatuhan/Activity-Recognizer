# Frameworks
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

from keras.optimizers import *
from keras.models import Model
from keras.layers import *
from keras.layers.core import *
from tensorflow.keras.callbacks import *
from keras.layers.convolutional import *
from keras import backend as K
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'  # FATAL
os.environ["CUDA_VISIBLE_DEVICES"]="-1"   
import tensorflow as tf
import logging
logging.getLogger('tensorflow').setLevel(logging.FATAL)

# Project methods
from DD_Model.utils.helpers import *

class DD_Model:
    def __init__(self):
        random.seed(1234)
        self.labels = ["Horizontal arm wave", "High arm wave", "Two hand wave", "Catch Cap", "High throw", "Draw X",
          "Draw Tick", "Toss Paper", "Forward Kick", "Side Kick", "Take Umbrella", "Bend", "Hand Clap",
          "Walk", "Phone Call", "Drink", "Sit down", "Stand up"]
        self.labels.sort()
        self.skeletons = []

    def create_model(self):
        M = Input(name='M', shape=(frame_l, feat_d))  
        P = Input(name='P',  shape=(frame_l, joint_n, joint_d)) 
    
        FM = self.build_FM()
        x = FM([M, P])
        x = GlobalMaxPool1D()(x)
        x = self.d1D(x, 128)
        x = Dropout(0.5)(x)
        x = self.d1D(x, 128)
        x = Dropout(0.5)(x)
        x = Dense(clc_num, activation='softmax')(x)
        self.model = Model(inputs=[M, P], outputs=x)
        return self.model

    def load_model(self):
        self.model.load_weights('./DD_Model/stored_models/model.h5')

    def evaluate(self, frames):
        X0, X1 = self.data_generator({"pose": frames})
        predictions = self.model.predict([X0, X1], verbose=0)
        pred_str = []
        for i in predictions:
            if(np.max(i) < 0.4): # a threshold for confidence level
                pred_str.append("Not sure")
            else:
                place_max = np.where(i == np.max(i))
                value_of_max = int(place_max[0][0])
                prediction = self.labels[value_of_max]
                pred_str.append(prediction)
        return pred_str

    # ----------------------------------------------------------------------------------
    # Helper Calculation Methods

    def poses_diff(self, x):
        H, W = x.get_shape()[1], x.get_shape()[2]
        x = tf.subtract(x[:,1:,...],x[:,:-1,...])
        x = tf.compat.v1.image.resize_nearest_neighbor(x, size=[H,W], align_corners=False) # should not alignment here
        return x

    def data_generator(self, T):
        X_0 = []
        X_1 = []
        
        for i in tqdm(range(len(T['pose']))): 
            p = np.copy(T['pose'][i])
            p = zoom(p, target_l=frame_l, joints_num=joint_n, joints_dim=joint_d) 
            M = get_CG(p)
            X_0.append(M)
            X_1.append(p)
        
        X_0 = np.stack(X_0)  
        X_1 = np.stack(X_1) 
        return X_0,X_1

    # ----------------------------------------------------------------------------------
    # Small Network Layer Combinations

    def pose_motion(self, P):
        P_diff_slow = Lambda(lambda x: self.poses_diff(x))(P)
        P_diff_slow = Reshape((frame_l, -1))(P_diff_slow)
        P_fast = Lambda(lambda x: x[:, ::2, ...])(P)
        P_diff_fast = Lambda(lambda x: self.poses_diff(x))(P_fast)
        P_diff_fast = Reshape((int(frame_l/2), -1))(P_diff_fast)
        return P_diff_slow, P_diff_fast
    
    def c1D(self, x, filters, kernel):
        x = Conv1D(filters, kernel_size=kernel, padding='same', use_bias=False)(x)
        x = BatchNormalization()(x)
        x = LeakyReLU(alpha=0.2)(x)
        return x

    def block(self, x, filters):
        x = self.c1D(x, filters, 3)
        x = self.c1D(x, filters, 3)
        return x

    def d1D(self, x, filters):
        x = Dense(filters, use_bias=False)(x)
        x = BatchNormalization()(x)
        x = LeakyReLU(alpha=0.2)(x)
        return x
    
    # ----------------------------------------------------------------------------------
    # A Big Network Flow

    def build_FM(self):   
        M = Input(shape=(frame_l, feat_d))
        P = Input(shape=(frame_l, joint_n, joint_d))

        diff_slow, diff_fast = self.pose_motion(P)

        x = self.c1D(M, filters*2, 1)
        x = SpatialDropout1D(0.1)(x)
        x = self.c1D(x, filters, 3)
        x = SpatialDropout1D(0.1)(x)
        x = self.c1D(x, filters, 1)
        x = MaxPooling1D(2)(x)
        x = SpatialDropout1D(0.1)(x)

        x_d_slow = self.c1D(diff_slow, filters*2, 1)
        x_d_slow = SpatialDropout1D(0.1)(x_d_slow)
        x_d_slow = self.c1D(x_d_slow, filters, 3)
        x_d_slow = SpatialDropout1D(0.1)(x_d_slow)
        x_d_slow = self.c1D(x_d_slow, filters, 1)
        x_d_slow = MaxPool1D(2)(x_d_slow)
        x_d_slow = SpatialDropout1D(0.1)(x_d_slow)

        x_d_fast = self.c1D(diff_fast, filters*2, 1)
        x_d_fast = SpatialDropout1D(0.1)(x_d_fast)
        x_d_fast = self.c1D(x_d_fast, filters, 3) 
        x_d_fast = SpatialDropout1D(0.1)(x_d_fast)
        x_d_fast = self.c1D(x_d_fast, filters, 1) 
        x_d_fast = SpatialDropout1D(0.1)(x_d_fast)
    
        x = concatenate([x, x_d_slow, x_d_fast])
        x = self.block(x, filters*2)
        x = MaxPool1D(2)(x)
        x = SpatialDropout1D(0.1)(x)
    
        x = self.block(x, filters*4)
        x = MaxPool1D(2)(x)
        x = SpatialDropout1D(0.1)(x)

        x = self.block(x, filters*8)
        x = SpatialDropout1D(0.1)(x)

        return Model(inputs=[M, P], outputs=x)