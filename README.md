# Anemometer Filter

## Filter 

The data is passed through an exponential filter then the median of recent points is taken. The filter can be adjusted by changing the values of **expWeightingFactor** and **medFactor**.

### Sample Filtered Data

![alt text](https://raw.githubusercontent.com/uwsailbot/anemometer_filter/master/Graphs/Median50_Exp07_%232.jpg "Graph of Filtered Data expWeightingFactor=0.07, medFactor=50")

Graphs of the data using parameters can be found in the graphs folder.


## Simulated Data Generation

The simulated data is comprised of the following three components. Each component can be modified by editing it's corresponding constants at the top of **Simulated_Data.ino**. Data points are generated at a rate of 10 ticks per second.

### 1. Smooth Data

The smooth data is intended to represent the true direction of the wind relative to the boat. It drifts slowly at a rate of up to **maxVel** with that rate changing by up to plus or minus **velChange** per tick. 

Every tick there is a one in **accelChance** of there being a large shift in the wind's direction. These shifts are intended to simulate a gust or change in the boat's heading. If that's the case the second derivative of the graph will increase to up to **accelRate**. The magnitude of this second derivative will decrease by up to **accelDecrease** every tick until it reaches zero. At this point the magnitude of the first derivative will decrease up to **velDecrease** every tick until it is within the normal range defined by **maxVel**.

![alt text](https://raw.githubusercontent.com/benajmin/anemometer-filters/master/Graphs/Smooth%20Data.png "Graph of Spikes")

### 2. Noise

The noise is meant to simulate vibrations in the anemometer or other small variations due to issues with the measuring devices. The noise level changes by up to **noiseChange** every tick, to a maximum of plus or minus **maxNoise**.

The data shown in the graph has been translated up by ten units. Normally the noise is centered around zero.

![alt text](https://raw.githubusercontent.com/benajmin/anemometer-filters/master/Graphs/Noise.png "Graph of Noise") 

### 3. Spikes

Spikes in data are intended to simulate the boat hitting a wave, or any other event that causes a brief shift in data after which the data returns to it's previous reading.

Spikes have a 1 in **spikeChance** of occurring each tick. When a spike occurs the reading will initially increase by **spikeMaxVel** per tick. This rate will decrease by up to **spikeVelDecrease** per tick. Once the rate has reached zero the data will return to it's previous reading, decreasing by up to **spikeDecrease** every tick until this has been reached.

![alt text](https://raw.githubusercontent.com/benajmin/anemometer-filters/master/Graphs/Spikes.png "Graph of Spikes")

This data is all summed to produce the full simulated data as shown. An efficient filter should be able to take this simulated data and produce something resembling the smooth data it was based on.