from Model.model import Act_Model

def load_model():
    model = Act_Model()
    model.create_model()
    model.load_model()
    return model

def add_skel_data(model, frame_data):
    if(len(model.skeletons) < 40):
        model.skeletons.append(frame_data)
    else:
        for i in range(1, len(model.skeletons)):
            model.skeletons[i - 1] = model.skeletons[i]
            model.skeletons[-1] = frame_data
    return 'SUCCESS'

def predict(model):
    return model.evaluate(model.skeletons)