import requests
# Testing the model
import tensorflow as tf
import os
import numpy as np


load_model = tf.keras.models.load_model
ku = tf.keras.utils
# "C:\Users\firas\OneDrive\Desktop\Projet_Tutore_3eme\garbage-classification\Model_Testing\plastic7.jpg"
new_model = load_model(os.path.join('models', 'C:\\Users\\firas\\OneDrive\\Desktop\\Projet_Tutore_3eme\\garbage-classification\\models\\zebla.keras'))
new_model.compile(optimizer='adam', loss='categorical_crossentropy')
number_to_class = ['glass',\
                'paper',\
                'plastic',\
                'trash',]

test_img = 'C:\\Users\\firas\\OneDrive\\Desktop\\Projet_Tutore_3eme\\Code\\esp32_cam_flask\\flask_app\\esp32_imgs\\plastic7.jpg'
img = ku.load_img(test_img, target_size = (32,32))
img = ku.img_to_array(img, dtype=np.uint8)
img = np.array(img)/255.0
prediction = new_model.predict(img[np.newaxis, ...])

#print("Predicted shape",p.shape)
print("Probability:",np.max(prediction[0], axis=-1))
predicted_class = number_to_class[np.argmax(prediction[0], axis=-1)]
print("Classified:",predicted_class,'\n')

# plt.axis('off')
# plt.imshow(img.squeeze())
# plt.title("Loaded Image")   

sendedValue = "trash"

# Replace with your ESP32's IP address
esp32_ip = "http://192.168.249.109/send"
# float_value = "Firas Hedfi"  # Example float from your CNN model

# Send the data to the ESP32
response = requests.get(esp32_ip, params={"value": sendedValue})
print("ESP32 Response:", response.text)
