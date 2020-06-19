from Model import model

def process_inputs(limbs):
    print("Printing Limbs:")
    index = 0
    for j in limbs:
        print('Index:', index, '- x:', j[0], '- y:', j[1], '- z:', j[2])
        index += 1

def connect_to_python(limbs):
    process_inputs(limbs)
    return model.predict()