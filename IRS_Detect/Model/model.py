# Network packages
import tensorflow as tf
from keras.applications import VGG19
from keras.models import Model
from keras.layers import *
from keras.optimizers import Adam
from tensorflow.keras.callbacks import *
# Project Packages
from IRS_Detect.Model.utils.config import Config

class Model:
    def __init__(self):
        self.power = 5
        self.lr = (3*self.power)*1e-6
        self.losses = {
            "x1" : "categorical_crossentropy",
            "x2" : "categorical_crossentropy",
            "x3" : "categorical_crossentropy",
            "x4" : "categorical_crossentropy"
        }
        self.C = Config()
        self.lossWeights = {"x1" : 0.25, "x2" : 0.25, "x3" : 0.25, "x4" : 0.25}
        self.metric = {
            "x1" : tf.keras.metrics.CategoricalAccuracy(),
            "x2" : tf.keras.metrics.CategoricalAccuracy(),
            "x3" : tf.keras.metrics.CategoricalAccuracy(),
            "x4" : tf.keras.metrics.CategoricalAccuracy()
        }
        base = VGG19(weights='imagenet',include_top =False, input_tensor=Input(shape=(224, 224, 3)))
        base.trainable = False
        self.base_model = Model(inputs=base.input, outputs=base.get_layer('block5_conv1').output)
    
    def create_model(self):
        input1 = Input(shape=(360,), name="input1")
        input2 = Input(shape=(360,), name="input2")
        input3 = Input(shape=(360,), name="input3")
        input4 = Input(shape=(360,), name="input4")
  
        x1 = Dense(512,activation="relu")(input1)
        x1 = Dense(self.C.clc_num, activation="softmax",name="x1")(x1)

        x2 = Dense(512,activation="relu")(input2)
        x2 = Dense(self.C.clc_num, activation="softmax",name="x2")(x2)

        x3 = Dense(512,activation="relu")(input3)
        x3 = Dense(self.C.clc_num, activation="softmax",name="x3")(x3)

        x4 = Dense(512,activation="relu")(input4)
        x4 = Dense(self.C.clc_num, activation="softmax",name="x4")(x4)

        self.model = Model(inputs=[input1, input2, input3, input4], outputs= [x1, x2, x3, x4])

    def load_model(self):
        self.model.load_weights('./stored_models/model.h5')
        self.model.compile(loss=losses, loss_weights=lossWeights,  optimizer=Adam(lr) ,metrics=["accuracy", "accuracy", "accuracy", "accuracy"])

    def save_model(self):
        self.model.save_weights("./stored_models/model.h5")

    def evaluate(self, frames):
        labels = ["Horizontal arm wave", "High arm wave", "Two hand wave", "Catch Cap", "High throw", "Draw X",
          "Draw Tick", "Toss Paper", "Forward Kick", "Side Kick", "Take Umbrella", "Bend", "Hand Clap",
          "Walk", "Phone Call", "Drink", "Sit down", "Stand up"]
        pass


