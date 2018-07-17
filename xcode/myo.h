//
//  Myo.h
//  myo
//
//  Created by Baptiste on 13/08/2015.
//
//

#ifndef __myo__Myo__
#define __myo__Myo__


#include <myo/myo.hpp>

#include <vector>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <array>
//#include "ext.h"


class TheMyo : public myo::DeviceListener {
    
public:
    
    TheMyo(int nc = 8);
    
    ~TheMyo() {};
    
    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp);
    
    // onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
    void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg);
    
    void onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel);
    
    void onGyroscopeData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &gyro);
    
    void onOrientationData(myo::Myo *myo, uint64_t timestamp, const myo::Quaternion< float > &rotation);
    
    void onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion);
    
    void onConnect(myo::Myo *myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion);
    
    std::vector<float> & getEmgData1();
    std::vector<float> & getEmgData2();
    
    std::vector<float> & getAccelerometerData1();
    std::vector<float> & getAccelerometerData2();
    
    std::vector<float> & getGyroscopeData1();
    std::vector<float> & getGyroscopeData2();
    
    std::vector<float> & getOrientationData1();
    std::vector<float> & getOrientationData2();
    
    int getNumDevices();
    
    int getNumOfChannels();
    
    void resetDataVectors();
    
    char* print();
    
private:
    
    // The values of this array is set by onEmgData() above.
    //    std::array<int8_t, 8> emgSamples;
    std::vector<float> emgdata1;
    std::vector<float> emgdata2;
    
    std::vector<float> accdata1;
    std::vector<float> accdata2;
    
    std::vector<float> gyrodata1;
    std::vector<float> gyrodata2;
    
    std::vector<float> quatdata1;
    std::vector<float> quatdata2;
    
    std::string debugMsg;
    
    int numChannels;
    int acclDim;
    int gyroDim;
    int quatDim;
    
    // This is a utility function implemented for this sample that maps a myo::Myo* to a unique ID starting at 1.
    // It does so by looking for the Myo pointer in knownMyos, which onPair() adds each Myo into as it is paired.
    size_t identifyMyo(myo::Myo* myo);
    
    // We store each Myo pointer that we pair with in this list, so that we can keep track of the order we've seen
    // each Myo and give it a unique short identifier (see onPair() and identifyMyo() above).
    std::vector<myo::Myo*> knownMyos;
    
    void setDebugMsg(std::string msg);
    
};

#endif /* defined(__myo__Myo__) */
