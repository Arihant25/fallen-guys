import pandas
import matplotlib.pyplot as plt

# Read feeds.csv
feeds = pandas.read_csv('feeds.csv')
# Remove unnecessary columns
feeds = feeds.drop(['created_at', 'latitude', 'longitude', 'elevation', 'status', 'field4', 'field8'], axis=1)
# Replace all NaN values with 0
feeds = feeds.fillna(0)

# Find min, max and avg of every column
print(feeds.describe())

# Print the 5th and 95th percentile for every column
print(feeds.describe(percentiles=[0.05, 0.95]).loc[['5%', '95%']])

# Print the median of every column
print(feeds.median())

# Plot each reading on different graphs
plt.figure()
plt.grid(True)

plt.plot(feeds['temperature'])
plt.title('Temperature')
plt.plot(feeds['temperature'], color='#ef7816')
# Save as image
plt.savefig('temperature.png')

plt.plot(feeds['heat_index'])
plt.title('Heat Index')
# Save as image
plt.plot(feeds['heat_index'], color='#ef7816')
plt.savefig('heat_index.png')

plt.plot(feeds['humidity'])
plt.title('Humidity')
plt.plot(feeds['humidity'], color='#ef7816')
# Save as image
plt.savefig('humidity.png')

plt.plot(feeds['smoke'])
plt.title('Smoke')
plt.plot(feeds['smoke'], color='#ef7816')
# Save as image
plt.savefig('smoke.png')

plt.plot(feeds['flame'])
plt.title('Flame')
plt.plot(feeds['flame'], color='#ef7816')
# Save as image
plt.savefig('flame.png')

plt.plot(feeds['fire'])
plt.title('Fire')
plt.plot(feeds['fire'], color='#ef7816')
# Save as image
plt.savefig('fire.png')

# Show the plot
plt.show()