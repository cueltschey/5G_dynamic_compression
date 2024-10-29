import matplotlib.pyplot as plt
import pandas as pd
import sys
import time

# Colors for each compression type
compression_colors = {0: 'purple', 1: 'green', 2: 'cyan', 3: 'magenta'}

# Initialize the figure and set up subplots
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(15, 12))  # Three vertical plots

def plot_live_data(df, metrics_df):
    ax1.cla()
    ax2.cla()
    ax3.cla()

    labels = ["None", "Block Floating Point", "Run Length Encoding", "LZ77"]
    for comp_type, color in compression_colors.items():
        subset = df[df['compression_type'] == comp_type]
        ax1.plot(subset['index'], subset['average_both'], label=labels[comp_type], color=color)

    ax1.set_title('Average Durations and Their Sum')
    ax1.set_xlabel('Index')
    ax1.set_ylabel('Duration (ms)')
    ax1.legend()
    ax1.grid()

    compression_counts = df['compression_type'].value_counts().sort_index()
    colors = [compression_colors[i] for i in compression_counts.index]

    ax2.pie(compression_counts, labels=labels, colors=colors, autopct='%1.1f%%', startangle=140)
    ax2.set_title('Compression Type Frequency')

    for comp_type, color in compression_colors.items():
        subset = metrics_df[metrics_df['action'] == comp_type]
        ax3.plot(subset['episode'], subset['reward'], label=labels[comp_type], color=color)

    ax3.plot(metrics_df['episode'], metrics_df['epsilon'], label='Epsilon', color='green')
    ax3.plot(metrics_df['episode'], metrics_df['total_duration'], label='Total Duration', color='red')

    ax3.set_title('Training Metrics')
    ax3.set_xlabel('Episode')
    ax3.set_ylabel('Values')
    ax3.legend()
    ax3.grid()

    plt.tight_layout()
    plt.pause(0.5)

csv_file = sys.argv[1]
training_file = sys.argv[2]
while True:
    df = pd.read_csv(csv_file)
    metrics_df = pd.read_csv(training_file)
    plot_live_data(df, metrics_df)
    time.sleep(0.5)

