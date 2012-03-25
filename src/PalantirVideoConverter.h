/*
 *  
 *  BFME Palantir Video Converter
 *  PalantirVideoConverter.h - class definition
 *  Copyright (C) 2011  Philippe Baumann <phil@revora.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */

#ifndef PALANTIR_VIDEO_CONVERTER_H_
#define PALANTIR_VIDEO_CONVERTER_H_

#include <iostream>
#include <fstream>
#include <cstring>
//#include <string>
//#include <map>
//#include <sstream>
using namespace std;



#if defined DEBUG_ACTIVE
const bool DEBUG = true;
#else
const bool DEBUG = false;
#endif



const char VERSION[] = "1.1";

//typedef map<string, const char*> af_data_t;

const char AVP6[8] = {0x41, 0x56, 0x50, 0x36, 0x08, 0x00, 0x00, 0x00};
const char AVHD[4] = {0x41, 0x56, 0x68, 0x64};



class PalantirVideoConverter
{
    private:
        int argc;
        char** argv;
        
        char buffer[32]; // a small buffer for header operations
        
        long inFilePos;
        
        size_t frame;
        size_t keyFrame;
        size_t deltaFrame;
        
        size_t totalFramesCount;
        size_t videoFrameSize;
        char* videoFrameBuffer;
        
        ifstream inFile;
        ofstream outFile;
        
//        af_data_t alphaFrameData;
//        const int maxAlphaFrameSize;
        
        
    public:
        PalantirVideoConverter(int argc, char* argv[]) :
            argc(argc), 
            argv(argv), 
            frame(0), 
            keyFrame(0), 
            deltaFrame(0) 
//            maxAlphaFrameSize(11696)
        { }
        
        ~PalantirVideoConverter()
        { }
        
        int run();

    private:
        int checkInFile();
        int readTotalFramesCount();
        int writeHeaders();
        int writeAlphaFrame(const char* theFrame, size_t frameSize);
        int writeVideoFrame();
        void banner();
        void usage();
        void startMsg();
        void endMsg();
        
//        void setupAlphaFrameData();
//        string itos(int i) 
//        {
//            stringstream sstm;
//            sstm << i;
//            return sstm.str();
//        }
        
}; // PalantirVideoConverter



#endif /* PALANTIR_VIDEO_CONVERTER_H_ */
