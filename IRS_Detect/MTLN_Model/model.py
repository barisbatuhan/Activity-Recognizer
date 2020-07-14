# Network packages
import tensorflow as tf
import numpy as np
from keras.applications import VGG19
from keras.models import Model
from keras.layers import *
from keras.optimizers import Adam
from tensorflow.keras.callbacks import *
# Project Packages
from MTLN_Model.utils.helpers import *


class MTLN_Model:
    def __init__(self):
        self.power = 5
        self.lr = (3*self.power)*1e-6
        self.losses = {
            "x1": "categorical_crossentropy",
            "x2": "categorical_crossentropy",
            "x3": "categorical_crossentropy",
            "x4": "categorical_crossentropy"
        }
        self.skeletons = []
        self.lossWeights = {"x1": 0.25, "x2": 0.25, "x3": 0.25, "x4": 0.25}
        self.metric = {
            "x1": tf.keras.metrics.CategoricalAccuracy(),
            "x2": tf.keras.metrics.CategoricalAccuracy(),
            "x3": tf.keras.metrics.CategoricalAccuracy(),
            "x4": tf.keras.metrics.CategoricalAccuracy()
        }
        self.labels = ["Horizontal arm wave", "High arm wave", "Two hand wave", "Catch Cap", "High throw", "Draw X",
        "Draw Tick", "Toss Paper", "Forward Kick", "Side Kick", "Take Umbrella", "Bend", "Hand Clap",
        "Walk", "Phone Call", "Drink", "Sit down", "Stand up"]
        self.labels.sort()

    def create_model(self):
        input1 = Input(shape=(21504,), name="input1")
        input2 = Input(shape=(21504,), name="input2")
        input3 = Input(shape=(21504,), name="input3")
        input4 = Input(shape=(21504,), name="input4")

        x1 = Dense(512,activation="relu")(input1)
        x2 = Dense(512,activation="relu")(input2)
        x3 = Dense(512,activation="relu")(input3)
        x4 = Dense(512,activation="relu")(input4)
        x5 = Add()([x1,x2,x3,x4])
        x5 = Dense(clc_num, activation="softmax",name="x5")(x5)
        self.model = Model(inputs=[input1, input2, input3, input4], outputs= x5)  

        base_model = VGG19(weights='imagenet',include_top =False, input_tensor=Input(shape=(224, 224, 3)))
        base_model.trainable = False
        self.model_VGG19 = Model(inputs=base_model.input, outputs=base_model.get_layer('block5_conv1').output)


    def load_model(self):
        self.model.load_weights('./MTLN_Model/stored_models/model.h5')
        print("[INFO][PYTHON] Model weights are loaded.")

    def save_model(self):
        self.model.save_weights("./MTLN_Model/stored_models/model.h5")

    def evaluate(self, frames):
        in1, in2, in3, in4 = self.data_generator(frames)
        predictions = self.model.predict({"input1":in1, "input2":in2, "input3":in3, "input4":in4})
        last_pred = None
        for i in predictions:
            place_max = np.where(i == np.max(i))
            value_of_max = int(place_max[0][0])
            prediction = self.labels[value_of_max]
            last_pred = prediction
            print("Prediction: ", prediction, value_of_max)
        return last_pred

    def resize_for_network(self, joint):
        list_of_all = input_preperation(joint)
        new_list = list()
        for i in list_of_all:
            new_picture = np.resize(i, (224, 224))
            new_picture = np.expand_dims(new_picture, axis=2)
            new_picture = np.concatenate(
                (new_picture, new_picture, new_picture), axis=2)
            new_list.append(new_picture)
        return new_list

    def temp_mean_pool(self, output):
        shape = output.shape
        new_array = np.zeros(shape[-2]*shape[-1])
        for i in range(shape[-2]-1):
            for j in range(shape[-1]-1):
                y = np.sum(output[0, :, i, j])
                new_array[shape[-1]*i+j] = y/shape[1]
        return new_array

    def data_generator(self, data):
        input1_list = list()
        input2_list = list()
        input3_list = list()
        input4_list = list()
        list_of_all = self.resize_for_network(np.array(data))     
        print("[INFO][PYTHON] Resizing the data for the network is finished")   
        processed_data = list()
        for j in list_of_all:
            output = self.model_VGG19.predict(np.expand_dims(j, axis=0))
            processed_data.append(self.temp_mean_pool(output))
        print("[INFO][PYTHON] VGG19 Prediction is finished")
        
        part_1 = np.concatenate(
            (processed_data[0], processed_data[1], processed_data[2]), axis=0)
        part_2 = np.concatenate(
            (processed_data[3], processed_data[4], processed_data[5]), axis=0)
        part_3 = np.concatenate(
            (processed_data[6], processed_data[7], processed_data[8]), axis=0)
        part_4 = np.concatenate(
            (processed_data[9], processed_data[10], processed_data[11]), axis=0)
        input_data = [part_1, part_2, part_3, part_4]
        
        input1_list.append(input_data[0])
        input2_list.append(input_data[1])
        input3_list.append(input_data[2])
        input4_list.append(input_data[3])


        # for i in range(len(data)):     
        #     list_of_all = self.resize_for_network(data[i])        
        #     processed_data = list()
        #     for j in list_of_all:
        #         output = self.base_model.predict(np.expand_dims(j, axis=0))
        #         processed_data.append(self.temp_mean_pool(output))
            
        #     part_1 = np.concatenate(
        #         (processed_data[0], processed_data[1], processed_data[2]), axis=0)
        #     part_2 = np.concatenate(
        #         (processed_data[3], processed_data[4], processed_data[5]), axis=0)
        #     part_3 = np.concatenate(
        #         (processed_data[6], processed_data[7], processed_data[8]), axis=0)
        #     part_4 = np.concatenate(
        #         (processed_data[9], processed_data[10], processed_data[11]), axis=0)
        #     input_data = [part_1, part_2, part_3, part_4]
            
        #     input1_list.append(input_data[0])
        #     input2_list.append(input_data[1])
        #     input3_list.append(input_data[2])
        #     input4_list.append(input_data[3])
  
        input1_list = np.stack(input1_list)
        input2_list = np.stack(input2_list)
        input3_list = np.stack(input3_list)
        input4_list = np.stack(input4_list)
        
        return input1_list, input2_list, input3_list, input4_list


