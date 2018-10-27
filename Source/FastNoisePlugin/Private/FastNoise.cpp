// UFastNoise.cpp
//
// MIT License
//
// Copyright(c) 2017 Jordan Peck
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The developer's email is jorzixdan.me2@gzixmail.com (for great email, take
// off every 'zix'.)
//

#include "FastNoise.h"

#include <math.h>
#include <assert.h>

#include <algorithm>
#include <random>


const float GRAD_X[] =
{
	1, -1, 1, -1,
	1, -1, 1, -1,
	0, 0, 0, 0
};
const float GRAD_Y[] =
{
	1, 1, -1, -1,
	0, 0, 0, 0,
	1, -1, 1, -1
};
const float GRAD_Z[] =
{
	0, 0, 0, 0,
	1, 1, -1, -1,
	1, 1, -1, -1
};

const float GRAD_4D[] =
{
	0,1,1,1,0,1,1,-1,0,1,-1,1,0,1,-1,-1,
	0,-1,1,1,0,-1,1,-1,0,-1,-1,1,0,-1,-1,-1,
	1,0,1,1,1,0,1,-1,1,0,-1,1,1,0,-1,-1,
	-1,0,1,1,-1,0,1,-1,-1,0,-1,1,-1,0,-1,-1,
	1,1,0,1,1,1,0,-1,1,-1,0,1,1,-1,0,-1,
	-1,1,0,1,-1,1,0,-1,-1,-1,0,1,-1,-1,0,-1,
	1,1,1,0,1,1,-1,0,1,-1,1,0,1,-1,-1,0,
	-1,1,1,0,-1,1,-1,0,-1,-1,1,0,-1,-1,-1,0
};

const float VAL_LUT[] =
{
	float(0.3490196078), float(0.4352941176), float(-0.4509803922), float(0.6392156863), float(0.5843137255), float(-0.1215686275), float(0.7176470588), float(-0.1058823529), float(0.3960784314), float(0.0431372549), float(-0.03529411765), float(0.3176470588), float(0.7254901961), float(0.137254902), float(0.8588235294), float(-0.8196078431),
	float(-0.7960784314), float(-0.3333333333), float(-0.6705882353), float(-0.3882352941), float(0.262745098), float(0.3254901961), float(-0.6470588235), float(-0.9215686275), float(-0.5294117647), float(0.5294117647), float(-0.4666666667), float(0.8117647059), float(0.3803921569), float(0.662745098), float(0.03529411765), float(-0.6156862745),
	float(-0.01960784314), float(-0.3568627451), float(-0.09019607843), float(0.7490196078), float(0.8352941176), float(-0.4039215686), float(-0.7490196078), float(0.9529411765), float(-0.0431372549), float(-0.9294117647), float(-0.6549019608), float(0.9215686275), float(-0.06666666667), float(-0.4431372549), float(0.4117647059), float(-0.4196078431),
	float(-0.7176470588), float(-0.8117647059), float(-0.2549019608), float(0.4901960784), float(0.9137254902), float(0.7882352941), float(-1.0), float(-0.4745098039), float(0.7960784314), float(0.8509803922), float(-0.6784313725), float(0.4588235294), float(1.0), float(-0.1843137255), float(0.4509803922), float(0.1450980392),
	float(-0.231372549), float(-0.968627451), float(-0.8588235294), float(0.4274509804), float(0.003921568627), float(-0.003921568627), float(0.2156862745), float(0.5058823529), float(0.7647058824), float(0.2078431373), float(-0.5921568627), float(0.5764705882), float(-0.1921568627), float(-0.937254902), float(0.08235294118), float(-0.08235294118),
	float(0.9058823529), float(0.8274509804), float(0.02745098039), float(-0.168627451), float(-0.7803921569), float(0.1137254902), float(-0.9450980392), float(0.2), float(0.01960784314), float(0.5607843137), float(0.2705882353), float(0.4431372549), float(-0.9607843137), float(0.6156862745), float(0.9294117647), float(-0.07450980392),
	float(0.3098039216), float(0.9921568627), float(-0.9137254902), float(-0.2941176471), float(-0.3411764706), float(-0.6235294118), float(-0.7647058824), float(-0.8901960784), float(0.05882352941), float(0.2392156863), float(0.7333333333), float(0.6549019608), float(0.2470588235), float(0.231372549), float(-0.3960784314), float(-0.05098039216),
	float(-0.2235294118), float(-0.3725490196), float(0.6235294118), float(0.7019607843), float(-0.8274509804), float(0.4196078431), float(0.07450980392), float(0.8666666667), float(-0.537254902), float(-0.5058823529), float(-0.8039215686), float(0.09019607843), float(-0.4823529412), float(0.6705882353), float(-0.7882352941), float(0.09803921569),
	float(-0.6078431373), float(0.8039215686), float(-0.6), float(-0.3254901961), float(-0.4117647059), float(-0.01176470588), float(0.4823529412), float(0.168627451), float(0.8745098039), float(-0.3647058824), float(-0.1607843137), float(0.568627451), float(-0.9921568627), float(0.9450980392), float(0.5137254902), float(0.01176470588),
	float(-0.1450980392), float(-0.5529411765), float(-0.5764705882), float(-0.1137254902), float(0.5215686275), float(0.1607843137), float(0.3725490196), float(-0.2), float(-0.7254901961), float(0.631372549), float(0.7098039216), float(-0.568627451), float(0.1294117647), float(-0.3098039216), float(0.7411764706), float(-0.8509803922),
	float(0.2549019608), float(-0.6392156863), float(-0.5607843137), float(-0.3176470588), float(0.937254902), float(0.9843137255), float(0.5921568627), float(0.6941176471), float(0.2862745098), float(-0.5215686275), float(0.1764705882), float(0.537254902), float(-0.4901960784), float(-0.4588235294), float(-0.2078431373), float(-0.2156862745),
	float(0.7725490196), float(0.3647058824), float(-0.2392156863), float(0.2784313725), float(-0.8823529412), float(0.8980392157), float(0.1215686275), float(0.1058823529), float(-0.8745098039), float(-0.9843137255), float(-0.7019607843), float(0.9607843137), float(0.2941176471), float(0.3411764706), float(0.1529411765), float(0.06666666667),
	float(-0.9764705882), float(0.3019607843), float(0.6470588235), float(-0.5843137255), float(0.05098039216), float(-0.5137254902), float(-0.137254902), float(0.3882352941), float(-0.262745098), float(-0.3019607843), float(-0.1764705882), float(-0.7568627451), float(0.1843137255), float(-0.5450980392), float(-0.4980392157), float(-0.2784313725),
	float(-0.9529411765), float(-0.09803921569), float(0.8901960784), float(-0.2862745098), float(-0.3803921569), float(0.5529411765), float(0.7803921569), float(-0.8352941176), float(0.6862745098), float(0.7568627451), float(0.4980392157), float(-0.6862745098), float(-0.8980392157), float(-0.7725490196), float(-0.7098039216), float(-0.2470588235),
	float(-0.9058823529), float(0.9764705882), float(0.1921568627), float(0.8431372549), float(-0.05882352941), float(0.3568627451), float(0.6078431373), float(0.5450980392), float(0.4039215686), float(-0.7333333333), float(-0.4274509804), float(0.6), float(0.6784313725), float(-0.631372549), float(-0.02745098039), float(-0.1294117647),
	float(0.3333333333), float(-0.8431372549), float(0.2235294118), float(-0.3490196078), float(-0.6941176471), float(0.8823529412), float(0.4745098039), float(0.4666666667), float(-0.7411764706), float(-0.2705882353), float(0.968627451), float(0.8196078431), float(-0.662745098), float(-0.4352941176), float(-0.8666666667), float(-0.1529411765),
};

const float CELL_2D_X[] =
{
	float(-0.6440658039), float(-0.08028078721), float(0.9983546168), float(0.9869492062), float(0.9284746418), float(0.6051097552), float(-0.794167404), float(-0.3488667991), float(-0.943136526), float(-0.9968171318), float(0.8740961579), float(0.1421139764), float(0.4282553608), float(-0.9986665833), float(0.9996760121), float(-0.06248383632),
	float(0.7120139305), float(0.8917660409), float(0.1094842955), float(-0.8730880804), float(0.2594811489), float(-0.6690063346), float(-0.9996834972), float(-0.8803608671), float(-0.8166554937), float(0.8955599676), float(-0.9398321388), float(0.07615451399), float(-0.7147270565), float(0.8707354457), float(-0.9580008579), float(0.4905965632),
	float(0.786775944), float(0.1079711577), float(0.2686638979), float(0.6113487322), float(-0.530770584), float(-0.7837268286), float(-0.8558691039), float(-0.5726093896), float(-0.9830740914), float(0.7087766359), float(0.6807027153), float(-0.08864708788), float(0.6704485923), float(-0.1350735482), float(-0.9381333003), float(0.9756655376),
	float(0.4231433671), float(-0.4959787385), float(0.1005554325), float(-0.7645857281), float(-0.5859053796), float(-0.9751154306), float(-0.6972258572), float(0.7907012002), float(-0.9109899213), float(-0.9584307894), float(-0.8269529333), float(0.2608264719), float(-0.7773760119), float(0.7606456974), float(-0.8961083758), float(-0.9838134719),
	float(0.7338893576), float(0.2161226729), float(0.673509891), float(-0.5512056873), float(0.6899744332), float(0.868004831), float(0.5897430311), float(-0.8950444221), float(-0.3595752773), float(0.8209486981), float(-0.2912360132), float(-0.9965011374), float(0.9766994634), float(0.738790822), float(-0.4730947722), float(0.8946479441),
	float(-0.6943628971), float(-0.6620468182), float(-0.0887255502), float(-0.7512250855), float(-0.5322986898), float(0.5226295385), float(0.2296318375), float(0.7915307344), float(-0.2756485999), float(-0.6900234522), float(0.07090588086), float(0.5981278485), float(0.3033429312), float(-0.7253142797), float(-0.9855874307), float(-0.1761843396),
	float(-0.6438468325), float(-0.9956136595), float(0.8541580762), float(-0.9999807666), float(-0.02152416253), float(-0.8705983095), float(-0.1197138014), float(-0.992107781), float(-0.9091181546), float(0.788610536), float(-0.994636402), float(0.4211256853), float(0.3110430857), float(-0.4031127839), float(0.7610684239), float(0.7685674467),
	float(0.152271555), float(-0.9364648723), float(0.1681333739), float(-0.3567427907), float(-0.418445483), float(-0.98774778), float(0.8705250765), float(-0.8911701067), float(-0.7315350966), float(0.6030885658), float(-0.4149130821), float(0.7585339481), float(0.6963196535), float(0.8332685012), float(-0.8086815232), float(0.7518116724),
	float(-0.3490535894), float(0.6972110903), float(-0.8795676928), float(-0.6442331882), float(0.6610236811), float(-0.9853565782), float(-0.590338458), float(0.09843602117), float(0.5646534882), float(-0.6023259233), float(-0.3539248861), float(0.5132728656), float(0.9380385118), float(-0.7599270056), float(-0.7425936564), float(-0.6679610562),
	float(-0.3018497816), float(0.814478266), float(0.03777430269), float(-0.7514235086), float(0.9662556939), float(-0.4720194901), float(-0.435054126), float(0.7091901235), float(0.929379209), float(0.9997434357), float(0.8306320299), float(-0.9434019629), float(-0.133133759), float(0.5048413216), float(0.3711995273), float(0.98552091),
	float(0.7401857005), float(-0.9999981398), float(-0.2144033253), float(0.4808624681), float(-0.413835885), float(0.644229305), float(0.9626648696), float(0.1833665934), float(0.5794129), float(0.01404446873), float(0.4388494993), float(0.5213612322), float(-0.5281609948), float(-0.9745306846), float(-0.9904373013), float(0.9100232252),
	float(-0.9914057719), float(0.7892627765), float(0.3364421659), float(-0.9416099764), float(0.7802732656), float(0.886302871), float(0.6524471291), float(0.5762186726), float(-0.08987644664), float(-0.2177026782), float(-0.9720345052), float(-0.05722538858), float(0.8105983127), float(0.3410261032), float(0.6452309645), float(-0.7810612152),
	float(0.9989395718), float(-0.808247815), float(0.6370177929), float(0.5844658772), float(0.2054070861), float(0.055960522), float(-0.995827561), float(0.893409165), float(-0.931516824), float(0.328969469), float(-0.3193837488), float(0.7314755657), float(-0.7913517714), float(-0.2204109786), float(0.9955900414), float(-0.7112353139),
	float(-0.7935008741), float(-0.9961918204), float(-0.9714163995), float(-0.9566188669), float(0.2748495632), float(-0.4681743221), float(-0.9614449642), float(0.585194072), float(0.4532946061), float(-0.9916113176), float(0.942479587), float(-0.9813704753), float(-0.6538429571), float(0.2923335053), float(-0.2246660704), float(-0.1800781949),
	float(-0.9581216256), float(0.552215082), float(-0.9296791922), float(0.643183699), float(0.9997325981), float(-0.4606920354), float(-0.2148721265), float(0.3482070809), float(0.3075517813), float(0.6274756393), float(0.8910881765), float(-0.6397771309), float(-0.4479080125), float(-0.5247665011), float(-0.8386507094), float(0.3901291416),
	float(0.1458336921), float(0.01624613149), float(-0.8273199879), float(0.5611100679), float(-0.8380219841), float(-0.9856122234), float(-0.861398618), float(0.6398413916), float(0.2694510795), float(0.4327334514), float(-0.9960265354), float(-0.939570655), float(-0.8846996446), float(0.7642113189), float(-0.7002080528), float(0.664508256),
};
const float CELL_2D_Y[] =
{
	float(0.7649700911), float(0.9967722885), float(0.05734160033), float(-0.1610318741), float(0.371395799), float(-0.7961420628), float(0.6076990492), float(-0.9371723195), float(0.3324056156), float(0.07972205329), float(-0.4857529277), float(-0.9898503007), float(0.9036577593), float(0.05162417479), float(-0.02545330525), float(-0.998045976),
	float(-0.7021653386), float(-0.4524967717), float(-0.9939885256), float(-0.4875625128), float(-0.9657481729), float(-0.7432567015), float(0.02515761212), float(0.4743044842), float(0.5771254669), float(0.4449408324), float(0.3416365773), float(0.9970960285), float(0.6994034849), float(0.4917517499), float(0.286765333), float(0.8713868327),
	float(0.6172387009), float(0.9941540269), float(0.9632339851), float(-0.7913613129), float(0.847515538), float(0.6211056739), float(0.5171924952), float(-0.8198283277), float(-0.1832084353), float(0.7054329737), float(0.7325597678), float(0.9960630973), float(0.7419559859), float(0.9908355749), float(-0.346274329), float(0.2192641299),
	float(-0.9060627411), float(-0.8683346653), float(0.9949314574), float(-0.6445220433), float(-0.8103794704), float(-0.2216977607), float(0.7168515217), float(0.612202264), float(-0.412428616), float(0.285325116), float(0.56227115), float(-0.9653857009), float(-0.6290361962), float(0.6491672535), float(0.443835306), float(-0.1791955706),
	float(-0.6792690269), float(-0.9763662173), float(0.7391782104), float(0.8343693968), float(0.7238337389), float(0.4965557504), float(0.8075909592), float(-0.4459769977), float(-0.9331160806), float(-0.5710019572), float(0.9566512346), float(-0.08357920318), float(0.2146116448), float(-0.6739348049), float(0.8810115417), float(0.4467718167),
	float(-0.7196250184), float(-0.749462481), float(0.9960561112), float(0.6600461127), float(-0.8465566164), float(-0.8525598897), float(-0.9732775654), float(0.6111293616), float(-0.9612584717), float(-0.7237870097), float(-0.9974830104), float(-0.8014006968), float(0.9528814544), float(-0.6884178931), float(-0.1691668301), float(0.9843571905),
	float(0.7651544003), float(-0.09355982605), float(-0.5200134429), float(-0.006202125807), float(-0.9997683284), float(0.4919944954), float(-0.9928084436), float(-0.1253880012), float(-0.4165383308), float(-0.6148930171), float(-0.1034332049), float(-0.9070022917), float(-0.9503958117), float(0.9151503065), float(-0.6486716073), float(0.6397687707),
	float(-0.9883386937), float(0.3507613761), float(0.9857642561), float(-0.9342026446), float(-0.9082419159), float(0.1560587169), float(0.4921240607), float(-0.453669308), float(0.6818037859), float(0.7976742329), float(0.9098610522), float(0.651633524), float(0.7177318024), float(-0.5528685241), float(0.5882467118), float(0.6593778956),
	float(0.9371027648), float(-0.7168658839), float(-0.4757737632), float(0.7648291307), float(0.7503650398), float(0.1705063456), float(-0.8071558121), float(-0.9951433815), float(-0.8253280792), float(-0.7982502628), float(0.9352738503), float(0.8582254747), float(-0.3465310238), float(0.65000842), float(-0.6697422351), float(0.7441962291),
	float(-0.9533555), float(0.5801940659), float(-0.9992862963), float(-0.659820211), float(0.2575848092), float(0.881588113), float(-0.9004043022), float(-0.7050172826), float(0.369126382), float(-0.02265088836), float(0.5568217228), float(-0.3316515286), float(0.991098079), float(-0.863212164), float(-0.9285531277), float(0.1695539323),
	float(-0.672402505), float(-0.001928841934), float(0.9767452145), float(-0.8767960349), float(0.9103515037), float(-0.7648324016), float(0.2706960452), float(-0.9830446035), float(0.8150341657), float(-0.9999013716), float(-0.8985605806), float(0.8533360801), float(0.8491442537), float(-0.2242541966), float(-0.1379635899), float(-0.4145572694),
	float(0.1308227633), float(0.6140555916), float(0.9417041303), float(-0.336705587), float(-0.6254387508), float(0.4631060578), float(-0.7578342456), float(-0.8172955655), float(-0.9959529228), float(-0.9760151351), float(0.2348380732), float(-0.9983612848), float(0.5856025746), float(-0.9400538266), float(-0.7639875669), float(0.6244544645),
	float(0.04604054566), float(0.5888424828), float(0.7708490978), float(-0.8114182882), float(0.9786766212), float(-0.9984329822), float(0.09125496582), float(-0.4492438803), float(-0.3636982357), float(0.9443405575), float(-0.9476254645), float(-0.6818676535), float(-0.6113610831), float(0.9754070948), float(-0.0938108173), float(-0.7029540015),
	float(-0.6085691109), float(-0.08718862881), float(-0.237381926), float(0.2913423132), float(0.9614872426), float(0.8836361266), float(-0.2749974196), float(-0.8108932717), float(-0.8913607575), float(0.129255541), float(-0.3342637104), float(-0.1921249337), float(-0.7566302845), float(-0.9563164339), float(-0.9744358146), float(0.9836522982),
	float(-0.2863615732), float(0.8337016872), float(0.3683701937), float(0.7657119102), float(-0.02312427772), float(0.8875600535), float(0.976642191), float(0.9374176384), float(0.9515313457), float(-0.7786361937), float(-0.4538302125), float(-0.7685604874), float(-0.8940796454), float(-0.8512462154), float(0.5446696133), float(0.9207601495),
	float(-0.9893091197), float(-0.9998680229), float(0.5617309299), float(-0.8277411985), float(0.545636467), float(0.1690223212), float(-0.5079295433), float(0.7685069899), float(-0.9630140787), float(0.9015219132), float(0.08905695279), float(-0.3423550559), float(-0.4661614943), float(-0.6449659371), float(0.7139388509), float(0.7472809229),
};
const float CELL_3D_X[] =
{
	float(0.3752498686), float(0.687188096), float(0.2248135212), float(0.6692006647), float(-0.4376476931), float(0.6139972552), float(0.9494563929), float(0.8065108882), float(-0.2218812853), float(0.8484661167), float(0.5551817596), float(0.2133903499), float(0.5195126593), float(-0.6440141975), float(-0.5192897331), float(-0.3697654077),
	float(-0.07927779647), float(0.4187757321), float(-0.750078731), float(0.6579554632), float(-0.6859803838), float(-0.6878407087), float(0.9490848347), float(0.5795829433), float(-0.5325976529), float(-0.1363699466), float(0.417665879), float(-0.9108236468), float(0.4438605427), float(0.819294887), float(-0.4033873915), float(-0.2817317705),
	float(0.3969665622), float(0.5323450134), float(-0.6833017297), float(0.3881436661), float(-0.7119144767), float(-0.2306979838), float(-0.9398873022), float(0.1701906676), float(-0.4261839496), float(-0.003712295499), float(-0.734675004), float(-0.3195046015), float(0.7345307424), float(0.9766246496), float(-0.02003735175), float(-0.4824156342),
	float(0.4245892007), float(0.9072427669), float(0.593346808), float(-0.8911762541), float(-0.7657571834), float(-0.5268198896), float(-0.8801903279), float(-0.6296409617), float(-0.09492481344), float(-0.4920470525), float(0.7307666154), float(-0.2514540636), float(-0.3356210347), float(-0.3522787894), float(0.87847885), float(-0.7424096346),
	float(0.5757585274), float(0.4519299338), float(0.6420368628), float(-0.1128478447), float(0.499874883), float(0.5291681739), float(-0.5098837195), float(0.5639583502), float(-0.8456386526), float(-0.9657134875), float(-0.576437342), float(-0.5666013014), float(0.5667702405), float(-0.481316582), float(0.7313389916), float(-0.3805628566),
	float(-0.6512675909), float(-0.2787156951), float(0.8648059114), float(-0.9730216276), float(-0.8335820906), float(0.2673159641), float(0.231150148), float(0.01286214638), float(0.6774953261), float(0.6542885718), float(-0.02545450161), float(0.2101238586), float(-0.5572105885), float(0.813705672), float(-0.7546026951), float(-0.2502500006),
	float(-0.9979289381), float(0.7024037039), float(0.08990874624), float(0.8170812432), float(0.4226980265), float(-0.2442153475), float(-0.9183326731), float(0.6068222411), float(0.818676691), float(-0.7236735282), float(-0.5383903295), float(-0.6269337242), float(-0.0939331121), float(0.9203878539), float(-0.7256396824), float(0.6292431149),
	float(0.4234156978), float(0.006685688024), float(-0.2598694113), float(0.6408036421), float(0.05899871622), float(0.7090281418), float(-0.5905222072), float(0.3128214264), float(-0.691925826), float(0.3634019349), float(-0.6772511147), float(-0.3204583896), float(-0.3906740409), float(-0.3342190395), float(-0.517779592), float(-0.6817711267),
	float(0.6422383105), float(0.4388482478), float(0.2968562611), float(-0.2019778353), float(0.6014865048), float(0.9519280722), float(0.3398889569), float(0.8179709354), float(0.2365522154), float(0.3262175096), float(-0.8060715954), float(-0.2068642503), float(0.6208057279), float(-0.5274282502), float(-0.3722334928), float(-0.8923412971),
	float(0.5341834201), float(-0.3663701513), float(-0.6114600319), float(0.5026307556), float(0.8396151729), float(0.9245042467), float(-0.7994843957), float(-0.5357200589), float(-0.6283359739), float(-0.61351886), float(-0.875632008), float(-0.5278879423), float(0.9087491985), float(-0.03500215466), float(-0.261365798), float(-0.579523541),
	float(-0.3765052689), float(-0.74398252), float(0.4257318052), float(-0.1214508921), float(0.8561809753), float(0.6802835104), float(-0.5452131039), float(-0.1997156478), float(0.4562348357), float(-0.811704301), float(0.67793962), float(-0.9237819106), float(0.6973511259), float(-0.5189506), float(0.5517320032), float(-0.396710831),
	float(0.5493762815), float(-0.2507853002), float(0.4788634005), float(0.387333516), float(-0.2176515694), float(0.6749832419), float(0.2148283022), float(-0.7521815872), float(0.4697000159), float(0.7890593699), float(-0.7606162952), float(0.01083397843), float(0.5254091908), float(-0.6748025877), float(0.751091524), float(0.05259056135),
	float(0.01889481232), float(-0.6037423727), float(-0.6542965129), float(0.08873301081), float(-0.6191345671), float(0.4331858488), float(-0.3858351946), float(-0.1429059747), float(0.4118221036), float(-0.6247153214), float(-0.611423014), float(0.5542939606), float(-0.9432768808), float(-0.4567870451), float(-0.7349133547), float(0.399304489),
	float(-0.7474927672), float(0.02589419753), float(0.783915821), float(0.6138668752), float(0.4276376047), float(-0.4347886353), float(0.02947841302), float(-0.833742746), float(0.3817221742), float(-0.8743368359), float(-0.3823443796), float(-0.6829243811), float(-0.3681903049), float(-0.367626833), float(-0.434583373), float(0.235891995),
	float(-0.6874880269), float(-0.5115661773), float(-0.5534962601), float(0.5632777056), float(0.686191532), float(-0.05095871588), float(-0.06865785057), float(-0.5975288531), float(-0.6429790056), float(-0.3729361548), float(0.2237917666), float(0.6046773225), float(-0.5041542295), float(-0.03972191174), float(0.7028828406), float(-0.5560856498),
	float(0.5898328456), float(-0.9308076766), float(0.4617069864), float(0.3190983137), float(0.9116567753), float(-0.45029554), float(0.3346334459), float(0.8525005645), float(0.2528483381), float(-0.8306630147), float(-0.6880390622), float(0.7448684026), float(-0.1963355843), float(-0.5900257974), float(0.9097057294), float(-0.2509196808),
};
const float CELL_3D_Y[] =
{
	float(-0.6760585049), float(-0.09136176499), float(0.1681325679), float(-0.6688468686), float(-0.4822753902), float(-0.7891068824), float(-0.1877509944), float(0.548470914), float(-0.463339443), float(-0.4050542082), float(0.3218158513), float(0.2546493823), float(-0.3753271935), float(0.4745384887), float(0.481254652), float(-0.8934416489),
	float(-0.6737085076), float(0.7469917228), float(0.3826230411), float(0.6751013678), float(-0.7248119515), float(-0.3224276742), float(-0.02076190936), float(-0.6404268166), float(-0.5292028444), float(0.7151414636), float(-0.6144655059), float(-0.369912124), float(0.6942067212), float(-0.4481558248), float(-0.6366894559), float(0.5956568471),
	float(0.564274539), float(0.7145584688), float(0.6871918316), float(0.5657918509), float(-0.6275978114), float(0.4146983062), float(0.2638993789), float(-0.792633138), float(0.5706133514), float(0.8606546462), float(0.6490900316), float(-0.8242699196), float(0.6765819124), float(0.1959534069), float(-0.8426769757), float(-0.5917672797),
	float(0.7517364266), float(0.03252559226), float(0.0883617105), float(0.4475064813), float(-0.1418643552), float(0.7343428473), float(0.3870192548), float(-0.7716703522), float(0.4839898327), float(0.7437439055), float(-0.5989573348), float(-0.8357068955), float(0.6086049038), float(0.9194627258), float(0.4718297238), float(-0.2650335884),
	float(-0.6470352599), float(-0.5555181303), float(0.1222351235), float(0.7802044684), float(-0.8636947022), float(-0.2341352163), float(0.683030874), float(-0.5005858287), float(0.2334616211), float(0.2576877608), float(0.6666816727), float(-0.7663996863), float(0.794201982), float(0.6189308788), float(0.6071033261), float(-0.4206058253),
	float(-0.3957336915), float(-0.8170257484), float(-0.1043240417), float(0.0002167596213), float(0.1816339018), float(-0.6838094939), float(-0.2495341969), float(-0.7116756954), float(-0.03361673621), float(-0.3350836431), float(0.2137186039), float(0.2557996786), float(0.7490117093), float(0.4942936549), float(-0.352686853), float(-0.3952445435),
	float(-0.0459964767), float(-0.7115787471), float(0.08022899756), float(0.5362268157), float(-0.8258613686), float(0.1114171723), float(0.3882823051), float(-0.7915404457), float(0.3250957662), float(0.6401346464), float(-0.2662724517), float(-0.6727907114), float(-0.994730818), float(-0.3596358977), float(0.2344610069), float(-0.6645215546),
	float(-0.7107590611), float(-0.4646617327), float(0.6717191355), float(0.5101893498), float(0.1185768238), float(0.236005093), float(-0.7811024061), float(0.5089325193), float(0.6073187658), float(-0.7930732557), float(-0.6822767155), float(0.3201532885), float(0.7545302807), float(0.1072664448), float(0.6784033173), float(-0.6595924967),
	float(0.7276509498), float(0.5586689436), float(-0.6498636788), float(0.6789333174), float(0.7105966551), float(-0.2872214155), float(0.496746217), float(-0.3880337977), float(0.7324070604), float(-0.9326634749), float(-0.5867839255), float(0.8003043651), float(-0.1631882481), float(-0.6796374681), float(-0.8066678503), float(0.4238177418),
	float(0.7715863549), float(0.5455367347), float(-0.03205115397), float(-0.6005545066), float(-0.5423640002), float(0.3569205906), float(-0.582071752), float(0.6407354361), float(0.7777142984), float(-0.09956428618), float(0.1100002681), float(0.8136349123), float(0.2923431904), float(0.9735794425), float(0.8324974864), float(-0.6179617717),
	float(-0.9248386523), float(-0.6448780771), float(-0.5274402761), float(-0.7862170565), float(0.2682099744), float(-0.5848777694), float(-0.6364561467), float(-0.7167402514), float(-0.8677012494), float(0.4205286707), float(-0.7007832749), float(0.243272451), float(-0.1899846085), float(-0.6146124977), float(-0.8093357692), float(-0.03545096987),
	float(-0.7191590868), float(0.7478645848), float(0.3623517328), float(0.8436992512), float(-0.2445711729), float(0.6897356637), float(-0.1708070787), float(0.4639272368), float(-0.7917186656), float(0.02980025428), float(0.6334156172), float(-0.9815544807), float(-0.2307217304), float(0.1080823318), float(0.5167601798), float(-0.845120016),
	float(0.441572562), float(0.5876789172), float(-0.6365908737), float(0.68350166), float(0.5849723959), float(0.1164114357), float(-0.7379813884), float(-0.9613237178), float(-0.9071943084), float(-0.7682111105), float(0.639074459), float(-0.619358298), float(0.2807257131), float(-0.01800868791), float(0.3776607289), float(0.7207567823),
	float(0.5536661486), float(-0.9974053117), float(-0.02047200006), float(-0.6739453804), float(-0.5607471297), float(0.8815553192), float(0.8275977415), float(0.3928902456), float(0.550991396), float(0.4247623676), float(-0.3436948871), float(-0.3653537677), float(0.3181702902), float(-0.6067173171), float(-0.8984128477), float(0.4220839766),
	float(0.7238407199), float(-0.7766913695), float(0.6460037842), float(0.2544775664), float(0.6488840578), float(0.805016833), float(-0.9183807036), float(0.4144046357), float(0.270587208), float(-0.8813684494), float(0.6985971877), float(-0.7795603017), float(-0.8624480731), float(0.5532697017), float(0.711179521), float(-0.7798160574),
	float(0.5225859041), float(0.1261859368), float(0.3398033582), float(-0.7472173667), float(-0.4032647119), float(-0.4246578154), float(0.8481212377), float(-0.2144838537), float(0.3431714491), float(0.5310188231), float(0.6682978632), float(0.3110433206), float(0.9263293599), float(-0.6155600569), float(0.07169784399), float(0.8985888773),
};
const float CELL_3D_Z[] =
{
	float(-0.6341391283), float(-0.7207118346), float(0.9597866014), float(0.3237504235), float(-0.7588642466), float(-0.01782410481), float(0.2515593809), float(0.2207257205), float(-0.8579541106), float(0.3406410681), float(0.7669470462), float(-0.9431957648), float(0.7676171537), float(-0.6000491115), float(-0.7062096948), float(0.2550207115),
	float(0.7347325213), float(0.5163625202), float(-0.5394270162), float(0.3336656285), float(-0.0638635111), float(-0.6503195787), float(0.3143356798), float(-0.5039217245), float(0.6605180464), float(-0.6855479011), float(-0.6693185756), float(0.1832083647), float(-0.5666258437), float(0.3576482138), float(-0.6571949095), float(-0.7522101635),
	float(-0.7238865886), float(0.4538887323), float(0.2467106257), float(0.7274778869), float(0.3151170655), float(-0.8802293764), float(-0.2167232729), float(0.5854637865), float(0.7019741052), float(0.5091756071), float(0.1973189533), float(0.46743546), float(0.05197599597), float(0.088354718), float(0.5380464843), float(-0.6458224544),
	float(-0.5045952393), float(0.419347884), float(0.8000823542), float(-0.07445020656), float(-0.6272881641), float(-0.428020311), float(-0.2747382083), float(-0.08987283726), float(0.8699098354), float(0.4524761885), float(-0.3274603257), float(0.4882262167), float(-0.7189983256), float(0.1746079907), float(0.0751772698), float(-0.6152927202),
	float(0.4998474673), float(-0.6979677227), float(0.7568667263), float(-0.6152612058), float(0.06447140991), float(-0.8155744872), float(-0.5229602449), float(0.6567836838), float(-0.4799905631), float(0.03153534591), float(0.4724992466), float(-0.3026458097), float(-0.2191225827), float(-0.620692287), float(0.3107552588), float(0.8235670294),
	float(0.6474915988), float(-0.5047637941), float(0.4911488878), float(-0.2307138167), float(-0.5216800015), float(0.6789305939), float(0.9403734863), float(0.702390397), float(0.7347584625), float(0.6779567958), float(0.9765635805), float(-0.9436177661), float(-0.358465925), float(-0.3058706624), float(0.5533414464), float(-0.8838306897),
	float(0.04496841812), float(0.01687374963), float(-0.9927133148), float(-0.211752318), float(0.3732015249), float(0.9632990593), float(-0.07682417004), float(-0.07232213047), float(0.4733721775), float(0.2579229713), float(0.7995216286), float(0.3928189967), float(0.04107517667), float(0.1534542912), float(0.6468965045), float(0.4030684878),
	float(-0.5617300988), float(-0.885463029), float(0.693729985), float(-0.5736527866), float(-0.9911905409), float(-0.66451538), float(0.2028855685), float(0.8019541421), float(-0.3903877149), float(-0.4888495114), float(-0.2753714057), float(-0.8915202143), float(0.5273119089), float(0.9363714773), float(-0.5212228249), float(-0.31642672),
	float(0.2409440761), float(-0.703776404), float(-0.6996810411), float(-0.7058714505), float(-0.3650566783), float(0.1064744278), float(0.7985729102), float(0.424680257), float(-0.6384535592), float(0.1540161646), float(-0.07702731943), float(-0.5627789132), float(-0.7667919169), float(-0.509815999), float(0.4590525092), float(0.1552595611),
	float(0.345402042), float(0.7537656024), float(0.7906259247), float(-0.6218493452), float(0.02979350071), float(-0.1337893489), float(-0.1483818606), float(0.549965562), float(0.01882482408), float(-0.7833783002), float(0.4702855809), float(0.2435827372), float(0.2978428332), float(0.2256499906), float(0.4885036897), float(0.5312962584),
	float(0.05401156992), float(0.1749922158), float(-0.7352273018), float(0.6058980284), float(0.4416079111), float(0.4417378638), float(0.5455879807), float(-0.6681295324), float(0.1973431441), float(0.4053292055), float(0.2220375492), float(0.2957118467), float(0.6910913512), float(0.5940890106), float(-0.2014135283), float(-0.9172588213),
	float(-0.4254361401), float(-0.6146586825), float(-0.7996193253), float(-0.3716777111), float(-0.9448876842), float(-0.2620349924), float(0.9615995749), float(-0.4679683524), float(0.3905937144), float(0.613593722), float(0.1422937358), float(0.1908754211), float(0.8189704912), float(-0.7300408736), float(-0.4108776451), float(-0.5319834504),
	float(-0.8970265651), float(-0.5386359045), float(0.4082255906), float(0.7245356676), float(0.5239080873), float(-0.8937552226), float(-0.553637673), float(0.2354455182), float(-0.0860293075), float(-0.1399373318), float(-0.4666323327), float(0.5560157407), float(0.1772619533), float(-0.8893937725), float(-0.5632714576), float(-0.5666264959),
	float(-0.3670263736), float(-0.06717242579), float(0.6205295181), float(-0.4110536264), float(0.7090054553), float(0.183899597), float(-0.5605470555), float(0.3879565548), float(0.7420893903), float(-0.2347595118), float(-0.8577217497), float(0.6325590203), float(-0.8736152276), float(0.7048011129), float(-0.06317948268), float(0.8753285574),
	float(-0.05843650473), float(-0.3674922622), float(-0.5256624401), float(0.7861039337), float(0.3287714416), float(0.5910593099), float(-0.3896960134), float(0.6864605361), float(0.7164918431), float(-0.290014277), float(-0.6796169617), float(0.1632515592), float(0.04485347486), float(0.8320545697), float(0.01339408056), float(-0.2874989857),
	float(0.615630723), float(0.3430367014), float(0.8193658136), float(-0.5829600957), float(0.07911697781), float(0.7854296063), float(-0.4107442306), float(0.4766964066), float(-0.9045999527), float(-0.1673856787), float(0.2828077348), float(-0.5902737632), float(-0.321506229), float(-0.5224513133), float(-0.4090169985), float(-0.3599685311),
};

static int32 FastFloor(float f) { return (f >= 0 ? (int32)f : (int32)f - 1); }
static int32 FastRound(float f) { return (f >= 0) ? (int32)(f + float(0.5)) : (int32)(f - float(0.5)); }
static int32 FastAbs(int32 i) { return abs(i); }
static float FastAbs(float f) { return fabs(f); }
static float Lerp(float a, float b, float t) { return a + t * (b - a); }
static float InterpHermiteFunc(float t) { return t*t*(3 - 2 * t); }
static float InterpQuinticFunc(float t) { return t*t*t*(t*(t * 6 - 15) + 10); }
static float CubicLerp(float a, float b, float c, float d, float t)
{
	float p = (d - c) - (a - b);
	return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
}

UFastNoise::UFastNoise()
	: Super()
#if WITH_EDITORONLY_DATA
	, ThumbnailTexture(nullptr)
	, ThumbnailScale(1.0f)
	, OffsetX(0.0f)
	, OffsetY(0.0f)
#endif
	, Seed(1337)
	, Frequency(0.01f)
	, Interpolation(EFNInterp::Quintic)
	, NoiseType(EFNNoiseType::Simplex)
	, FractalOctaves(3)
	, FractalLacunarity(2.0f)
	, FractalGain(0.5f)
	, FractalType(EFNFractalType::FBM)
	, CellularDistanceFunction(EFNCellularDistanceFunction::Euclidean)
	, CellularReturnType(EFNCellularReturnType::CellValue)
	, CellularDistanceIndex0(0)
	, CellularDistanceIndex1(1)
	, CellularJitter(0.45f)
	, GradientPerturbAmp(1.0f)
{
	UpdateSeed();
	CalculateFractalBounding();
}

void UFastNoise::UpdateSeed()
{
	std::mt19937_64 gen(Seed);

	for (int32 i = 0; i < 256; i++)
		m_perm[i] = i;

	for (int32 j = 0; j < 256; j++)
	{
		int32 rng = (int32)(gen() % (256 - j));
		int32 k = rng + j;
		int32 l = m_perm[j];
		m_perm[j] = m_perm[j + 256] = m_perm[k];
		m_perm[k] = l;
		m_perm12[j] = m_perm12[j + 256] = m_perm[j] % 12;
	}
}

void UFastNoise::SetSeed(int32 seed)
{
	if (Seed == seed)
		return;

	Seed = seed;
	UpdateSeed();
}

void UFastNoise::CalculateFractalBounding()
{
	float amp = FractalGain;
	float ampFractal = 1.0f;
	for (int32 i = 1; i < FractalOctaves; i++)
	{
		ampFractal += amp;
		amp *= FractalGain;
	}
	m_fractalBounding = 1.0f / ampFractal;
}

void UFastNoise::SetCellularDistance2Indices(int32 cellularDistanceIndex0, int32 cellularDistanceIndex1)
{
	CellularDistanceIndex0 = std::min(cellularDistanceIndex0, cellularDistanceIndex1);
	CellularDistanceIndex1 = std::max(cellularDistanceIndex0, cellularDistanceIndex1);

	CellularDistanceIndex0 = std::min(std::max(CellularDistanceIndex0, 0), FN_CELLULAR_INDEX_MAX);
	CellularDistanceIndex1 = std::min(std::max(CellularDistanceIndex1, 0), FN_CELLULAR_INDEX_MAX);
}

void UFastNoise::GetCellularDistance2Indices(int32& cellularDistanceIndex0, int32& cellularDistanceIndex1) const
{
	cellularDistanceIndex0 = CellularDistanceIndex0;
	cellularDistanceIndex1 = CellularDistanceIndex1;
}

uint8 UFastNoise::Index2D_12(uint8 offset, int32 x, int32 y) const
{
	return m_perm12[(x & 0xff) + m_perm[(y & 0xff) + offset]];
}
uint8 UFastNoise::Index3D_12(uint8 offset, int32 x, int32 y, int32 z) const
{
	return m_perm12[(x & 0xff) + m_perm[(y & 0xff) + m_perm[(z & 0xff) + offset]]];
}
uint8 UFastNoise::Index4D_32(uint8 offset, int32 x, int32 y, int32 z, int32 w) const
{
	return m_perm[(x & 0xff) + m_perm[(y & 0xff) + m_perm[(z & 0xff) + m_perm[(w & 0xff) + offset]]]] & 31;
}
uint8 UFastNoise::Index2D_256(uint8 offset, int32 x, int32 y) const
{
	return m_perm[(x & 0xff) + m_perm[(y & 0xff) + offset]];
}
uint8 UFastNoise::Index3D_256(uint8 offset, int32 x, int32 y, int32 z) const
{
	return m_perm[(x & 0xff) + m_perm[(y & 0xff) + m_perm[(z & 0xff) + offset]]];
}
uint8 UFastNoise::Index4D_256(uint8 offset, int32 x, int32 y, int32 z, int32 w) const
{
	return m_perm[(x & 0xff) + m_perm[(y & 0xff) + m_perm[(z & 0xff) + m_perm[(w & 0xff) + offset]]]];
}

// Hashing
#define X_PRIME 1619
#define Y_PRIME 31337
#define Z_PRIME 6971
#define W_PRIME 1013

static float ValCoord2D(int32 seed, int32 x, int32 y)
{
	int32 n = seed;
	n ^= X_PRIME * x;
	n ^= Y_PRIME * y;

	return (n * n * n * 60493) / float(2147483648);
}
static float ValCoord3D(int32 seed, int32 x, int32 y, int32 z)
{
	int32 n = seed;
	n ^= X_PRIME * x;
	n ^= Y_PRIME * y;
	n ^= Z_PRIME * z;

	return (n * n * n * 60493) / float(2147483648);
}
static float ValCoord4D(int32 seed, int32 x, int32 y, int32 z, int32 w)
{
	int32 n = seed;
	n ^= X_PRIME * x;
	n ^= Y_PRIME * y;
	n ^= Z_PRIME * z;
	n ^= W_PRIME * w;

	return (n * n * n * 60493) / float(2147483648);
}

float UFastNoise::ValCoord2DFast(uint8 offset, int32 x, int32 y) const
{
	return VAL_LUT[Index2D_256(offset, x, y)];
}
float UFastNoise::ValCoord3DFast(uint8 offset, int32 x, int32 y, int32 z) const
{
	return VAL_LUT[Index3D_256(offset, x, y, z)];
}

float UFastNoise::GradCoord2D(uint8 offset, int32 x, int32 y, float xd, float yd) const
{
	uint8 lutPos = Index2D_12(offset, x, y);

	return xd*GRAD_X[lutPos] + yd*GRAD_Y[lutPos];
}
float UFastNoise::GradCoord3D(uint8 offset, int32 x, int32 y, int32 z, float xd, float yd, float zd) const
{
	uint8 lutPos = Index3D_12(offset, x, y, z);

	return xd*GRAD_X[lutPos] + yd*GRAD_Y[lutPos] + zd*GRAD_Z[lutPos];
}
float UFastNoise::GradCoord4D(uint8 offset, int32 x, int32 y, int32 z, int32 w, float xd, float yd, float zd, float wd) const
{
	uint8 lutPos = Index4D_32(offset, x, y, z, w) << 2;

	return xd*GRAD_4D[lutPos] + yd*GRAD_4D[lutPos + 1] + zd*GRAD_4D[lutPos + 2] + wd*GRAD_4D[lutPos + 3];
}

float UFastNoise::GetNoise3D(float x, float y, float z) const
{
	x *= Frequency;
	y *= Frequency;
	z *= Frequency;

	switch (NoiseType)
	{
	case EFNNoiseType::Value:
		return SingleValue(0, x, y, z);
	case EFNNoiseType::ValueFractal:
		switch (FractalType)
		{
		case EFNFractalType::FBM:
			return SingleValueFractalFBM(x, y, z);
		case EFNFractalType::Billow:
			return SingleValueFractalBillow(x, y, z);
		case EFNFractalType::RigidMulti:
			return SingleValueFractalRigidMulti(x, y, z);
		default:
			return 0;
		}
	case EFNNoiseType::Perlin:
		return SinglePerlin(0, x, y, z);
	case EFNNoiseType::PerlinFractal:
		switch (FractalType)
		{
		case EFNFractalType::FBM:
			return SinglePerlinFractalFBM(x, y, z);
		case EFNFractalType::Billow:
			return SinglePerlinFractalBillow(x, y, z);
		case EFNFractalType::RigidMulti:
			return SinglePerlinFractalRigidMulti(x, y, z);
		default:
			return 0;
		}
	case EFNNoiseType::Simplex:
		return SingleSimplex(0, x, y, z);
	case EFNNoiseType::SimplexFractal:
		switch (FractalType)
		{
		case EFNFractalType::FBM:
			return SingleSimplexFractalFBM(x, y, z);
		case EFNFractalType::Billow:
			return SingleSimplexFractalBillow(x, y, z);
		case EFNFractalType::RigidMulti:
			return SingleSimplexFractalRigidMulti(x, y, z);
		default:
			return 0;
		}
	case EFNNoiseType::Cellular:
		switch (CellularReturnType)
		{
		case EFNCellularReturnType::CellValue:
		case EFNCellularReturnType::NoiseLookup:
		case EFNCellularReturnType::Distance:
			return SingleCellular(x, y, z);
		default:
			return SingleCellular2Edge(x, y, z);
		}
	case EFNNoiseType::WhiteNoise:
		return GetWhiteNoise3D(x, y, z);
	case EFNNoiseType::Cubic:
		return SingleCubic(0, x, y, z);
	case EFNNoiseType::CubicFractal:
		switch (FractalType)
		{
		case EFNFractalType::FBM:
			return SingleCubicFractalFBM(x, y, z);
		case EFNFractalType::Billow:
			return SingleCubicFractalBillow(x, y, z);
		case EFNFractalType::RigidMulti:
			return SingleCubicFractalRigidMulti(x, y, z);
		}
	default:
		return 0;
	}
}

float UFastNoise::GetNoise2D(float x, float y) const
{
	x *= Frequency;
	y *= Frequency;

	switch (NoiseType)
	{
	case EFNNoiseType::Value:
		return SingleValue(0, x, y);
	case EFNNoiseType::ValueFractal:
		switch (FractalType)
		{
		case EFNFractalType::FBM:
			return SingleValueFractalFBM(x, y);
		case EFNFractalType::Billow:
			return SingleValueFractalBillow(x, y);
		case EFNFractalType::RigidMulti:
			return SingleValueFractalRigidMulti(x, y);
		}
	case EFNNoiseType::Perlin:
		return SinglePerlin(0, x, y);
	case EFNNoiseType::PerlinFractal:
		switch (FractalType)
		{
		case EFNFractalType::FBM:
			return SinglePerlinFractalFBM(x, y);
		case EFNFractalType::Billow:
			return SinglePerlinFractalBillow(x, y);
		case EFNFractalType::RigidMulti:
			return SinglePerlinFractalRigidMulti(x, y);
		}
	case EFNNoiseType::Simplex:
		return SingleSimplex(0, x, y);
	case EFNNoiseType::SimplexFractal:
		switch (FractalType)
		{
		case EFNFractalType::FBM:
			return SingleSimplexFractalFBM(x, y);
		case EFNFractalType::Billow:
			return SingleSimplexFractalBillow(x, y);
		case EFNFractalType::RigidMulti:
			return SingleSimplexFractalRigidMulti(x, y);
		}
	case EFNNoiseType::Cellular:
		switch (CellularReturnType)
		{
		case EFNCellularReturnType::CellValue:
		case EFNCellularReturnType::NoiseLookup:
		case EFNCellularReturnType::Distance:
			return SingleCellular(x, y);
		default:
			return SingleCellular2Edge(x, y);
		}
	case EFNNoiseType::WhiteNoise:
		return GetWhiteNoise2D(x, y);
	case EFNNoiseType::Cubic:
		return SingleCubic(0, x, y);
	case EFNNoiseType::CubicFractal:
		switch (FractalType)
		{
		case EFNFractalType::FBM:
			return SingleCubicFractalFBM(x, y);
		case EFNFractalType::Billow:
			return SingleCubicFractalBillow(x, y);
		case EFNFractalType::RigidMulti:
			return SingleCubicFractalRigidMulti(x, y);
		}
	}
	return 0;
}

// White Noise
float UFastNoise::GetWhiteNoise4D(float x, float y, float z, float w) const
{
	return ValCoord4D(Seed,
		*reinterpret_cast<int32*>(&x) ^ (*reinterpret_cast<int32*>(&x) >> 16),
		*reinterpret_cast<int32*>(&y) ^ (*reinterpret_cast<int32*>(&y) >> 16),
		*reinterpret_cast<int32*>(&z) ^ (*reinterpret_cast<int32*>(&z) >> 16),
		*reinterpret_cast<int32*>(&w) ^ (*reinterpret_cast<int32*>(&w) >> 16));
}

float UFastNoise::GetWhiteNoise3D(float x, float y, float z) const
{
	return ValCoord3D(Seed,
		*reinterpret_cast<int32*>(&x) ^ (*reinterpret_cast<int32*>(&x) >> 16),
		*reinterpret_cast<int32*>(&y) ^ (*reinterpret_cast<int32*>(&y) >> 16),
		*reinterpret_cast<int32*>(&z) ^ (*reinterpret_cast<int32*>(&z) >> 16));
}

float UFastNoise::GetWhiteNoise2D(float x, float y) const
{
	return ValCoord2D(Seed,
		*reinterpret_cast<int32*>(&x) ^ (*reinterpret_cast<int32*>(&x) >> 16),
		*reinterpret_cast<int32*>(&y) ^ (*reinterpret_cast<int32*>(&y) >> 16));
}

float UFastNoise::GetWhiteNoiseInt4D(int32 x, int32 y, int32 z, int32 w) const
{
	return ValCoord4D(Seed, x, y, z, w);
}

float UFastNoise::GetWhiteNoiseInt3D(int32 x, int32 y, int32 z) const
{
	return ValCoord3D(Seed, x, y, z);
}

float UFastNoise::GetWhiteNoiseInt2D(int32 x, int32 y) const
{
	return ValCoord2D(Seed, x, y);
}

// Value Noise
float UFastNoise::GetValueFractal3D(float x, float y, float z) const
{
	x *= Frequency;
	y *= Frequency;
	z *= Frequency;

	switch (FractalType)
	{
	case EFNFractalType::FBM:
		return SingleValueFractalFBM(x, y, z);
	case EFNFractalType::Billow:
		return SingleValueFractalBillow(x, y, z);
	case EFNFractalType::RigidMulti:
		return SingleValueFractalRigidMulti(x, y, z);
	default:
		return 0;
	}
}

float UFastNoise::SingleValueFractalFBM(float x, float y, float z) const
{
	float sum = SingleValue(m_perm[0], x, y, z);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum += SingleValue(m_perm[i], x, y, z) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleValueFractalBillow(float x, float y, float z) const
{
	float sum = FastAbs(SingleValue(m_perm[0], x, y, z)) * 2 - 1;
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum += (FastAbs(SingleValue(m_perm[i], x, y, z)) * 2 - 1) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleValueFractalRigidMulti(float x, float y, float z) const
{
	float sum = 1 - FastAbs(SingleValue(m_perm[0], x, y, z));
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum -= (1 - FastAbs(SingleValue(m_perm[i], x, y, z))) * amp;
	}

	return sum;
}

float UFastNoise::GetValue3D(float x, float y, float z) const
{
	return SingleValue(0, x * Frequency, y * Frequency, z * Frequency);
}

float UFastNoise::SingleValue(uint8 offset, float x, float y, float z) const
{
	int32 x0 = FastFloor(x);
	int32 y0 = FastFloor(y);
	int32 z0 = FastFloor(z);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;
	int32 z1 = z0 + 1;

	float xs = 0;
	float ys = 0;
	float zs = 0;
	switch (Interpolation)
	{
	case EFNInterp::Linear:
		xs = x - (float)x0;
		ys = y - (float)y0;
		zs = z - (float)z0;
		break;
	case EFNInterp::Hermite:
		xs = InterpHermiteFunc(x - (float)x0);
		ys = InterpHermiteFunc(y - (float)y0);
		zs = InterpHermiteFunc(z - (float)z0);
		break;
	case EFNInterp::Quintic:
		xs = InterpQuinticFunc(x - (float)x0);
		ys = InterpQuinticFunc(y - (float)y0);
		zs = InterpQuinticFunc(z - (float)z0);
		break;
	}

	float xf00 = Lerp(ValCoord3DFast(offset, x0, y0, z0), ValCoord3DFast(offset, x1, y0, z0), xs);
	float xf10 = Lerp(ValCoord3DFast(offset, x0, y1, z0), ValCoord3DFast(offset, x1, y1, z0), xs);
	float xf01 = Lerp(ValCoord3DFast(offset, x0, y0, z1), ValCoord3DFast(offset, x1, y0, z1), xs);
	float xf11 = Lerp(ValCoord3DFast(offset, x0, y1, z1), ValCoord3DFast(offset, x1, y1, z1), xs);

	float yf0 = Lerp(xf00, xf10, ys);
	float yf1 = Lerp(xf01, xf11, ys);

	return Lerp(yf0, yf1, zs);
}

float UFastNoise::GetValueFractal2D(float x, float y) const
{
	x *= Frequency;
	y *= Frequency;

	switch (FractalType)
	{
	case EFNFractalType::FBM:
		return SingleValueFractalFBM(x, y);
	case EFNFractalType::Billow:
		return SingleValueFractalBillow(x, y);
	case EFNFractalType::RigidMulti:
		return SingleValueFractalRigidMulti(x, y);
	default:
		return 0;
	}
}

float UFastNoise::SingleValueFractalFBM(float x, float y) const
{
	float sum = SingleValue(m_perm[0], x, y);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum += SingleValue(m_perm[i], x, y) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleValueFractalBillow(float x, float y) const
{
	float sum = FastAbs(SingleValue(m_perm[0], x, y)) * 2 - 1;
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		amp *= FractalGain;
		sum += (FastAbs(SingleValue(m_perm[i], x, y)) * 2 - 1) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleValueFractalRigidMulti(float x, float y) const
{
	float sum = 1 - FastAbs(SingleValue(m_perm[0], x, y));
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum -= (1 - FastAbs(SingleValue(m_perm[i], x, y))) * amp;
	}

	return sum;
}

float UFastNoise::GetValue2D(float x, float y) const
{
	return SingleValue(0, x * Frequency, y * Frequency);
}

float UFastNoise::SingleValue(uint8 offset, float x, float y) const
{
	int32 x0 = FastFloor(x);
	int32 y0 = FastFloor(y);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;

	float xs = 0;
	float ys = 0;
	switch (Interpolation)
	{
	case EFNInterp::Linear:
		xs = x - (float)x0;
		ys = y - (float)y0;
		break;
	case EFNInterp::Hermite:
		xs = InterpHermiteFunc(x - (float)x0);
		ys = InterpHermiteFunc(y - (float)y0);
		break;
	case EFNInterp::Quintic:
		xs = InterpQuinticFunc(x - (float)x0);
		ys = InterpQuinticFunc(y - (float)y0);
		break;
	}

	float xf0 = Lerp(ValCoord2DFast(offset, x0, y0), ValCoord2DFast(offset, x1, y0), xs);
	float xf1 = Lerp(ValCoord2DFast(offset, x0, y1), ValCoord2DFast(offset, x1, y1), xs);

	return Lerp(xf0, xf1, ys);
}

// Perlin Noise
float UFastNoise::GetPerlinFractal3D(float x, float y, float z) const
{
	x *= Frequency;
	y *= Frequency;
	z *= Frequency;

	switch (FractalType)
	{
	case EFNFractalType::FBM:
		return SinglePerlinFractalFBM(x, y, z);
	case EFNFractalType::Billow:
		return SinglePerlinFractalBillow(x, y, z);
	case EFNFractalType::RigidMulti:
		return SinglePerlinFractalRigidMulti(x, y, z);
	default:
		return 0;
	}
}

float UFastNoise::SinglePerlinFractalFBM(float x, float y, float z) const
{
	float sum = SinglePerlin(m_perm[0], x, y, z);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum += SinglePerlin(m_perm[i], x, y, z) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SinglePerlinFractalBillow(float x, float y, float z) const
{
	float sum = FastAbs(SinglePerlin(m_perm[0], x, y, z)) * 2 - 1;
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum += (FastAbs(SinglePerlin(m_perm[i], x, y, z)) * 2 - 1) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SinglePerlinFractalRigidMulti(float x, float y, float z) const
{
	float sum = 1 - FastAbs(SinglePerlin(m_perm[0], x, y, z));
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum -= (1 - FastAbs(SinglePerlin(m_perm[i], x, y, z))) * amp;
	}

	return sum;
}

float UFastNoise::GetPerlin3D(float x, float y, float z) const
{
	return SinglePerlin(0, x * Frequency, y * Frequency, z * Frequency);
}

float UFastNoise::SinglePerlin(uint8 offset, float x, float y, float z) const
{
	int32 x0 = FastFloor(x);
	int32 y0 = FastFloor(y);
	int32 z0 = FastFloor(z);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;
	int32 z1 = z0 + 1;

	float xs = 0;
	float ys = 0;
	float zs = 0;
	switch (Interpolation)
	{
	case EFNInterp::Linear:
		xs = x - (float)x0;
		ys = y - (float)y0;
		zs = z - (float)z0;
		break;
	case EFNInterp::Hermite:
		xs = InterpHermiteFunc(x - (float)x0);
		ys = InterpHermiteFunc(y - (float)y0);
		zs = InterpHermiteFunc(z - (float)z0);
		break;
	case EFNInterp::Quintic:
		xs = InterpQuinticFunc(x - (float)x0);
		ys = InterpQuinticFunc(y - (float)y0);
		zs = InterpQuinticFunc(z - (float)z0);
		break;
	}

	float xd0 = x - (float)x0;
	float yd0 = y - (float)y0;
	float zd0 = z - (float)z0;
	float xd1 = xd0 - 1;
	float yd1 = yd0 - 1;
	float zd1 = zd0 - 1;

	float xf00 = Lerp(GradCoord3D(offset, x0, y0, z0, xd0, yd0, zd0), GradCoord3D(offset, x1, y0, z0, xd1, yd0, zd0), xs);
	float xf10 = Lerp(GradCoord3D(offset, x0, y1, z0, xd0, yd1, zd0), GradCoord3D(offset, x1, y1, z0, xd1, yd1, zd0), xs);
	float xf01 = Lerp(GradCoord3D(offset, x0, y0, z1, xd0, yd0, zd1), GradCoord3D(offset, x1, y0, z1, xd1, yd0, zd1), xs);
	float xf11 = Lerp(GradCoord3D(offset, x0, y1, z1, xd0, yd1, zd1), GradCoord3D(offset, x1, y1, z1, xd1, yd1, zd1), xs);

	float yf0 = Lerp(xf00, xf10, ys);
	float yf1 = Lerp(xf01, xf11, ys);

	return Lerp(yf0, yf1, zs);
}

float UFastNoise::GetPerlinFractal2D(float x, float y) const
{
	x *= Frequency;
	y *= Frequency;

	switch (FractalType)
	{
	case EFNFractalType::FBM:
		return SinglePerlinFractalFBM(x, y);
	case EFNFractalType::Billow:
		return SinglePerlinFractalBillow(x, y);
	case EFNFractalType::RigidMulti:
		return SinglePerlinFractalRigidMulti(x, y);
	default:
		return 0;
	}
}

float UFastNoise::SinglePerlinFractalFBM(float x, float y) const
{
	float sum = SinglePerlin(m_perm[0], x, y);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum += SinglePerlin(m_perm[i], x, y) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SinglePerlinFractalBillow(float x, float y) const
{
	float sum = FastAbs(SinglePerlin(m_perm[0], x, y)) * 2 - 1;
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum += (FastAbs(SinglePerlin(m_perm[i], x, y)) * 2 - 1) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SinglePerlinFractalRigidMulti(float x, float y) const
{
	float sum = 1 - FastAbs(SinglePerlin(m_perm[0], x, y));
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum -= (1 - FastAbs(SinglePerlin(m_perm[i], x, y))) * amp;
	}

	return sum;
}

float UFastNoise::GetPerlin2D(float x, float y) const
{
	return SinglePerlin(0, x * Frequency, y * Frequency);
}

float UFastNoise::SinglePerlin(uint8 offset, float x, float y) const
{
	int32 x0 = FastFloor(x);
	int32 y0 = FastFloor(y);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;

	float xs = 0;
	float ys = 0;
	switch (Interpolation)
	{
	case EFNInterp::Linear:
		xs = x - (float)x0;
		ys = y - (float)y0;
		break;
	case EFNInterp::Hermite:
		xs = InterpHermiteFunc(x - (float)x0);
		ys = InterpHermiteFunc(y - (float)y0);
		break;
	case EFNInterp::Quintic:
		xs = InterpQuinticFunc(x - (float)x0);
		ys = InterpQuinticFunc(y - (float)y0);
		break;
	}

	float xd0 = x - (float)x0;
	float yd0 = y - (float)y0;
	float xd1 = xd0 - 1;
	float yd1 = yd0 - 1;

	float xf0 = Lerp(GradCoord2D(offset, x0, y0, xd0, yd0), GradCoord2D(offset, x1, y0, xd1, yd0), xs);
	float xf1 = Lerp(GradCoord2D(offset, x0, y1, xd0, yd1), GradCoord2D(offset, x1, y1, xd1, yd1), xs);

	return Lerp(xf0, xf1, ys);
}

// Simplex Noise

float UFastNoise::GetSimplexFractal3D(float x, float y, float z) const
{
	x *= Frequency;
	y *= Frequency;
	z *= Frequency;

	switch (FractalType)
	{
	case EFNFractalType::FBM:
		return SingleSimplexFractalFBM(x, y, z);
	case EFNFractalType::Billow:
		return SingleSimplexFractalBillow(x, y, z);
	case EFNFractalType::RigidMulti:
		return SingleSimplexFractalRigidMulti(x, y, z);
	default:
		return 0;
	}
}

float UFastNoise::SingleSimplexFractalFBM(float x, float y, float z) const
{
	float sum = SingleSimplex(m_perm[0], x, y, z);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum += SingleSimplex(m_perm[i], x, y, z) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleSimplexFractalBillow(float x, float y, float z) const
{
	float sum = FastAbs(SingleSimplex(m_perm[0], x, y, z)) * 2 - 1;
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum += (FastAbs(SingleSimplex(m_perm[i], x, y, z)) * 2 - 1) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleSimplexFractalRigidMulti(float x, float y, float z) const
{
	float sum = 1 - FastAbs(SingleSimplex(m_perm[0], x, y, z));
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum -= (1 - FastAbs(SingleSimplex(m_perm[i], x, y, z))) * amp;
	}

	return sum;
}

float UFastNoise::GetSimplex3D(float x, float y, float z) const
{
	return SingleSimplex(0, x * Frequency, y * Frequency, z * Frequency);
}

static const float F3 = 1 / float(3);
static const float G3 = 1 / float(6);

float UFastNoise::SingleSimplex(uint8 offset, float x, float y, float z) const
{
	float t = (x + y + z) * F3;
	int32 i = FastFloor(x + t);
	int32 j = FastFloor(y + t);
	int32 k = FastFloor(z + t);

	t = (i + j + k) * G3;
	float X0 = i - t;
	float Y0 = j - t;
	float Z0 = k - t;

	float x0 = x - X0;
	float y0 = y - Y0;
	float z0 = z - Z0;

	int32 i1, j1, k1;
	int32 i2, j2, k2;

	if (x0 >= y0)
	{
		if (y0 >= z0)
		{
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
		else if (x0 >= z0)
		{
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;
		}
		else // x0 < z0
		{
			i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
		}
	}
	else // x0 < y0
	{
		if (y0 < z0)
		{
			i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;
		}
		else if (x0 < z0)
		{
			i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;
		}
		else // x0 >= z0
		{
			i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
	}

	float x1 = x0 - i1 + G3;
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + 2*G3;
	float y2 = y0 - j2 + 2*G3;
	float z2 = z0 - k2 + 2*G3;
	float x3 = x0 - 1 + 3*G3;
	float y3 = y0 - 1 + 3*G3;
	float z3 = z0 - 1 + 3*G3;

	float n0, n1, n2, n3;

	t = float(0.6) - x0*x0 - y0*y0 - z0*z0;
	if (t < 0) n0 = 0;
	else
	{
		t *= t;
		n0 = t*t*GradCoord3D(offset, i, j, k, x0, y0, z0);
	}

	t = float(0.6) - x1*x1 - y1*y1 - z1*z1;
	if (t < 0) n1 = 0;
	else
	{
		t *= t;
		n1 = t*t*GradCoord3D(offset, i + i1, j + j1, k + k1, x1, y1, z1);
	}

	t = float(0.6) - x2*x2 - y2*y2 - z2*z2;
	if (t < 0) n2 = 0;
	else
	{
		t *= t;
		n2 = t*t*GradCoord3D(offset, i + i2, j + j2, k + k2, x2, y2, z2);
	}

	t = float(0.6) - x3*x3 - y3*y3 - z3*z3;
	if (t < 0) n3 = 0;
	else
	{
		t *= t;
		n3 = t*t*GradCoord3D(offset, i + 1, j + 1, k + 1, x3, y3, z3);
	}

	return 32 * (n0 + n1 + n2 + n3);
}

float UFastNoise::GetSimplexFractal2D(float x, float y) const
{
	x *= Frequency;
	y *= Frequency;

	switch (FractalType)
	{
	case EFNFractalType::FBM:
		return SingleSimplexFractalFBM(x, y);
	case EFNFractalType::Billow:
		return SingleSimplexFractalBillow(x, y);
	case EFNFractalType::RigidMulti:
		return SingleSimplexFractalRigidMulti(x, y);
	default:
		return 0;
	}
}

float UFastNoise::SingleSimplexFractalFBM(float x, float y) const
{
	float sum = SingleSimplex(m_perm[0], x, y);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum += SingleSimplex(m_perm[i], x, y) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleSimplexFractalBillow(float x, float y) const
{
	float sum = FastAbs(SingleSimplex(m_perm[0], x, y)) * 2 - 1;
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum += (FastAbs(SingleSimplex(m_perm[i], x, y)) * 2 - 1) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleSimplexFractalRigidMulti(float x, float y) const
{
	float sum = 1 - FastAbs(SingleSimplex(m_perm[0], x, y));
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum -= (1 - FastAbs(SingleSimplex(m_perm[i], x, y))) * amp;
	}

	return sum;
}

float UFastNoise::SingleSimplexFractalBlend(float x, float y) const
{
	float sum = SingleSimplex(m_perm[0], x, y);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum *= SingleSimplex(m_perm[i], x, y) * amp + 1;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::GetSimplex2D(float x, float y) const
{
	return SingleSimplex(0, x * Frequency, y * Frequency);
}

//static const float F2 = 1 / float(2);
//static const float G2 = 1 / float(4);

static const float SQRT3 = float(1.7320508075688772935274463415059);
static const float F2 = float(0.5) * (SQRT3 - float(1.0));
static const float G2 = (float(3.0) - SQRT3) / float(6.0);

float UFastNoise::SingleSimplex(uint8 offset, float x, float y) const
{
	float t = (x + y) * F2;
	int32 i = FastFloor(x + t);
	int32 j = FastFloor(y + t);

	t = (i + j) * G2;
	float X0 = i - t;
	float Y0 = j - t;

	float x0 = x - X0;
	float y0 = y - Y0;

	int32 i1, j1;
	if (x0 > y0)
	{
		i1 = 1; j1 = 0;
	}
	else
	{
		i1 = 0; j1 = 1;
	}

	float x1 = x0 - (float)i1 + G2;
	float y1 = y0 - (float)j1 + G2;
	float x2 = x0 - 1 + 2*G2;
	float y2 = y0 - 1 + 2*G2;

	float n0, n1, n2;

	t = float(0.5) - x0*x0 - y0*y0;
	if (t < 0) n0 = 0;
	else
	{
		t *= t;
		n0 = t * t * GradCoord2D(offset, i, j, x0, y0);
	}

	t = float(0.5) - x1*x1 - y1*y1;
	if (t < 0) n1 = 0;
	else
	{
		t *= t;
		n1 = t*t*GradCoord2D(offset, i + i1, j + j1, x1, y1);
	}

	t = float(0.5) - x2*x2 - y2*y2;
	if (t < 0) n2 = 0;
	else
	{
		t *= t;
		n2 = t*t*GradCoord2D(offset, i + 1, j + 1, x2, y2);
	}

	return 70 * (n0 + n1 + n2);
}

float UFastNoise::GetSimplex4D(float x, float y, float z, float w) const
{
	return SingleSimplex(0, x * Frequency, y * Frequency, z * Frequency, w * Frequency);
}

static const uint8 SIMPLEX_4D[] =
{
	0,1,2,3,0,1,3,2,0,0,0,0,0,2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,0,
	0,2,1,3,0,0,0,0,0,3,1,2,0,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,1,3,2,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,2,0,3,0,0,0,0,1,3,0,2,0,0,0,0,0,0,0,0,0,0,0,0,2,3,0,1,2,3,1,0,
	1,0,2,3,1,0,3,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0,3,1,0,0,0,0,2,1,3,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	2,0,1,3,0,0,0,0,0,0,0,0,0,0,0,0,3,0,1,2,3,0,2,1,0,0,0,0,3,1,2,0,
	2,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,3,1,0,2,0,0,0,0,3,2,0,1,3,2,1,0
};

static const float F4 = (sqrt(float(5)) - 1) / 4;
static const float G4 = (5 - sqrt(float(5))) / 20;

float UFastNoise::SingleSimplex(uint8 offset, float x, float y, float z, float w) const
{
	float n0, n1, n2, n3, n4;
	float t = (x + y + z + w) * F4;
	int32 i = FastFloor(x + t);
	int32 j = FastFloor(y + t);
	int32 k = FastFloor(z + t);
	int32 l = FastFloor(w + t);
	t = (i + j + k + l) * G4;
	float X0 = i - t;
	float Y0 = j - t;
	float Z0 = k - t;
	float W0 = l - t;
	float x0 = x - X0;
	float y0 = y - Y0;
	float z0 = z - Z0;
	float w0 = w - W0;

	int32 c = (x0 > y0) ? 32 : 0;
	c += (x0 > z0) ? 16 : 0;
	c += (y0 > z0) ? 8 : 0;
	c += (x0 > w0) ? 4 : 0;
	c += (y0 > w0) ? 2 : 0;
	c += (z0 > w0) ? 1 : 0;
	c <<= 2;

	int32 i1 = SIMPLEX_4D[c] >= 3 ? 1 : 0;
	int32 i2 = SIMPLEX_4D[c] >= 2 ? 1 : 0;
	int32 i3 = SIMPLEX_4D[c++] >= 1 ? 1 : 0;
	int32 j1 = SIMPLEX_4D[c] >= 3 ? 1 : 0;
	int32 j2 = SIMPLEX_4D[c] >= 2 ? 1 : 0;
	int32 j3 = SIMPLEX_4D[c++] >= 1 ? 1 : 0;
	int32 k1 = SIMPLEX_4D[c] >= 3 ? 1 : 0;
	int32 k2 = SIMPLEX_4D[c] >= 2 ? 1 : 0;
	int32 k3 = SIMPLEX_4D[c++] >= 1 ? 1 : 0;
	int32 l1 = SIMPLEX_4D[c] >= 3 ? 1 : 0;
	int32 l2 = SIMPLEX_4D[c] >= 2 ? 1 : 0;
	int32 l3 = SIMPLEX_4D[c] >= 1 ? 1 : 0;

	float x1 = x0 - i1 + G4;
	float y1 = y0 - j1 + G4;
	float z1 = z0 - k1 + G4;
	float w1 = w0 - l1 + G4;
	float x2 = x0 - i2 + 2*G4;
	float y2 = y0 - j2 + 2*G4;
	float z2 = z0 - k2 + 2*G4;
	float w2 = w0 - l2 + 2*G4;
	float x3 = x0 - i3 + 3*G4;
	float y3 = y0 - j3 + 3*G4;
	float z3 = z0 - k3 + 3*G4;
	float w3 = w0 - l3 + 3*G4;
	float x4 = x0 - 1 + 4*G4;
	float y4 = y0 - 1 + 4*G4;
	float z4 = z0 - 1 + 4*G4;
	float w4 = w0 - 1 + 4*G4;

	t = float(0.6) - x0*x0 - y0*y0 - z0*z0 - w0*w0;
	if (t < 0) n0 = 0;
	else {
		t *= t;
		n0 = t * t * GradCoord4D(offset, i, j, k, l, x0, y0, z0, w0);
	}
	t = float(0.6) - x1*x1 - y1*y1 - z1*z1 - w1*w1;
	if (t < 0) n1 = 0;
	else {
		t *= t;
		n1 = t * t * GradCoord4D(offset, i + i1, j + j1, k + k1, l + l1, x1, y1, z1, w1);
	}
	t = float(0.6) - x2*x2 - y2*y2 - z2*z2 - w2*w2;
	if (t < 0) n2 = 0;
	else {
		t *= t;
		n2 = t * t * GradCoord4D(offset, i + i2, j + j2, k + k2, l + l2, x2, y2, z2, w2);
	}
	t = float(0.6) - x3*x3 - y3*y3 - z3*z3 - w3*w3;
	if (t < 0) n3 = 0;
	else {
		t *= t;
		n3 = t * t * GradCoord4D(offset, i + i3, j + j3, k + k3, l + l3, x3, y3, z3, w3);
	}
	t = float(0.6) - x4*x4 - y4*y4 - z4*z4 - w4*w4;
	if (t < 0) n4 = 0;
	else {
		t *= t;
		n4 = t * t * GradCoord4D(offset, i + 1, j + 1, k + 1, l + 1, x4, y4, z4, w4);
	}

	return 27 * (n0 + n1 + n2 + n3 + n4);
}

// Cubic Noise
float UFastNoise::GetCubicFractal3D(float x, float y, float z) const
{
	x *= Frequency;
	y *= Frequency;
	z *= Frequency;

	switch (FractalType)
	{
	case EFNFractalType::FBM:
		return SingleCubicFractalFBM(x, y, z);
	case EFNFractalType::Billow:
		return SingleCubicFractalBillow(x, y, z);
	case EFNFractalType::RigidMulti:
		return SingleCubicFractalRigidMulti(x, y, z);
	default:
		return 0;
	}
}

float UFastNoise::SingleCubicFractalFBM(float x, float y, float z) const
{
	float sum = SingleCubic(m_perm[0], x, y, z);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum += SingleCubic(m_perm[i], x, y, z) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleCubicFractalBillow(float x, float y, float z) const
{
	float sum = FastAbs(SingleCubic(m_perm[0], x, y, z)) * 2 - 1;
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum += (FastAbs(SingleCubic(m_perm[i], x, y, z)) * 2 - 1) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleCubicFractalRigidMulti(float x, float y, float z) const
{
	float sum = 1 - FastAbs(SingleCubic(m_perm[0], x, y, z));
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;
		z *= FractalLacunarity;

		amp *= FractalGain;
		sum -= (1 - FastAbs(SingleCubic(m_perm[i], x, y, z))) * amp;
	}

	return sum;
}

float UFastNoise::GetCubic3D(float x, float y, float z) const
{
	return SingleCubic(0, x * Frequency, y * Frequency, z * Frequency);
}

const float CUBIC_3D_BOUNDING = 1 / (float(1.5) * float(1.5) * float(1.5));

float UFastNoise::SingleCubic(uint8 offset, float x, float y, float z) const
{
	int32 x1 = FastFloor(x);
	int32 y1 = FastFloor(y);
	int32 z1 = FastFloor(z);

	int32 x0 = x1 - 1;
	int32 y0 = y1 - 1;
	int32 z0 = z1 - 1;
	int32 x2 = x1 + 1;
	int32 y2 = y1 + 1;
	int32 z2 = z1 + 1;
	int32 x3 = x1 + 2;
	int32 y3 = y1 + 2;
	int32 z3 = z1 + 2;

	float xs = x - (float)x1;
	float ys = y - (float)y1;
	float zs = z - (float)z1;

	return CubicLerp(
		CubicLerp(
		CubicLerp(ValCoord3DFast(offset, x0, y0, z0), ValCoord3DFast(offset, x1, y0, z0), ValCoord3DFast(offset, x2, y0, z0), ValCoord3DFast(offset, x3, y0, z0), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y1, z0), ValCoord3DFast(offset, x1, y1, z0), ValCoord3DFast(offset, x2, y1, z0), ValCoord3DFast(offset, x3, y1, z0), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y2, z0), ValCoord3DFast(offset, x1, y2, z0), ValCoord3DFast(offset, x2, y2, z0), ValCoord3DFast(offset, x3, y2, z0), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y3, z0), ValCoord3DFast(offset, x1, y3, z0), ValCoord3DFast(offset, x2, y3, z0), ValCoord3DFast(offset, x3, y3, z0), xs),
		ys),
		CubicLerp(
		CubicLerp(ValCoord3DFast(offset, x0, y0, z1), ValCoord3DFast(offset, x1, y0, z1), ValCoord3DFast(offset, x2, y0, z1), ValCoord3DFast(offset, x3, y0, z1), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y1, z1), ValCoord3DFast(offset, x1, y1, z1), ValCoord3DFast(offset, x2, y1, z1), ValCoord3DFast(offset, x3, y1, z1), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y2, z1), ValCoord3DFast(offset, x1, y2, z1), ValCoord3DFast(offset, x2, y2, z1), ValCoord3DFast(offset, x3, y2, z1), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y3, z1), ValCoord3DFast(offset, x1, y3, z1), ValCoord3DFast(offset, x2, y3, z1), ValCoord3DFast(offset, x3, y3, z1), xs),
		ys),
		CubicLerp(
		CubicLerp(ValCoord3DFast(offset, x0, y0, z2), ValCoord3DFast(offset, x1, y0, z2), ValCoord3DFast(offset, x2, y0, z2), ValCoord3DFast(offset, x3, y0, z2), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y1, z2), ValCoord3DFast(offset, x1, y1, z2), ValCoord3DFast(offset, x2, y1, z2), ValCoord3DFast(offset, x3, y1, z2), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y2, z2), ValCoord3DFast(offset, x1, y2, z2), ValCoord3DFast(offset, x2, y2, z2), ValCoord3DFast(offset, x3, y2, z2), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y3, z2), ValCoord3DFast(offset, x1, y3, z2), ValCoord3DFast(offset, x2, y3, z2), ValCoord3DFast(offset, x3, y3, z2), xs),
		ys),
		CubicLerp(
		CubicLerp(ValCoord3DFast(offset, x0, y0, z3), ValCoord3DFast(offset, x1, y0, z3), ValCoord3DFast(offset, x2, y0, z3), ValCoord3DFast(offset, x3, y0, z3), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y1, z3), ValCoord3DFast(offset, x1, y1, z3), ValCoord3DFast(offset, x2, y1, z3), ValCoord3DFast(offset, x3, y1, z3), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y2, z3), ValCoord3DFast(offset, x1, y2, z3), ValCoord3DFast(offset, x2, y2, z3), ValCoord3DFast(offset, x3, y2, z3), xs),
		CubicLerp(ValCoord3DFast(offset, x0, y3, z3), ValCoord3DFast(offset, x1, y3, z3), ValCoord3DFast(offset, x2, y3, z3), ValCoord3DFast(offset, x3, y3, z3), xs),
		ys),
		zs) * CUBIC_3D_BOUNDING;
}


float UFastNoise::GetCubicFractal2D(float x, float y) const
{
	x *= Frequency;
	y *= Frequency;

	switch (FractalType)
	{
	case EFNFractalType::FBM:
		return SingleCubicFractalFBM(x, y);
	case EFNFractalType::Billow:
		return SingleCubicFractalBillow(x, y);
	case EFNFractalType::RigidMulti:
		return SingleCubicFractalRigidMulti(x, y);
	default:
		return 0;
	}
}

float UFastNoise::SingleCubicFractalFBM(float x, float y) const
{
	float sum = SingleCubic(m_perm[0], x, y);
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum += SingleCubic(m_perm[i], x, y) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleCubicFractalBillow(float x, float y) const
{
	float sum = FastAbs(SingleCubic(m_perm[0], x, y)) * 2 - 1;
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum += (FastAbs(SingleCubic(m_perm[i], x, y)) * 2 - 1) * amp;
	}

	return sum * m_fractalBounding;
}

float UFastNoise::SingleCubicFractalRigidMulti(float x, float y) const
{
	float sum = 1 - FastAbs(SingleCubic(m_perm[0], x, y));
	float amp = 1;
	int32 i = 0;

	while (++i < FractalOctaves)
	{
		x *= FractalLacunarity;
		y *= FractalLacunarity;

		amp *= FractalGain;
		sum -= (1 - FastAbs(SingleCubic(m_perm[i], x, y))) * amp;
	}

	return sum;
}

float UFastNoise::GetCubic2D(float x, float y) const
{
	x *= Frequency;
	y *= Frequency;

	return SingleCubic(0, x, y);
}

const float CUBIC_2D_BOUNDING = 1 / (float(1.5) * float(1.5));

float UFastNoise::SingleCubic(uint8 offset, float x, float y) const
{
	int32 x1 = FastFloor(x);
	int32 y1 = FastFloor(y);

	int32 x0 = x1 - 1;
	int32 y0 = y1 - 1;
	int32 x2 = x1 + 1;
	int32 y2 = y1 + 1;
	int32 x3 = x1 + 2;
	int32 y3 = y1 + 2;

	float xs = x - (float)x1;
	float ys = y - (float)y1;

	return CubicLerp(
		CubicLerp(ValCoord2DFast(offset, x0, y0), ValCoord2DFast(offset, x1, y0), ValCoord2DFast(offset, x2, y0), ValCoord2DFast(offset, x3, y0), xs),
		CubicLerp(ValCoord2DFast(offset, x0, y1), ValCoord2DFast(offset, x1, y1), ValCoord2DFast(offset, x2, y1), ValCoord2DFast(offset, x3, y1), xs),
		CubicLerp(ValCoord2DFast(offset, x0, y2), ValCoord2DFast(offset, x1, y2), ValCoord2DFast(offset, x2, y2), ValCoord2DFast(offset, x3, y2), xs),
		CubicLerp(ValCoord2DFast(offset, x0, y3), ValCoord2DFast(offset, x1, y3), ValCoord2DFast(offset, x2, y3), ValCoord2DFast(offset, x3, y3), xs),
		ys) * CUBIC_2D_BOUNDING;
}

// Cellular Noise
float UFastNoise::GetCellular3D(float x, float y, float z) const
{
	x *= Frequency;
	y *= Frequency;
	z *= Frequency;

	switch (CellularReturnType)
	{
	case EFNCellularReturnType::CellValue:
	case EFNCellularReturnType::NoiseLookup:
	case EFNCellularReturnType::Distance:
		return SingleCellular(x, y, z);
	default:
		return SingleCellular2Edge(x, y, z);
	}
}

float UFastNoise::SingleCellular(float x, float y, float z) const
{
	int32 xr = FastRound(x);
	int32 yr = FastRound(y);
	int32 zr = FastRound(z);

	float distance = 999999;

	int32 xc = 0;
	int32 yc = 0;
	int32 zc = 0;

	switch (CellularDistanceFunction)
	{
	case EFNCellularDistanceFunction::Euclidean:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos] * CellularJitter;
					float vecY = yi - y + CELL_3D_Y[lutPos] * CellularJitter;
					float vecZ = zi - z + CELL_3D_Z[lutPos] * CellularJitter;

					float newDistance = vecX * vecX + vecY * vecY + vecZ * vecZ;

					if (newDistance < distance)
					{
						distance = newDistance;
						xc = xi;
						yc = yi;
						zc = zi;
					}
				}
			}
		}
		break;
	case EFNCellularDistanceFunction::Manhattan:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos] * CellularJitter;
					float vecY = yi - y + CELL_3D_Y[lutPos] * CellularJitter;
					float vecZ = zi - z + CELL_3D_Z[lutPos] * CellularJitter;

					float newDistance = FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ);

					if (newDistance < distance)
					{
						distance = newDistance;
						xc = xi;
						yc = yi;
						zc = zi;
					}
				}
			}
		}
		break;
	case EFNCellularDistanceFunction::Natural:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos] * CellularJitter;
					float vecY = yi - y + CELL_3D_Y[lutPos] * CellularJitter;
					float vecZ = zi - z + CELL_3D_Z[lutPos] * CellularJitter;

					float newDistance = (FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ)) + (vecX * vecX + vecY * vecY + vecZ * vecZ);

					if (newDistance < distance)
					{
						distance = newDistance;
						xc = xi;
						yc = yi;
						zc = zi;
					}
				}
			}
		}
		break;
	default:
		break;
	}

	uint8 lutPos;
	switch (CellularReturnType)
	{
	case EFNCellularReturnType::CellValue:
		return ValCoord3D(Seed, xc, yc, zc);

	case EFNCellularReturnType::NoiseLookup:
		assert(CellularNoiseLookup);

		lutPos = Index3D_256(0, xc, yc, zc);
		return CellularNoiseLookup->GetNoise3D(xc + CELL_3D_X[lutPos] * CellularJitter, yc + CELL_3D_Y[lutPos] * CellularJitter, zc + CELL_3D_Z[lutPos] * CellularJitter);

	case EFNCellularReturnType::Distance:
		return distance;
	default:
		return 0;
	}
}

float UFastNoise::SingleCellular2Edge(float x, float y, float z) const
{
	int32 xr = FastRound(x);
	int32 yr = FastRound(y);
	int32 zr = FastRound(z);

	float distance[FN_CELLULAR_INDEX_MAX+1] = { 999999,999999,999999,999999 };

	switch (CellularDistanceFunction)
	{
	case EFNCellularDistanceFunction::Euclidean:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos] * CellularJitter;
					float vecY = yi - y + CELL_3D_Y[lutPos] * CellularJitter;
					float vecZ = zi - z + CELL_3D_Z[lutPos] * CellularJitter;

					float newDistance = vecX * vecX + vecY * vecY + vecZ * vecZ;

					for (int32 i = CellularDistanceIndex1; i > 0; i--)
						distance[i] = fmax(fmin(distance[i], newDistance), distance[i - 1]);
					distance[0] = fmin(distance[0], newDistance); 
				}
			}
		}
		break;
	case EFNCellularDistanceFunction::Manhattan:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos] * CellularJitter;
					float vecY = yi - y + CELL_3D_Y[lutPos] * CellularJitter;
					float vecZ = zi - z + CELL_3D_Z[lutPos] * CellularJitter;

					float newDistance = FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ);

					for (int32 i = CellularDistanceIndex1; i > 0; i--)
						distance[i] = fmax(fmin(distance[i], newDistance), distance[i - 1]);
					distance[0] = fmin(distance[0], newDistance);
				}
			}
		}
		break;
	case EFNCellularDistanceFunction::Natural:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos] * CellularJitter;
					float vecY = yi - y + CELL_3D_Y[lutPos] * CellularJitter;
					float vecZ = zi - z + CELL_3D_Z[lutPos] * CellularJitter;

					float newDistance = (FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ)) + (vecX * vecX + vecY * vecY + vecZ * vecZ);

					for (int32 i = CellularDistanceIndex1; i > 0; i--)
						distance[i] = fmax(fmin(distance[i], newDistance), distance[i - 1]);
					distance[0] = fmin(distance[0], newDistance);
				}
			}
		}
		break;
	default:
		break;
	}

	switch (CellularReturnType)
	{
	case EFNCellularReturnType::Distance2:
		return distance[CellularDistanceIndex1];
	case EFNCellularReturnType::Distance2Add:
		return distance[CellularDistanceIndex1] + distance[CellularDistanceIndex0];
	case EFNCellularReturnType::Distance2Sub:
		return distance[CellularDistanceIndex1] - distance[CellularDistanceIndex0];
	case EFNCellularReturnType::Distance2Mul:
		return distance[CellularDistanceIndex1] * distance[CellularDistanceIndex0];
	case EFNCellularReturnType::Distance2Div:
		return distance[CellularDistanceIndex0] / distance[CellularDistanceIndex1];
	default:
		return 0;
	}
}

float UFastNoise::GetCellular2D(float x, float y) const
{
	x *= Frequency;
	y *= Frequency;

	switch (CellularReturnType)
	{
	case EFNCellularReturnType::CellValue:
	case EFNCellularReturnType::NoiseLookup:
	case EFNCellularReturnType::Distance:
		return SingleCellular(x, y);
	default:
		return SingleCellular2Edge(x, y);
	}
}

float UFastNoise::SingleCellular(float x, float y) const
{
	int32 xr = FastRound(x);
	int32 yr = FastRound(y);

	float distance = 999999;
	int32 xc = 0;
	int32 yc = 0;

	switch (CellularDistanceFunction)
	{
	default:
	case EFNCellularDistanceFunction::Euclidean:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos] * CellularJitter;
				float vecY = yi - y + CELL_2D_Y[lutPos] * CellularJitter;
															
				float newDistance = vecX * vecX + vecY * vecY;

				if (newDistance < distance)
				{
					distance = newDistance;
					xc = xi;
					yc = yi;
				}
			}
		}
		break;
	case EFNCellularDistanceFunction::Manhattan:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos] * CellularJitter;
				float vecY = yi - y + CELL_2D_Y[lutPos] * CellularJitter;

				float newDistance = (FastAbs(vecX) + FastAbs(vecY));

				if (newDistance < distance)
				{
					distance = newDistance;
					xc = xi;
					yc = yi;
				}
			}
		}
		break;
	case EFNCellularDistanceFunction::Natural:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos] * CellularJitter;
				float vecY = yi - y + CELL_2D_Y[lutPos] * CellularJitter;

				float newDistance = (FastAbs(vecX) + FastAbs(vecY)) + (vecX * vecX + vecY * vecY);

				if (newDistance < distance)
				{
					distance = newDistance;
					xc = xi;
					yc = yi;
				}
			}
		}
		break;
	}

	uint8 lutPos;
	switch (CellularReturnType)
	{
	case EFNCellularReturnType::CellValue:
		return ValCoord2D(Seed, xc, yc);

	case EFNCellularReturnType::NoiseLookup:
		assert(CellularNoiseLookup);

		lutPos = Index2D_256(0, xc, yc);
		return CellularNoiseLookup->GetNoise2D(xc + CELL_2D_X[lutPos] * CellularJitter, yc + CELL_2D_Y[lutPos] * CellularJitter);

	case EFNCellularReturnType::Distance:
		return distance;
	default:
		return 0;
	}
}

float UFastNoise::SingleCellular2Edge(float x, float y) const
{
	int32 xr = FastRound(x);
	int32 yr = FastRound(y);

	float distance[FN_CELLULAR_INDEX_MAX + 1] = { 999999,999999,999999,999999 };

	switch (CellularDistanceFunction)
	{
	default:
	case EFNCellularDistanceFunction::Euclidean:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos] * CellularJitter;
				float vecY = yi - y + CELL_2D_Y[lutPos] * CellularJitter;

				float newDistance = vecX * vecX + vecY * vecY;

				for (int32 i = CellularDistanceIndex1; i > 0; i--)
					distance[i] = fmax(fmin(distance[i], newDistance), distance[i - 1]);
				distance[0] = fmin(distance[0], newDistance);
			}
		}
		break;
	case EFNCellularDistanceFunction::Manhattan:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos] * CellularJitter;
				float vecY = yi - y + CELL_2D_Y[lutPos] * CellularJitter;

				float newDistance = FastAbs(vecX) + FastAbs(vecY);

				for (int32 i = CellularDistanceIndex1; i > 0; i--)
					distance[i] = fmax(fmin(distance[i], newDistance), distance[i - 1]);
				distance[0] = fmin(distance[0], newDistance);
			}
		}
		break;
	case EFNCellularDistanceFunction::Natural:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos] * CellularJitter;
				float vecY = yi - y + CELL_2D_Y[lutPos] * CellularJitter;

				float newDistance = (FastAbs(vecX) + FastAbs(vecY)) + (vecX * vecX + vecY * vecY);

				for (int32 i = CellularDistanceIndex1; i > 0; i--)
					distance[i] = fmax(fmin(distance[i], newDistance), distance[i - 1]);
				distance[0] = fmin(distance[0], newDistance);
			}
		}
		break;
	}

	switch (CellularReturnType)
	{
	case EFNCellularReturnType::Distance2:
		return distance[CellularDistanceIndex1];
	case EFNCellularReturnType::Distance2Add:
		return distance[CellularDistanceIndex1] + distance[CellularDistanceIndex0];
	case EFNCellularReturnType::Distance2Sub:
		return distance[CellularDistanceIndex1] - distance[CellularDistanceIndex0];
	case EFNCellularReturnType::Distance2Mul:
		return distance[CellularDistanceIndex1] * distance[CellularDistanceIndex0];
	case EFNCellularReturnType::Distance2Div:
		return distance[CellularDistanceIndex0] / distance[CellularDistanceIndex1];
	default:
		return 0;
	}
}

void UFastNoise::GradientPerturb3D(float& x, float& y, float& z) const
{
	SingleGradientPerturb(0, GradientPerturbAmp, Frequency, x, y, z);
}

void UFastNoise::GradientPerturbFractal3D(float& x, float& y, float& z) const
{
	float amp = GradientPerturbAmp * m_fractalBounding;
	float freq = Frequency;
	int32 i = 0;

	SingleGradientPerturb(m_perm[0], amp, Frequency, x, y, z);

	while (++i < FractalOctaves)
	{
		freq *= FractalLacunarity;
		amp *= FractalGain;
		SingleGradientPerturb(m_perm[i], amp, freq, x, y, z);
	}
}

void UFastNoise::SingleGradientPerturb(uint8 offset, float warpAmp, float frequency, float& x, float& y, float& z) const
{
	float xf = x * frequency;
	float yf = y * frequency;
	float zf = z * frequency;

	int32 x0 = FastFloor(xf);
	int32 y0 = FastFloor(yf);
	int32 z0 = FastFloor(zf);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;
	int32 z1 = z0 + 1;

	float xs, ys, zs;
	switch (Interpolation)
	{
	default:
	case EFNInterp::Linear:
		xs = xf - (float)x0;
		ys = yf - (float)y0;
		zs = zf - (float)z0;
		break;
	case EFNInterp::Hermite:
		xs = InterpHermiteFunc(xf - (float)x0);
		ys = InterpHermiteFunc(yf - (float)y0);
		zs = InterpHermiteFunc(zf - (float)z0);
		break;
	case EFNInterp::Quintic:
		xs = InterpQuinticFunc(xf - (float)x0);
		ys = InterpQuinticFunc(yf - (float)y0);
		zs = InterpQuinticFunc(zf - (float)z0);
		break;
	}

	int32 lutPos0 = Index3D_256(offset, x0, y0, z0);
	int32 lutPos1 = Index3D_256(offset, x1, y0, z0);

	float lx0x = Lerp(CELL_3D_X[lutPos0], CELL_3D_X[lutPos1], xs);
	float ly0x = Lerp(CELL_3D_Y[lutPos0], CELL_3D_Y[lutPos1], xs);
	float lz0x = Lerp(CELL_3D_Z[lutPos0], CELL_3D_Z[lutPos1], xs);

	lutPos0 = Index3D_256(offset, x0, y1, z0);
	lutPos1 = Index3D_256(offset, x1, y1, z0);

	float lx1x = Lerp(CELL_3D_X[lutPos0], CELL_3D_X[lutPos1], xs);
	float ly1x = Lerp(CELL_3D_Y[lutPos0], CELL_3D_Y[lutPos1], xs);
	float lz1x = Lerp(CELL_3D_Z[lutPos0], CELL_3D_Z[lutPos1], xs);

	float lx0y = Lerp(lx0x, lx1x, ys);
	float ly0y = Lerp(ly0x, ly1x, ys);
	float lz0y = Lerp(lz0x, lz1x, ys);

	lutPos0 = Index3D_256(offset, x0, y0, z1);
	lutPos1 = Index3D_256(offset, x1, y0, z1);

	lx0x = Lerp(CELL_3D_X[lutPos0], CELL_3D_X[lutPos1], xs);
	ly0x = Lerp(CELL_3D_Y[lutPos0], CELL_3D_Y[lutPos1], xs);
	lz0x = Lerp(CELL_3D_Z[lutPos0], CELL_3D_Z[lutPos1], xs);

	lutPos0 = Index3D_256(offset, x0, y1, z1);
	lutPos1 = Index3D_256(offset, x1, y1, z1);

	lx1x = Lerp(CELL_3D_X[lutPos0], CELL_3D_X[lutPos1], xs);
	ly1x = Lerp(CELL_3D_Y[lutPos0], CELL_3D_Y[lutPos1], xs);
	lz1x = Lerp(CELL_3D_Z[lutPos0], CELL_3D_Z[lutPos1], xs);

	x += Lerp(lx0y, Lerp(lx0x, lx1x, ys), zs) * warpAmp;
	y += Lerp(ly0y, Lerp(ly0x, ly1x, ys), zs) * warpAmp;
	z += Lerp(lz0y, Lerp(lz0x, lz1x, ys), zs) * warpAmp;
}

void UFastNoise::GradientPerturb2D(float& x, float& y) const
{
	SingleGradientPerturb(0, GradientPerturbAmp, Frequency, x, y);
}

void UFastNoise::GradientPerturbFractal2D(float& x, float& y) const
{
	float amp = GradientPerturbAmp * m_fractalBounding;
	float freq = Frequency;
	int32 i = 0;

	SingleGradientPerturb(m_perm[0], amp, Frequency, x, y);

	while (++i < FractalOctaves)
	{
		freq *= FractalLacunarity;
		amp *= FractalGain;
		SingleGradientPerturb(m_perm[i], amp, freq, x, y);
	}
}

void UFastNoise::SingleGradientPerturb(uint8 offset, float warpAmp, float frequency, float& x, float& y) const
{
	float xf = x * frequency;
	float yf = y * frequency;

	int32 x0 = FastFloor(xf);
	int32 y0 = FastFloor(yf);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;

	float xs, ys;
	switch (Interpolation)
	{
	default:
	case EFNInterp::Linear:
		xs = xf - (float)x0;
		ys = yf - (float)y0;
		break;
	case EFNInterp::Hermite:
		xs = InterpHermiteFunc(xf - (float)x0);
		ys = InterpHermiteFunc(yf - (float)y0);
		break;
	case EFNInterp::Quintic:
		xs = InterpQuinticFunc(xf - (float)x0);
		ys = InterpQuinticFunc(yf - (float)y0);
		break;
	}

	int32 lutPos0 = Index2D_256(offset, x0, y0);
	int32 lutPos1 = Index2D_256(offset, x1, y0);

	float lx0x = Lerp(CELL_2D_X[lutPos0], CELL_2D_X[lutPos1], xs);
	float ly0x = Lerp(CELL_2D_Y[lutPos0], CELL_2D_Y[lutPos1], xs);

	lutPos0 = Index2D_256(offset, x0, y1);
	lutPos1 = Index2D_256(offset, x1, y1);

	float lx1x = Lerp(CELL_2D_X[lutPos0], CELL_2D_X[lutPos1], xs);
	float ly1x = Lerp(CELL_2D_Y[lutPos0], CELL_2D_Y[lutPos1], xs);

	x += Lerp(lx0x, lx1x, ys) * warpAmp;
	y += Lerp(ly0x, ly1x, ys) * warpAmp;
}
