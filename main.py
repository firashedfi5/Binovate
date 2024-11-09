import requests

# Replace with your ESP32's IP address
esp32_ip = "http://192.168.1.15/send"
float_value = 53.33  # Example float from your CNN model

# Send the data to the ESP32
response = requests.get(esp32_ip, params={"value": float_value})
print("ESP32 Response:", response.text)
