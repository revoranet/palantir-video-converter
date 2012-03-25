/*
 *  
 *  BFME Palantir Video Converter
 *  PalantirVideoConverter.cpp - class logic
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

#include "PalantirVideoConverter.h"
#include "AlphaChannelFrames.h"


// this is kinda ugly, but why write the same stuff twice?
#define writeAlphaFrameM(X) writeAlphaFrame(X, sizeof(X))



int PalantirVideoConverter::run()
{
    banner();
    
    if ( argc < 3 )
    {
        usage();
        return 1;
    }
    
    
    //
    // Open source file
    //
    inFile.open(argv[1], ios::in|ios::binary);
    if ( ! inFile.is_open() )
    {
        cerr << "Could not open source file '" << argv[1] << "'. Aborting..." << endl;
        return 1;
    }
    
    
    //
    // Check if output file exists and ask for confirmation
    //
    outFile.open(argv[2], ios::in);
    if ( outFile.is_open() && DEBUG == false )
    {
        char confirm;
        cout << "WARNING: The file '" << argv[2] << "' already exists. Overwrite it? (y/n): ";
        cin >> confirm;
        
        if ( confirm == 'y' || confirm == 'Y' )
        {
            cout << "\n";
        }
        else
        {
            cout << "Aborting..." << endl;
            return 1;
        }
    }
    outFile.close();
    outFile.clear();
    
    
    //
    // Open output file
    //
    outFile.open(argv[2], ios::out|ios::trunc|ios::binary);
    if ( ! outFile.is_open() )
    {
        cerr << "Could not create output file '" << argv[2] << "'. Aborting..." << endl;
        return 1;
    }
    
    
    //
    // Check the source file
    //
    if ( checkInFile() != 0 )
    {
        return 1;
    }
    
    //
    // Get number of frames
    //
    if ( readTotalFramesCount() != 0 )
    {
        return 1;
    }
    
    //
    // Set up the new output file headers
    //
    if ( writeHeaders() != 0 )
    {
        return 1;
    }
    
    
    startMsg();
    
    
    
    inFilePos = 32; // end of MVhd
    
    //
    // loop through frame by frame, copying the video frame and 
    // adding the appropriate alpha channel frame
    //
    while ( frame < totalFramesCount )
    {
        //
        // Key frame
        //
        if ( frame % 120 == 0 ) // every 120th frame is a key frame
        {
            if ( keyFrame == 0 ) // first key frame is different from the rest
            {
                writeAlphaFrameM(ALPHA_KEY_FRAME_TYPE_1);
                if ( DEBUG ) cout << "DEBUG: Special key frame #" << keyFrame;
            }
            else
            {
                writeAlphaFrameM(ALPHA_KEY_FRAME_TYPE_2);
                if ( DEBUG ) cout << "DEBUG: Regular key frame #" << keyFrame;
            }
            
            // Increase key frame counter
            keyFrame++;
        }

        //
        // Delta frame
        //
        else
        {
            if ( frame % 120 == 1 && frame > 120 ) // special delta frame after a (regular) key frame
            {
                writeAlphaFrameM(ALPHA_DELTA_FRAME_AFTER_KEY);
                if ( DEBUG ) cout << "DEBUG: Special delta frame after key frame #" << deltaFrame;
            }
            else if ( deltaFrame < 13 ) // special 13 delta frames at the beginning
            {
                switch( deltaFrame ) 
                {
                    case 0 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_0); break;
                    case 1 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_1); break;
                    case 2 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_2); break;
                    case 3 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_3); break;
                    case 4 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_4); break;
                    case 5 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_5); break;
                    case 6 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_6); break;
                    case 7 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_7); break;
                    case 8 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_8); break;
                    case 9 : writeAlphaFrameM(ALPHA_DELTA_FRAME_START_9); break;
                    case 10: writeAlphaFrameM(ALPHA_DELTA_FRAME_START_10); break;
                    case 11: writeAlphaFrameM(ALPHA_DELTA_FRAME_START_11); break;
                    case 12: writeAlphaFrameM(ALPHA_DELTA_FRAME_START_12); break;
                }
                if ( DEBUG ) cout << "DEBUG: Special start delta frame #" << deltaFrame;
            }
            else if ( frame >= totalFramesCount-7 ) // special 7 delta frames at the end
            {
                switch( totalFramesCount - frame ) // = frames left
                {
                    case 7: writeAlphaFrameM(ALPHA_DELTA_FRAME_END_0); break;
                    case 6: writeAlphaFrameM(ALPHA_DELTA_FRAME_END_1); break;
                    case 5: writeAlphaFrameM(ALPHA_DELTA_FRAME_END_2); break;
                    case 4: writeAlphaFrameM(ALPHA_DELTA_FRAME_END_3); break;
                    case 3: writeAlphaFrameM(ALPHA_DELTA_FRAME_END_4); break;
                    case 2: writeAlphaFrameM(ALPHA_DELTA_FRAME_END_5); break;
                    case 1: writeAlphaFrameM(ALPHA_DELTA_FRAME_END_6); break;
                }
                if ( DEBUG ) cout << "DEBUG: Special end delta frame #" << deltaFrame;
            }
            else // regular delta frames (2 types)
            {
                if ( frame < 120 ) // type 1: before the second key frame
                {
                    writeAlphaFrameM(ALPHA_DELTA_FRAME_TYPE_1);
                    if ( DEBUG ) cout << "DEBUG: Regular delta frame type 1 #" << deltaFrame;
                }
                else // type 2: after second key frame
                {
                    writeAlphaFrameM(ALPHA_DELTA_FRAME_TYPE_2);
                    if ( DEBUG ) cout << "DEBUG: Regular delta frame type 2 #" << deltaFrame;
                }
            }
            
            // Increase delta frame counter
            deltaFrame++;
        }
        if ( DEBUG ) cout << " [frame #" << frame << "]" << endl;
        
        
        //
        // Copy source frame
        //
        writeVideoFrame();
        
        // Increase general frame counter
        frame++;
    }
    
    
    endMsg();
    
    inFile.close();
    outFile.close();
    
    return 0;
}



int PalantirVideoConverter::checkInFile()
{
    inFile.seekg(0);
    inFile.read(buffer, 4);
    
    if ( strncmp(buffer, "MVhd", 4) != 0 )
    {
        cerr << "Error: file header does not start with 'MVhd'.\nAborting..." << endl;
        return 1;
    }
    
    return 0;
}


int PalantirVideoConverter::readTotalFramesCount()
{
    inFile.seekg(16);
    inFile.read(reinterpret_cast<char*>(&totalFramesCount), 4);
    
    if ( ! totalFramesCount )
    {
        cerr << "Error: Unable to read number of frames in source file.\nAborting..." << endl;
        return 1;
    }
    
    return 0;
}


int PalantirVideoConverter::writeHeaders()
{
    // write AVP6 block
    outFile.write(AVP6, 8);
    
    // copy MVhd block from source
    inFile.seekg(0);
    inFile.read(buffer, 32);
    outFile.write(buffer, 32);
    
    // write/copy AVhd block
    outFile.write(AVHD, 4); // 'AVhd'
    inFile.seekg(4);
    inFile.read(buffer, 28);
    outFile.write(buffer, 28);
    
    if ( outFile.fail() )
    {
        cerr << "Error: Unable to write headers to output file.\nAborting..." << endl;
        return 1;
    }
    
    return 0;
}


int PalantirVideoConverter::writeVideoFrame()
{
    // read size of next frame
    inFile.seekg(inFilePos+4);
    inFile.read(reinterpret_cast<char*>(&videoFrameSize), 4);
    
    if ( ! videoFrameSize )
    {
        cerr << "Error: Unable to read size of frame #" << frame << ".\nAborting..." << endl;
        return 1;
    }
    
    // read frame into memory
    videoFrameBuffer = new char[videoFrameSize];
    inFile.seekg(inFilePos);
    inFile.read(videoFrameBuffer, videoFrameSize);
    inFilePos += videoFrameSize;
    
    // write frame
    if ( DEBUG ) cout << "DEBUG: Writing video frame #" << frame << ", size: " << videoFrameSize << " bytes\n";
    outFile.write(videoFrameBuffer, videoFrameSize);
    
    delete [] videoFrameBuffer;
    
    if ( outFile.fail() )
    {
        cerr << "Error: Unable to write video frame #" << frame << " (size: " << videoFrameSize << " bytes).\nAborting..." << endl;
        return 1;
    }
    
    return 0;
}


int PalantirVideoConverter::writeAlphaFrame(const char* theFrame, size_t frameSize)
{
    if ( DEBUG ) cout << "DEBUG: Writing alpha frame #" << frame << ", size: " << frameSize << " bytes\n";
    outFile.write(theFrame, frameSize);
    
    if ( outFile.fail() )
    {
        cerr << "Error: Unable to write alpha frame #" << frame << " (size: " << frameSize << " bytes).\nAborting..." << endl;
        return 1;
    }
    
    return 0;
}


void PalantirVideoConverter::banner()
{
    cout << "--------------------------------------------\n";
    cout << "BFME Palantir Video Converter version " << VERSION << "\n";
    cout << "--------------------------------------------\n";
    cout << "Copyright (c) 2011 Philippe Baumann <phil@revora.net>\n";
    cout << "Written for Revora.net and The3rdAge.net, a BFME modding community\n";
    cout << "\n";
}


void PalantirVideoConverter::usage()
{
    cout << "Usage: " << argv[0] << " infile.vp6 outfile.vp6\n";
    cout << "- invile.vp6 must be a 224x224 px EA VP6 video file\n";
    cout << "- outfile.vp6 will contain the new Palantir video file with alpha channel" << endl;
}


void PalantirVideoConverter::startMsg()
{
    cout << "-------- Start --------\n";
    cout << "Copying from source file '" << argv[1] << "'\n";
    cout << "Writing to output file '" << argv[2] << "'\n";
    cout << "Total frames to process: " << totalFramesCount << "\n\n";
}


void PalantirVideoConverter::endMsg()
{
    cout << "\n\n-------- SUCCESS! --------\n";
    cout << "Total source video frames copied: " << totalFramesCount << "\n";
    cout << "Total alpha channel key frames injected: " << keyFrame << "\n";
    cout << "Total alpha channel delta frames injected: " << deltaFrame << endl;
}


//void PalantirVideoConverter::setupAlphaFrameData()
//{
//    alphaFrameData["ALPHA_KEY_FRAME_TYPE_1"]       = ALPHA_KEY_FRAME_TYPE_1;
//    alphaFrameData["ALPHA_KEY_FRAME_TYPE_2"]       = ALPHA_KEY_FRAME_TYPE_2;
//    alphaFrameData["ALPHA_DELTA_FRAME_TYPE_1"]     = ALPHA_DELTA_FRAME_TYPE_1;
//    alphaFrameData["ALPHA_DELTA_FRAME_TYPE_2"]     = ALPHA_DELTA_FRAME_TYPE_2;
//    alphaFrameData["ALPHA_DELTA_FRAME_AFTER_KEY"]  = ALPHA_DELTA_FRAME_AFTER_KEY;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_0"]    = ALPHA_DELTA_FRAME_START_0;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_1"]    = ALPHA_DELTA_FRAME_START_1;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_2"]    = ALPHA_DELTA_FRAME_START_2;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_3"]    = ALPHA_DELTA_FRAME_START_3;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_4"]    = ALPHA_DELTA_FRAME_START_4;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_5"]    = ALPHA_DELTA_FRAME_START_5;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_6"]    = ALPHA_DELTA_FRAME_START_6;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_7"]    = ALPHA_DELTA_FRAME_START_7;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_8"]    = ALPHA_DELTA_FRAME_START_8;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_9"]    = ALPHA_DELTA_FRAME_START_9;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_10"]   = ALPHA_DELTA_FRAME_START_10;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_11"]   = ALPHA_DELTA_FRAME_START_11;
//    alphaFrameData["ALPHA_DELTA_FRAME_START_12"]   = ALPHA_DELTA_FRAME_START_12;
//    alphaFrameData["ALPHA_DELTA_FRAME_END_0"]      = ALPHA_DELTA_FRAME_END_0;
//    alphaFrameData["ALPHA_DELTA_FRAME_END_1"]      = ALPHA_DELTA_FRAME_END_1;
//    alphaFrameData["ALPHA_DELTA_FRAME_END_2"]      = ALPHA_DELTA_FRAME_END_2;
//    alphaFrameData["ALPHA_DELTA_FRAME_END_3"]      = ALPHA_DELTA_FRAME_END_3;
//    alphaFrameData["ALPHA_DELTA_FRAME_END_4"]      = ALPHA_DELTA_FRAME_END_4;
//    alphaFrameData["ALPHA_DELTA_FRAME_END_5"]      = ALPHA_DELTA_FRAME_END_5;
//    alphaFrameData["ALPHA_DELTA_FRAME_END_6"]      = ALPHA_DELTA_FRAME_END_6;
//}
