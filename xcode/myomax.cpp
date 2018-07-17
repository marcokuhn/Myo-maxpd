#include "ext.h"
#include "ext_common.h"
#include "ext_obex.h"
#include "ext_time.h"
#include "ext_itm.h"
#include "myo.h"




typedef struct myomax
{
    t_object d_obj;
    
    TheMyo * mymyo;
    myo::Myo * thalmic;
    myo::Hub * hub;
    
    void *outlet;
} t_myomax;



void *myomax_new(t_symbol *s, long argc, t_atom *argv);
void myomax_free(t_myomax *x);

void myomax_bang(t_myomax *x);
void myomax_connect(t_myomax *x, t_symbol *s, long argc, t_atom *argv);
void myomax_process(t_myomax *x, t_symbol *s, long argc, t_atom *argv);

void myomax_numdevices(t_myomax *x, t_symbol *s, long argc, t_atom *argv);
void myomax_getemgdata(t_myomax *x, t_symbol *s, long argc, t_atom *argv);

static t_class *s_myomax_class = NULL;





int C74_EXPORT main(void)
{
    t_class *c = class_new(	"myomax", (method)myomax_new, (method)myomax_free, sizeof(t_myomax), (method)0L, A_GIMME, 0);
    
    class_addmethod(c, (method)myomax_bang,			"bang",			0);
    class_addmethod(c, (method)myomax_connect,		"connect",		A_GIMME, 0);
    class_addmethod(c, (method)myomax_process,		"process",		A_GIMME, 0);
    class_addmethod(c, (method)myomax_numdevices,	"numdevices",	A_GIMME, 0);
    class_addmethod(c, (method)myomax_getemgdata,	"getemgdata",	A_GIMME, 0);
   
    class_register(CLASS_BOX, c);
    
    s_myomax_class = c;
    return 0;
}




// initial optional arg is delay time
void *myomax_new(t_symbol *s, long argc, t_atom *argv)
{
    t_myomax *x = (t_myomax *)object_alloc(s_myomax_class);
    
    x->mymyo    = new TheMyo();
    x->outlet   = outlet_new(x, NULL);
    
    return x;
}


void myomax_free(t_myomax *x)
{
    delete x->mymyo;
}



// "process" message
void myomax_process(t_myomax *x, t_symbol *s, long argc, t_atom *argv)
{
    //ncueeiu
}


// "connect" message
void myomax_connect(t_myomax *x, t_symbol *s, long argc, t_atom *argv)
{
    
    // First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
    // publishing your application. The Hub provides access to one or more Myos.
    x->hub = new myo::Hub("com.example.emg-data-sample");
    
    post("Attempting to find a Myo...");
    
    // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
    // immediately.
    // waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
    // if that fails, the function will return a null pointer.
    x->thalmic = x->hub->waitForMyo(1000);
    
    post("Connected to a Myo armband!");
    //
    // Next we enable EMG streaming on the found Myo.
    x->thalmic->setStreamEmg(myo::Myo::streamEmgEnabled);
    
    x->hub->addListener(x->mymyo);
    
    
}

void myomax_numdevices(t_myomax *x, t_symbol *s, long argc, t_atom *argv)
{
    int getNumDevices = x->mymyo->getNumDevices();
    std::string tmp_s = std::to_string(getNumDevices);
    char const *pchar = tmp_s.c_str();  //use char const* as target type
    
    char result[100];   // array to hold the result.
    strcpy(result,"devices "); // copy string one into the result.
    strcat(result,pchar); // append string two to the result.
    
    post(result);
    
}

void myomax_getemgdata(t_myomax *x, t_symbol *s, long argc, t_atom *argv)
{
    // After processing events, we get the EMG Data
    std::vector<float> emgData1 = x->mymyo->getEmgData1();
    std::vector<float> emgData2 = x->mymyo->getEmgData2();
    
    std::string tmp_s = std::to_string(emgData1.size());
    char const *pchar = tmp_s.c_str();  //use char const* as target type
    
    char result[100];   // array to hold the result.
    strcpy(result,"emg1 datasize "); // copy string one into the result.
    strcat(result,pchar); // append string two to the result.
    post(result);
    
    for(int i = 0; i < emgData1.size();i++){
        
        std::string tmp_s = std::to_string(emgData1[i]);
        char const *pchar = tmp_s.c_str();  //use char const* as target type
        
        char result[100];   // array to hold the result.
        strcpy(result,"emg1 "); // copy string one into the result.
        strcat(result,pchar); // append string two to the result.
        post(result);
   
    }
    
    tmp_s = std::to_string(emgData2.size());
    pchar = tmp_s.c_str();  //use char const* as target type
    
    strcpy(result,"emg2 datasize "); // copy string one into the result.
    strcat(result,pchar); // append string two to the result.
    post(result);

    
    for(int i = 0; i < emgData2.size();i++){
        
        std::string tmp_s = std::to_string(emgData2[i]);
        char const *pchar = tmp_s.c_str();  //use char const* as target type
        
        char result[100];   // array to hold the result.
        strcpy(result,"emg2 "); // copy string one into the result.
        strcat(result,pchar); // append string two to the result.
        post(result);
        
    }

    
}

// get EMG data
void myomax_bang(t_myomax *x)
{
    
    // In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
    // In this case, we wish to update our display 50 times a second, so we run for 1000/20 milliseconds.
    
    //eventloop in ms - updateRate
    x->hub->run(1);
    
    // After processing events, we get the EMG Data
    std::vector<float> emgData1 = x->mymyo->getEmgData1();
    std::vector<float> emgData2 = x->mymyo->getEmgData2();
    
    std::vector<float> accData1 = x->mymyo->getAccelerometerData1();
    std::vector<float> accData2 = x->mymyo->getAccelerometerData2();
    
    std::vector<float> gyroData1 = x->mymyo->getGyroscopeData1();
    std::vector<float> gyroData2 = x->mymyo->getGyroscopeData2();
    
    std::vector<float> eulerData1 = x->mymyo->getOrientationData1();
    std::vector<float> eulerData2 = x->mymyo->getOrientationData2();
    
    int numOfChannels = x->mymyo->getNumOfChannels();
    //int getNumDevices = x->mymyo->getNumDevices();
    
    /*
    std::string s = std::to_string(numOfChannels);
    char const *pchar = s.c_str();  //use char const* as target type
    post(pchar);
    */
    
    t_atom *outAtoms = new t_atom[numOfChannels];
    
    if(emgData1.size() == numOfChannels)
    {
        for (int k=0; k < numOfChannels; k++)
            atom_setfloat(&outAtoms[k], emgData1[k]);
        
        outlet_anything(x->outlet, gensym("emg1"), numOfChannels, outAtoms);
        delete[] outAtoms;
    }
    
    if(emgData2.size() == numOfChannels)
    {
        for (int k=0; k < numOfChannels; k++)
            atom_setfloat(&outAtoms[k], emgData2[k]);
        
        outlet_anything(x->outlet, gensym("emg2"), numOfChannels, outAtoms);
        delete[] outAtoms;
    }
        
    //send only data if vector hold all values from all devices.
    if(accData1.size() != 0)//getNumDevices * 3 )
    {
        int dim = accData1.size();
        outAtoms = new t_atom[dim];
        for (int k=0; k<dim; k++)
            atom_setfloat(&outAtoms[k], accData1[k]);
        outlet_anything(x->outlet, gensym("acc1"), dim, outAtoms);
        delete[] outAtoms;
    }
    
    if(accData2.size() != 0)//getNumDevices * 3 )
    {
        int dim = accData2.size();
        outAtoms = new t_atom[dim];
        for (int k=0; k<dim; k++)
            atom_setfloat(&outAtoms[k], accData2[k]);
        outlet_anything(x->outlet, gensym("acc2"), dim, outAtoms);
        delete[] outAtoms;
    }

    
    if(gyroData1.size() != 0  )
    {
        int dim = gyroData1.size();
        outAtoms = new t_atom[dim];
        for (int k=0; k<dim; k++)
            atom_setfloat(&outAtoms[k], gyroData1[k]);
        outlet_anything(x->outlet, gensym("gyro1"), dim, outAtoms);
        delete[] outAtoms;
    }
    
    if(gyroData2.size() != 0 )
    {
        int dim = gyroData2.size();
        outAtoms = new t_atom[dim];
        for (int k=0; k<dim; k++)
            atom_setfloat(&outAtoms[k], gyroData2[k]);
        outlet_anything(x->outlet, gensym("gyro2"), dim, outAtoms);
        delete[] outAtoms;
    }

    
    /*std::string s = std::to_string(eulerData.size());
    char const *pchar = s.c_str();  //use char const* as target type
    post(pchar);
    */
    
    if(eulerData1.size() != 0 )
    {
        int dim = eulerData1.size();
        outAtoms = new t_atom[dim];
        for (int k=0; k<dim; k++)
            atom_setfloat(&outAtoms[k], eulerData1[k]);
        outlet_anything(x->outlet, gensym("euler1"), dim, outAtoms);
        delete[] outAtoms;
    }
    
    if(eulerData2.size() != 0 )
    {
        int dim = eulerData2.size();
        outAtoms = new t_atom[dim];
        for (int k=0; k<dim; k++)
            atom_setfloat(&outAtoms[k], eulerData2[k]);
        outlet_anything(x->outlet, gensym("euler2"), dim, outAtoms);
        delete[] outAtoms;
    }

    
    
    
    //After transmitting the value clear the Data vectors
    x->mymyo->resetDataVectors();
    
    
    
}
