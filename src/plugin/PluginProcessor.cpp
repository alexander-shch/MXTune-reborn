/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginGui.h"
#include <list>



//==============================================================================
MXTuneAudioProcessor::MXTuneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    for (std::uint32_t i = 0; i < sizeof(_parameters) / sizeof(_parameters[0]); i++)
    {
        _parameters[i].parameter = new PluginParameter(_parameters[i].name, _parameters[i].def / _parameters[i].scale,
            _parameters[i].min / _parameters[i].scale, _parameters[i].max / _parameters[i].scale, _parameters[i].is_boolean);
        _parameters[i].parameter->addListener(this);
        addParameter(_parameters[i].parameter);
    }
    
    _notes[0] = get_parameter(PARAMETER_ID_A) > 0 ? 1: -1;
    _notes[1] = get_parameter(PARAMETER_ID_Bb) > 0 ? 1: -1;
    _notes[2] = get_parameter(PARAMETER_ID_B) > 0 ? 1: -1;
    _notes[3] = get_parameter(PARAMETER_ID_C) > 0 ? 1: -1;
    _notes[4] = get_parameter(PARAMETER_ID_Db) > 0 ? 1: -1;
    _notes[5] = get_parameter(PARAMETER_ID_D) > 0 ? 1: -1;
    _notes[6] = get_parameter(PARAMETER_ID_Eb) > 0 ? 1: -1;
    _notes[7] = get_parameter(PARAMETER_ID_E) > 0 ? 1: -1;
    _notes[8] = get_parameter(PARAMETER_ID_F) > 0 ? 1: -1;
    _notes[9] = get_parameter(PARAMETER_ID_Gb) > 0 ? 1: -1;
    _notes[10] = get_parameter(PARAMETER_ID_G) > 0 ? 1: -1;
    _notes[11] = get_parameter(PARAMETER_ID_Ab) > 0 ? 1: -1;
    
    _at_amount = get_parameter(PARAMETER_ID_AT_AMOUNT);
    _at_smooth = get_parameter(PARAMETER_ID_AT_SMOOTH);
    _is_enable_at = get_parameter(PARAMETER_ID_ENABLE_AUTOTUNE);
    _is_enable_track = get_parameter(PARAMETER_ID_ENABLE_TRACK);
    _det_alg = get_parameter(PARAMETER_ID_DET_ALG);
    _sft_alg = get_parameter(PARAMETER_ID_SFT_ALG);
    
    _conf_thresh = get_parameter(PARAMETER_ID_VTHRESH);
    
    _misc_param = "st.sequence_ms=16\n"
                    "st.seekwindow_ms=4\n"
                    "st.overlap_ms=4\n"
                    "midi.record=0\n"
                    "midi.export=0\n";
                    
    _create_mxtune(_sample_rate);
}

MXTuneAudioProcessor::~MXTuneAudioProcessor()
{
    for (std::uint32_t i = 0; i < sizeof(_parameters) / sizeof(_parameters[0]); i++)
    {
        _parameters[i].parameter->removeListener(this);
    }
}

//==============================================================================
const String MXTuneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MXTuneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MXTuneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MXTuneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MXTuneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MXTuneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MXTuneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MXTuneAudioProcessor::setCurrentProgram (int index)
{
}

const String MXTuneAudioProcessor::getProgramName (int index)
{
    return {};
}

void MXTuneAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MXTuneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    if (_mx_tune && _sample_rate != (std::uint32_t)sampleRate)
    {
        _sample_rate = (std::uint32_t)sampleRate;
        _mx_tune->set_sample_rate(_sample_rate);
        _report_latency_samples();
    }
}

void MXTuneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MXTuneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MXTuneAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (_is_bypassed)
    {
        _is_bypassed = false;
        _report_latency_samples();
    }
    
    AudioPlayHead *play_head = getPlayHead();
    if (play_head)
    {
        if (auto pos = play_head->getPosition())
        {
            if (auto t   = pos->getTimeInSeconds())  _cur_time             = *t;
            if (auto b   = pos->getBpm())            _bpm                  = *b;
            if (auto ppq = pos->getPpqPosition())    _ppq_position         = *ppq;
            if (auto ts  = pos->getTimeSignature())  _time_sig_denominator = ts->denominator;
            _is_playing = pos->getIsPlaying();
        }
    }
    
    if (totalNumInputChannels > 0)
    {
        auto* channel_data = buffer.getWritePointer (0);
        std::int32_t num_samples =  buffer.getNumSamples();
        if (_mx_tune)
        {
            _mx_tune->run(channel_data, channel_data, num_samples, _cur_time);
            _record_midi_to_note(midiMessages, num_samples, _cur_time);
            _output_midi_from_note(midiMessages, num_samples, _cur_time);
        }
        
        for (int channel = 1; channel < totalNumInputChannels; ++channel)
        {
            buffer.copyFrom(channel, 0, channel_data, num_samples);
        }
    }
    
}


void MXTuneAudioProcessor::processBlockBypassed (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    if (!_is_bypassed)
    {
        _is_bypassed = true;
        _report_latency_samples();
    }
    AudioProcessor::processBlockBypassed(buffer, midiMessages);
}

//==============================================================================
bool MXTuneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MXTuneAudioProcessor::createEditor()
{
    return new PluginGui (*this);
}

//==============================================================================
void MXTuneAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    if (_mx_tune == nullptr)
        return;

    float time_begin = 0;
    float time_end = _mx_tune->get_manual_tune().get_time_len();

    ValueTree root("MXTuneState");

    // inpitch: one child element per pitch node
    {
        ValueTree inpitchTree("inpitch");
        std::list<std::pair<manual_tune::pitch_node, float>> inpitch =
            _mx_tune->get_manual_tune().get_inpitch(time_begin, time_end);
        for (auto& i : inpitch)
        {
            ValueTree node("p");
            node.setProperty("pitch", i.first.pitch, nullptr);
            node.setProperty("conf",  i.first.conf,  nullptr);
            node.setProperty("time",  i.second,       nullptr);
            inpitchTree.appendChild(node, nullptr);
        }
        root.appendChild(inpitchTree, nullptr);
    }

    // tune nodes
    {
        ValueTree tuneTree("tune");
        std::list<std::shared_ptr<manual_tune::tune_node>> tune =
            _mx_tune->get_manual_tune().get_tune(time_begin, time_end);
        for (auto& i : tune)
        {
            ValueTree node("n");
            node.setProperty("is_manual",   (int)i->is_manual,   nullptr);
            node.setProperty("time_start",  i->time_start,        nullptr);
            node.setProperty("time_end",    i->time_end,          nullptr);
            node.setProperty("pitch_start", i->pitch_start,       nullptr);
            node.setProperty("pitch_end",   i->pitch_end,         nullptr);
            node.setProperty("attack",      i->attack,            nullptr);
            node.setProperty("release",     i->release,           nullptr);
            node.setProperty("amount",      i->amount,            nullptr);
            tuneTree.appendChild(node, nullptr);
        }
        root.appendChild(tuneTree, nullptr);
    }

    // parameters — one child element per parameter, preserving order
    {
        ValueTree paramsTree("params");
        for (std::uint32_t i = 0; i < sizeof(_parameters) / sizeof(_parameters[0]); i++)
        {
            ValueTree p("p");
            p.setProperty("v", get_parameter(i), nullptr);
            paramsTree.appendChild(p, nullptr);
        }
        root.appendChild(paramsTree, nullptr);
    }

    // misc_param
    root.setProperty("misc", String(_misc_param.c_str(), _misc_param.length()), nullptr);

    MemoryOutputStream stream(destData, false);
    root.writeToStream(stream);
}

void MXTuneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    if (sizeInBytes < 1 || _mx_tune == nullptr)
    {
        return;
    }

    const char *first = static_cast<const char *>(data);
    const char *last = first + sizeInBytes - 1;


    _mx_tune->get_manual_tune().disable_history();
    
    if (*first == '{' && *last == '}')
    {
        var root = JSON::fromString(String(first, sizeInBytes));
        if (root.isVoid())
        {
            return;
        }
        
        if (!root.hasProperty("inpitch")
            /*|| !root.hasProperty("outpitch")*/
            || !root.hasProperty("tune"))
        {
            return;
        }
        
        {
            var inpitch = root["inpitch"];
            if (!inpitch.isArray())
            {
                return;
            }
            
            std::int32_t size = inpitch.size();
            if (size > 0
                && inpitch[0].hasProperty("pitch")
                && inpitch[0].hasProperty("conf")
                && inpitch[0].hasProperty("time"))
            {
                manual_tune::pitch_node last_pitch;
                last_pitch.pitch = inpitch[0]["pitch"];
                last_pitch.conf = inpitch[0]["conf"];
                float last_time = inpitch[0]["time"];
                for (std::int32_t i = 1; i < size; i++)
                {
                    if (inpitch[i].hasProperty("pitch")
                        && inpitch[i].hasProperty("conf")
                        && inpitch[i].hasProperty("time"))
                    {
                        manual_tune::pitch_node pitch;
                        pitch.pitch = inpitch[i]["pitch"];
                        pitch.conf = inpitch[i]["conf"];
                        float time = inpitch[i]["time"];
                        
                        _mx_tune->get_manual_tune().set_inpitch(last_time, time, last_pitch);
                        last_pitch = pitch;
                        last_time = time;
                    }
                }
            }
        }
        
        /*
        {
            var outpitch = root["outpitch"];
            if (!outpitch.isArray())
            {
                return;
            }
            
            std::int32_t size = outpitch.size();
            if (size > 0
                && outpitch[0].hasProperty("pitch")
                && outpitch[0].hasProperty("conf")
                && outpitch[0].hasProperty("time"))
            {
                manual_tune::pitch_node last_pitch;
                last_pitch.pitch = outpitch[0]["pitch"];
                last_pitch.conf = outpitch[0]["conf"];
                float last_time = outpitch[0]["time"];
                for (std::int32_t i = 1; i < size; i++)
                {
                    if (outpitch[i].hasProperty("pitch")
                        && outpitch[i].hasProperty("conf")
                        && outpitch[i].hasProperty("time"))
                    {
                        manual_tune::pitch_node pitch;
                        pitch.pitch = outpitch[i]["pitch"];
                        pitch.conf = outpitch[i]["conf"];
                        float time = outpitch[i]["time"];
                        
                        _mx_tune->get_manual_tune().set_outpitch(last_time, time, last_pitch);
                        last_pitch = pitch;
                        last_time = time;
                    }
                }
            }
        }
        */
        
        {
            var tune = root["tune"];
            if (!tune.isArray())
            {
                return;
            }
            
            std::int32_t size = tune.size();
            for (std::int32_t i = 0; i < size; i++)
            {
                std::shared_ptr<manual_tune::tune_node> node(new manual_tune::tune_node);
                
                if (tune[i].hasProperty("is_manual"))
                {
                    node->is_manual = tune[i]["is_manual"];
                }
                
                if (tune[i].hasProperty("time_start")
                    && tune[i].hasProperty("time_end")
                    && tune[i].hasProperty("pitch_start")
                    && tune[i].hasProperty("pitch_end")
                    && tune[i].hasProperty("attack")
                    && tune[i].hasProperty("release")
                    && tune[i].hasProperty("amount"))
                {
                    node->time_start = tune[i]["time_start"];
                    node->time_end = tune[i]["time_end"];
                    node->pitch_start = tune[i]["pitch_start"];
                    node->pitch_end = tune[i]["pitch_end"];
                    node->attack = tune[i]["attack"];
                    node->release = tune[i]["release"];
                    node->amount = tune[i]["amount"];
                    
                    _mx_tune->get_manual_tune().add_tune(node);
                }
            }
        }
        
        if (root.hasProperty("paramters"))
        {
            var paramters = root["paramters"];
            if (!paramters.isArray())
            {
                return;
            }
            
            std::int32_t size = paramters.size();
            for (std::int32_t i = 0; i < size; i++)
            {
                set_parameter(i, paramters[i]);
            }
        }

        if (root.hasProperty("misc"))
        {
            var misc = root["misc"];
            if (!misc.isString())
            {
                return;
            }
            _misc_param = misc.toString().toStdString();
            _mx_tune->set_misc_param(_misc_param);
            _apply_misc_param();
            _report_latency_samples();
        }
    }
    else
    {
        // New ValueTree binary format
        MemoryInputStream stream(data, (size_t)sizeInBytes, false);
        ValueTree root = ValueTree::readFromStream(stream);

        if (!root.isValid() || root.getType() != Identifier("MXTuneState"))
        {
            _mx_tune->get_manual_tune().enable_history();
            return;
        }

        ValueTree inpitchTree = root.getChildWithName("inpitch");
        ValueTree tuneTree    = root.getChildWithName("tune");

        if (!inpitchTree.isValid() || !tuneTree.isValid())
        {
            _mx_tune->get_manual_tune().enable_history();
            return;
        }

        // inpitch: set_inpitch expects consecutive (last, current) pairs
        {
            int numNodes = inpitchTree.getNumChildren();
            if (numNodes > 0)
            {
                ValueTree firstNode = inpitchTree.getChild(0);
                manual_tune::pitch_node last_pitch;
                last_pitch.pitch = (float)(double)firstNode.getProperty("pitch", 0.0);
                last_pitch.conf  = (float)(double)firstNode.getProperty("conf",  0.0);
                float last_time  = (float)(double)firstNode.getProperty("time",  0.0);

                for (int i = 1; i < numNodes; i++)
                {
                    ValueTree node = inpitchTree.getChild(i);
                    manual_tune::pitch_node pitch;
                    pitch.pitch = (float)(double)node.getProperty("pitch", 0.0);
                    pitch.conf  = (float)(double)node.getProperty("conf",  0.0);
                    float time  = (float)(double)node.getProperty("time",  0.0);

                    _mx_tune->get_manual_tune().set_inpitch(last_time, time, last_pitch);
                    last_pitch = pitch;
                    last_time  = time;
                }
            }
        }

        // tune nodes
        for (int i = 0; i < tuneTree.getNumChildren(); i++)
        {
            ValueTree node = tuneTree.getChild(i);
            auto tuneNode = std::make_shared<manual_tune::tune_node>();
            tuneNode->is_manual   = (int)node.getProperty("is_manual",   0);
            tuneNode->time_start  = (float)(double)node.getProperty("time_start",  0.0);
            tuneNode->time_end    = (float)(double)node.getProperty("time_end",    0.0);
            tuneNode->pitch_start = (float)(double)node.getProperty("pitch_start", 0.0);
            tuneNode->pitch_end   = (float)(double)node.getProperty("pitch_end",   0.0);
            tuneNode->attack      = (float)(double)node.getProperty("attack",      0.0);
            tuneNode->release     = (float)(double)node.getProperty("release",     0.0);
            tuneNode->amount      = (float)(double)node.getProperty("amount",      0.0);
            _mx_tune->get_manual_tune().add_tune(tuneNode);
        }

        // parameters
        ValueTree paramsTree = root.getChildWithName("params");
        if (paramsTree.isValid())
        {
            int numParams = paramsTree.getNumChildren();
            for (int i = 0; i < numParams; i++)
                set_parameter(i, (float)(double)paramsTree.getChild(i).getProperty("v", 0.0));
        }

        // misc_param
        if (root.hasProperty("misc"))
        {
            _misc_param = root.getProperty("misc").toString().toStdString();
            _mx_tune->set_misc_param(_misc_param);
            _apply_misc_param();
            _report_latency_samples();
        }
    }
    
    _mx_tune->get_manual_tune().enable_history();
}


void MXTuneAudioProcessor::parameterValueChanged (int parameterIndex, float newValue)
{
    std::lock_guard<std::mutex> l(_mtx);

    if (parameterIndex >= PARAMETER_ID_A && parameterIndex <= PARAMETER_ID_Ab)
    {
        _notes[parameterIndex - PARAMETER_ID_A] = (newValue > 0)? 1: -1;
        if (_mx_tune)
        {
            _mx_tune->set_at_note(_notes);
        }
    }
    else if (parameterIndex == PARAMETER_ID_AT_AMOUNT)
    {
        _at_amount = newValue;
        if (_mx_tune)
        {
            _mx_tune->set_at_amount(newValue);
        }
    }
    else if (parameterIndex == PARAMETER_ID_AT_SMOOTH)
    {
        _at_smooth = newValue;
        if (_mx_tune)
        {
            _mx_tune->set_at_smooth(newValue);
        }
    }
    else if (parameterIndex == PARAMETER_ID_ENABLE_AUTOTUNE)
    {
        _is_enable_at = newValue > 0.;
        if (_mx_tune)
        {
            _mx_tune->enable_auto_tune(_is_enable_at);
        }
    }
    else if (parameterIndex == PARAMETER_ID_ENABLE_TRACK)
    {
        _is_enable_track = newValue > 0.;
        if (_mx_tune)
        {
            _mx_tune->enable_track(_is_enable_track);
        }
    }
    else if (parameterIndex == PARAMETER_ID_DET_ALG)
    {
        _det_alg = round(newValue * _parameters[parameterIndex].scale);
        if (_mx_tune)
        {
            _mx_tune->set_detector(_det_alg);
        }
    }
    else if (parameterIndex == PARAMETER_ID_SFT_ALG)
    {
        _sft_alg = round(newValue * _parameters[parameterIndex].scale);
        if (_mx_tune)
        {
            _mx_tune->set_shifter(_sft_alg);
            _report_latency_samples();
        }
    }
    else if (parameterIndex == PARAMETER_ID_VTHRESH)
    {
        _conf_thresh = newValue * _parameters[parameterIndex].scale;
        if (_mx_tune)
        {
            _mx_tune->set_conf_shift_thresh(_conf_thresh);
            _mx_tune->set_conf_detect_thresh((_conf_thresh > 0.2)? (_conf_thresh - 0.2): 0.1);
        }
    }
    else if (parameterIndex == PARAMETER_ID_AFREQ)
    {
        _afreq = newValue * _parameters[parameterIndex].scale;
        if (_mx_tune)
        {
            _mx_tune->set_aref(_afreq);
        }
    }
    else if (parameterIndex == PARAMETER_ID_DET_GATE)
    {
        _det_gate = newValue * _parameters[parameterIndex].scale;
        if (_mx_tune)
        {
            _mx_tune->set_detect_gate(-_det_gate);
        }
    }
    else if (parameterIndex == PARAMETER_ID_DET_MIN_FREQ)
    {
        _det_min_freq = newValue * _parameters[parameterIndex].scale;
        if (_mx_tune)
        {
            _mx_tune->set_detect_freq_range(_det_min_freq, _det_max_freq);
        }
    }
    else if (parameterIndex == PARAMETER_ID_DET_MAX_FREQ)
    {
        _det_max_freq = newValue * _parameters[parameterIndex].scale;
        if (_mx_tune)
        {
            _mx_tune->set_detect_freq_range(_det_min_freq, _det_max_freq);
        }
    }
    
    if (!_gesture_is_starting)
    {
        _parameter_update_id++;
    }
    
}

void MXTuneAudioProcessor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
{
    _gesture_is_starting = gestureIsStarting;
}


float MXTuneAudioProcessor::get_parameter(std::uint32_t id)
{
    if (id < PARAMETER_ID_NUM)
    {
        return _parameters[id].parameter->getValue() * _parameters[id].scale;
    }
    return 0.;
}

void MXTuneAudioProcessor::set_parameter(std::uint32_t id, float v)
{
    if (id < PARAMETER_ID_NUM)
    {
        if (v < _parameters[id].min)
        {
            v = _parameters[id].min;
        }
        if (v > _parameters[id].max)
        {
            v = _parameters[id].max;
        }
        v = v / _parameters[id].scale;
        if (v > 1.0)
        {
            v = 1.0;
        }
        _parameters[id].parameter->beginChangeGesture();
        _parameters[id].parameter->setValueNotifyingHost(v);
        _parameters[id].parameter->endChangeGesture();
    }
}


void MXTuneAudioProcessor::set_misc_param(const std::string& misc_param)
{
    _misc_param = misc_param;
    if (_mx_tune)
    {
        _mx_tune->set_misc_param(_misc_param);
        _apply_misc_param();
    }
}

void MXTuneAudioProcessor::_create_mxtune(std::uint32_t sample_rate)
{
    _mx_tune.reset(new (std::nothrow) mx_tune(sample_rate));
    if (_mx_tune)
    {
        _mx_tune->set_at_note(_notes);
        _mx_tune->set_at_amount(_at_amount);
        _mx_tune->set_at_smooth(_at_smooth);
        _mx_tune->enable_auto_tune(_is_enable_at);
        _mx_tune->enable_track(_is_enable_track);
        _mx_tune->set_detector(_det_alg);
        _mx_tune->set_shifter(_sft_alg);
        _mx_tune->set_conf_shift_thresh(_conf_thresh);
        _mx_tune->set_conf_detect_thresh((_conf_thresh > 0.2)? (_conf_thresh - 0.2): 0.1);
        _mx_tune->set_aref(_afreq);
        _mx_tune->set_detect_gate(-_det_gate);
        _mx_tune->set_detect_freq_range(_det_min_freq, _det_max_freq);
        _mx_tune->set_misc_param(_misc_param);
        setLatencySamples(_mx_tune->get_latency());
    }
}


void MXTuneAudioProcessor::_report_latency_samples()
{
    if (_is_bypassed)
    {
        setLatencySamples(0);
    }
    else
    {
        setLatencySamples(_mx_tune->get_latency());
    }
}

void MXTuneAudioProcessor::_record_midi_to_note(MidiBuffer& midiMessages, std::int32_t num_samples, float timestamp)
{
    if (_midi_record)
    {
        std::list<mx_tune::midi_msg_node> msg_list;
        for (const auto metadata : midiMessages)
        {
            const MidiMessage result = metadata.getMessage();
            const std::int32_t sample_position = metadata.samplePosition;
            if (result.isNoteOn())
            {
                mx_tune::midi_msg_node node;
                node.msg.note_on(1, result.getNoteNumber(), result.getVelocity());
                node.sample_position = sample_position;
                msg_list.push_back(node);
            }
            else if (result.isNoteOff())
            {
                mx_tune::midi_msg_node node;
                node.msg.note_off(1, result.getNoteNumber(), result.getVelocity());
                node.sample_position = sample_position;
                msg_list.push_back(node);
            }
        }
        
        if (!msg_list.empty())
        {
            _mx_tune->record_midi_to_note(num_samples, timestamp, msg_list,
                                            get_parameter(PARAMETER_ID_DEF_ATTACK),
                                            get_parameter(PARAMETER_ID_DEF_RELEASE),
                                            get_parameter(PARAMETER_ID_DEF_AMOUNT));
        }
    }
            
}
    
void MXTuneAudioProcessor::_output_midi_from_note(MidiBuffer& midiMessages, std::int32_t num_samples, float timestamp)
{
    if (_midi_export)
    {
        midiMessages.clear();
        
        std::list<mx_tune::midi_msg_node> msg_list = _mx_tune->output_midi_from_note(num_samples, timestamp);
        for (auto& midi_msg: msg_list)
        {
            if (midi_msg.msg.is_note_on())
            {
                MidiMessage msg = MidiMessage::noteOn(midi_msg.msg.get_channel(), midi_msg.msg.get_note(), (float)60);
                midiMessages.addEvent(msg, midi_msg.sample_position);
            }
            else if (midi_msg.msg.is_note_off())
            {
                MidiMessage msg = MidiMessage::noteOff(midi_msg.msg.get_channel(), midi_msg.msg.get_note(), (float)60);
                midiMessages.addEvent(msg, midi_msg.sample_position);
            }
        }
    }
}

void MXTuneAudioProcessor::_apply_misc_param()
{
    _midi_record = _misc_param.find("midi.record=1") != _misc_param.npos;
    _midi_export = _misc_param.find("midi.export=1") != _misc_param.npos;
}
    
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MXTuneAudioProcessor();
}
