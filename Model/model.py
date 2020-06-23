class Model:
    def __init__(self, name):
        self.name = name
        print("Model created:", self.name)
    
    def predict(self, arr_of_limbs):
        print("Entered to predict area!")
        for i in arr_of_limbs:
            print("Limb:", i)
        return "standing"
    