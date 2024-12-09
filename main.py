from flask import Flask, request
import tensorflow as tf
import numpy as np
import cv2
import os
import requests

# Initialize Flask app
app = Flask(__name__)
app.secret_key = 'sdfhj43uop23opjuhjg234jghds8'

# Directory to save images
img_dir = 'esp32_imgs'
if not os.path.isdir(img_dir):
    os.mkdir(img_dir)

# Load the model
model_path = os.path.join('models', 'C:\\Users\\firas\\OneDrive\\Desktop\\Projet_Tutore_3eme\\garbage-classification\\models\\zebla.keras'
)
new_model = tf.keras.models.load_model(model_path)
new_model.compile(optimizer='adam', loss='categorical_crossentropy')

# Mapping predictions to classes
number_to_class = ['glass', 'paper', 'plastic', 'trash']

# ESP32 IP address
esp32_ip = "http://192.168.249.109/send"

# Function to preprocess image for prediction
def preprocess_img(img_path):
    img = tf.keras.utils.load_img(img_path, target_size=(32, 32))
    img = tf.keras.utils.img_to_array(img, dtype=np.uint8)
    img = np.array(img) / 255.0
    return img[np.newaxis, ...]

# Function to predict class
def predict_class(img_path):
    img = preprocess_img(img_path)
    prediction = new_model.predict(img)
    predicted_class = number_to_class[np.argmax(prediction[0])]
    return predicted_class

# Function to save the image
def save_img(img, filename="uploaded_img.jpg"):
    """
    Saves the given image to the specified directory with a default name.
    """
    img_path = os.path.join(img_dir, filename)
    cv2.imwrite(img_path, img)
    return img_path

# Function to delete existing images in the directory
def delete_existing_images(directory):
    """
    Deletes all images in the specified directory to ensure only one image exists.
    """
    for file in os.listdir(directory):
        file_path = os.path.join(directory, file)
        if os.path.isfile(file_path) and file.lower().endswith(('.png', '.jpg', '.jpeg')):
            os.remove(file_path)
    print("[INFO] Existing images deleted.")

# Route for image upload
@app.route('/upload', methods=['POST'])
def upload():
    if 'imageFile' in request.files:
        file = request.files['imageFile']
        # Convert string of image data to uint8
        nparr = np.frombuffer(file.read(), np.uint8)
        # Decode image
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

        # Delete any existing images in the directory
        delete_existing_images(img_dir)

        # Save the new image
        img_path = save_img(img)

        # Make prediction
        predicted_class = predict_class(img_path)

        # Send prediction to ESP32
        response = requests.get(esp32_ip, params={"value": predicted_class})
        print("ESP32 Response:", response.text)

        return f"[SUCCESS] Image Received and Classified: {predicted_class}", 201
    else:
        return "[FAILED] Image Not Received", 204

# Run the server
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
