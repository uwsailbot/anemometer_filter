// Constants for generateing smooth data
const float accelChance = 300; // chance of larger change
const float accelRate = 0.8; //rate of larger change
const float accelDecrease = 0.1; // rate at which larger change stops
const float velDecrease = 0.5; // rate at which larger change stops
const float velChange = 0.03; // rate of small changes
const float maxVel = 0.5/3; // maximum velocity for slow drift

//Constants for noise
const float maxNoise = 5;
const float noiseChange = 2;

// Constants for spikes
const float spikeChance = 100; 
const float spikeMaxVel = 20; // Rate at which spike first goes up
const float spikeVelDecrease = 3; // Rate at which spike slowd
const float spikeDecrease = 15; // Rate at which spike goes back down

float sign(float value) { 
return float((value>0)-(value<0)); 
}

float smoothAccel = 0, smoothVel = 0, smooth = 180;
int smoothData() {
    if (smoothAccel == 0){
        //random chance of sudden larger change
        if (random(0,accelChance) == 0 && smoothVel < maxVel && smoothVel > -1 * maxVel){
            smoothAccel = random(-100, 100)*(accelRate/100);
        }
        //small random changes
        if (smoothVel < maxVel && smoothVel > -1 * maxVel){
            smoothVel += random(-100, 100)*(velChange/100);
        }else{ //slow down after larger change
            smoothVel -= sign(smoothVel) * random(0, 100)*(velDecrease/100);
        }
    }else{
        //slow down after larger change
        smoothAccel -= sign(smoothAccel) * random(0, 100)*(accelDecrease/100);
        if (smoothAccel < accelDecrease && smoothAccel > -1 * accelDecrease){
            smoothAccel = 0;
        }
    }

    smoothVel += smoothAccel;
    smooth += smoothVel;
    
    return (int) smooth; 
}

float noiseLevel = 0;
int noise(){
    if (noiseLevel > maxNoise){
        noiseLevel -= 1;
    }else if (noiseLevel < (-1) * maxNoise){
        noiseLevel += 1;
    }else{
        noiseLevel += random(-100, 100) * (noiseChange/100);
    }
    
    return (int) noiseLevel;
}

float spikeLevel = 0, spikeVel = 0;
int spike(){
    if (spikeLevel == 0){
        // Randomly cause spikes
        if (random(0, spikeChance) == 0){
            spikeVel = random(-100,100) * (spikeMaxVel/100);
        }
    }else{
        if (spikeVel == 0){
            //Bring spike back down
            spikeLevel -= sign(spikeLevel) * random(0, 100) * (spikeDecrease / 100);
        }else if (spikeVel < spikeVelDecrease && spikeVel > (-1) * spikeVelDecrease){
            spikeVel = 0;
        }else{
            // Slow down spike
            spikeVel -= sign(spikeVel) * random(0, 100) * (spikeVelDecrease / 100);
        }
        
        //Prevent oscillation at end of spike
        if (spikeLevel < spikeDecrease && spikeLevel > -1 * spikeDecrease){
            spikeLevel = 0;
        }
    }
    
    spikeLevel += spikeVel;
    
    return spikeLevel;
}

int getData(){
    return ((((smoothData() + noise() + spike()) % 360) + 360) % 360); // because mod with negatives is dumb
}

