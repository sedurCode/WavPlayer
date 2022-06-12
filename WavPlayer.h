#pragma once

#include <stdio.h>
#include <string.h>
#include "daisy_pod.h"
#include "sedsLib.h"
//#include "daisy_patch.h"

using namespace daisy;
using namespace sedsLib;

static constexpr size_t blocksize = 512;
static constexpr size_t frameSize = 256;
static constexpr size_t samplerate = 48000;
static constexpr size_t samplepoolseconds = 128;
static constexpr size_t numVoices = 8;
static constexpr size_t scratchLength = 1024;

float DSY_SDRAM_BSS _pool[samplepoolseconds*samplerate];
float scratch[frameSize];
int32_t DSY_SDRAM_BSS i_scratch[scratchLength];
float DSY_SDRAM_BSS f_scratch[scratchLength];

DaisyPod       hw;
SdmmcHandler   sdcard;
FatFSInterface fsi;

AudioFileSystem fileSystemHandler;
AudioFileStreamer<frameSize, samplerate> streamer;
AudioSampler<numVoices, frameSize, samplerate> sampler;
SDRAManager<samplepoolseconds, samplerate> samplePool;
MidiUsbHandler midi;
MidiUsbHandler::Config midi_cfg;
void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size);

// Typical Switch case for Message Type.
void HandleMidiMessage(MidiEvent m);

void HandleHardwareInputs();

void UpdateUI();

void DebugPrintMidiEvent(MidiEvent m)
{
    char buff[512];
    sprintf(buff,
            "Note Received:\t%d\t%d\t%d\r\n",
            m.channel,
            m.data[0],
            m.data[1]);
    hw.seed.usb_handle.TransmitInternal((uint8_t *)buff, strlen(buff));
};

void PreloadVoices()
{
    size_t result = 0;
    for (size_t index = 0; index < numVoices; ++index)
    {
        // Get the region size we need to allocate
        size_t _numSamples = fileSystemHandler.GetNumSamples(index);
        // Assign the pointer to a region of memory we can put samples into
        SampleRegion& _region = samplePool.GetNextFreeRegion(_numSamples);
        result = fileSystemHandler.ReadFileToBuffer(index, _region.buffer, &i_scratch[0], &f_scratch[0], scratchLength);
        sampler.Init(index, _region.buffer, _numSamples);
    }

    // for (size_t index = 0; index < numVoices; ++index)
    // {
    //     // Get the region size we need to allocate
    //     size_t _numSamples = fileSystemHandler.GetNumSamples(index);
    //     float* _channelBuffer;
    //     // Assign the pointer to a region of memory we can put samples into
    //     samplePool.GetSamples(_channelBuffer, _numSamples);
    //     // Read file into the memory region we have just allocated to ourselves
    //     result = fileSystemHandler.ReadFileToBuffer(index, _channelBuffer, &i_scratch[0], &f_scratch[0], scratchLength);
    //     if (result != 0)
    //     {
    //         continue;
    //     }
    //     // Init the samples with the channel buffer
    //     sampler.Init(index, _channelBuffer, _numSamples);
    // }
};