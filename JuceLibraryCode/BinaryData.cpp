/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#include <cstring>

namespace BinaryData
{

//================== midius.xml ==================
static const unsigned char temp_binary_data_0[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
"\r\n"
"<magic>\r\n"
"  <Styles>\r\n"
"    <Style name=\"default\">\r\n"
"      <Nodes/>\r\n"
"      <Classes>\r\n"
"        <plot-view border=\"2\" background-color=\"black\" border-color=\"silver\" display=\"contents\"/>\r\n"
"        <nomargin margin=\"0\" padding=\"0\" border=\"0\"/>\r\n"
"        <group margin=\"5\" padding=\"5\" border=\"2\" flex-direction=\"column\"/>\r\n"
"        <transparent background-color=\"transparentblack\"/>\r\n"
"      </Classes>\r\n"
"      <Types>\r\n"
"        <Slider border=\"0\" slider-textbox=\"textbox-below\"/>\r\n"
"        <ToggleButton border=\"0\" max-height=\"50\" caption-size=\"0\" text=\"Active\"/>\r\n"
"        <TextButton border=\"0\" max-height=\"50\" caption-size=\"0\"/>\r\n"
"        <ComboBox border=\"0\" max-height=\"50\" caption-size=\"0\"/>\r\n"
"        <Plot border=\"0\" margin=\"0\" padding=\"0\" background-color=\"00000000\"\r\n"
"              radius=\"0\"/>\r\n"
"        <XYDragComponent border=\"0\" margin=\"0\" padding=\"0\" background-color=\"00000000\"\r\n"
"                         radius=\"0\"/>\r\n"
"      </Types>\r\n"
"      <Palettes>\r\n"
"        <default/>\r\n"
"      </Palettes>\r\n"
"    </Style>\r\n"
"  </Styles>\r\n"
"  <View id=\"root\" resizable=\"1\" resize-corner=\"1\" flex-direction=\"column\"\r\n"
"        margin=\"0\" padding=\"0\" border-color=\"FF000000\" width=\"1000\" height=\"700\"\r\n"
"        min-width=\"100\" min-height=\"100\" max-width=\"1200\" max-height=\"900\">\r\n"
"    <View flex-direction=\"row\" id=\"Oscillators\">\r\n"
"      <View flex-direction=\"column\" id=\"OSC1\" background-color=\"FF2D2424\">\r\n"
"        <View flex-grow=\"2\">\r\n"
"          <Slider caption=\"Octave 1\" parameter=\"osc1Octave\" caption-placement=\"centred-left\"/>\r\n"
"          <Slider caption=\"Cent 1\" parameter=\"osc1Cent\" caption-placement=\"centred-left\"/>\r\n"
"          <Slider caption=\"Gain 1\" parameter=\"osc1Gain\" caption-placement=\"centred-left\"/>\r\n"
"          <Slider caption=\"PW 1\" parameter=\"osc1PulseWidth\" caption-placement=\"centred-left\"/>\r\n"
"        </View>\r\n"
"        <View flex-grow=\"1.2\">\r\n"
"          <ComboBox caption=\"Waveform 1\" parameter=\"osc1WaveformType\" pos-x=\"-2.04499%\"\r\n"
"                    pos-y=\"-17.5439%\" pos-width=\"100%\" pos-height=\"100%\" flex-grow=\"1.2\"\r\n"
"                    caption-placement=\"centred-left\"/>\r\n"
"        </View>\r\n"
"      </View>\r\n"
"      <View id=\"OSC2\" lookAndFeel=\"FoleysFinest\" flex-direction=\"column\"\r\n"
"            background-color=\"FF2D2424\">\r\n"
"        <View flex-grow=\"2\">\r\n"
"          <Slider caption=\"Octave 2\" parameter=\"osc2Octave\" caption-placement=\"centred-left\"/>\r\n"
"          <Slider caption=\"Cent 2\" parameter=\"osc2Cent\" caption-placement=\"centred-left\"/>\r\n"
"          <Slider caption=\"Gain 2\" parameter=\"osc2Gain\" caption-placement=\"centred-left\"/>\r\n"
"          <Slider caption=\"PW 2\" parameter=\"osc2PulseWidth\" caption-placement=\"centred-left\"/>\r\n"
"        </View>\r\n"
"        <View flex-grow=\"1.2\">\r\n"
"          <ComboBox caption=\"Waveform 2\" parameter=\"osc2WaveformType\" caption-placement=\"centred-left\"/>\r\n"
"        </View>\r\n"
"      </View>\r\n"
"      <View flex-grow=\"0.2\" pos-x=\"94.322%\" pos-y=\"-4.87805%\" pos-width=\"4.83051%\"\r\n"
"            pos-height=\"100%\">\r\n"
"        <Slider caption=\"Noise\" parameter=\"noiseGain\"/>\r\n"
"      </View>\r\n"
"    </View>\r\n"
"    <View id=\"ADSRS\" flex-grow=\"1\" pos-x=\"0%\" pos-y=\"28.5566%\" pos-width=\"100%\"\r\n"
"          pos-height=\"14.2264%\">\r\n"
"      <View id=\"ADSR OSC\" flex-grow=\"1.0\" background-color=\"FF2D2424\">\r\n"
"        <Slider caption=\"ADSR Attack\" parameter=\"adsr1attack\"/>\r\n"
"        <Slider caption=\"ADSR Decay\" parameter=\"adsr1decay\"/>\r\n"
"        <Slider caption=\"ADSR Sustain\" parameter=\"adsr1sustain\"/>\r\n"
"        <Slider caption=\"ADSR Release\" parameter=\"adsr1release\"/>\r\n"
"      </View>\r\n"
"      <View id=\"Filt\" flex-grow=\"1\">\r\n"
"        <View flex-direction=\"column\" background-color=\"FF2D2424\">\r\n"
"          <View>\r\n"
"            <Slider caption=\"Resonance\" parameter=\"resonance\"/>\r\n"
"            <Slider caption=\"Cutoff\" parameter=\"cutoff\"/>\r\n"
"            <Slider caption=\"Filter Amount\" parameter=\"filterAmount\"/>\r\n"
"          </View>\r\n"
"          <View flex-grow=\"0.7\">\r\n"
"            <ComboBox caption=\"Filter Type\" parameter=\"filterType\" caption-placement=\"centred-left\"/>\r\n"
"          </View>\r\n"
"        </View>\r\n"
"      </View>\r\n"
"      <View id=\"ADSR FILT\" flex-grow=\"1\" background-color=\"FF2D2424\">\r\n"
"        <Slider caption=\"Filter Attack\" parameter=\"adsr2attack\"/>\r\n"
"        <Slider caption=\"Filter Decay\" parameter=\"adsr2decay\"/>\r\n"
"        <Slider caption=\"Filter Release\" parameter=\"adsr2release\" pos-x=\"-1.92308%\"\r\n"
"                pos-y=\"-10.3093%\" pos-width=\"25%\" pos-height=\"100%\"/>\r\n"
"        <Slider caption=\"Filter Sustain\" parameter=\"adsr2sustain\"/>\r\n"
"      </View>\r\n"
"    </View>\r\n"
"    <View id=\"Filter + LFO\" pos-x=\"-0.0909918%\" pos-y=\"49.9481%\" pos-width=\"100%\"\r\n"
"          pos-height=\"25.026%\">\r\n"
"      <View id=\"LFO\" flex-direction=\"column\" background-color=\"FF2D2424\">\r\n"
"        <View flex-grow=\"2\">\r\n"
"          <Slider caption=\"Depth\" parameter=\"depthSlider\"/>\r\n"
"          <Slider caption=\"Rate\" parameter=\"rateSlider\"/>\r\n"
"        </View>\r\n"
"        <View flex-grow=\"1.2\">\r\n"
"          <ComboBox caption=\"Source\" parameter=\"sourceComboBox\" caption-placement=\"centred-left\"\r\n"
"                    pos-x=\"-3.92157%\" pos-y=\"-20%\" pos-width=\"49.8039%\" pos-height=\"100%\"/>\r\n"
"          <ComboBox caption=\"Type\" parameter=\"typeComboBox\" caption-placement=\"centred-left\"/>\r\n"
"        </View>\r\n"
"      </View>\r\n"
"      <View id=\"Delay\" background-color=\"FF2D2424\">\r\n"
"        <Slider caption=\"Delay Time\" parameter=\"delayTime\" pos-x=\"-4%\" pos-y=\"-4.97512%\"\r\n"
"                pos-width=\"33.2%\" pos-height=\"100%\"/>\r\n"
"        <Slider caption=\"Feedback\" parameter=\"feedback\"/>\r\n"
"        <Slider caption=\"Delay Mix\" parameter=\"delayMix\"/>\r\n"
"      </View>\r\n"
"      <View id=\"Distortion\" background-color=\"FF2D2424\">\r\n"
"        <Slider caption=\"Distortion Mix\" parameter=\"distortionMix\"/>\r\n"
"        <Slider caption=\"Tone\" parameter=\"tone\"/>\r\n"
"        <Slider caption=\"Drive\" parameter=\"drive\"/>\r\n"
"      </View>\r\n"
"      <View background-color=\"FF2D2424\">\r\n"
"        <Slider caption=\"Master Gain\" parameter=\"masterGain\"/>\r\n"
"      </View>\r\n"
"    </View>\r\n"
"    <View class=\"parameters nomargin\" id=\"Metering\">\r\n"
"      <View display=\"tabbed\" id=\"OSC+Freq\">\r\n"
"        <Plot source=\"waveform\" plot-color=\"FFEF0000\" plot-fill-color=\"00FFFFFF\"/>\r\n"
"        <Plot source=\"analyser\" plot-color=\"FF1300FF\" plot-fill-color=\"FF00B45A\"\r\n"
"              plot-gradient=\"\"/>\r\n"
"      </View>\r\n"
"      <View flex-grow=\"0.1\" id=\"Zoom\" flex-direction=\"column\">\r\n"
"        <View flex-grow=\"0.2\" caption=\"Zoom\">\r\n"
"          <Label/>\r\n"
"        </View>\r\n"
"        <Slider pos-x=\"-13.75%\" pos-y=\"-4.87805%\" pos-width=\"100%\" pos-height=\"82.9268%\"\r\n"
"                parameter=\"oscZoom\" slider-textbox=\"no-textbox\"/>\r\n"
"      </View>\r\n"
"      <View flex-grow=\"0.2\" id=\"HPF\" flex-direction=\"column\">\r\n"
"        <View flex-grow=\"0.3\">\r\n"
"          <Label caption=\"Low Cut\"/>\r\n"
"        </View>\r\n"
"        <Slider parameter=\"hpfCutoff\"/>\r\n"
"      </View>\r\n"
"      <View flex-grow=\"0.1\" id=\"Volume\">\r\n"
"        <Meter source=\"output\"/>\r\n"
"      </View>\r\n"
"    </View>\r\n"
"  </View>\r\n"
"</magic>\r\n";

const char* midius_xml = (const char*) temp_binary_data_0;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes);
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0xe7e31557:  numBytes = 6881; return midius_xml;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "midius_xml"
};

const char* originalFilenames[] =
{
    "midius.xml"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
        if (strcmp (namedResourceList[i], resourceNameUTF8) == 0)
            return originalFilenames[i];

    return nullptr;
}

}
