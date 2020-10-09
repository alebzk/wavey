#include "spectrogram.hpp"
#include <fftw3.h>
#include <chrono>
#include <cmath>
#include <iostream>

namespace {
constexpr float kHannWindow[kInputSize] = {
    0.0f,
    9.43076911874563e-06f,
    3.772272071722593e-05f,
    8.487478753615996e-05f,
    0.00015088519085437735f,
    0.00023575144055643094f,
    0.00033947033522635506f,
    0.0004620379622682358f,
    0.0006034496980542037f,
    0.0007637002080985167f,
    0.0009427834472588992f,
    0.0011406926599646372f,
    0.0013574203804713747f,
    0.0015929584331427216f,
    0.0018472979327587291f,
    0.0021204292848509554f,
    0.0024123421860645644f,
    0.0027230256245468487f,
    0.003052467880362675f,
    0.003400656525936685f,
    0.0037675784265219225f,
    0.004153219740695435f,
    0.00455756592088058f,
    0.004980601713895361f,
    0.005422311161528193f,
    0.0058826776011399184f,
    0.006361683666291973f,
    0.006859311287402081f,
    0.00737554169242538f,
    0.007910355407562741f,
    0.008463732257995571f,
    0.009035651368646702f,
    0.009626091164967598f,
    0.010235029373752758f,
    0.010862443023979595f,
    0.011508308447674975f,
    0.01217260128080816f,
    0.012855296464209687f,
    0.013556368244517059f,
    0.014275790175145908f,
    0.015013535117287646f,
    0.01576957524093342f,
    0.01654388202592383f,
    0.01733642626302484f,
    0.018147178055029578f,
    0.018976106817886196f,
    0.01982318128185151f,
    0.02068836949267089f,
    0.021571638812783123f,
    0.022472955922552162f,
    0.023392286821523722f,
    0.02432959682970809f,
    0.02528485058888813f,
    0.026258012063953118f,
    0.02724904454425836f,
    0.028257910645009876f,
    0.029284572308674595f,
    0.030328990806415934f,
    0.03139112673955513f,
    0.032470940041057106f,
    0.03356838997704226f,
    0.034683435148322506f,
    0.03581603349196372f,
    0.03696614228287187f,
    0.03813371813540528f,
    0.039318717005010495f,
    0.0405210941898847f,
    0.04174080433266136f,
    0.0429778014221216f,
    0.04423203879492943f,
    0.04550346913739256f,
    0.046792044487247f,
    0.04809771623546638f,
    0.0494204351280953f,
    0.050760151268107845f,
    0.05211681411728969f,
    0.05349037249814442f,
    0.054880774595824244f,
    0.05628796796008434f,
    0.05771189950726191f,
    0.059152515522278404f,
    0.06060976166066545f,
    0.06208358295061572f,
    0.063573923795056f,
    0.06508072797374453f,
    0.06660393864539221f,
    0.06814349834980621f,
    0.06969934901005809f,
    0.07127143193467456f,
    0.07285968781985103f,
    0.07446405675168938f,
    0.0760844782084577f,
    0.07772089106287344f,
    0.07937323358440945f,
    0.08104144344162229f,
    0.0827254577045039f,
    0.08442521284685572f,
    0.08614064474868421f,
    0.08787168869862066f,
    0.08961827939636186f,
    0.0913803509551332f,
    0.09315783690417478f,
    0.09495067019124792f,
    0.09675878318516545f,
    0.09858210767834291f,
    0.10042057488937067f,
    0.1022741154656096f,
    0.10414265948580692f,
    0.10602613646273357f,
    0.1079244753458436f,
    0.10983760452395414f,
    0.11176545182794673f,
    0.1137079445334902f,
    0.11566500936378388f,
    0.11763657249232107f,
    0.11962255954567536f,
    0.12162289560630524f,
    0.12363750521538047f,
    0.12566631237562892f,
    0.12770924055420257f,
    0.1297662126855657f,
    0.13183715117440142f,
    0.1339219778985385f,
    0.13602061421189898f,
    0.1381329809474649f,
    0.14025899842026424f,
    0.1423985864303775f,
    0.14455166426596222f,
    0.14671815070629868f,
    0.1488979640248534f,
    0.15109102199236152f,
    0.15329724187992982f,
    0.15551654046215668f,
    0.15774883402027207f,
    0.15999403834529546f,
    0.16225206874121217f,
    0.16452284002816892f,
    0.16680626654568703f,
    0.16910226215589302f,
    0.1714107402467691f,
    0.17373161373541968f,
    0.17606479507135658f,
    0.17841019623980214f,
    0.1807677287650083f,
    0.1831373037135955f,
    0.18551883169790695f,
    0.18791222287937998f,
    0.19031738697193618f,
    0.19273423324538658f,
    0.1951626705288544f,
    0.19760260721421424f,
    0.2000539512595481f,
    0.2025166101926168f,
    0.2049904911143492f,
    0.20747550070234627f,
    0.20997154521440098f,
    0.2124785304920354f,
    0.21499636196405225f,
    0.21752494465010241f,
    0.2200641831642679f,
    0.22261398171865976f,
    0.22517424412703235f,
    0.22774487380841113f,
    0.23032577379073577f,
    0.232916846714519f,
    0.23551799483651875f,
    0.2381291200334255f,
    0.240750123805564f,
    0.24338090728060807f,
    0.24602137121731193f,
    0.24867141600925213f,
    0.2513309416885865f,
    0.25399984792982455f,
    0.2566780340536119f,
    0.25936539903052874f,
    0.2620618414849004f,
    0.26476725969862225f,
    0.26748155161499565f,
    0.2702046148425795f,
    0.2729363466590511f,
    0.27567664401508224f,
    0.2784254035382263f,
    0.28118252153681766f,
    0.2839478940038831f,
    0.28672141662106576f,
    0.2895029847625595f,
    0.29229249349905684f,
    0.29508983760170626f,
    0.2978949115460824f,
    0.30070760951616615f,
    0.303527825408337f,
    0.3063554528353752f,
    0.30919038513047503f,
    0.31203251535126864f,
    0.3148817362838606f,
    0.3177379404468715f,
    0.32060102009549346f,
    0.3234708672255538f,
    0.3263473735775899f,
    0.32923043064093255f,
    0.3321199296577998f,
    0.335015761627399f,
    0.33791781731003967f,
    0.340825987231253f,
    0.34374016168592303f,
    0.3466602307424235f,
    0.3495860842467665f,
    0.3525176118267562f,
    0.3554547028961538f,
    0.358397246658848f,
    0.36134513211303565f,
    0.3642982480554081f,
    0.3672564830853471f,
    0.37021972560912597f,
    0.3731878638441207f,
    0.3761607858230257f,
    0.3791383793980776f,
    0.3821205322452864f,
    0.38510713186867207f,
    0.38809806560450877f,
    0.3910932206255742f,
    0.39409248394540675f,
    0.39709574242256657f,
    0.4001028827649047f,
    0.4031137915338358f,
    0.40612835514861834f,
    0.4091464598906383f,
    0.4121679919076999f,
    0.4151928372183197f,
    0.4182208817160268f,
    0.42125201117366684f,
    0.4242861112477117f,
    0.4273230674825721f,
    0.4303627653149159f,
    0.43340509007798916f,
    0.4364499270059423f,
    0.43949716123815896f,
    0.4425466778235892f,
    0.44559836172508555f,
    0.44865209782374305f,
    0.451707770923241f,
    0.4547652657541897f,
    0.45782446697847756f,
    0.46088525919362305f,
    0.4639475269371272f,
    0.46701115469082993f,
    0.4700760268852669f,
    0.47314202790403026f,
    0.47620904208812886f,
    0.4792769537403523f,
    0.4823456471296344f,
    0.48541500649542013f,
    0.4884849160520311f,
    0.4915552599930345f,
    0.49462592249561094f,
    0.49769678772492415f,
    0.5007677398384901f,
    0.5038386629905474f,
    0.5069094413364271f,
    0.509979959036923f,
    0.5130501002626608f,
    0.5161197491984684f,
    0.5191887900477443f,
    0.5222571070368257f,
    0.5253245844193564f,
    0.5283911064806521f,
    0.5314565575420671f,
    0.5345208219653561f,
    0.5375837841570382f,
    0.5406453285727564f,
    0.5437053397216364f,
    0.5467637021706437f,
    0.5498203005489378f,
    0.5528750195522243f,
    0.5559277439471046f,
    0.558978358575423f,
    0.5620267483586104f,
    0.5650727983020261f,
    0.568116393499295f,
    0.5711574191366425f,
    0.5741957604972264f,
    0.5772313029654629f,
    0.5802639320313514f,
    0.5832935332947936f,
    0.5863199924699095f,
    0.5893431953893482f,
    0.5923630280085947f,
    0.5953793764102722f,
    0.5983921268084392f,
    0.601401165552882f,
    0.6044063791334022f,
    0.6074076541840977f,
    0.6104048774876406f,
    0.6133979359795467f,
    0.6163867167524418f,
    0.6193711070603202f,
    0.6223509943227981f,
    0.6253262661293603f,
    0.6282968102436011f,
    0.6312625146074574f,
    0.634223267345437f,
    0.637178956768838f,
    0.6401294713799617f,
    0.6430746998763204f,
    0.646014531154834f,
    0.6489488543160224f,
    0.6518775586681885f,
    0.6548005337315943f,
    0.6577176692426276f,
    0.6606288551579629f,
    0.6635339816587108f,
    0.6664329391545625f,
    0.6693256182879224f,
    0.6722119099380345f,
    0.675091705225098f,
    0.6779648955143753f,
    0.6808313724202892f,
    0.6836910278105124f,
    0.6865437538100456f,
    0.689389442805288f,
    0.6922279874480959f,
    0.6950592806598327f,
    0.697883215635408f,
    0.7006996858473068f,
    0.7035085850496079f,
    0.7063098072819919f,
    0.7091032468737383f,
    0.7118887984477116f,
    0.714666356924337f,
    0.7174358175255635f,
    0.7201970757788172f,
    0.7229500275209417f,
    0.7256945689021279f,
    0.7284305963898314f,
    0.7311580067726784f,
    0.7338766971643578f,
    0.7365865650075045f,
    0.7392875080775664f,
    0.7419794244866619f,
    0.7446622126874218f,
    0.7473357714768221f,
    0.75f,
    0.7526547977540591f,
    0.755300064591861f,
    0.7579357007258021f,
    0.7605616067315791f,
    0.763177683551939f,
    0.7657838325004159f,
    0.7683799552650534f,
    0.7709659539121143f,
    0.7735417308897735f,
    0.7761071890317994f,
    0.7786622315612182f,
    0.7812067620939651f,
    0.7837406846425202f,
    0.7862639036195298f,
    0.7887763238414109f,
    0.7912778505319435f,
    0.7937683893258453f,
    0.7962478462723306f,
    0.7987161278386554f,
    0.8011731409136453f,
    0.8036187928112086f,
    0.8060529912738315f,
    0.808475644476059f,
    0.8108866610279594f,
    0.8132859499785707f,
    0.8156734208193328f,
    0.8180489834874999f,
    0.8204125483695401f,
    0.8227640263045148f,
    0.8251033285874421f,
    0.8274303669726426f,
    0.8297450536770695f,
    0.8320473013836195f,
    0.8343370232444262f,
    0.8366141328841366f,
    0.8388785444031694f,
    0.8411301723809563f,
    0.8433689318791626f,
    0.8455947384448924f,
    0.8478075081138745f,
    0.8500071574136295f,
    0.8521936033666189f,
    0.8543667634933743f,
    0.8565265558156101f,
    0.8586728988593157f,
    0.8608057116578283f,
    0.8629249137548874f,
    0.8650304252076703f,
    0.8671221665898072f,
    0.8692000589943785f,
    0.8712640240368899f,
    0.8733139838582299f,
    0.8753498611276077f,
    0.8773715790454695f,
    0.8793790613463954f,
    0.8813722323019773f,
    0.8833510167236746f,
    0.8853153399656513f,
    0.8872651279275916f,
    0.889200307057495f,
    0.8911208043544511f,
    0.8930265473713936f,
    0.8949174642178332f,
    0.8967934835625688f,
    0.898654534636379f,
    0.9005005472346923f,
    0.902331451720234f,
    0.9041471790256541f,
    0.9059476606561327f,
    0.9077328286919637f,
    0.9095026157911167f,
    0.9112569551917773f,
    0.9129957807148665f,
    0.9147190267665359f,
    0.9164266283406433f,
    0.9181185210212034f,
    0.9197946409848194f,
    0.9214549250030899f,
    0.9230993104449938f,
    0.9247277352792533f,
    0.9263401380766738f,
    0.9279364580124613f,
    0.9295166348685169f,
    0.9310806090357081f,
    0.9326283215161177f,
    0.9341597139252698f,
    0.935674728494331f,
    0.937173308072291f,
    0.9386553961281179f,
    0.9401209367528905f,
    0.9415698746619079f,
    0.9430021551967743f,
    0.9444177243274617f,
    0.9458165286543475f,
    0.9471985154102291f,
    0.9485636324623146f,
    0.9499118283141886f,
    0.9512430521077564f,
    0.9525572536251605f,
    0.9538543832906772f,
    0.9551343921725843f,
    0.9563972319850091f,
    0.9576428550897489f,
    0.958871214498068f,
    0.9600822638724704f,
    0.9612759575284482f,
    0.9624522504362039f,
    0.9636110982223505f,
    0.9647524571715842f,
    0.9658762842283342f,
    0.9669825369983864f,
    0.968071173750483f,
    0.9691421534178966f,
    0.9701954355999791f,
    0.9712309805636863f,
    0.9722487492450764f,
    0.9732487032507837f,
    0.9742308048594664f,
    0.9751950170232309f,
    0.9761413033690276f,
    0.9770696282000244f,
    0.9779799564969529f,
    0.9788722539194289f,
    0.9797464868072487f,
    0.9806026221816581f,
    0.9814406277465968f,
    0.982260471889917f,
    0.9830621236845754f,
    0.9838455528897998f,
    0.9846107299522304f,
    0.9853576260070347f,
    0.9860862128789953f,
    0.9867964630835742f,
    0.9874883498279483f,
    0.9881618470120215f,
    0.9888169292294076f,
    0.9894535717683901f,
    0.990071750612854f,
    0.9906714424431913f,
    0.9912526246371813f,
    0.9918152752708437f,
    0.9923593731192655f,
    0.9928848976574018f,
    0.9933918290608507f,
    0.9938801482065998f,
    0.9943498366737487f,
    0.9948008767442034f,
    0.9952332514033447f,
    0.9956469443406706f,
    0.9960419399504106f,
    0.9964182233321149f,
    0.996775780291217f,
    0.9971145973395684f,
    0.9974346616959475f,
    0.9977359612865423f,
    0.9980184847454051f,
    0.998282221414882f,
    0.9985271613460144f,
    0.9987532952989144f,
    0.9989606147431138f,
    0.9991491118578855f,
    0.999318779532538f,
    0.9994696113666849f,
    0.9996016016704854f,
    0.999714745464859f,
    0.9998090384816739f,
    0.9998844771639073f,
    0.99994105866578f,
    0.9999787808528637f,
    0.9999976423021616f,
    0.9999976423021616f,
    0.9999787808528637f,
    0.99994105866578f,
    0.9998844771639073f,
    0.9998090384816739f,
    0.999714745464859f,
    0.9996016016704854f,
    0.9994696113666849f,
    0.999318779532538f,
    0.9991491118578855f,
    0.9989606147431139f,
    0.9987532952989144f,
    0.9985271613460144f,
    0.998282221414882f,
    0.9980184847454051f,
    0.9977359612865423f,
    0.9974346616959475f,
    0.9971145973395684f,
    0.9967757802912172f,
    0.9964182233321149f,
    0.9960419399504106f,
    0.9956469443406706f,
    0.9952332514033447f,
    0.9948008767442034f,
    0.9943498366737487f,
    0.9938801482065998f,
    0.9933918290608508f,
    0.9928848976574018f,
    0.9923593731192655f,
    0.9918152752708437f,
    0.9912526246371813f,
    0.9906714424431913f,
    0.990071750612854f,
    0.9894535717683901f,
    0.9888169292294076f,
    0.9881618470120215f,
    0.9874883498279483f,
    0.9867964630835742f,
    0.9860862128789953f,
    0.9853576260070347f,
    0.9846107299522304f,
    0.9838455528897998f,
    0.9830621236845754f,
    0.982260471889917f,
    0.9814406277465968f,
    0.9806026221816581f,
    0.9797464868072487f,
    0.978872253919429f,
    0.9779799564969529f,
    0.9770696282000244f,
    0.9761413033690276f,
    0.9751950170232309f,
    0.9742308048594664f,
    0.9732487032507837f,
    0.9722487492450764f,
    0.9712309805636863f,
    0.9701954355999791f,
    0.9691421534178966f,
    0.968071173750483f,
    0.9669825369983864f,
    0.9658762842283342f,
    0.9647524571715842f,
    0.9636110982223505f,
    0.962452250436204f,
    0.9612759575284482f,
    0.9600822638724704f,
    0.958871214498068f,
    0.957642855089749f,
    0.9563972319850091f,
    0.9551343921725843f,
    0.9538543832906772f,
    0.9525572536251606f,
    0.9512430521077564f,
    0.9499118283141886f,
    0.9485636324623146f,
    0.9471985154102291f,
    0.9458165286543475f,
    0.9444177243274617f,
    0.9430021551967743f,
    0.9415698746619079f,
    0.9401209367528905f,
    0.9386553961281179f,
    0.937173308072291f,
    0.935674728494331f,
    0.9341597139252698f,
    0.9326283215161177f,
    0.9310806090357081f,
    0.9295166348685169f,
    0.9279364580124614f,
    0.9263401380766738f,
    0.9247277352792533f,
    0.9230993104449938f,
    0.9214549250030899f,
    0.9197946409848194f,
    0.9181185210212034f,
    0.9164266283406433f,
    0.9147190267665362f,
    0.9129957807148665f,
    0.9112569551917773f,
    0.9095026157911167f,
    0.9077328286919638f,
    0.9059476606561327f,
    0.9041471790256541f,
    0.902331451720234f,
    0.9005005472346924f,
    0.898654534636379f,
    0.8967934835625688f,
    0.8949174642178332f,
    0.8930265473713939f,
    0.8911208043544511f,
    0.889200307057495f,
    0.8872651279275916f,
    0.8853153399656514f,
    0.8833510167236746f,
    0.8813722323019773f,
    0.8793790613463954f,
    0.8773715790454696f,
    0.8753498611276077f,
    0.8733139838582299f,
    0.8712640240368899f,
    0.8692000589943787f,
    0.8671221665898072f,
    0.8650304252076703f,
    0.8629249137548874f,
    0.8608057116578284f,
    0.8586728988593157f,
    0.8565265558156101f,
    0.8543667634933743f,
    0.8521936033666189f,
    0.8500071574136296f,
    0.8478075081138745f,
    0.8455947384448924f,
    0.8433689318791626f,
    0.8411301723809564f,
    0.8388785444031694f,
    0.8366141328841366f,
    0.8343370232444262f,
    0.8320473013836196f,
    0.8297450536770695f,
    0.8274303669726426f,
    0.8251033285874418f,
    0.8227640263045148f,
    0.8204125483695401f,
    0.8180489834874999f,
    0.8156734208193328f,
    0.813285949978571f,
    0.8108866610279595f,
    0.8084756444760592f,
    0.8060529912738312f,
    0.8036187928112086f,
    0.8011731409136453f,
    0.7987161278386554f,
    0.7962478462723306f,
    0.7937683893258454f,
    0.7912778505319438f,
    0.788776323841411f,
    0.7862639036195296f,
    0.7837406846425202f,
    0.7812067620939651f,
    0.7786622315612182f,
    0.7761071890317994f,
    0.7735417308897736f,
    0.7709659539121144f,
    0.7683799552650536f,
    0.7657838325004158f,
    0.763177683551939f,
    0.7605616067315791f,
    0.7579357007258021f,
    0.755300064591861f,
    0.7526547977540593f,
    0.7500000000000002f,
    0.7473357714768223f,
    0.744662212687422f,
    0.7419794244866617f,
    0.7392875080775664f,
    0.7365865650075045f,
    0.7338766971643578f,
    0.7311580067726784f,
    0.7284305963898317f,
    0.7256945689021281f,
    0.722950027520942f,
    0.7201970757788171f,
    0.7174358175255635f,
    0.714666356924337f,
    0.7118887984477116f,
    0.7091032468737383f,
    0.706309807281992f,
    0.7035085850496081f,
    0.700699685847307f,
    0.6978832156354078f,
    0.6950592806598326f,
    0.6922279874480959f,
    0.689389442805288f,
    0.6865437538100457f,
    0.6836910278105125f,
    0.6808313724202895f,
    0.6779648955143756f,
    0.675091705225098f,
    0.6722119099380344f,
    0.6693256182879224f,
    0.6664329391545625f,
    0.6635339816587109f,
    0.660628855157963f,
    0.6577176692426279f,
    0.6548005337315945f,
    0.6518775586681884f,
    0.6489488543160223f,
    0.646014531154834f,
    0.6430746998763204f,
    0.6401294713799619f,
    0.637178956768838f,
    0.6342232673454372f,
    0.6312625146074576f,
    0.6282968102436008f,
    0.6253262661293602f,
    0.622350994322798f,
    0.6193711070603202f,
    0.6163867167524418f,
    0.6133979359795468f,
    0.6104048774876407f,
    0.6074076541840979f,
    0.6044063791334024f,
    0.6014011655528819f,
    0.5983921268084391f,
    0.5953793764102722f,
    0.5923630280085947f,
    0.5893431953893483f,
    0.5863199924699096f,
    0.5832935332947938f,
    0.5802639320313516f,
    0.5772313029654628f,
    0.5741957604972263f,
    0.5711574191366425f,
    0.568116393499295f,
    0.5650727983020262f,
    0.5620267483586106f,
    0.5589783585754232f,
    0.5559277439471049f,
    0.5528750195522242f,
    0.5498203005489377f,
    0.5467637021706437f,
    0.5437053397216364f,
    0.5406453285727564f,
    0.5375837841570383f,
    0.5345208219653563f,
    0.5314565575420673f,
    0.528391106480652f,
    0.5253245844193563f,
    0.5222571070368257f,
    0.5191887900477443f,
    0.5161197491984685f,
    0.513050100262661f,
    0.5099799590369232f,
    0.5069094413364273f,
    0.5038386629905472f,
    0.50076773983849f,
    0.49769678772492404f,
    0.49462592249561094f,
    0.4915552599930345f,
    0.4884849160520312f,
    0.48541500649542024f,
    0.4823456471296346f,
    0.4792769537403525f,
    0.47620904208812875f,
    0.47314202790403015f,
    0.4700760268852669f,
    0.46701115469082993f,
    0.4639475269371273f,
    0.46088525919362316f,
    0.4578244669784778f,
    0.4547652657541899f,
    0.4517077709232409f,
    0.44865209782374293f,
    0.44559836172508555f,
    0.4425466778235892f,
    0.4394971612381591f,
    0.43644992700594243f,
    0.4334050900779894f,
    0.43036276531491613f,
    0.427323067482572f,
    0.4242861112477116f,
    0.42125201117366684f,
    0.4182208817160268f,
    0.4151928372183198f,
    0.41216799190770004f,
    0.4091464598906385f,
    0.40612835514861856f,
    0.4031137915338357f,
    0.4001028827649046f,
    0.39709574242256657f,
    0.39409248394540675f,
    0.39109322062557433f,
    0.3880980656045089f,
    0.3851071318686723f,
    0.38212053224528664f,
    0.3791383793980775f,
    0.3761607858230256f,
    0.3731878638441207f,
    0.37021972560912597f,
    0.3672564830853471f,
    0.3642982480554082f,
    0.36134513211303576f,
    0.3583972466588482f,
    0.355454702896154f,
    0.3525176118267561f,
    0.3495860842467664f,
    0.3466602307424235f,
    0.34374016168592303f,
    0.3408259872312531f,
    0.3379178173100398f,
    0.3350157616273992f,
    0.3321199296578f,
    0.32923043064093244f,
    0.3263473735775898f,
    0.3234708672255538f,
    0.32060102009549346f,
    0.3177379404468716f,
    0.31488173628386074f,
    0.31203251535126886f,
    0.30919038513047525f,
    0.3063554528353751f,
    0.30352782540833695f,
    0.30070760951616615f,
    0.2978949115460824f,
    0.2950898376017064f,
    0.29229249349905695f,
    0.28950298476255965f,
    0.2867214166210659f,
    0.2839478940038831f,
    0.28118252153681766f,
    0.2784254035382263f,
    0.27567664401508224f,
    0.2729363466590511f,
    0.2702046148425795f,
    0.26748155161499587f,
    0.26476725969862247f,
    0.2620618414849004f,
    0.25936539903052874f,
    0.2566780340536119f,
    0.25399984792982455f,
    0.2513309416885865f,
    0.24867141600925213f,
    0.24602137121731193f,
    0.24338090728060824f,
    0.2407501238055642f,
    0.2381291200334255f,
    0.23551799483651875f,
    0.232916846714519f,
    0.23032577379073577f,
    0.22774487380841113f,
    0.22517424412703252f,
    0.22261398171865998f,
    0.22006418316426807f,
    0.21752494465010241f,
    0.21499636196405225f,
    0.2124785304920354f,
    0.20997154521440098f,
    0.20747550070234627f,
    0.20499049111434936f,
    0.20251661019261702f,
    0.20005395125954833f,
    0.19760260721421424f,
    0.1951626705288544f,
    0.19273423324538658f,
    0.19031738697193618f,
    0.18791222287937998f,
    0.18551883169790695f,
    0.18313730371359566f,
    0.18076772876500846f,
    0.17841019623980198f,
    0.17606479507135658f,
    0.17373161373541968f,
    0.1714107402467691f,
    0.16910226215589302f,
    0.16680626654568703f,
    0.16452284002816908f,
    0.16225206874121229f,
    0.15999403834529535f,
    0.15774883402027207f,
    0.15551654046215668f,
    0.15329724187992982f,
    0.15109102199236152f,
    0.1488979640248534f,
    0.14671815070629884f,
    0.1445516642659624f,
    0.14239858643037767f,
    0.14025899842026424f,
    0.1381329809474649f,
    0.13602061421189898f,
    0.1339219778985385f,
    0.13183715117440142f,
    0.12976621268556587f,
    0.12770924055420274f,
    0.12566631237562909f,
    0.12363750521538047f,
    0.12162289560630524f,
    0.11962255954567536f,
    0.11763657249232107f,
    0.11566500936378388f,
    0.11370794453349037f,
    0.1117654518279469f,
    0.1098376045239543f,
    0.1079244753458436f,
    0.10602613646273357f,
    0.10414265948580692f,
    0.1022741154656096f,
    0.10042057488937067f,
    0.09858210767834291f,
    0.09675878318516562f,
    0.09495067019124803f,
    0.09315783690417467f,
    0.0913803509551332f,
    0.08961827939636186f,
    0.08787168869862066f,
    0.08614064474868421f,
    0.08442521284685572f,
    0.08272545770450401f,
    0.0810414434416224f,
    0.07937323358440934f,
    0.07772089106287344f,
    0.0760844782084577f,
    0.07446405675168938f,
    0.07285968781985103f,
    0.07127143193467456f,
    0.0696993490100582f,
    0.06814349834980632f,
    0.0666039386453921f,
    0.06508072797374453f,
    0.063573923795056f,
    0.06208358295061572f,
    0.06060976166066545f,
    0.059152515522278404f,
    0.05771189950726202f,
    0.05628796796008445f,
    0.054880774595824355f,
    0.05349037249814442f,
    0.05211681411728969f,
    0.050760151268107845f,
    0.0494204351280953f,
    0.04809771623546638f,
    0.04679204448724711f,
    0.04550346913739267f,
    0.044232038794929485f,
    0.04297780142212149f,
    0.04174080433266136f,
    0.0405210941898847f,
    0.039318717005010495f,
    0.03813371813540528f,
    0.03696614228287198f,
    0.03581603349196377f,
    0.03468343514832262f,
    0.0335683899770422f,
    0.032470940041057106f,
    0.03139112673955513f,
    0.030328990806415934f,
    0.029284572308674595f,
    0.02825791064500993f,
    0.02724904454425847f,
    0.026258012063953173f,
    0.025284850588888075f,
    0.02432959682970809f,
    0.023392286821523722f,
    0.022472955922552162f,
    0.021571638812783123f,
    0.02068836949267089f,
    0.019823181281851565f,
    0.01897610681788625f,
    0.018147178055029523f,
    0.01733642626302484f,
    0.01654388202592383f,
    0.01576957524093342f,
    0.015013535117287646f,
    0.014275790175145908f,
    0.013556368244517114f,
    0.012855296464209742f,
    0.01217260128080816f,
    0.011508308447674975f,
    0.010862443023979595f,
    0.010235029373752758f,
    0.009626091164967598f,
    0.009035651368646702f,
    0.008463732257995626f,
    0.007910355407562797f,
    0.00737554169242538f,
    0.006859311287402081f,
    0.006361683666291973f,
    0.0058826776011399184f,
    0.005422311161528193f,
    0.004980601713895361f,
    0.00455756592088058f,
    0.004153219740695491f,
    0.0037675784265219225f,
    0.0034006565259366295f,
    0.003052467880362675f,
    0.0027230256245468487f,
    0.0024123421860645644f,
    0.0021204292848509554f,
    0.0018472979327587291f,
    0.0015929584331427216f,
    0.0013574203804713747f,
    0.0011406926599646372f,
    0.0009427834472588992f,
    0.0007637002080985167f,
    0.0006034496980542037f,
    0.0004620379622682358f,
    0.00033947033522635506f,
    0.00023575144055643094f,
    0.00015088519085437735f,
    8.487478753615996e-05f,
    3.772272071722593e-05f,
    9.43076911874563e-06f,
    0.0f,
};

constexpr float kLogMin = -10.f;
constexpr float kLogMax = 2.f;
constexpr float kOffset = kLogMin;
constexpr float kScale = 1.f / (kLogMax - kLogMin);
}  // namespace

Spectrogram::Spectrogram(const float* samples, int num_channels, int num_frames) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    float* input_buffer = static_cast<float*>(fftwf_malloc(kInputSize * sizeof(float)));
    fftwf_complex* output_buffer =
        static_cast<fftwf_complex*>(fftwf_malloc(kOutputSize * sizeof(fftwf_complex)));

    fftwf_plan plan = fftwf_plan_dft_r2c_1d(kInputSize, input_buffer, output_buffer, FFTW_ESTIMATE);

    power_spectra.resize(num_channels);

    for (int c = 0; c < num_channels; c++) {
        auto& power_spectra_channel = power_spectra[c];
        const int num_spectra_per_chanel =
            std::max((num_frames - kInputAdvance) / kInputAdvance, 0);
        power_spectra_channel.resize(num_spectra_per_chanel);

        for (int i = 0; i < num_spectra_per_chanel; i++) {
            // Fill input buffer and apply Hann window.
            int src_idx = i * kInputAdvance * num_channels + c;
            for (int k = 0; k < kInputSize; k++) {
                input_buffer[k] = samples[src_idx] * kHannWindow[k];
                src_idx += num_channels;
            }

            // Transform.
            fftwf_execute(plan);

            // Power spectrum.
            auto& power = power_spectra_channel[i];
            for (int k = 0; k < kOutputSize; k++) {
                const float re = output_buffer[k][0];
                const float im = output_buffer[k][1];
                const float p = log10(re * re + im * im);
                power[k] = std::max(std::min((p - kOffset) * kScale, 1.f), 0.f);
            }
        }
    }

    fftwf_destroy_plan(plan);
    fftwf_free(input_buffer);
    fftwf_free(output_buffer);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cerr << "Power spectrum computed in "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms"
              << std::endl;
}
