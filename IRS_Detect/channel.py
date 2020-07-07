from Model.model import Model

def load_model(path):
    print("Inside model load...")
    return Model(path)

def predict(obj, inputs):
    print("Inside channel predict...")
    return obj.predict(inputs)