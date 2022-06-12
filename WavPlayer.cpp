// # WavPlayer
// ## Description
// Fairly simply sample player.
// Loads 16
//
// Play .wav file from the SD Card.
//
#include "WavPlayer.h"

int main(void)
{
    // Init hardware
    hw.Init();
    hw.seed.usb_handle.Init(UsbHandle::FS_BOTH);
    midi_cfg.transport_config.periph = MidiUsbTransport::Config::EXTERNAL;
    midi.Init(midi_cfg);
    System::Delay(250);
    samplePool.Init(&(_pool[0]));
    //    hw.ClearLeds();
    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    sdcard.Init(sd_cfg);
    fsi.Init(FatFSInterface::Config::MEDIA_SD);
    f_mount(&fsi.GetSDFileSystem(), "/", 1);
    fileSystemHandler.Init(fsi.GetSDPath());

    // Load the first couple samples into ram
    PreloadVoices();
    //
    streamer.Init(&fileSystemHandler);
    streamer.SetFile(0);
    // streamer.Init(fsi.GetSDPath());
    // streamer.SetLooping(true);
    // SET LED to indicate Looping status.

    //hw.SetLed(DaisyPatch::LED_2_B, streamer.GetLooping());

    // Init Audio
    hw.SetAudioBlockSize(blocksize);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.StartAudio(AudioCallback);
    // hw.midi.Init();
    // hw.midi.StartReceive();
    
    // Loop forever...
    for(;;)
    {
        streamer.NextFrame();
        // TODO: get next frame of samples by DMA
        // streamer.Prepare();
        // Debounce digital controls
        hw.ProcessDigitalControls();
        // Get Midi Messages
        // hw.midi.Listen();
        midi.Listen();
        // Handle MIDI Events
        while(midi.HasEvents()) // while(hw.midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent()); // HandleMidiMessage(hw.midi.PopEvent());
        }
        // Handle Hardware Inputs
        HandleHardwareInputs();
        // Update UI Elements
        UpdateUI();
    }
}

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    memset(&(scratch[0]), 0, frameSize*sizeof(float));
    streamer.Process(scratch, frameSize);
    sampler.Process(scratch, frameSize);
    for(size_t i = 0, j = 0; i < size; i += 2, ++j)
    {
        out[i] = out[i + 1] = scratch[j] * 0.5f;
    }  
}

// Typical Switch case for Message Type.
void HandleMidiMessage(MidiEvent m)
{
    switch(m.type)
    {
        case NoteOn:
        {
            NoteOnEvent p = m.AsNoteOn();
            DebugPrintMidiEvent(m);
            
            // This is to avoid Max/MSP Note outs for now..
            if(m.data[1] != 0)
            {
                size_t targetChannel = m.channel;
                sampler.Play(targetChannel);
                // streamer.Play();
                // p = m.AsNoteOn();
                // osc.SetFreq(mtof(p.note));
                // osc.SetAmp((p.velocity / 127.0f));
            }
            break;
        }
        case NoteOff:
        {
            size_t targetChannel = m.channel;
            sampler.Stop(targetChannel);
            // streamer.Stop();
            break;
        }
        case ControlChange:
        {
            ControlChangeEvent p = m.AsControlChange();
            switch(p.control_number)
            {
                case 1:
                    // CC 1 for cutoff.
                    // filt.SetFreq(mtof((float)p.value));
                    streamer.NextFile();
                    break;
                case 2:
                    // CC 2 for res.
                    // filt.SetRes(((float)p.value / 127.0f));
                    streamer.PreviousFile();
                    break;
                default: break;
            }
            break;
        }
        default: break;
    }
}

void HandleHardwareInputs()
{
// Change file with encoder.
    uint32_t inc = hw.encoder.Increment();

    if(inc > 0)
    {
        streamer.NextFile();
    }
    else if(inc < 0)
    {
        streamer.PreviousFile();
    }

    if(hw.button1.RisingEdge())
    {
        streamer.Play();
    }

    if(hw.button2.RisingEdge())
    {
        streamer.SetLooping(!streamer.GetLooping());

        //hw.SetLed(DaisyPod::LED_2_B, sampler.GetLooping());
        //dsy_gpio_write(&hw.leds[DaisyPatch::LED_2_B],
        //               static_cast<uint8_t>(!sampler.GetLooping()));
        if(streamer.GetLooping())
        {
            hw.led2.Set(0.f, 1.f, 0.f);
        }
        else
        {
            hw.led2.Set(1.f, 0.f, 0.f);
        }
    }
}

void UpdateUI()
{

    if(!streamer.GetLooping())
    {
        hw.led1.Set(1.f, 0.f, 0.f);
    }
    else
    {
        hw.led1.Set(0.f, 1.f, 0.f);
    }

    hw.led1.Update();
    hw.led2.Update();
}
