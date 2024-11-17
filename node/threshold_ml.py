import requests
import pandas as pd
import numpy as np
from sklearn.metrics import f1_score, precision_score, recall_score
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestClassifier
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier
from datetime import datetime
import time

# ThingSpeak channel details
CHANNEL_ID = "2684114"
READ_API_KEY = "TTIBU3CIFKLESX0Z"
PASSWORD = "CodeMonkeys"


def fetch_thingspeak_data():
    """Fetch all relevant data from ThingSpeak"""
    url = f"https://api.thingspeak.com/channels/{
        CHANNEL_ID}/feeds.json?api_key={READ_API_KEY}&results=0"

    try:
        response = requests.get(url)
        data = response.json()
        n_data_points = int(data['channel']['last_entry_id'])
        url = f"https://api.thingspeak.com/channels/{
            CHANNEL_ID}/feeds.json?api_key={READ_API_KEY}&results={n_data_points - 500}"
        
        response = requests.get(url)
        data = response.json()
        feeds = data['feeds']
        df = pd.DataFrame(feeds)
        df['field4'] = pd.to_numeric(df['field4'])
        df['field8'] = pd.to_numeric(df['field8'])

        # Process fall detection entries
        i = len(df) - 1
        while i >= 2:
            if df.at[i, 'field8'] == 1:
                if i - 1 >= 0:
                    df.at[i - 1, 'field8'] = 1
                if i - 2 >= 0:
                    df.at[i - 2, 'field8'] = 1
                i -= 2
            i -= 1

        i = 0
        while i < len(df):
            if df.at[i, 'field8'] == 1:
                start = i
                while i < len(df) and df.at[i, 'field8'] == 1:
                    i += 1
                end = i
                max_idx = df.loc[start:end-1, 'field4'].idxmax()
                df.loc[start:end-1, 'field8'] = 0
                df.at[max_idx, 'field8'] = 1
            i += 1

        return df[['field4', 'field8']]

    except Exception as e:
        print(f"Error fetching data: {e}")
        return None


def train_and_evaluate_models(X, y):
    """Train multiple models and return the best one"""
    X = X.reshape(-1, 1)

    # Print data statistics
    print(f"\nData Statistics:")
    print(f"Total samples: {len(X)}")
    print(f"Number of falls (1): {np.sum(y == 1)}")
    print(f"Number of non-falls (0): {np.sum(y == 0)}")

    # Check if we have both classes
    if np.sum(y == 1) == 0:
        print("No fall samples in the dataset!")
        return None, None, None, None

    # Handle class imbalance using SMOTE
    from imblearn.over_sampling import SMOTE
    from imblearn.under_sampling import RandomUnderSampler
    from imblearn.pipeline import Pipeline

    # Define resampling strategy
    over = SMOTE(sampling_strategy=0.5, random_state=42)
    under = RandomUnderSampler(sampling_strategy=0.8, random_state=42)
    steps = [('o', over), ('u', under)]
    pipeline = Pipeline(steps=steps)

    # Split data
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=y)

    # Scale features
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)

    # Apply resampling
    X_train_resampled, y_train_resampled = pipeline.fit_resample(
        X_train_scaled, y_train)

    print(f"\nAfter resampling:")
    print(f"Training samples: {len(X_train_resampled)}")
    print(f"Falls in training: {np.sum(y_train_resampled == 1)}")
    print(f"Non-falls in training: {np.sum(y_train_resampled == 0)}")

    # Initialize models with adjusted parameters
    models = {
        'Random Forest': RandomForestClassifier(
            n_estimators=100,
            class_weight='balanced',
            min_samples_leaf=2,
            random_state=42
        ),
        'SVM': SVC(
            kernel='rbf',
            class_weight='balanced',
            probability=True,
            random_state=42
        ),
        'Decision Tree': DecisionTreeClassifier(
            class_weight='balanced',
            min_samples_leaf=2,
            random_state=42
        )
    }

    best_model = None
    best_f1 = 0
    best_threshold = None
    best_metrics = None

    for name, model in models.items():
        try:
            # Train model
            model.fit(X_train_resampled, y_train_resampled)

            # For SVM and Random Forest, we can get probability estimates
            if hasattr(model, 'predict_proba'):
                y_pred_proba = model.predict_proba(X_test_scaled)[:, 1]

                # Try different probability thresholds
                thresholds = np.linspace(0, 1, 100)
                best_local_f1 = 0
                best_local_threshold = 0.5

                for thresh in thresholds:
                    y_pred = (y_pred_proba >= thresh).astype(int)
                    f1 = f1_score(y_test, y_pred, zero_division=0)
                    if f1 > best_local_f1:
                        best_local_f1 = f1
                        best_local_threshold = thresh

                y_pred = (y_pred_proba >= best_local_threshold).astype(int)
            else:
                y_pred = model.predict(X_test_scaled)

            # Calculate metrics
            f1 = f1_score(y_test, y_pred, zero_division=0)
            precision = precision_score(y_test, y_pred, zero_division=0)
            recall = recall_score(y_test, y_pred, zero_division=0)

            print(f"\n{name} Results:")
            print(f"F1 Score: {f1:.2f}")
            print(f"Precision: {precision:.2f}")
            print(f"Recall: {recall:.2f}")

            if f1 > best_f1:
                best_f1 = f1
                best_model = model
                best_metrics = (precision, recall, f1)

                # Calculate equivalent acceleration threshold
                if hasattr(model, 'predict_proba'):
                    # Find the acceleration value that corresponds to the probability threshold
                    proba = model.predict_proba(
                        X_scaled := scaler.transform(X.reshape(-1, 1)))[:, 1]
                    threshold_idx = np.where(proba >= best_local_threshold)[0]
                    if len(threshold_idx) > 0:
                        best_threshold = np.min(X[threshold_idx])
                    else:
                        best_threshold = np.median(X)
                else:
                    predictions = model.predict(
                        X_scaled := scaler.transform(X.reshape(-1, 1)))
                    true_positives = X[predictions == 1]
                    if len(true_positives) > 0:
                        best_threshold = np.min(true_positives)
                    else:
                        best_threshold = np.median(X)

        except Exception as e:
            print(f"Error training {name}: {e}")

    if best_model is None:
        print("No successful model training!")
        return None, None, None, None

    return best_model, scaler, best_threshold, best_metrics


def main():
    while True:
        print(f"\nFetching data at {datetime.now()}")
        df = fetch_thingspeak_data()

        if df is not None and not df.empty:
            X = df['field4'].values
            y = df['field8'].values

            # Train models and get the best one
            best_model, scaler, threshold, metrics = train_and_evaluate_models(
                X, y)

            if metrics is not None:
                precision, recall, f1 = metrics

                print(f"\nBest Model Performance:")
                print(f"Model: {best_model.__class__.__name__}")
                print(f"Equivalent Threshold: {threshold:.2f}")
                print(f"Performance Metrics:")
                print(f"  Precision: {precision:.2f}")
                print(f"  Recall: {recall:.2f}")
                print(f"  F1 Score: {f1:.2f}")

                # Check if threshold should be updated
                url = "https://api.thingspeak.com/channels/2678150/feeds.json?api_key=RZH4FA34SCB2XOQX"
                response = requests.get(url)
                data = response.json()
                feeds = data['feeds']
                df_control = pd.DataFrame(feeds)
                df_control['field4'] = df_control['field4'].astype(str)
                df_control['field5'] = df_control['field5'].astype(str)

                i = len(df_control) - 1
                while i >= 0:
                    if df_control.at[i, 'field4'] == PASSWORD:
                        if df_control.at[i, 'field5'] == "true":
                            print("Updating threshold at ThingSpeak...")
                            url = f"https://api.thingspeak.com/update?api_key=GJRF5PDPH9MKTT54&field1={threshold}&field2={
                                df_control.at[i, 'field2']}&field3={df_control.at[i, 'field3']}&field4={PASSWORD}&field5=true"
                            response = requests.get(url)
                            if response.status_code == 200 and response.text != '0':
                                print("Threshold updated successfully!")
                            else:
                                print(
                                    "Error updating threshold! Will retry after 15 seconds")
                        else:
                            print(
                                "User opted out of Machine Learning powered automatic threshold management")
                        break
                    i -= 1
            else:
                print("Could not train models successfully")

        else:
            print("No data available or error in fetching data")

        time.sleep(15)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nProgram terminated by user")
