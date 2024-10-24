import matplotlib.pyplot as plt
import pandas as pd
import sys

# Read data from CSV file
df = pd.read_csv(sys.argv[1])

# Plotting
plt.figure(figsize=(10, 6))

plt.plot(df['index'], df['average_transmission_duration'], label='Avg Transmission Duration', color='blue')
plt.plot(df['index'], df['average_compression_duration'], label='Avg Compression Duration', color='orange')
plt.plot(df['index'], df['average_both'], label='Average Total Duration', color='green')
plt.plot(df['index'], df['compression_type'] * 100, label='Compression Type', color='red')

# Adding titles and labels
plt.title('Average Durations and Their Sum')
plt.xlabel('Index')
plt.ylabel('Duration (ms)')
plt.xticks(df['index'])  # Set x-ticks to the index values
plt.legend()
plt.grid()
plt.tight_layout()

# Show plot
plt.show()
