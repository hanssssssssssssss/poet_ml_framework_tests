import tensorflow as tf
import pandas as pd
import numpy as np

class Model(tf.Module):

    def __init__(self, pretrained_model_path):
        # Load the pre-trained model
        self.model = tf.keras.models.load_model(pretrained_model_path)

        # Get input and output shapes
        self.input_shape = self.model.input_shape[1:]
        print(f"Model loaded with input shape: {self.input_shape}")
        # Recompile the model
        self.model.compile(
            optimizer=tf.keras.optimizers.Adam(),
            loss=tf.keras.losses.MeanSquaredError())
        
        self.train = self._get_train_function()
        self.infer = self._get_infer_function()
        
    def _get_train_function(self):
        @tf.function(input_signature=[
            tf.TensorSpec([None, *self.input_shape], tf.float32),
            tf.TensorSpec([None, *self.input_shape], tf.float32),
        ])
        def train(x, y):
            with tf.GradientTape() as tape:
                prediction = self.model(x, training=True)
                loss = self.model.loss(y, prediction)
            
            gradients = tape.gradient(loss, self.model.trainable_variables)
            self.model.optimizer.apply_gradients(
                zip(gradients, self.model.trainable_variables))
            return loss
        return train
        
    def _get_infer_function(self):
        @tf.function(input_signature=[
            tf.TensorSpec([None, *self.input_shape], tf.float32),
        ])
        def infer(x):
            prediction = self.model(x)
            return prediction
        return infer

    @tf.function(input_signature=[tf.TensorSpec(shape=[], dtype=tf.string)])
    def save(self, checkpoint_path):
        tensor_names = [weight.name for weight in self.model.weights]
        tensors_to_save = self.model.weights
        tf.raw_ops.Save(
            filename=checkpoint_path, tensor_names=tensor_names,
            data=tensors_to_save, name="save")
        return {
            "checkpoint_path": checkpoint_path
        }

# Load TensorFlow or Keras model
pretrained_model_path = "../process_output/output.model.20263.keras"
tflite_model_path = pretrained_model_path[:pretrained_model_path.rfind(".")] + ".tflite"
model_custom = Model(pretrained_model_path)

tf.saved_model.save(
    model_custom,
    tflite_model_path,
    signatures={
        'train':
            model_custom.train.get_concrete_function(),
        'infer':
            model_custom.infer.get_concrete_function(),
        'save':
            model_custom.save.get_concrete_function()
    })
"""
# Test the model
n_epochs = 5
batch_size = 32

x = pd.read_csv("../RData/temp_T_for_python.csv", dtype="float32")
y = pd.read_csv("../RData/temp_C_for_python.csv", dtype="float32")

tf_data = tf.data.Dataset.from_tensor_slices((x, y))
tf_data = tf_data.batch(batch_size)

losses = np.zeros([n_epochs])
for i in range(n_epochs):
    for x, y in tf_data:
        result = model_custom.train(x, y)
    losses[i] = result
    print(f"Finished {i+1} epochs")
    print(f"  loss: {losses[i]:.3f}")

# Make sure that the model performs as ecpected
model_original = tf.keras.models.load_model(pretrained_model_path)
model_original.compile(optimizer="adam",
                       loss=tf.keras.losses.MeanSquaredError())
model_original.fit(x, y, batch_size = batch_size, epochs = n_epochs)

"""
# Convert the model
converter = tf.lite.TFLiteConverter.from_saved_model(tflite_model_path)
converter.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS,  # enable TensorFlow Lite ops.
    tf.lite.OpsSet.SELECT_TF_OPS  # enable TensorFlow ops.
]
converter.experimental_enable_resource_variables = True
converter.experimental_new_converter = True
converter.allow_custom_ops = True
converter.optimizations = [tf.lite.Optimize.DEFAULT]
tflite_model = converter.convert()