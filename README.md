## Model

Jupyter notebooks for creating a TensorFlow Lite model for "wake word" recognition.

A pre-trained model has already been generated and added to the firmware folder.

If you want to train your own, I added a couple of extra folders to the training data they are available here:

## Firmware

ESP32 firmware built using Platform.io. This runs the neural network trying to detect the words `Left`, `Right`, `Forward` and `Backward`.

The code assumes there are two continuous servos attached to the board such as the FS90R servo - these are readily available from various suppliers and you can normally buy them with wheels as well.

# Robot_su
