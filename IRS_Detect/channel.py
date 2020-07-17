from MTLN_Model.model import MTLN_Model
from DD_Model.model import DD_Model

import numpy as np

def load_model():
    # change this line for changing the model: MTLN or DD
    selected_model = "DD"
    model = None
    if(selected_model == "MTLN"):
        model = MTLN_Model()
    elif(selected_model == "DD"):
        model = DD_Model()
    model.create_model()
    model.load_model()
    return model

def add_skel_data(model, frame_data, frame_idx):
    if(frame_idx == 0):
        model.skeletons = np.zeros((len(frame_data), 32, 18, 3))
    if(frame_idx < 32):
        for i in range(len(model.skeletons)):
            model.skeletons[i][frame_idx] = np.array(frame_data[i])
    else:
        for i in range(1, 32):
            model.skeletons[:, i - 1] = model.skeletons[:, i]
            model.skeletons[:, -1] = frame_data
    return 'SUCCESS'

def predict(model):
    pred_res = model.evaluate(model.skeletons)
    print("Results:", pred_res)
    return pred_res