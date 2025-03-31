

import numpy as np
from sklearn.preprocessing import MinMaxScaler, StandardScaler

def get_train(df, ret, window_size):
    # Initialize the scaler
    X_scaler = StandardScaler()
    y_scaler = StandardScaler()
    scaled_X = X_scaler.fit_transform(df)  # Exclude last column for scaling
    # Apply scaling to the target variable (the last column)
    scaled_y = y_scaler.fit_transform(ret.values.reshape(-1, 1))  # Reshape to 2D array
    X, y = [], []
    len_values = len(df)
    # Generate X and y using a sliding window approach
    for i in range(window_size, len_values):
        # X is the previous 'window_size' rows, from i - window_size to i
        X.append(scaled_X[i-window_size:i])
        # y is the value of the last column (target variable) for the current row i
        y.append(scaled_y[i])
    # Convert X and y into numpy arrays
    X, y = np.asarray(X), np.asarray(y)
    # Reshape X to (samples, time_steps, features)
    X = np.reshape(X, (X.shape[0], X.shape[1], X.shape[2]))  # Ensure 3D for LSTM
    print(f"X shape: {X.shape}, y shape: {y.shape}")
    return X, y, X_scaler, y_scaler

def get_val_test(df,ret, window_size, X_scaler, y_scaler):
    scaled_X = X_scaler.transform(df)  # Exclude last column for scaling
    scaled_y = y_scaler.transform(ret.values.reshape(-1,1))
    X, y = [], []
    len_values = len(df)
    for i in range(window_size, len_values):
        X.append(scaled_X[i-window_size:i])
        y.append(scaled_y[i])
    X, y = np.asarray(X), np.asarray(y)
    X = np.reshape(X, (X.shape[0], X.shape[1], X.shape[2]))  # Ensure 3D for LSTM
    print(f"X {X.shape}, y {y.shape}")
    return X, y

