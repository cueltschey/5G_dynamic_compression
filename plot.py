import matplotlib.pyplot as plt
import pandas as pd
import sys

# Read data from CSV file
df = pd.read_csv(sys.argv[1])

# Plotting
plt.figure(figsize=(10, 6))

plt.plot(df['index'], df['average_transmission_duration'], marker='o', label='Avg Transmission Duration', color='blue')
plt.plot(df['index'], df['average_compression_duration'], marker='o', label='Avg Compression Duration', color='orange')
plt.plot(df['index'], df['average_both'], marker='o', label='Average Total Duration', color='green')

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
