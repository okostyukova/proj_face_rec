import numpy as np
from keras.models import model_from_json
from keras.preprocessing import image
import argparse as ar

parser = ar.ArgumentParser()
parser.add_argument('path', metavar='path', type=str, nargs='+', help='an integer for the accumulator')
#img_path = "C:/Users/FLUX PAVILLION/Pictures/oohoo/1.jpg"  
img_path = parser.parse_args().path[0]
img = image.load_img(img_path, target_size=(150, 150))

#print("blah")

x = image.img_to_array(img)
x /= 255
x = np.expand_dims(x, axis=0)

json_file_v = open("D:/c++/testOpenCV/affect_model_v.json", "r")
json_file_a = open("D:/c++/testOpenCV/affect_model_a.json", "r")

loaded_model_json_v = json_file_v.read()
loaded_model_json_a = json_file_a.read()

json_file_v.close()
json_file_a.close()

loaded_model_v = model_from_json(loaded_model_json_v)
loaded_model_a = model_from_json(loaded_model_json_a)

loaded_model_v.load_weights("D:/c++/testOpenCV/affect_model_v.h5")
loaded_model_a.load_weights("D:/c++/testOpenCV/affect_model_a.h5")

loaded_model_v.compile(loss='mean_squared_error', optimizer='sgd')
loaded_model_a.compile(loss='mean_squared_error', optimizer='sgd')

prediction_v = loaded_model_v.predict(x)
prediction_a = loaded_model_a.predict(x)

if prediction_v >= -0.5 and prediction_v <= 0.5 and prediction_a <= -0.66:
    print("focus")
elif prediction_v >= -0.2 and prediction_a >= -0.66 and prediction_a <= 0:
    print("relaxation")
elif prediction_v <= 0 and prediction_a <= 0:
    print("stress")
elif prediction_v >= 0 and prediction_v <= 0.4 and prediction_a >= 0 and prediction_a <= 0.4:
    print("interest")
elif (prediction_v >= 0 and prediction_v <= 0.7 and prediction_a >= 0.4 and prediction_a <= 0.7 or
      prediction_v >= 0.4 and prediction_v <= 0.7 and prediction_a >= 0 and prediction_a <= 0.4 or
      prediction_v >= 0.7 and prediction_v <= 1 and prediction_a >= 0 and prediction_a <= 1):
    print("engagement")
elif prediction_v >= 0 and prediction_v <= 1 and prediction_a >= 0.7 and prediction_a <= 1:
    print("excitement")
else:
    print("Cannot determine emotion")
