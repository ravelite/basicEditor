#include "chuckhighlighter.h"

ChuckHighlighter::ChuckHighlighter(QTextDocument *document):
    SuperWordHighlighter(document)
{
    HighlightingRule rule;

    //DO STRINGS HERE
    /* quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule); */

    //QRegExp("\".*\"");
    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::gray);
    rule.pattern = QRegExp("\"(?:(?!\\/\\/).)+\"");

    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    //NUMBER LITERALS?



    //DO KEYWORDS HERE

    QTextCharFormat kw1Format;
    kw1Format.setForeground(Qt::darkBlue);
    kw1Format.setFontWeight(QFont::Bold);

    QString kw1 =
            "int,float,time,dur,void,same,if,else,while,until,for,repeat,"
            "break,continue,return,switch,class,extends,public,static,pure,"
            "this,super,interface,implements,protected,private,function,fun,spork,const,new,"
            "now,true,false,maybe,null,NULL,me,pi,samp,ms,second,minute,hour,day,week,"
            "dac,adc,blackhole";

    QStringList kw1Patterns = kw1.split(",");
    keywordPatternsFormat(kw1Patterns, kw1Format);

    QString kw2 = "Object,string,UAnaBlob,Shred,Thread,Class,Event,"
            "IO,FileIO,SerialIO,StdOut,StdErr,Windowing,Machine,Std,KBHit,"
            "ConsoleInput,StringTokenizer,RegEx,Math,OscSend,OscEvent,OscRecv "
            "MidiMsg,MidiIn,MidiOut,MidiRW,MidiMsgIn,MidiMsgOut,MidiFileIn,"
            "HidMsg,Hid";
    QStringList kw2Patterns = kw2.split(",");

    QTextCharFormat kw2Format;
    kw2Format.setForeground(Qt::darkGreen);
    kw2Format.setFontWeight(QFont::Bold);

    keywordPatternsFormat(kw2Patterns, kw2Format);

    QString kw3 = "UGen,UAna,Osc,Phasor,SinOsc,TriOsc,SawOsc,PulseOsc,SqrOsc,"
            "GenX,Gen5,Gen7,Pan2,Gen9,Gen10,Gen17,CurveTable,WarpTable,Chubgraph,"
            "Chugen,UGen_Stereo,UGen_Multi,DAC,ADC,Mix2,Gain,Noise,CNoise,Impulse,"
            "Step,HalfRect,FullRect,DelayP,SndBuf,SndBuf2,Dyno,LiSa,FilterBasic,"
            "BPF,BRF,LPF,HPF,ResonZ,BiQuad,Teabox,StkInstrument,BandedWG,BlowBotl,BlowHole,"
            "Bowed,Brass,Clarinet,Flute,Mandolin,ModalBar,Moog,Saxofony,Shakers,Sitar,StifKarp,"
            "VoicForm,FM,BeeThree,FMVoices,HevyMetl,PercFlut,Rhodey,TubeBell,Wurley,Delay,"
            "DelayA,DelayL,Echo,Envelope,ADSR,FilterStk,OnePole,TwoPole,OneZero,TwoZero,PoleZero,"
            "JCRev,NRev,PRCRev,Chorus,Modulate,PitShift,SubNoise,WvIn,WaveLoop,WvOut,WvOut2,"
            "BLT,BlitSquare,Blit,BlitSaw,JetTabl,Mesh2D,FFT,IFFT,Flip,pilF,DCT,IDCT,FeatureCollector,"
            "Centroid,Flux,RMS,RollOff,AutoCorr,XCorr,ZeroX";
    QStringList kw3Patterns = kw3.split(",");

    QTextCharFormat kw3Format;
    kw3Format.setForeground(Qt::darkCyan);
    kw3Format.setFontWeight(QFont::Bold);

    keywordPatternsFormat(kw3Patterns, kw3Format);

    QString kw4 = "TimeGrid,GTimeGrid,Scale,AutoOut,Drum,OGen,"
           "PanOut,LimitOut,Clip,Fade,MidiInst,Wiidata,PitchClip";
    QStringList kw4Patterns = kw4.split(",");

    QString scaleStr = "scale,arp,tri,triL,min,har,asc,nea,maj,mixo,dor,lyd,pent,jewish,"
            "gypsy,arabic,whole_tone,dim,ind,neap";
    QStringList kw5Patterns = scaleStr.split(",");

    QTextCharFormat kw4Format;
    kw4Format.setForeground(Qt::darkMagenta);
    kw4Format.setFontWeight(QFont::Bold);

    keywordPatternsFormat(kw4Patterns, kw4Format);
    keywordPatternsFormat(kw5Patterns, kw4Format);

    //DO COMMENTS HERE

    //single line comment rule
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor::fromHsv(120,255,200));
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    //multiline comments dealt with in parent

} //end of ChuckHighlighter constructor

