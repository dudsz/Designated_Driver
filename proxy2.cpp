Skip to content
This repository  
Search
Pull requests
Issues
Gist
 @dudsz
 Unwatch 91
  Star 12
 Fork 6 se-research/2015-mini-smart-vehicles PRIVATE
 Code  Issues 0  Pull requests 3  Wiki  Pulse  Graphs
Branch: master Find file Copy path2015-mini-smart-vehicles/group-9/Designated Driver/sources/OpenDaVINCI-msv/apps/proxy/src/Proxy2.cpp
bc706b0  on 24 May 2015
@dudsz dudsz Update Proxy2.cpp
1 contributor
RawBlameHistory     297 lines (253 sloc)  9.99 KB
/**
 * proxy - Sample application to encapsulate HW/SW interfacing with embedded systems.
 * Copyright (C) 2012 - 2015 Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <ctype.h>
#include <cstring>
#include <cmath>

#include "core/data/control/VehicleControl.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/TimeStamp.h"

#include "OpenCVCamera.h"

#include "GeneratedHeaders_Data.h"

#include "Proxy.h"
#include <fstream>
#include <iostream> 
#include <ctime>
#include <unistd.h>
#include <fcntl.h> 
#include <termios.h> 
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h> 

// Markus Erlach
#define USB "/dev/ttyACM0"

namespace msv { 
    int port, CSUM, len, toCheck, IR1, IR2, IR3, US1, US2, WE;
    void open_port(std::string adr);
    void port_config();
    string readSerial(); 
    string encode(string x);
    msv::SensorBoardData sensorBoardData;


    const char *USB_PORT;

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace tools::recorder;
    using namespace core::data::control;

    Proxy::Proxy(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "proxy"),
        m_recorder(NULL),
        m_camera(NULL)
    {}

    Proxy::~Proxy() {
    }

    void Proxy::setUp() {
        
        msv::open_port(USB); // Connect to arduino
        msv::port_config();
        // This method will be called automatically _before_ running body().
        if (getFrequency() < 20) {
            cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
        }

        // Get configuration data.
        KeyValueConfiguration kv = getKeyValueConfiguration();

        // Create built-in recorder.
        /*
        const bool useRecorder = kv.getValue<uint32_t>("proxy.useRecorder") == 1;
        if (useRecorder) {
            // URL for storing containers.
            stringstream recordingURL;
            recordingURL << "file://" << "proxy_" << TimeStamp().getYYYYMMDD_HHMMSS() << ".rec";
            // Size of memory segments.
            const uint32_t MEMORY_SEGMENT_SIZE = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.memorySegmentSize");
            // Number of memory segments.
            const uint32_t NUMBER_OF_SEGMENTS = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.numberOfMemorySegments");
            // Run recorder in asynchronous mode to allow real-time recording in background.
            const bool THREADING = true;
            m_recorder = new Recorder(recordingURL.str(), MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING);
        }
*/
        // Create the camera grabber.
        const string NAME = getKeyValueConfiguration().getValue<string>("proxy.camera.name");
        string TYPE = getKeyValueConfiguration().getValue<string>("proxy.camera.type");
        std::transform(TYPE.begin(), TYPE.end(), TYPE.begin(), ::tolower);
        const uint32_t ID = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.id");
        const uint32_t WIDTH = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.width");
        const uint32_t HEIGHT = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.height");
        const uint32_t BPP = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.bpp");

        if (TYPE.compare("opencv") == 0) {
            m_camera = new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP);
        }

        if (m_camera == NULL) {
            cerr << "No valid camera type defined." << endl;
        }
    }

    void Proxy::tearDown() {
        // This method will be call automatically _after_ return from body().
        OPENDAVINCI_CORE_DELETE_POINTER(m_recorder);
        OPENDAVINCI_CORE_DELETE_POINTER(m_camera);
    }

    void Proxy::distribute(Container c) {
        // Store data to recorder.
        if (m_recorder != NULL) {
            // Time stamp data before storing.
            c.setReceivedTimeStamp(TimeStamp());
            m_recorder->store(c);
        }

        // Share data.
        getConference().send(c);
    }

    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Proxy::body() {

        uint32_t captureCounter = 0;
        while (getModuleState() == ModuleState::RUNNING) {
            // Capture frame.
            if (m_camera != NULL) {
                core::data::image::SharedImage si = m_camera->capture();

                Container c(Container::SHARED_IMAGE, si);
                distribute(c);
                captureCounter++;
            }
            Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);
                VehicleControl vc = containerVehicleControl.getData<VehicleControl>();
                cerr << "Speed: '" << vc.getSpeed() << "'" << endl;
                cerr << "Angle: '" << vc.getSteeringWheelAngle() << "'" << endl;

            // TODO: Here, you need to implement the data links to the embedded system
            // to read data from IR/US.

            // Test ***************************
            // Markus Erlach
            string in = "";
            string rec;
            char command[10];
            cout << "Enter command to send, " << endl;
            cout << "Command alternatives: w, f, s, r, n, h, v, m" << endl;
            cin >> in;
            /* w, f, s, r, n, h, v
            w = set speed to 1560, f = accelerate by 10
            s = slow down, r = reverse, n = neutral
            h = turn right, v = turn left    
            */
            strcpy(command, in.c_str());            
            write(port, command, 10);
            cout << "Proxy2 wrote: "<< command << endl;
            rec = msv::readSerial();
            decode(rec);

            Container c = Container(Container::USER_DATA_0, sensorBoardData);
            distribute(c);

            /*int IR1Data = sensorBoardData.getValueForKey_MapOfDistances(0);
            cout << "SBD IR1: " << IR1Data << endl;
            */
            //flushes the input queue, which contains data that have been received but not yet read.
            tcflush(port, TCIFLUSH); 
           
        }
        cout << "Proxy: Captured " << captureCounter << " frames." << endl;

        return ModuleState::OKAY;
    }

// Markus Erlach
void open_port(std::string adr) {
    USB_PORT = adr.c_str();
    /*
    O_RDWR = Posix read write
    O_NOCTTY = Not a controlling terminal 
    O_NDELAY = Ignore dcd signal state
    */
    port = open(USB_PORT, O_RDWR | O_NOCTTY);
    if (port != 0) {
        port_config();
    } 
    
}

// Markus Erlach
void port_config() {
    // Setup control structure
    struct termios options;
    struct termios oldOptions;
    memset (&options, 0, sizeof(options));
    
    // Get currently set options
    // Error handling
    if (tcgetattr(port, &options) != 0) {
        std::cout << "Error: " << errno << "from tcgetattr" << strerror(errno) << std::endl;
    }
    // Save old options
    oldOptions = options;
    // Set read and write speed to 9600, 19200 or 384000 baud
    cfsetispeed(&options, B38400);
    cfsetospeed(&options, B38400);
    // 8bits, no parity, no stop bits
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    /*
    No hw flow control and ignore status lines
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CREAD | CLOCAL
    */
    // turn off s/w flow ctrl and make raw
    options.c_iflag &= ~(IXON | IXOFF | IXANY); 
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
    options.c_oflag &= ~OPOST; 
    // wait for min 0 chars and wait time min
    options.c_cc[VMIN]  = 20;
    options.c_cc[VTIME] = 0;
    // Commit
    tcflush(port, TCIFLUSH);
    tcsetattr(port, TCSANOW, &options);
}

// Fredric Ola Eidsvik & PeiLi Ge
string decode(string x) {
    
    len = atoi(x.substr(1, 2).c_str());
    string toCheck_Str = x.substr(4, 3);
    toCheck = atoi(toCheck_Str.c_str());
    string IR1_Str = x.substr(8, 2);
    IR1 = atoi(IR1_Str.c_str());
    string IR2_Str = x.substr(10, 2);
    IR2 = atoi(IR2_Str.c_str());
    //string IR3_Str = x.substr(12, 2);
    //IR3 = atoi(IR3_Str.c_str());
    string US1_Str = x.substr(12, 2);
    US1 = atoi(US1_Str.c_str());
    string US2_Str = x.substr(14, 2);
    US2 = atoi(US2_Str.c_str());
    string WE_Str = x.substr(16, 3); 
    WE = atoi(WE_Str.c_str());
    CSUM = IR1 + IR2 + US1 + US2 + WE;  // Add IR3 when assembled.
    if (toCheck == CSUM) {
        cout << "Checksum matches" << endl;
        cout << "Wheel Encoder value: " << WE << endl;
        //store into container 
        sensorBoardData.putTo_MapOfDistances(0,IR1);
        sensorBoardData.putTo_MapOfDistances(1,IR2);
        //sensorBoardData.putTo_MapOfDistances(2,IR3);
        sensorBoardData.putTo_MapOfDistances(3,US1);
        sensorBoardData.putTo_MapOfDistances(4,US2);
    }
    
    return x;
}

// Markus Erlach
string readSerial() {
    char start[255] = "";
    
    cout << "Serial available: " << port << endl;
    
    int n = read(port, start, 255);
    start[n] = 0;
    cout << "N: " << n << endl;
    string received(start);
    return received;
}

} // msv

Status API Training Shop Blog About Pricing
© 2016 GitHub, Inc. Terms Privacy Security Contact Help
