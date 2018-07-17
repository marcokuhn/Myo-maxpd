//
//  Myo.cpp
//  myo
//
//  Created by Baptiste on 13/08/2015.
//
//

#include "myo.h"




TheMyo::TheMyo(int nc)
{
    numChannels = nc;
    acclDim = 3;
    gyroDim = 3;
    quatDim = 3;
}



// Every time Myo Connect successfully pairs with a Myo armband, this function will be called.
//
// You can rely on the following rules:
//  - onPair() will only be called once for each Myo device
//  - no other events will occur involving a given Myo device before onPair() is called with it
//
// If you need to do some kind of per-Myo preparation before handling events, you can safely do it in onPair().
void TheMyo::onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)
{
    // Print out the MAC address of the armband we paired with.
    // The pointer address we get for a Myo is unique - in other words, it's safe to compare two Myo pointers to
    // see if they're referring to the same Myo.
    // Add the Myo pointer to our list of known Myo devices. This list is used to implement identifyMyo() below so
    // that we can give each Myo a nice short identifier.
    knownMyos.push_back(myo);
    // Now that we've added it to our list, get our short ID for it and print it out.
    std::cout << "Paired with " << identifyMyo(myo) << "." << std::endl;
}

void TheMyo::onConnect(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)
{
    myo->setStreamEmg(myo::Myo::streamEmgEnabled);
    //iterate through vector and identify if myo is already connected - if not add it
    if(std::find(knownMyos.begin(), knownMyos.end(), myo) != knownMyos.end()) {
        /* v contains x */
    } else {
        knownMyos.push_back(myo);
    }
         
}



void TheMyo::onUnpair(myo::Myo* myo, uint64_t timestamp)
{
    emgdata1.clear();
    emgdata2.clear();
    
    
    accdata1.clear();
    accdata2.clear();
    
    gyrodata1.clear();
    gyrodata2.clear();
    
    quatdata1.clear();
    quatdata2.clear();
    
    
    //remove a the unpaired myo element from the vector
    knownMyos.erase(std::remove(knownMyos.begin(), knownMyos.end(), myo), knownMyos.end());
    
    for (int k=0; k<numChannels;k++) emgdata1.push_back(0.0);
    for (int k=0; k<numChannels;k++) emgdata2.push_back(0.0);
        
    for (int k=0; k<acclDim;k++) accdata1.push_back(0.0);
    for (int k=0; k<acclDim;k++) accdata2.push_back(0.0);
    
    for (int k=0; k<gyroDim;k++) gyrodata1.push_back(0.0);
    for (int k=0; k<gyroDim;k++) gyrodata2.push_back(0.0);
    
    for (int k=0; k<quatDim;k++) quatdata1.push_back(0.0);
    for (int k=0; k<quatDim;k++) quatdata2.push_back(0.0);
}

void TheMyo::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
    emgdata1.clear();
    emgdata2.clear();
    for (size_t i = 0; i < knownMyos.size(); ++i) {
        // If two Myo pointers compare equal, they refer to the same Myo device.
        if (knownMyos[i] == myo) {
            for (int j = 0; j < numChannels; ++j)
                if(i == 0){
                    emgdata1.push_back(emg[j]/(float)256.0);
                }else{
                    emgdata2.push_back(emg[j]/(float)256.0);
                }
        }
    }

    
   /*for (size_t i = 0; i < knownMyos.size(); ++i) {
       
        // If two Myo pointers compare equal, they refer to the same Myo device.
       if (i == 0) {
       //if (identifyMyo(myo) == 1) {
            emgdata1.clear();
            for (int j = 0; j < numChannels; j++)
            {
                emgdata1.push_back(emg[j]/(float)256.0);
        }
       }
        if (i == 1) {

            emgdata2.clear();
            for (int j= 0; j < numChannels; j++)
                emgdata2.push_back(emg[j]/(float)256.0);
        }
    }*/

}


void TheMyo::onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel)
{
    accdata1.clear();
    //for (int j = 0; j < acclDim; j++)
    //    accdata.push_back(accel[j]);
    
    for (size_t i = 0; i < knownMyos.size(); ++i) {
        // If two Myo pointers compare equal, they refer to the same Myo device.
        if (i == 0 && knownMyos[i] == myo) {
            accdata1.clear();
            for (int j = 0; j < acclDim; j++)
                accdata1.push_back(accel[j]);
        }
        else if (i == 1 && knownMyos[i] == myo) {
            accdata2.clear();
            for (int j = 0; j < acclDim; j++)
                accdata2.push_back(accel[j]);
        }
    }
}

void TheMyo::onGyroscopeData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &gyro)
{
    for (size_t i = 0; i < knownMyos.size(); ++i) {
        
        if (i == 0 && knownMyos[i] == myo) {
            gyrodata1.clear();
            for (int j = 0; j < gyroDim; j++)
                gyrodata1.push_back(gyro[j]);
            
        }
        else if (i == 1 && knownMyos[i] == myo) {
            gyrodata2.clear();
            for (int j = 0; j < gyroDim; j++)
                gyrodata2.push_back(gyro[j]);
            
        }
    }
}

void TheMyo::onOrientationData(myo::Myo *myo, uint64_t timestamp, const myo::Quaternion< float > &rotation)
{
    using std::atan2;
    using std::asin;
    using std::sqrt;
    using std::max;
    using std::min;
     
    /*
    
    // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
    float roll = atan2(2.0f * (rotation.w() * rotation.x() + rotation.y() * rotation.z()),
                       1.0f - 2.0f * (rotation.x() * rotation.x() + rotation.y() * rotation.y()));
    float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (rotation.w() * rotation.y() - rotation.z() * rotation.x()))));
    float yaw = atan2(2.0f * (rotation.w() * rotation.z() + rotation.x() * rotation.y()),
                      1.0f - 2.0f * (rotation.y() * rotation.y() + rotation.z() * rotation.z()));
    
    quatdata.clear();
    quatdata.push_back(roll);
    quatdata.push_back(pitch);
    quatdata.push_back(yaw);
     */
    
    for (size_t i = 0; i < knownMyos.size(); ++i) {
        
        // If two Myo pointers compare equal, they refer to the same Myo device.
        if (i == 0 && knownMyos[i] == myo) {
            quatdata1.clear();
            // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
            float roll = atan2(2.0f * (rotation.w() * rotation.x() + rotation.y() * rotation.z()),
                               1.0f - 2.0f * (rotation.x() * rotation.x() + rotation.y() * rotation.y()));
            float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (rotation.w() * rotation.y() - rotation.z() * rotation.x()))));
            float yaw = atan2(2.0f * (rotation.w() * rotation.z() + rotation.x() * rotation.y()),
                              1.0f - 2.0f * (rotation.y() * rotation.y() + rotation.z() * rotation.z()));
            
            //quatdata.clear();
            quatdata1.push_back(roll);
            quatdata1.push_back(pitch);
            quatdata1.push_back(yaw);

        }
        else if (i == 1 && knownMyos[i] == myo) {
            quatdata2.clear();
            // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
            float roll = atan2(2.0f * (rotation.w() * rotation.x() + rotation.y() * rotation.z()),
                               1.0f - 2.0f * (rotation.x() * rotation.x() + rotation.y() * rotation.y()));
            float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (rotation.w() * rotation.y() - rotation.z() * rotation.x()))));
            float yaw = atan2(2.0f * (rotation.w() * rotation.z() + rotation.x() * rotation.y()),
                              1.0f - 2.0f * (rotation.y() * rotation.y() + rotation.z() * rotation.z()));
            
            //quatdata.clear();
            quatdata2.push_back(roll);
            quatdata2.push_back(pitch);
            quatdata2.push_back(yaw);

        }

        /*
        if (knownMyos[i] == myo) {
            // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
            float roll = atan2(2.0f * (rotation.w() * rotation.x() + rotation.y() * rotation.z()),
                               1.0f - 2.0f * (rotation.x() * rotation.x() + rotation.y() * rotation.y()));
            float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (rotation.w() * rotation.y() - rotation.z() * rotation.x()))));
            float yaw = atan2(2.0f * (rotation.w() * rotation.z() + rotation.x() * rotation.y()),
                              1.0f - 2.0f * (rotation.y() * rotation.y() + rotation.z() * rotation.z()));
            
            //quatdata.clear();
            quatdata.push_back(roll);
            quatdata.push_back(pitch);
            quatdata.push_back(yaw);
        }*/
    }
    
}


// This is a utility function implemented for this sample that maps a myo::Myo* to a unique ID starting at 1.
// It does so by looking for the Myo pointer in knownMyos, which onPair() adds each Myo into as it is paired.
size_t TheMyo::identifyMyo(myo::Myo* myo) {
    // Walk through the list of Myo devices that we've seen pairing events for.
    for (size_t i = 0; i < knownMyos.size(); ++i) {
        // If two Myo pointers compare equal, they refer to the same Myo device.
        if (knownMyos[i] == myo) {
            return i + 1;
        }
    }
    return 0;
}



std::vector<float>& TheMyo::getEmgData1()
{
    return emgdata1;
}

std::vector<float>& TheMyo::getEmgData2()
{
    return emgdata2;
}

std::vector<float> & TheMyo::getAccelerometerData1()
{
    return accdata1;
    
}

std::vector<float> & TheMyo::getAccelerometerData2()
{
    return accdata2;
    
}

std::vector<float> & TheMyo::getGyroscopeData1()
{
    return gyrodata1;
}


std::vector<float> & TheMyo::getGyroscopeData2()
{
    return gyrodata2;
}

std::vector<float> & TheMyo::getOrientationData1()
{
    return quatdata1;
}

std::vector<float> & TheMyo::getOrientationData2()
{
    return quatdata2;
}

int TheMyo::getNumOfChannels()
{
    return numChannels;
}

int TheMyo::getNumDevices(){
    return knownMyos.size();
}

void TheMyo::setDebugMsg(std::string msg)
{
    debugMsg = msg;
}


char* TheMyo::print()
{
    char const *pchar = debugMsg.c_str();  //use char const* as target type
    char result[100];   // array to hold the result.
    strcpy(result,"devices "); // copy string one into the result.
    strcat(result,pchar); // append string two to the result.
    
    return result; // converted for max post() paramter

}


void TheMyo::resetDataVectors(){
    //accdata1.clear();
    //gyrodata.clear();
    //quatdata.clear();
}

//
////std::vector<float> getEMG()
////{
////
////}
