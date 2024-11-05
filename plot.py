import matplotlib.pyplot as plt
import pandas as pd
import sys
import time

# Colors for each compression type
compression_colors = {0: 'purple', 1: 'green', 2: 'cyan', 3: 'magenta'}

# Set up a 3x2 grid of subplots
fig = plt.figure(figsize=(15, 16))
ax1 = fig.add_subplot(2, 2, 1)  # First row, spans the full row
ax4 = fig.add_subplot(2, 2, 2)  # First row, spans the full row
ax2 = fig.add_subplot(2, 2, 3)  # Second row, left
ax3 = fig.add_subplot(2, 2, 4)  # Second row, right

def plot_live_data(df, metrics_df):
    ax1.cla()
    ax2.cla()
    ax3.cla()
    ax4.cla()

    labels = ["None", "Block Floating Point", "Run Length Encoding", "LZ77"]
    
    # Scatter plot for average durations
    for comp_type, color in compression_colors.items():
        subset = df[df['compression_type'] == comp_type][100:]
        ax1.scatter(subset['index'], subset['average_both'], label=labels[comp_type], color=color, s=10)  # s=10 for smaller points

    ax1.set_title('Average Durations and Their Sum')
    ax1.set_xlabel('Index')
    ax1.set_ylabel('Duration (ms)')
    ax1.legend()
    ax1.grid()

    # Pie chart for compression type frequency
    compression_counts = df['compression_type'].value_counts().sort_index()
    colors = [compression_colors[i] for i in compression_counts.index]

    ax2.pie(compression_counts, labels=labels, colors=colors, autopct='%1.1f%%', startangle=140)
    ax2.set_title('Compression Type Frequency')

    # Bar chart for average reward by compression type
    avg_rewards = metrics_df.groupby('action')['reward'].mean()
    ax3.bar(avg_rewards.index, avg_rewards.values, color=[compression_colors[i] for i in avg_rewards.index])
    ax3.set_xticks(avg_rewards.index)
    ax3.set_title('Average Reward by Compression Type')
    ax3.set_xlabel('Compression Type')
    ax3.set_ylabel('Average Reward')
    ax3.grid(axis='y')

    # Scatter plot for training metrics
    for comp_type, color in compression_colors.items():
        subset = metrics_df[metrics_df['action'] == comp_type]
        ax4.scatter(subset['episode'], subset['reward'], label=labels[comp_type], color=color, s=10)  # s=10 for smaller points

    ax4.plot(metrics_df['episode'], metrics_df['epsilon'], label='Epsilon', color='green')

    ax4.set_title('Training Metrics')
    ax4.set_xlabel('Episode')
    ax4.set_ylabel('Values')
    ax4.legend()
    ax4.grid()

    plt.tight_layout()
    plt.pause(0.5)

# File names from command line arguments
csv_file = sys.argv[1]
training_file = sys.argv[2]

# Continuous plot updating
while True:
    df = pd.read_csv(csv_file)
    metrics_df = pd.read_csv(training_file)
    plot_live_data(df, metrics_df)
    time.sleep(0.5)

