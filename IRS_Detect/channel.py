from DD_Model.model import DD_Model
from DD_Model.utils.helpers import *

import numpy as np

def load_model():
    model = DD_Model()
    model.create_model()
    model.load_model()
    return model

def add_skel_data(model, frame_data, frame_idx):
    if(frame_idx == 0):
        model.skeletons = np.zeros((len(frame_data), frame_l, joint_n, joint_d))
    if(frame_idx < frame_l):
        for i in range(len(model.skeletons)):
            model.skeletons[i][frame_idx] = np.array(frame_data[i])
    else:
        for i in range(1, frame_l):
            model.skeletons[:, i - 1] = model.skeletons[:, i]
        for el in range(len(frame_data)):
            model.skeletons[el][-1] = frame_data[el]
    return 'SUCCESS'

def predict(model):
    pred_res = model.evaluate(model.skeletons)
    # print("Results:", pred_res)
    return pred_res