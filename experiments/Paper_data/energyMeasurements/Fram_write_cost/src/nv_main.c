/*
 Author: Amjad Yousef Majid
 Date: 06/feb/2017
*/
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>

#ifdef CONFIG_EDB
#include <libedb/edb.h>
#endif

#ifdef TEST_EIF_PRINTF
#include <libio/printf.h>
#endif // TEST_EIF_PRINTF

#include "pins.h"

#define __nv __attribute__((section(".nv_vars")))

volatile __nv unsigned int nv_num[400] = {0} ;
volatile unsigned int i, j;


static void init_hw()
{
    msp_watchdog_disable();
    msp_gpio_unlock();
    msp_clock_setup();
}


int main() {
init_hw();  
#ifdef CONFIG_EDB
    edb_init();
#endif

for(i=0; i <= 10; i++ )
  {
    WATCHPOINT(0);

nv_num[239] = 50608;
nv_num[181] = 17802;
nv_num[107] = 15152;
nv_num[11] = 64731;
nv_num[372] = 57071;
nv_num[246] = 51221;
nv_num[333] = 30053;
nv_num[58] = 22800;
nv_num[223] = 51231;
nv_num[347] = 28078;
nv_num[164] = 60849;
nv_num[7] = 43650;
nv_num[227] = 52078;
nv_num[380] = 43426;
nv_num[135] = 63205;
nv_num[71] = 6670;
nv_num[194] = 44194;
nv_num[32] = 49166;
nv_num[392] = 48523;
nv_num[320] = 6934;
nv_num[148] = 48794;
nv_num[161] = 44762;
nv_num[282] = 59818;
nv_num[153] = 50739;
nv_num[129] = 17455;
nv_num[35] = 62174;
nv_num[114] = 34164;
nv_num[86] = 13617;
nv_num[138] = 33565;
nv_num[300] = 5675;
nv_num[232] = 14092;
nv_num[109] = 23554;
nv_num[45] = 36129;
nv_num[4] = 32770;
nv_num[369] = 11943;
nv_num[321] = 51013;
nv_num[306] = 11259;
nv_num[384] = 18010;
nv_num[115] = 42948;
nv_num[63] = 17665;
nv_num[30] = 25736;
nv_num[394] = 32171;
nv_num[229] = 56606;
nv_num[125] = 13630;
nv_num[0] = 49736;
nv_num[386] = 32709;
nv_num[219] = 63326;
nv_num[328] = 5618;
nv_num[60] = 12717;
nv_num[193] = 7050;
nv_num[362] = 26993;
nv_num[286] = 20447;
nv_num[59] = 3840;
nv_num[85] = 39712;
nv_num[273] = 14265;
nv_num[139] = 30510;
nv_num[344] = 55994;
nv_num[153] = 5498;
nv_num[108] = 60369;
nv_num[369] = 48767;
nv_num[315] = 17686;
nv_num[78] = 42001;
nv_num[8] = 1469;
nv_num[282] = 28405;
nv_num[205] = 39926;
nv_num[230] = 54899;
nv_num[385] = 56555;
nv_num[14] = 9192;
nv_num[242] = 64991;
nv_num[291] = 38159;
nv_num[43] = 41225;
nv_num[58] = 61890;
nv_num[354] = 768;
nv_num[329] = 59443;
nv_num[392] = 39098;
nv_num[119] = 26225;
nv_num[373] = 55404;
nv_num[213] = 31094;
nv_num[370] = 23837;
nv_num[201] = 10446;
nv_num[260] = 3032;
nv_num[204] = 47663;
nv_num[19] = 46949;
nv_num[255] = 1698;
nv_num[287] = 57748;
nv_num[314] = 24540;
nv_num[130] = 24576;
nv_num[253] = 41159;
nv_num[34] = 6207;
nv_num[36] = 39784;
nv_num[221] = 24284;
nv_num[102] = 50815;
nv_num[264] = 51402;
nv_num[383] = 8249;
nv_num[71] = 47443;
nv_num[201] = 21826;
nv_num[378] = 48160;
nv_num[324] = 38460;
nv_num[216] = 28011;
nv_num[16] = 6926;
nv_num[246] = 27053;
nv_num[256] = 57896;
nv_num[177] = 35195;
nv_num[84] = 23831;
nv_num[105] = 18241;
nv_num[2] = 57542;
nv_num[57] = 2074;
nv_num[289] = 12204;
nv_num[20] = 60842;
nv_num[293] = 53466;
nv_num[59] = 45401;
nv_num[58] = 47997;
nv_num[19] = 59877;
nv_num[386] = 3851;
nv_num[237] = 41701;
nv_num[150] = 34188;
nv_num[42] = 53555;
nv_num[193] = 57292;
nv_num[163] = 45228;
nv_num[13] = 21540;
nv_num[34] = 59642;
nv_num[166] = 32628;
nv_num[104] = 7416;
nv_num[174] = 64868;
nv_num[172] = 23947;
nv_num[210] = 27989;
nv_num[285] = 58968;
nv_num[30] = 57807;
nv_num[259] = 63274;
nv_num[20] = 64928;
nv_num[206] = 38568;
nv_num[73] = 43063;
nv_num[77] = 11493;
nv_num[329] = 47944;
nv_num[335] = 29878;
nv_num[269] = 48016;
nv_num[292] = 169;
nv_num[361] = 22185;
nv_num[91] = 38610;
nv_num[149] = 130;
nv_num[27] = 25777;
nv_num[142] = 55345;
nv_num[170] = 14533;
nv_num[275] = 19468;
nv_num[261] = 51336;
nv_num[289] = 47614;
nv_num[335] = 52825;
nv_num[19] = 14964;
nv_num[75] = 25574;
nv_num[234] = 57820;
nv_num[399] = 7476;
nv_num[144] = 20708;
nv_num[383] = 21886;
nv_num[346] = 17566;
nv_num[305] = 6209;
nv_num[381] = 20280;
nv_num[101] = 36593;
nv_num[365] = 24875;
nv_num[358] = 34596;
nv_num[223] = 54460;
nv_num[360] = 24937;
nv_num[180] = 52916;
nv_num[173] = 50057;
nv_num[104] = 2939;
nv_num[205] = 3717;
nv_num[355] = 12030;
nv_num[4] = 44765;
nv_num[340] = 8719;
nv_num[373] = 57315;
nv_num[299] = 20799;
nv_num[336] = 24765;
nv_num[156] = 52519;
nv_num[15] = 42288;
nv_num[213] = 29198;
nv_num[324] = 39861;
nv_num[328] = 63714;
nv_num[86] = 15324;
nv_num[12] = 50465;
nv_num[134] = 41655;
nv_num[166] = 44955;
nv_num[332] = 29024;
nv_num[150] = 31399;
nv_num[386] = 25004;
nv_num[40] = 21702;
nv_num[369] = 47174;
nv_num[384] = 11438;
nv_num[347] = 44285;
nv_num[226] = 49379;
nv_num[292] = 51887;
nv_num[369] = 62024;
nv_num[58] = 17120;
nv_num[36] = 24483;
nv_num[304] = 10005;
nv_num[44] = 18296;
nv_num[61] = 13586;
nv_num[358] = 24844;
nv_num[0] = 40190;
nv_num[204] = 34835;
nv_num[3] = 39763;
nv_num[186] = 59138;
nv_num[204] = 38900;
nv_num[134] = 44691;
nv_num[136] = 25619;
nv_num[190] = 15586;
nv_num[258] = 11867;
nv_num[316] = 28615;
nv_num[26] = 35206;
nv_num[185] = 41380;
nv_num[31] = 61023;
nv_num[291] = 24263;
nv_num[30] = 24698;
nv_num[0] = 15107;
nv_num[207] = 25897;
nv_num[194] = 8497;
nv_num[243] = 51388;
nv_num[305] = 61699;
nv_num[392] = 47081;
nv_num[240] = 17338;
nv_num[1] = 53551;
nv_num[196] = 16907;
nv_num[368] = 41846;
nv_num[106] = 60418;
nv_num[274] = 15059;
nv_num[335] = 10869;
nv_num[306] = 22586;
nv_num[377] = 17201;
nv_num[375] = 54907;
nv_num[292] = 57035;
nv_num[136] = 34714;
nv_num[350] = 54128;
nv_num[180] = 4960;
nv_num[77] = 50823;
nv_num[226] = 61810;
nv_num[394] = 9702;
nv_num[134] = 6446;
nv_num[171] = 64387;
nv_num[280] = 23511;
nv_num[61] = 61792;
nv_num[130] = 5127;
nv_num[73] = 44385;
nv_num[396] = 29276;
nv_num[249] = 14677;
nv_num[303] = 14944;
nv_num[334] = 45248;
nv_num[87] = 47627;
nv_num[27] = 25193;
nv_num[176] = 53290;
nv_num[382] = 44548;
nv_num[320] = 34647;
nv_num[324] = 26895;
nv_num[105] = 49750;
nv_num[92] = 2409;
nv_num[279] = 61552;
nv_num[87] = 22382;
nv_num[62] = 24179;
nv_num[88] = 10059;
nv_num[290] = 29019;
nv_num[164] = 27852;
nv_num[313] = 1550;
nv_num[375] = 33889;
nv_num[80] = 19032;
nv_num[331] = 26613;
nv_num[184] = 10250;
nv_num[368] = 40297;
nv_num[74] = 45846;
nv_num[205] = 5551;
nv_num[226] = 29801;
nv_num[127] = 44483;
nv_num[104] = 24118;
nv_num[182] = 20991;
nv_num[299] = 59100;
nv_num[213] = 19388;
nv_num[152] = 6390;
nv_num[289] = 24635;
nv_num[258] = 7884;
nv_num[195] = 39331;
nv_num[254] = 33186;
nv_num[113] = 20781;
nv_num[99] = 32800;
nv_num[86] = 30276;
nv_num[268] = 63115;
nv_num[280] = 48322;
nv_num[180] = 28997;
nv_num[353] = 33277;
nv_num[116] = 38551;
nv_num[345] = 53845;
nv_num[129] = 26818;
nv_num[272] = 58182;
nv_num[24] = 20997;
nv_num[120] = 13611;
nv_num[337] = 63950;
nv_num[147] = 9679;
nv_num[250] = 11738;
nv_num[376] = 1600;
nv_num[197] = 28934;
nv_num[394] = 31508;
nv_num[297] = 44060;
nv_num[179] = 45174;
nv_num[387] = 1088;
nv_num[335] = 48428;
nv_num[254] = 41009;
nv_num[373] = 62162;
nv_num[343] = 53164;
nv_num[326] = 34163;
nv_num[294] = 49505;
nv_num[174] = 15441;
nv_num[103] = 17077;
nv_num[293] = 42472;
nv_num[168] = 22614;
nv_num[102] = 35530;
nv_num[56] = 16285;
nv_num[77] = 20363;
nv_num[397] = 25524;
nv_num[218] = 38771;
nv_num[262] = 34176;
nv_num[224] = 14360;
nv_num[211] = 22130;
nv_num[122] = 33827;
nv_num[310] = 37476;
nv_num[98] = 51899;
nv_num[154] = 25507;
nv_num[20] = 15144;
nv_num[175] = 19789;
nv_num[101] = 62140;
nv_num[351] = 7525;
nv_num[191] = 5511;
nv_num[63] = 62930;
nv_num[388] = 44680;
nv_num[203] = 10589;
nv_num[354] = 4737;
nv_num[298] = 64069;
nv_num[344] = 1841;
nv_num[3] = 30855;
nv_num[368] = 551;
nv_num[359] = 24418;
nv_num[123] = 62947;
nv_num[221] = 56107;
nv_num[0] = 49538;
nv_num[273] = 23585;
nv_num[278] = 40993;
nv_num[124] = 58460;
nv_num[264] = 28342;
nv_num[301] = 25586;
nv_num[129] = 49698;
nv_num[223] = 50449;
nv_num[202] = 22421;
nv_num[71] = 12203;
nv_num[49] = 40109;
nv_num[219] = 46645;
nv_num[332] = 14120;
nv_num[240] = 9933;
nv_num[198] = 22269;
nv_num[319] = 28517;
nv_num[275] = 33073;
nv_num[34] = 20637;
nv_num[149] = 55521;
nv_num[174] = 41833;
nv_num[1] = 24563;
nv_num[294] = 14541;
nv_num[136] = 11411;
nv_num[376] = 29093;
nv_num[49] = 12273;
nv_num[111] = 14838;
nv_num[347] = 6910;
nv_num[293] = 51038;
nv_num[389] = 62191;
nv_num[135] = 26972;
nv_num[382] = 13655;
nv_num[135] = 61423;
nv_num[137] = 30742;
nv_num[271] = 27244;
nv_num[2] = 41485;
nv_num[291] = 41855;
nv_num[346] = 60779;
nv_num[2] = 3028;
nv_num[350] = 34433;
nv_num[349] = 10762;
nv_num[14] = 58250;
nv_num[142] = 7556;
nv_num[123] = 1322;
nv_num[45] = 63085;
nv_num[195] = 2206;
nv_num[397] = 32905;
nv_num[219] = 6959;
nv_num[117] = 35693;
nv_num[9] = 9615;
nv_num[28] = 46068;
nv_num[193] = 61523;
nv_num[326] = 27969;
nv_num[207] = 29703;
nv_num[147] = 40518;
nv_num[158] = 4702;
nv_num[99] = 46762;
nv_num[281] = 3938;
nv_num[278] = 4335;
nv_num[342] = 34288;
nv_num[189] = 55447;
nv_num[276] = 5201;
nv_num[380] = 2210;
nv_num[197] = 64968;
nv_num[239] = 20275;
nv_num[181] = 28513;
nv_num[107] = 46220;
nv_num[11] = 47472;
nv_num[372] = 55160;
nv_num[246] = 39782;
nv_num[333] = 13073;
nv_num[58] = 57291;
nv_num[223] = 28828;
nv_num[347] = 36785;
nv_num[164] = 61244;
nv_num[7] = 21115;
nv_num[227] = 679;
nv_num[380] = 28777;
nv_num[135] = 32518;
nv_num[71] = 62013;
nv_num[194] = 37500;
nv_num[32] = 29559;
nv_num[392] = 52550;
nv_num[320] = 34111;
nv_num[148] = 22927;
nv_num[161] = 58007;
nv_num[282] = 41390;
nv_num[153] = 49759;
nv_num[129] = 6033;
nv_num[35] = 7436;
nv_num[114] = 54586;
nv_num[86] = 22712;
nv_num[138] = 48171;
nv_num[300] = 55085;
nv_num[232] = 21339;
nv_num[109] = 50431;
nv_num[45] = 41872;
nv_num[4] = 24039;
nv_num[369] = 59065;
nv_num[321] = 7639;
nv_num[306] = 8380;
nv_num[384] = 13272;
nv_num[115] = 14751;
nv_num[63] = 24445;
nv_num[30] = 19614;
nv_num[394] = 39287;
nv_num[229] = 32687;
nv_num[125] = 32270;
nv_num[0] = 24525;
nv_num[386] = 25886;
nv_num[219] = 47804;
nv_num[328] = 62951;
nv_num[60] = 6969;
nv_num[193] = 46543;
nv_num[362] = 47115;
nv_num[286] = 64423;
nv_num[59] = 23350;
nv_num[85] = 56056;
nv_num[273] = 34701;
nv_num[139] = 60481;
nv_num[344] = 11147;
nv_num[153] = 25386;
nv_num[108] = 21525;
nv_num[369] = 17837;
nv_num[315] = 12353;
nv_num[78] = 54207;
nv_num[8] = 13103;
nv_num[282] = 64567;
nv_num[205] = 4932;
nv_num[230] = 6848;
nv_num[385] = 15251;
nv_num[14] = 44365;
nv_num[242] = 20707;
nv_num[291] = 43934;
nv_num[43] = 45019;
nv_num[58] = 56106;
nv_num[354] = 8123;
nv_num[329] = 35943;
nv_num[392] = 36089;
nv_num[119] = 63395;
nv_num[373] = 63853;
nv_num[213] = 62105;
nv_num[370] = 14141;
nv_num[201] = 37661;
nv_num[260] = 9635;
nv_num[204] = 51340;
nv_num[19] = 64971;
nv_num[255] = 14230;
nv_num[287] = 13248;
nv_num[314] = 4103;
nv_num[130] = 18920;
nv_num[253] = 36965;
nv_num[34] = 812;
nv_num[36] = 25843;
nv_num[221] = 6536;
nv_num[102] = 25578;
nv_num[264] = 18345;
nv_num[383] = 34040;
nv_num[71] = 25080;
nv_num[201] = 45484;
nv_num[378] = 25811;
nv_num[324] = 12557;
nv_num[216] = 2854;
nv_num[16] = 12846;
nv_num[246] = 44966;
nv_num[256] = 52081;
nv_num[177] = 12142;
nv_num[84] = 30613;
nv_num[105] = 48090;
nv_num[2] = 41879;
nv_num[57] = 29770;
nv_num[289] = 63883;
nv_num[20] = 44426;
nv_num[293] = 9152;
nv_num[59] = 47656;
nv_num[58] = 57994;
nv_num[19] = 6194;
nv_num[386] = 56833;
nv_num[237] = 37367;
nv_num[150] = 9169;
nv_num[42] = 40130;
nv_num[193] = 49935;
nv_num[163] = 13078;
nv_num[13] = 13066;
nv_num[34] = 50454;
nv_num[166] = 10344;
nv_num[104] = 4739;
nv_num[174] = 47348;
nv_num[172] = 55803;
nv_num[210] = 36847;
nv_num[285] = 14357;
nv_num[30] = 51094;
nv_num[259] = 15690;
nv_num[20] = 60620;
nv_num[206] = 39543;
nv_num[73] = 43765;
nv_num[77] = 52898;
nv_num[329] = 47705;
nv_num[335] = 38097;
nv_num[269] = 16297;
nv_num[292] = 8328;
nv_num[361] = 3834;
nv_num[91] = 54487;
nv_num[149] = 8590;
nv_num[27] = 62649;
nv_num[142] = 60106;
nv_num[170] = 23400;
nv_num[275] = 13654;
nv_num[261] = 47062;
nv_num[289] = 25457;
nv_num[335] = 57636;
nv_num[19] = 41325;
nv_num[75] = 19683;
nv_num[234] = 20223;
nv_num[399] = 19571;
nv_num[144] = 47467;
nv_num[383] = 50056;
nv_num[346] = 5749;
nv_num[305] = 5005;
nv_num[381] = 37461;
nv_num[101] = 22440;
nv_num[365] = 9604;
nv_num[358] = 13605;
nv_num[223] = 27747;
nv_num[360] = 29933;
nv_num[180] = 12561;
nv_num[173] = 8350;
nv_num[104] = 35893;
nv_num[205] = 50693;
nv_num[355] = 5432;
nv_num[4] = 62203;
nv_num[340] = 45032;
nv_num[373] = 61217;
nv_num[299] = 21317;
nv_num[336] = 61613;
nv_num[156] = 48292;
nv_num[15] = 48203;
nv_num[213] = 20047;
nv_num[324] = 8788;
nv_num[328] = 18897;
nv_num[86] = 61516;
nv_num[12] = 28091;
nv_num[134] = 32094;
nv_num[166] = 51815;
nv_num[332] = 46985;
nv_num[150] = 4710;
nv_num[386] = 9826;
nv_num[40] = 20746;
nv_num[369] = 64961;
nv_num[384] = 20536;
nv_num[347] = 5979;
nv_num[226] = 15820;
nv_num[292] = 13354;
nv_num[369] = 22249;
nv_num[58] = 7728;
nv_num[36] = 13259;
nv_num[304] = 20077;
nv_num[44] = 34137;
nv_num[61] = 55669;
nv_num[358] = 6693;
nv_num[0] = 23276;
nv_num[204] = 29658;
nv_num[3] = 51829;
nv_num[186] = 45269;
nv_num[204] = 21763;
nv_num[134] = 2502;
nv_num[136] = 4849;
nv_num[190] = 34306;
nv_num[258] = 57213;
nv_num[316] = 50319;
nv_num[26] = 15089;
nv_num[185] = 40211;
nv_num[31] = 22358;
nv_num[291] = 51845;
nv_num[30] = 35366;
nv_num[0] = 9242;
nv_num[207] = 31917;
nv_num[194] = 58422;
nv_num[243] = 32339;
nv_num[305] = 23852;
nv_num[392] = 32146;
nv_num[240] = 38493;
nv_num[1] = 56856;
nv_num[196] = 24184;
nv_num[368] = 11261;
nv_num[106] = 23003;
nv_num[274] = 60032;
nv_num[335] = 43862;
nv_num[306] = 15477;
nv_num[377] = 11621;
nv_num[375] = 10152;
nv_num[292] = 36116;
nv_num[136] = 25591;
nv_num[350] = 8739;
nv_num[180] = 46177;
nv_num[77] = 11183;
nv_num[226] = 19068;
nv_num[394] = 52943;
nv_num[134] = 1655;
nv_num[171] = 35285;
nv_num[280] = 39831;
nv_num[61] = 11051;
nv_num[130] = 34078;
nv_num[73] = 41359;
nv_num[396] = 61851;
nv_num[249] = 46798;
nv_num[303] = 64545;
nv_num[334] = 3770;
nv_num[87] = 35281;
nv_num[27] = 10222;
nv_num[176] = 32205;
nv_num[382] = 42057;
nv_num[320] = 20580;
nv_num[324] = 24011;
nv_num[105] = 57059;
nv_num[92] = 12854;
nv_num[279] = 61857;
nv_num[87] = 36337;
nv_num[62] = 36707;
nv_num[88] = 25693;
nv_num[290] = 26333;
nv_num[164] = 19114;
nv_num[313] = 22610;
nv_num[375] = 6480;
nv_num[80] = 61421;
nv_num[331] = 63245;
nv_num[184] = 44757;
nv_num[368] = 5211;
nv_num[74] = 57187;
nv_num[205] = 10694;
nv_num[226] = 28536;
nv_num[127] = 37137;
nv_num[104] = 36773;
nv_num[182] = 28668;
nv_num[299] = 58993;
nv_num[213] = 3055;
nv_num[152] = 43056;
nv_num[289] = 11423;
nv_num[258] = 24416;
nv_num[195] = 41927;
nv_num[254] = 62694;
nv_num[113] = 10535;
nv_num[99] = 7996;
nv_num[86] = 9630;
nv_num[268] = 18362;
nv_num[280] = 60906;
nv_num[180] = 59897;
nv_num[353] = 52739;
nv_num[116] = 29384;
nv_num[345] = 4795;
nv_num[129] = 42857;
nv_num[272] = 60750;
nv_num[24] = 27941;
nv_num[120] = 29858;
nv_num[337] = 16362;
nv_num[147] = 57099;
nv_num[250] = 10370;
nv_num[376] = 44140;
nv_num[197] = 25486;
nv_num[394] = 39648;
nv_num[297] = 22163;
nv_num[179] = 41311;
nv_num[387] = 20208;
nv_num[335] = 46529;
nv_num[254] = 44363;
nv_num[373] = 53311;
nv_num[343] = 42629;
nv_num[326] = 31935;
nv_num[294] = 43527;
nv_num[174] = 41057;
nv_num[103] = 46654;
nv_num[293] = 31861;
nv_num[168] = 54614;
nv_num[102] = 40102;
nv_num[56] = 3201;
nv_num[77] = 61716;
nv_num[397] = 20385;
nv_num[218] = 2158;
nv_num[262] = 31392;
nv_num[224] = 27357;
nv_num[211] = 31813;
nv_num[122] = 28449;
nv_num[310] = 21090;
nv_num[98] = 3937;
nv_num[154] = 46859;
nv_num[20] = 34581;
nv_num[175] = 2369;
nv_num[101] = 63699;
nv_num[351] = 26099;
nv_num[191] = 63187;
nv_num[63] = 16550;
nv_num[388] = 14177;
nv_num[203] = 45864;
nv_num[354] = 31771;
nv_num[298] = 37284;
nv_num[344] = 14409;
nv_num[3] = 17445;
nv_num[368] = 24211;
nv_num[359] = 52382;
nv_num[123] = 37388;
nv_num[221] = 8365;
nv_num[0] = 6058;
nv_num[273] = 21836;
nv_num[278] = 30504;
nv_num[124] = 64728;
nv_num[264] = 64706;
nv_num[301] = 16629;
nv_num[129] = 44664;
nv_num[223] = 17433;
nv_num[202] = 1429;
nv_num[71] = 10741;
nv_num[49] = 46635;
nv_num[219] = 10915;
nv_num[332] = 46868;
nv_num[240] = 55071;
nv_num[198] = 13671;
nv_num[319] = 55564;
nv_num[275] = 2600;
nv_num[34] = 37748;
nv_num[149] = 21228;
nv_num[174] = 13255;
nv_num[1] = 36000;
nv_num[294] = 49428;
nv_num[136] = 32185;
nv_num[376] = 13487;
nv_num[49] = 19829;
nv_num[111] = 9220;
nv_num[347] = 45345;
nv_num[293] = 39268;
nv_num[389] = 58292;
nv_num[135] = 49835;
nv_num[382] = 25710;
nv_num[135] = 22656;
nv_num[137] = 55999;
nv_num[271] = 58389;
nv_num[2] = 24197;
nv_num[291] = 44259;
nv_num[346] = 64301;
nv_num[2] = 15109;
nv_num[350] = 15405;
nv_num[349] = 55499;
nv_num[14] = 45702;
nv_num[142] = 35819;
nv_num[123] = 41623;
nv_num[45] = 21168;
nv_num[195] = 16001;
nv_num[397] = 15100;
nv_num[219] = 26518;
nv_num[117] = 46486;
nv_num[9] = 42246;
nv_num[28] = 55380;
nv_num[193] = 15494;
nv_num[326] = 7265;
nv_num[207] = 38845;
nv_num[147] = 22302;
nv_num[158] = 17099;
nv_num[99] = 61110;
nv_num[281] = 5745;
nv_num[278] = 56353;
nv_num[342] = 38423;
nv_num[189] = 60530;
nv_num[276] = 20089;
nv_num[380] = 16815;
nv_num[197] = 11752;
nv_num[239] = 4860;
nv_num[181] = 8883;
nv_num[107] = 50874;
nv_num[11] = 7965;
nv_num[372] = 61231;
nv_num[246] = 36750;
nv_num[333] = 51325;
nv_num[58] = 30112;
nv_num[223] = 26418;
nv_num[347] = 51435;
nv_num[164] = 50374;
nv_num[7] = 63107;
nv_num[227] = 59160;
nv_num[380] = 13829;
nv_num[135] = 40306;
nv_num[71] = 7726;
nv_num[194] = 20186;
nv_num[32] = 13313;
nv_num[392] = 42660;
nv_num[320] = 12414;
nv_num[148] = 22679;
nv_num[161] = 22421;
nv_num[282] = 41223;
nv_num[153] = 47024;
nv_num[129] = 27053;
nv_num[35] = 61779;
nv_num[114] = 30336;
nv_num[86] = 366;
nv_num[138] = 1998;
nv_num[300] = 20985;
nv_num[232] = 64636;
nv_num[109] = 61741;
nv_num[45] = 55023;
nv_num[4] = 28523;
nv_num[369] = 53709;
nv_num[321] = 30123;
nv_num[306] = 46948;
nv_num[384] = 11292;
nv_num[115] = 1382;
nv_num[63] = 38719;
nv_num[30] = 60738;
nv_num[394] = 40857;
nv_num[229] = 11065;
nv_num[125] = 5070;
nv_num[0] = 41199;
nv_num[386] = 34434;
nv_num[219] = 16177;
nv_num[328] = 57824;
nv_num[60] = 5426;
nv_num[193] = 51544;
nv_num[362] = 48916;
nv_num[286] = 1331;
nv_num[59] = 56337;
nv_num[85] = 62765;
nv_num[273] = 5867;
nv_num[139] = 17584;
nv_num[344] = 42169;
nv_num[153] = 26433;
nv_num[108] = 33833;
nv_num[369] = 3494;
nv_num[315] = 5840;
nv_num[78] = 48311;
nv_num[8] = 51752;
nv_num[282] = 43558;
nv_num[205] = 51786;
nv_num[230] = 27595;
nv_num[385] = 38492;
nv_num[14] = 10320;
nv_num[242] = 29360;
nv_num[291] = 13394;
nv_num[43] = 49251;
nv_num[58] = 6008;
nv_num[354] = 41503;
nv_num[329] = 35687;
nv_num[392] = 10016;
nv_num[119] = 8068;
nv_num[373] = 18178;
nv_num[213] = 42239;
nv_num[370] = 17982;
nv_num[201] = 45828;
nv_num[260] = 9984;
nv_num[204] = 32516;
nv_num[19] = 43547;
nv_num[255] = 57350;
nv_num[287] = 22827;
nv_num[314] = 51065;
nv_num[130] = 18677;
nv_num[253] = 58631;
nv_num[34] = 41907;
nv_num[36] = 52681;
nv_num[221] = 41961;
nv_num[102] = 388;
nv_num[264] = 59724;
nv_num[383] = 80;
nv_num[71] = 18623;
nv_num[201] = 4029;
nv_num[378] = 25750;
nv_num[324] = 54588;
nv_num[216] = 26649;
nv_num[16] = 10394;
nv_num[246] = 20018;
nv_num[256] = 20578;
nv_num[177] = 3427;
nv_num[84] = 33007;
nv_num[105] = 49709;
nv_num[2] = 487;
nv_num[57] = 33066;
nv_num[289] = 38742;
nv_num[20] = 61069;
nv_num[293] = 61409;
nv_num[59] = 16329;
nv_num[58] = 51650;
nv_num[19] = 29716;
nv_num[386] = 39402;
nv_num[237] = 23079;
nv_num[150] = 53636;
nv_num[42] = 37848;
nv_num[193] = 27963;
nv_num[163] = 32814;
nv_num[13] = 7968;
nv_num[34] = 50964;
nv_num[166] = 35352;
nv_num[104] = 53601;
nv_num[174] = 4301;
nv_num[172] = 44553;
nv_num[210] = 19339;
nv_num[285] = 32857;
nv_num[30] = 5197;
nv_num[259] = 1409;
nv_num[20] = 20579;
nv_num[206] = 4510;
nv_num[73] = 52999;
nv_num[77] = 35758;
nv_num[329] = 59683;
nv_num[335] = 62646;
nv_num[269] = 21749;
nv_num[292] = 30210;
nv_num[361] = 4495;
nv_num[91] = 6148;
nv_num[149] = 9840;
nv_num[27] = 54868;
nv_num[142] = 7266;
nv_num[170] = 55980;
nv_num[275] = 13060;
nv_num[261] = 62011;
nv_num[289] = 10284;
nv_num[335] = 61957;
nv_num[19] = 2029;
nv_num[75] = 49519;
nv_num[234] = 9595;
nv_num[399] = 38120;
nv_num[144] = 51039;
nv_num[383] = 2681;
nv_num[346] = 47544;
nv_num[305] = 28340;
nv_num[381] = 27606;
nv_num[101] = 11372;
nv_num[365] = 50107;
nv_num[358] = 14982;
nv_num[223] = 11679;
nv_num[360] = 8124;
nv_num[180] = 59219;
nv_num[173] = 7168;
nv_num[104] = 53472;
nv_num[205] = 42303;
nv_num[355] = 6826;
nv_num[4] = 54319;
nv_num[340] = 931;
nv_num[373] = 9707;
nv_num[299] = 37771;
nv_num[336] = 13249;
nv_num[156] = 50597;
nv_num[15] = 36388;
nv_num[213] = 39861;
nv_num[324] = 23813;
nv_num[328] = 14471;
nv_num[86] = 492;
nv_num[12] = 3476;
nv_num[134] = 32400;
nv_num[166] = 23620;
nv_num[332] = 30594;
nv_num[150] = 13313;
nv_num[386] = 45042;
nv_num[40] = 55129;
nv_num[369] = 12442;
nv_num[384] = 14819;
nv_num[347] = 34499;
nv_num[226] = 28957;
nv_num[292] = 14611;
nv_num[369] = 56675;
nv_num[58] = 51485;
nv_num[36] = 44387;
nv_num[304] = 58400;
nv_num[44] = 36325;
nv_num[61] = 43365;
nv_num[358] = 1678;
nv_num[0] = 57635;
nv_num[204] = 21204;
nv_num[3] = 192;
nv_num[186] = 1080;
nv_num[204] = 59230;
nv_num[134] = 6599;
nv_num[136] = 43594;
nv_num[190] = 3932;
nv_num[258] = 42312;
nv_num[316] = 29545;
nv_num[26] = 58806;
nv_num[185] = 16389;
nv_num[31] = 36140;
nv_num[291] = 23221;
nv_num[30] = 45051;
nv_num[0] = 51169;
nv_num[207] = 17530;
nv_num[194] = 17789;
nv_num[243] = 58169;
nv_num[305] = 9713;
nv_num[392] = 33915;
nv_num[240] = 60030;
nv_num[1] = 29131;
nv_num[196] = 63542;
nv_num[368] = 34343;
nv_num[106] = 17666;
nv_num[274] = 17771;
nv_num[335] = 7692;
nv_num[306] = 14598;
nv_num[377] = 35093;
nv_num[375] = 343;
nv_num[292] = 17351;
nv_num[136] = 63515;
nv_num[350] = 38615;
nv_num[180] = 64443;
nv_num[77] = 31879;
nv_num[226] = 57153;
nv_num[394] = 52043;
nv_num[134] = 32115;
nv_num[171] = 5154;
nv_num[280] = 57418;
nv_num[61] = 24599;
nv_num[130] = 11235;
nv_num[73] = 64519;
nv_num[396] = 48580;
nv_num[249] = 13699;
nv_num[303] = 9220;
nv_num[334] = 40857;
nv_num[87] = 33629;
nv_num[27] = 64933;
nv_num[176] = 52786;
nv_num[382] = 22976;
nv_num[320] = 58450;
nv_num[324] = 35155;
nv_num[105] = 19521;
nv_num[92] = 55203;
nv_num[279] = 59517;
nv_num[87] = 14238;
nv_num[62] = 50035;
nv_num[88] = 50691;
nv_num[290] = 20178;
nv_num[164] = 30960;
nv_num[313] = 56732;
nv_num[375] = 29700;
nv_num[80] = 55944;
nv_num[331] = 22482;
nv_num[184] = 820;
nv_num[368] = 10438;
nv_num[74] = 37606;
nv_num[205] = 42097;
nv_num[226] = 12636;
nv_num[127] = 41918;
nv_num[104] = 43836;
nv_num[182] = 42748;
nv_num[299] = 8282;
nv_num[213] = 60816;
nv_num[152] = 29129;
nv_num[289] = 48266;
nv_num[258] = 4104;
nv_num[195] = 23749;
nv_num[254] = 7189;
nv_num[113] = 51993;
nv_num[99] = 51130;
nv_num[86] = 19135;
nv_num[268] = 13994;
nv_num[280] = 33577;
nv_num[180] = 1655;
nv_num[353] = 43388;
nv_num[116] = 42107;
nv_num[345] = 39263;
nv_num[129] = 18201;
nv_num[272] = 24715;
nv_num[24] = 60223;
nv_num[120] = 39791;
nv_num[337] = 48582;
nv_num[147] = 42126;
nv_num[250] = 39074;
nv_num[376] = 46224;
nv_num[197] = 47982;
nv_num[394] = 24576;
nv_num[297] = 14292;
nv_num[179] = 35767;
nv_num[387] = 24176;
nv_num[335] = 10665;
nv_num[254] = 55654;
nv_num[373] = 42748;
nv_num[343] = 26255;
nv_num[326] = 30853;
nv_num[294] = 44135;
nv_num[174] = 61321;
nv_num[103] = 41013;
nv_num[293] = 24139;
nv_num[168] = 2127;
nv_num[102] = 34269;
nv_num[56] = 15932;
nv_num[77] = 28915;
nv_num[397] = 191;
nv_num[218] = 28513;
nv_num[262] = 31825;
nv_num[224] = 47934;
nv_num[211] = 63600;
nv_num[122] = 42715;
nv_num[310] = 46765;
nv_num[98] = 59627;
nv_num[154] = 48932;
nv_num[20] = 62745;
nv_num[175] = 45444;
nv_num[101] = 1442;
nv_num[351] = 3461;
nv_num[191] = 39431;
nv_num[63] = 29783;
nv_num[388] = 32977;
nv_num[203] = 55386;
nv_num[354] = 47491;
nv_num[298] = 63182;
nv_num[344] = 7027;
nv_num[3] = 26475;
nv_num[368] = 63311;
nv_num[359] = 724;
nv_num[123] = 26384;
nv_num[221] = 12025;
nv_num[0] = 5766;
nv_num[273] = 32921;
nv_num[278] = 49698;
nv_num[124] = 22627;
nv_num[264] = 17168;
nv_num[301] = 20627;
nv_num[129] = 35504;
nv_num[223] = 5146;
nv_num[202] = 25112;
nv_num[71] = 55536;
nv_num[49] = 39559;
nv_num[219] = 22083;
nv_num[332] = 63391;
nv_num[240] = 43702;
nv_num[198] = 44714;
nv_num[319] = 12811;
nv_num[275] = 38971;
nv_num[34] = 30740;
nv_num[149] = 20555;
nv_num[174] = 16565;
nv_num[1] = 49416;
nv_num[294] = 53553;
nv_num[136] = 58822;
nv_num[376] = 58952;
nv_num[49] = 28578;
nv_num[111] = 40783;
nv_num[347] = 50218;
nv_num[293] = 48705;
nv_num[389] = 62020;
nv_num[135] = 48172;
nv_num[382] = 62560;
nv_num[135] = 55603;
nv_num[137] = 27282;
nv_num[271] = 35769;
nv_num[2] = 547;
nv_num[291] = 34067;
nv_num[346] = 19979;
nv_num[2] = 169;
nv_num[350] = 48754;
nv_num[349] = 36238;
nv_num[14] = 49314;
nv_num[142] = 54959;
nv_num[123] = 55265;
nv_num[45] = 7716;
nv_num[195] = 62307;
nv_num[397] = 9492;
nv_num[219] = 21340;
nv_num[117] = 39362;
nv_num[9] = 12089;
nv_num[28] = 215;
nv_num[193] = 1014;
nv_num[326] = 33368;
nv_num[207] = 51104;
nv_num[147] = 22588;
nv_num[158] = 36141;
nv_num[99] = 13803;
nv_num[281] = 22941;
nv_num[278] = 20464;
nv_num[342] = 34745;
nv_num[189] = 27938;
nv_num[276] = 40121;
nv_num[380] = 63901;
nv_num[197] = 50156;
nv_num[239] = 43763;
nv_num[181] = 63166;
nv_num[107] = 220;
nv_num[11] = 7967;
nv_num[372] = 48983;
nv_num[246] = 46657;
nv_num[333] = 60384;
nv_num[58] = 40437;
nv_num[223] = 57245;
nv_num[347] = 62855;
nv_num[164] = 46687;
nv_num[7] = 42714;
nv_num[227] = 32728;
nv_num[380] = 8762;
nv_num[135] = 4839;
nv_num[71] = 5539;
nv_num[194] = 21855;
nv_num[32] = 64986;
nv_num[392] = 51194;
nv_num[320] = 18168;
nv_num[148] = 58256;
nv_num[161] = 29672;
nv_num[282] = 25410;
nv_num[153] = 21357;
nv_num[129] = 43785;
nv_num[35] = 9983;
nv_num[114] = 38194;
nv_num[86] = 46328;
nv_num[138] = 13695;
nv_num[300] = 6380;
nv_num[232] = 18566;
nv_num[109] = 3653;
nv_num[45] = 9207;
nv_num[4] = 43616;
nv_num[369] = 26555;
nv_num[321] = 56662;
nv_num[306] = 39539;
nv_num[384] = 61478;
nv_num[115] = 36896;
nv_num[63] = 26468;
nv_num[30] = 41085;
nv_num[394] = 62681;
nv_num[229] = 10736;
nv_num[125] = 36210;
nv_num[0] = 33977;
nv_num[386] = 38224;
nv_num[219] = 6326;
nv_num[328] = 32265;
nv_num[60] = 37998;
nv_num[193] = 45338;
nv_num[362] = 27504;
nv_num[286] = 16678;
nv_num[59] = 23630;
nv_num[85] = 35074;
nv_num[273] = 47029;
nv_num[139] = 20923;
nv_num[344] = 36057;
nv_num[153] = 8044;
nv_num[108] = 49070;
nv_num[369] = 17522;
nv_num[315] = 1553;
nv_num[78] = 28141;
nv_num[8] = 33962;
nv_num[282] = 58423;
nv_num[205] = 14256;
nv_num[230] = 18777;
nv_num[385] = 6011;
nv_num[14] = 6474;
nv_num[242] = 28416;
nv_num[291] = 63091;
nv_num[43] = 29003;
nv_num[58] = 46055;
nv_num[354] = 54061;
nv_num[329] = 51942;
nv_num[392] = 28987;
nv_num[119] = 14785;
nv_num[373] = 5765;
nv_num[213] = 29305;
nv_num[370] = 34811;
nv_num[201] = 5493;
nv_num[260] = 6533;
nv_num[204] = 28944;
nv_num[19] = 5302;
nv_num[255] = 31062;
nv_num[287] = 46375;
nv_num[314] = 62643;
nv_num[130] = 62382;
nv_num[253] = 24824;
nv_num[34] = 2441;
nv_num[36] = 13419;
nv_num[221] = 9326;
nv_num[102] = 62722;
nv_num[264] = 10771;
nv_num[383] = 64750;
nv_num[71] = 9108;
nv_num[201] = 45591;
nv_num[378] = 7846;
nv_num[324] = 46317;
nv_num[216] = 58707;
nv_num[16] = 60830;
nv_num[246] = 4076;
nv_num[256] = 51974;
nv_num[177] = 16755;
nv_num[84] = 46778;
nv_num[105] = 38150;
nv_num[2] = 30724;
nv_num[57] = 24870;
nv_num[289] = 39559;
nv_num[20] = 4635;
nv_num[293] = 45625;
nv_num[59] = 44471;
nv_num[58] = 22743;
nv_num[19] = 29946;
nv_num[386] = 49755;
nv_num[237] = 30849;
nv_num[150] = 10308;
nv_num[42] = 34864;
nv_num[193] = 13394;
nv_num[163] = 20433;
nv_num[13] = 47849;
nv_num[34] = 40933;
nv_num[166] = 46395;
nv_num[104] = 18555;
nv_num[174] = 61554;
nv_num[172] = 18393;
nv_num[210] = 9150;
nv_num[285] = 37378;
nv_num[30] = 48897;
nv_num[259] = 28575;
nv_num[20] = 12200;
nv_num[206] = 9872;
nv_num[73] = 39615;
nv_num[77] = 47095;
nv_num[329] = 64684;
nv_num[335] = 37081;
nv_num[269] = 6550;
nv_num[292] = 6745;
nv_num[361] = 14119;
nv_num[91] = 64544;
nv_num[149] = 1512;
nv_num[27] = 45340;
nv_num[142] = 5636;
nv_num[170] = 64827;
nv_num[275] = 60715;
nv_num[261] = 24392;
nv_num[289] = 26688;
nv_num[335] = 24293;
nv_num[19] = 54912;
nv_num[75] = 57756;
nv_num[234] = 634;
nv_num[399] = 37523;
nv_num[144] = 38206;
nv_num[383] = 41374;
nv_num[346] = 53171;
nv_num[305] = 47293;
nv_num[381] = 53047;
nv_num[101] = 48910;
nv_num[365] = 21529;
nv_num[358] = 6166;
nv_num[223] = 44530;
nv_num[360] = 17256;
nv_num[180] = 29061;
nv_num[173] = 26460;
nv_num[104] = 36890;
nv_num[205] = 51491;
nv_num[355] = 1486;
nv_num[4] = 35722;
nv_num[340] = 15787;
nv_num[373] = 46238;
nv_num[299] = 34775;
nv_num[336] = 26888;
nv_num[156] = 54556;
nv_num[15] = 53842;
nv_num[213] = 52376;
nv_num[324] = 3579;
nv_num[328] = 64658;
nv_num[86] = 36259;
nv_num[12] = 17567;
nv_num[134] = 4276;
nv_num[166] = 28122;
nv_num[332] = 19117;
nv_num[150] = 13058;
nv_num[386] = 23862;
nv_num[40] = 63770;
nv_num[369] = 31256;
nv_num[384] = 57436;
nv_num[347] = 26703;
nv_num[226] = 29691;
nv_num[292] = 17533;
nv_num[369] = 17625;
nv_num[58] = 58140;
nv_num[36] = 20113;
nv_num[304] = 13590;
nv_num[44] = 891;
nv_num[61] = 44949;
nv_num[358] = 44613;
nv_num[0] = 42762;
nv_num[204] = 59365;
nv_num[3] = 31983;
nv_num[186] = 62128;
nv_num[204] = 26327;
nv_num[134] = 59408;
nv_num[136] = 54997;
nv_num[190] = 3490;
nv_num[258] = 62105;
nv_num[316] = 22824;
nv_num[26] = 2974;
nv_num[185] = 54004;
nv_num[31] = 16231;
nv_num[291] = 17901;
nv_num[30] = 56595;
nv_num[0] = 52645;
nv_num[207] = 60683;
nv_num[194] = 55706;
nv_num[243] = 18350;
nv_num[305] = 6676;
nv_num[392] = 21630;
nv_num[240] = 14514;
nv_num[1] = 1549;
nv_num[196] = 16283;
nv_num[368] = 42809;
nv_num[106] = 35969;
nv_num[274] = 41393;
nv_num[335] = 2137;
nv_num[306] = 4582;
nv_num[377] = 58520;
nv_num[375] = 29440;
nv_num[292] = 27874;
nv_num[136] = 59341;
nv_num[350] = 13982;
nv_num[180] = 29015;
nv_num[77] = 8363;
nv_num[226] = 21705;
nv_num[394] = 23697;
nv_num[134] = 25152;
nv_num[171] = 58176;
nv_num[280] = 55115;
nv_num[61] = 43566;
nv_num[130] = 61496;
nv_num[73] = 3717;
nv_num[396] = 39033;
nv_num[249] = 7041;
nv_num[303] = 9602;
nv_num[334] = 61982;
nv_num[87] = 18514;
nv_num[27] = 28021;
nv_num[176] = 19990;
nv_num[382] = 34477;
nv_num[320] = 23007;
nv_num[324] = 6833;
nv_num[105] = 26974;
nv_num[92] = 63133;
nv_num[279] = 3157;
nv_num[87] = 14328;
nv_num[62] = 5231;
nv_num[88] = 59798;
nv_num[290] = 30054;
nv_num[164] = 62580;
nv_num[313] = 40385;
nv_num[375] = 4609;
nv_num[80] = 6613;
nv_num[331] = 49685;
nv_num[184] = 27312;
nv_num[368] = 62194;
nv_num[74] = 63402;
nv_num[205] = 2591;
nv_num[226] = 55029;
nv_num[127] = 40836;
nv_num[104] = 3206;
nv_num[182] = 20250;
nv_num[299] = 10977;
nv_num[213] = 40691;
nv_num[152] = 35647;
nv_num[289] = 21495;
nv_num[258] = 13671;
nv_num[195] = 37759;
nv_num[254] = 16138;
nv_num[113] = 63292;
nv_num[99] = 20021;
nv_num[86] = 61480;
nv_num[268] = 35199;
nv_num[280] = 25784;
nv_num[180] = 58626;
nv_num[353] = 21446;
nv_num[116] = 64038;
nv_num[345] = 45552;
nv_num[129] = 45041;
nv_num[272] = 20692;
nv_num[24] = 27324;
nv_num[120] = 21289;
nv_num[337] = 55901;
nv_num[147] = 54386;
nv_num[250] = 14774;
nv_num[376] = 48203;
nv_num[197] = 36508;
nv_num[394] = 37672;
nv_num[297] = 36011;
nv_num[179] = 16809;
nv_num[387] = 53987;
nv_num[335] = 56421;
nv_num[254] = 2687;
nv_num[373] = 43381;
nv_num[343] = 41568;
nv_num[326] = 59952;
nv_num[294] = 32440;
nv_num[174] = 36566;
nv_num[103] = 52412;
nv_num[293] = 24305;
nv_num[168] = 17761;
nv_num[102] = 10349;
nv_num[56] = 58394;
nv_num[77] = 10377;
nv_num[397] = 6148;
nv_num[218] = 41713;
nv_num[262] = 64726;
nv_num[224] = 57318;
nv_num[211] = 41866;
nv_num[122] = 6893;
nv_num[310] = 2333;
nv_num[98] = 9758;
nv_num[154] = 13983;
nv_num[20] = 5917;
nv_num[175] = 14423;
nv_num[101] = 34986;
nv_num[351] = 1766;
nv_num[191] = 45712;
nv_num[63] = 59789;
nv_num[388] = 15284;
nv_num[203] = 9391;
nv_num[354] = 15779;
nv_num[298] = 18238;
nv_num[344] = 38268;
nv_num[3] = 41238;
nv_num[368] = 25234;
nv_num[359] = 64066;
nv_num[123] = 17582;
nv_num[221] = 37080;
nv_num[0] = 50621;
nv_num[273] = 3751;
nv_num[278] = 23177;
nv_num[124] = 17784;
nv_num[264] = 56185;
nv_num[301] = 14535;
nv_num[129] = 39130;
nv_num[223] = 20943;
nv_num[202] = 56954;
nv_num[71] = 24185;
nv_num[49] = 22581;
nv_num[219] = 3050;
nv_num[332] = 30081;
nv_num[240] = 35167;
nv_num[198] = 11302;
nv_num[319] = 41871;
nv_num[275] = 19089;
nv_num[34] = 49521;
nv_num[149] = 51659;
nv_num[174] = 49476;
nv_num[1] = 1445;
nv_num[294] = 41884;
nv_num[136] = 50053;
nv_num[376] = 54510;
nv_num[49] = 56977;
nv_num[111] = 43521;
nv_num[347] = 46653;
nv_num[293] = 61908;
nv_num[389] = 26699;
nv_num[135] = 56354;
nv_num[382] = 12230;
nv_num[135] = 18851;
nv_num[137] = 40188;
nv_num[271] = 19591;
nv_num[2] = 31431;
nv_num[291] = 57093;
nv_num[346] = 39546;
nv_num[2] = 46947;
nv_num[350] = 44798;
nv_num[349] = 28322;
nv_num[14] = 6;
nv_num[142] = 41047;
nv_num[123] = 56216;
nv_num[45] = 13935;
nv_num[195] = 43808;
nv_num[397] = 39824;
nv_num[219] = 13300;
nv_num[117] = 22192;
nv_num[9] = 61687;
nv_num[28] = 64114;
nv_num[193] = 38459;
nv_num[326] = 27277;
nv_num[207] = 28492;
nv_num[147] = 51904;
nv_num[158] = 36677;
nv_num[99] = 5935;
nv_num[281] = 6227;
nv_num[278] = 24402;
nv_num[342] = 13965;
nv_num[189] = 14989;
nv_num[276] = 17247;
nv_num[380] = 49018;
nv_num[197] = 18883;


    WATCHPOINT(1);
  }

    return 0;
}
