/* The start_new_season functions of the competition classes (slot +0x8 of each vtable), wrapped for the
 * season change (season_change.h). Made by tools/seasons/make_start_wraps.py: do not edit by hand. */
#include "win.h"
#include "season_change.h"

/* Each wrap: season_change_start(this, 0), the original, season_change_start(this, 1). The recompiler
 * reads the RT_WRAP lines as they are (tools/recomp/recomp.py), so they are written out in full. */

/* AfricanNations::start_new_season, 1 classes */
void o_00401d20(void);
RT_WRAP(00401d20, start_401d20)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00401d20();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ArgPrm::start_new_season, 1 classes */
void o_00405720(void);
RT_WRAP(00405720, start_405720)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00405720();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ArgSecond::start_new_season, 1 classes */
void o_00407f00(void);
RT_WRAP(00407f00, start_407f00)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00407f00();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* AsiaClubChamp::start_new_season, 1 classes */
void o_0040b700(void);
RT_WRAP(0040b700, start_40b700)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0040b700();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* AsiaCupWinner::start_new_season, 1 classes */
void o_0040d5b0(void);
RT_WRAP(0040d5b0, start_40d5b0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0040d5b0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* AsiaNations::start_new_season, 1 classes */
void o_0040f720(void);
RT_WRAP(0040f720, start_40f720)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0040f720();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* AsiaSuperCup::start_new_season, 1 classes */
void o_00410c50(void);
RT_WRAP(00410c50, start_410c50)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00410c50();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* AusNsl::start_new_season, 1 classes */
void o_00411de0(void);
RT_WRAP(00411de0, start_411de0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00411de0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BelFaCup::start_new_season, 1 classes */
void o_0041db90(void);
RT_WRAP(0041db90, start_41db90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0041db90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BelFirst::start_new_season, 1 classes */
void o_0041ea90(void);
RT_WRAP(0041ea90, start_41ea90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0041ea90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BelSecond::start_new_season, 1 classes */
void o_0041ff50(void);
RT_WRAP(0041ff50, start_41ff50)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0041ff50();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BelSuper::start_new_season, 1 classes */
void o_00421540(void);
RT_WRAP(00421540, start_421540)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00421540();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BelThird::start_new_season, 1 classes */
void o_004227b0(void);
RT_WRAP(004227b0, start_4227b0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_004227b0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraChampCup::start_new_season, 1 classes */
void o_004262c0(void);
RT_WRAP(004262c0, start_4262c0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_004262c0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraCup::start_new_season, 1 classes */
void o_00427d10(void);
RT_WRAP(00427d10, start_427d10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00427d10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraNatFirst::start_new_season, 1 classes */
void o_0042a640(void);
RT_WRAP(0042a640, start_42a640)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0042a640();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraNatSecond::start_new_season, 1 classes */
void o_0042cc10(void);
RT_WRAP(0042cc10, start_42cc10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0042cc10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraNatThird::start_new_season, 1 classes */
void o_0042def0(void);
RT_WRAP(0042def0, start_42def0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0042def0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegBahia::start_new_season, 1 classes */
void o_0042fe10(void);
RT_WRAP(0042fe10, start_42fe10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0042fe10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegCentral::start_new_season, 1 classes */
void o_00431090(void);
RT_WRAP(00431090, start_431090)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00431090();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegGaucho::start_new_season, 1 classes */
void o_00432370(void);
RT_WRAP(00432370, start_432370)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00432370();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegGoias::start_new_season, 1 classes */
void o_004336f0(void);
RT_WRAP(004336f0, start_4336f0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_004336f0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegMinasGerais::start_new_season, 1 classes */
void o_00434910(void);
RT_WRAP(00434910, start_434910)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00434910();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegNorth::start_new_season, 1 classes */
void o_00435e80(void);
RT_WRAP(00435e80, start_435e80)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00435e80();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegNortheast::start_new_season, 1 classes */
void o_00436e90(void);
RT_WRAP(00436e90, start_436e90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00436e90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegParana::start_new_season, 1 classes */
void o_00438060(void);
RT_WRAP(00438060, start_438060)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00438060();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegPern::start_new_season, 1 classes */
void o_004394d0(void);
RT_WRAP(004394d0, start_4394d0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_004394d0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegRio::start_new_season, 1 classes */
void o_0043a6a0(void);
RT_WRAP(0043a6a0, start_43a6a0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0043a6a0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegSanta::start_new_season, 1 classes */
void o_0043bd10(void);
RT_WRAP(0043bd10, start_43bd10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0043bd10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* BraRegSp::start_new_season, 1 classes */
void o_0043cd40(void);
RT_WRAP(0043cd40, start_43cd40)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0043cd40();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ConChamp::start_new_season, 1 classes */
void o_004c1440(void);
RT_WRAP(004c1440, start_4c1440)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_004c1440();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ConMercCup::start_new_season, 1 classes */
void o_004c1fb0(void);
RT_WRAP(004c1fb0, start_4c1fb0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_004c1fb0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ConmebolLiber::start_new_season, 1 classes */
void o_004c3980(void);
RT_WRAP(004c3980, start_4c3980)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_004c3980();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ConmebolMerc::start_new_season, 1 classes */
void o_004c5040(void);
RT_WRAP(004c5040, start_4c5040)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_004c5040();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* CroA1::start_new_season, 1 classes */
void o_005149a0(void);
RT_WRAP(005149a0, start_5149a0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005149a0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* CroA2a::start_new_season, 1 classes */
void o_00515ea0(void);
RT_WRAP(00515ea0, start_515ea0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00515ea0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* CroA2b::start_new_season, 1 classes */
void o_00516fc0(void);
RT_WRAP(00516fc0, start_516fc0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00516fc0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* CroCup::start_new_season, 1 classes */
void o_00517d60(void);
RT_WRAP(00517d60, start_517d60)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00517d60();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* Cup::start_new_season, 3 classes */
void o_00518780(void);
RT_WRAP(00518780, start_518780)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00518780();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* DenCup::start_new_season, 1 classes */
void o_00551500(void);
RT_WRAP(00551500, start_551500)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00551500();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* DenFirst::start_new_season, 1 classes */
void o_00552450(void);
RT_WRAP(00552450, start_552450)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00552450();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* DenPrm::start_new_season, 1 classes */
void o_00553aa0(void);
RT_WRAP(00553aa0, start_553aa0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00553aa0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* DenSecond::start_new_season, 1 classes */
void o_00554c00(void);
RT_WRAP(00554c00, start_554c00)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00554c00();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngAutoCup::start_new_season, 1 classes */
void o_0056b4b0(void);
RT_WRAP(0056b4b0, start_56b4b0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0056b4b0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngCcCup::start_new_season, 1 classes */
void o_0056d130(void);
RT_WRAP(0056d130, start_56d130)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0056d130();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngCharity::start_new_season, 1 classes */
void o_0056d9b0(void);
RT_WRAP(0056d9b0, start_56d9b0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0056d9b0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngConf::start_new_season, 1 classes */
void o_0056ee80(void);
RT_WRAP(0056ee80, start_56ee80)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0056ee80();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngFaCup::start_new_season, 1 classes */
void o_00570390(void);
RT_WRAP(00570390, start_570390)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00570390();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngFaTrophy::start_new_season, 1 classes */
void o_005714a0(void);
RT_WRAP(005714a0, start_5714a0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005714a0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngFirst::start_new_season, 1 classes */
void o_00572e30(void);
RT_WRAP(00572e30, start_572e30)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00572e30();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngPrm::start_new_season, 1 classes */
void o_00574c10(void);
RT_WRAP(00574c10, start_574c10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00574c10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngSecond::start_new_season, 1 classes */
void o_00576820(void);
RT_WRAP(00576820, start_576820)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00576820();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EngThird::start_new_season, 1 classes */
void o_00578170(void);
RT_WRAP(00578170, start_578170)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00578170();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EurSuperCup::start_new_season, 1 classes */
void o_0057a6d0(void);
RT_WRAP(0057a6d0, start_57a6d0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0057a6d0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EuroChamp::start_new_season, 1 classes */
void o_0057b750(void);
RT_WRAP(0057b750, start_57b750)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0057b750();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EuroChampQual::start_new_season, 1 classes */
void o_0057eb20(void);
RT_WRAP(0057eb20, start_57eb20)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0057eb20();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* EuropeanCup::start_new_season, 1 classes */
void o_00584d60(void);
RT_WRAP(00584d60, start_584d60)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00584d60();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FifaConfed::start_new_season, 1 classes */
void o_0058bff0(void);
RT_WRAP(0058bff0, start_58bff0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0058bff0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FinCup::start_new_season, 1 classes */
void o_00592d00(void);
RT_WRAP(00592d00, start_592d00)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00592d00();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FinFirst::start_new_season, 1 classes */
void o_00593c20(void);
RT_WRAP(00593c20, start_593c20)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00593c20();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FinPrm::start_new_season, 1 classes */
void o_00595d40(void);
RT_WRAP(00595d40, start_595d40)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00595d40();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FraCup::start_new_season, 1 classes */
void o_005bae50(void);
RT_WRAP(005bae50, start_5bae50)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005bae50();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FraFirst::start_new_season, 1 classes */
void o_005bc4d0(void);
RT_WRAP(005bc4d0, start_5bc4d0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005bc4d0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FraLgeCup::start_new_season, 1 classes */
void o_005bd420(void);
RT_WRAP(005bd420, start_5bd420)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005bd420();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FraSecond::start_new_season, 1 classes */
void o_005bf160(void);
RT_WRAP(005bf160, start_5bf160)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005bf160();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FraSuper::start_new_season, 1 classes */
void o_005bf9a0(void);
RT_WRAP(005bf9a0, start_5bf9a0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005bf9a0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* FraThird::start_new_season, 1 classes */
void o_005c07c0(void);
RT_WRAP(005c07c0, start_5c07c0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005c07c0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GerCup::start_new_season, 1 classes */
void o_005d9780(void);
RT_WRAP(005d9780, start_5d9780)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005d9780();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GerFirst::start_new_season, 1 classes */
void o_005db4a0(void);
RT_WRAP(005db4a0, start_5db4a0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005db4a0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GerLgeCup::start_new_season, 1 classes */
void o_005dc9e0(void);
RT_WRAP(005dc9e0, start_5dc9e0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005dc9e0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GerRegional::start_new_season, 1 classes */
void o_005dda90(void);
RT_WRAP(005dda90, start_5dda90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005dda90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GerSecond::start_new_season, 1 classes */
void o_005df490(void);
RT_WRAP(005df490, start_5df490)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005df490();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GoldCup::start_new_season, 1 classes */
void o_005e1530(void);
RT_WRAP(005e1530, start_5e1530)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005e1530();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GreCup::start_new_season, 1 classes */
void o_005e93f0(void);
RT_WRAP(005e93f0, start_5e93f0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005e93f0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GrePrm::start_new_season, 1 classes */
void o_005eaf50(void);
RT_WRAP(005eaf50, start_5eaf50)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005eaf50();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GreSecond::start_new_season, 1 classes */
void o_005ebe90(void);
RT_WRAP(005ebe90, start_5ebe90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005ebe90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* GreSuper::start_new_season, 1 classes */
void o_005ec6b0(void);
RT_WRAP(005ec6b0, start_5ec6b0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005ec6b0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* HolCup::start_new_season, 1 classes */
void o_005f3f10(void);
RT_WRAP(005f3f10, start_5f3f10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005f3f10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* HolFirst::start_new_season, 1 classes */
void o_005f63f0(void);
RT_WRAP(005f63f0, start_5f63f0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005f63f0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* HolPrm::start_new_season, 1 classes */
void o_005f8610(void);
RT_WRAP(005f8610, start_5f8610)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005f8610();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* HolSuper::start_new_season, 1 classes */
void o_005f8e40(void);
RT_WRAP(005f8e40, start_5f8e40)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_005f8e40();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* InterAmerCup::start_new_season, 1 classes */
void o_00632910(void);
RT_WRAP(00632910, start_632910)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00632910();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IntertotoCup::start_new_season, 1 classes */
void o_00633920(void);
RT_WRAP(00633920, start_633920)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00633920();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IreChalCup::start_new_season, 1 classes */
void o_00634a60(void);
RT_WRAP(00634a60, start_634a60)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00634a60();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IreFirst::start_new_season, 1 classes */
void o_00635b30(void);
RT_WRAP(00635b30, start_635b30)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00635b30();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IreLeinsterCup::start_new_season, 1 classes */
void o_00636c20(void);
RT_WRAP(00636c20, start_636c20)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00636c20();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IreLgeCup::start_new_season, 1 classes */
void o_00637760(void);
RT_WRAP(00637760, start_637760)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00637760();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IreMunsterCup::start_new_season, 1 classes */
void o_006390d0(void);
RT_WRAP(006390d0, start_6390d0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_006390d0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IrePresCup::start_new_season, 1 classes */
void o_00639820(void);
RT_WRAP(00639820, start_639820)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00639820();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IrePrm::start_new_season, 1 classes */
void o_0063a9b0(void);
RT_WRAP(0063a9b0, start_63a9b0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0063a9b0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* IreSuperCup::start_new_season, 1 classes */
void o_0063b550(void);
RT_WRAP(0063b550, start_63b550)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0063b550();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaC1Super::start_new_season, 1 classes */
void o_0063caa0(void);
RT_WRAP(0063caa0, start_63caa0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0063caa0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaCCup::start_new_season, 1 classes */
void o_0063d780(void);
RT_WRAP(0063d780, start_63d780)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0063d780();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaCup::start_new_season, 1 classes */
void o_0063f3e0(void);
RT_WRAP(0063f3e0, start_63f3e0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0063f3e0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaSerA::start_new_season, 1 classes */
void o_00641700(void);
RT_WRAP(00641700, start_641700)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00641700();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaSerB::start_new_season, 1 classes */
void o_00646960(void);
RT_WRAP(00646960, start_646960)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00646960();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaSerC1a::start_new_season, 1 classes */
void o_0064b820(void);
RT_WRAP(0064b820, start_64b820)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0064b820();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaSerC1b::start_new_season, 1 classes */
void o_0064f9e0(void);
RT_WRAP(0064f9e0, start_64f9e0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0064f9e0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaSerC2a::start_new_season, 1 classes */
void o_00653c40(void);
RT_WRAP(00653c40, start_653c40)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00653c40();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaSerC2b::start_new_season, 1 classes */
void o_00658100(void);
RT_WRAP(00658100, start_658100)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00658100();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaSerC2c::start_new_season, 1 classes */
void o_0065c290(void);
RT_WRAP(0065c290, start_65c290)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0065c290();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ItaSuper::start_new_season, 1 classes */
void o_0065fc90(void);
RT_WRAP(0065fc90, start_65fc90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0065fc90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* JapEmpCup::start_new_season, 1 classes */
void o_00662950(void);
RT_WRAP(00662950, start_662950)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00662950();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* JapJ1::start_new_season, 1 classes */
void o_006636d0(void);
RT_WRAP(006636d0, start_6636d0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_006636d0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* JapJ2::start_new_season, 1 classes */
void o_006651f0(void);
RT_WRAP(006651f0, start_6651f0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_006651f0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* JapJCup::start_new_season, 1 classes */
void o_00665bc0(void);
RT_WRAP(00665bc0, start_665bc0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00665bc0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* JapSuper::start_new_season, 1 classes */
void o_006664e0(void);
RT_WRAP(006664e0, start_6664e0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_006664e0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* KorFaCup::start_new_season, 1 classes */
void o_0066acc0(void);
RT_WRAP(0066acc0, start_66acc0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0066acc0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* KorLeague::start_new_season, 1 classes */
void o_0066bd60(void);
RT_WRAP(0066bd60, start_66bd60)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0066bd60();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* KorLeagueCup::start_new_season, 1 classes */
void o_0066c900(void);
RT_WRAP(0066c900, start_66c900)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0066c900();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* KorSuperCup::start_new_season, 1 classes */
void o_0066de90(void);
RT_WRAP(0066de90, start_66de90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0066de90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* League::start_new_season, 6 classes */
void o_00682760(void);
RT_WRAP(00682760, start_682760)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00682760();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* NirCharity::start_new_season, 1 classes */
void o_0078a410(void);
RT_WRAP(0078a410, start_78a410)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0078a410();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* NirCup::start_new_season, 1 classes */
void o_0078ae30(void);
RT_WRAP(0078ae30, start_78ae30)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0078ae30();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* NirFirst::start_new_season, 1 classes */
void o_0078be10(void);
RT_WRAP(0078be10, start_78be10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0078be10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* NirLgeCup::start_new_season, 1 classes */
void o_0078d160(void);
RT_WRAP(0078d160, start_78d160)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0078d160();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* NirPrm::start_new_season, 1 classes */
void o_0078ee30(void);
RT_WRAP(0078ee30, start_78ee30)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0078ee30();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* NorCup::start_new_season, 1 classes */
void o_0078fb90(void);
RT_WRAP(0078fb90, start_78fb90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0078fb90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* NorFirst::start_new_season, 1 classes */
void o_00790b90(void);
RT_WRAP(00790b90, start_790b90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00790b90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* NorPrm::start_new_season, 1 classes */
void o_007927a0(void);
RT_WRAP(007927a0, start_7927a0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007927a0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* OceaniaClubChamp::start_new_season, 1 classes */
void o_00797d60(void);
RT_WRAP(00797d60, start_797d60)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00797d60();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* OceaniaNations::start_new_season, 1 classes */
void o_007998e0(void);
RT_WRAP(007998e0, start_7998e0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007998e0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* Olympics::start_new_season, 1 classes */
void o_007a0450(void);
RT_WRAP(007a0450, start_7a0450)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007a0450();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PolCup::start_new_season, 1 classes */
void o_007c88a0(void);
RT_WRAP(007c88a0, start_7c88a0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007c88a0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PolFirst::start_new_season, 1 classes */
void o_007c9740(void);
RT_WRAP(007c9740, start_7c9740)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007c9740();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PolLgeCup::start_new_season, 1 classes */
void o_007cafe0(void);
RT_WRAP(007cafe0, start_7cafe0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007cafe0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PolSecond::start_new_season, 1 classes */
void o_007cc000(void);
RT_WRAP(007cc000, start_7cc000)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007cc000();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PolSuper::start_new_season, 1 classes */
void o_007ccf90(void);
RT_WRAP(007ccf90, start_7ccf90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007ccf90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PorCup::start_new_season, 1 classes */
void o_007ce540(void);
RT_WRAP(007ce540, start_7ce540)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007ce540();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PorPrm::start_new_season, 1 classes */
void o_007cf9e0(void);
RT_WRAP(007cf9e0, start_7cf9e0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007cf9e0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PorSecond::start_new_season, 1 classes */
void o_007d1760(void);
RT_WRAP(007d1760, start_7d1760)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007d1760();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PorSecondB::start_new_season, 1 classes */
void o_007d26b0(void);
RT_WRAP(007d26b0, start_7d26b0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007d26b0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* PorSuper::start_new_season, 1 classes */
void o_007d3490(void);
RT_WRAP(007d3490, start_7d3490)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007d3490();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* RusCup::start_new_season, 1 classes */
void o_007e9d10(void);
RT_WRAP(007e9d10, start_7e9d10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007e9d10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* RusFirst::start_new_season, 1 classes */
void o_007ea8c0(void);
RT_WRAP(007ea8c0, start_7ea8c0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007ea8c0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* RusPrm::start_new_season, 1 classes */
void o_007eb800(void);
RT_WRAP(007eb800, start_7eb800)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007eb800();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ScoChalCup::start_new_season, 1 classes */
void o_007ed130(void);
RT_WRAP(007ed130, start_7ed130)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007ed130();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ScoFaCup::start_new_season, 1 classes */
void o_007ede10(void);
RT_WRAP(007ede10, start_7ede10)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007ede10();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ScoFirst::start_new_season, 1 classes */
void o_007eef00(void);
RT_WRAP(007eef00, start_7eef00)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007eef00();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ScoLgeCup::start_new_season, 1 classes */
void o_007f0930(void);
RT_WRAP(007f0930, start_7f0930)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007f0930();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ScoPrm::start_new_season, 1 classes */
void o_007f2a70(void);
RT_WRAP(007f2a70, start_7f2a70)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007f2a70();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ScoSecond::start_new_season, 1 classes */
void o_007f4030(void);
RT_WRAP(007f4030, start_7f4030)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007f4030();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* ScoThird::start_new_season, 1 classes */
void o_007f4fb0(void);
RT_WRAP(007f4fb0, start_7f4fb0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_007f4fb0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SpaCup::start_new_season, 1 classes */
void o_0084df70(void);
RT_WRAP(0084df70, start_84df70)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0084df70();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SpaFirst::start_new_season, 1 classes */
void o_0084feb0(void);
RT_WRAP(0084feb0, start_84feb0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0084feb0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SpaLower::start_new_season, 1 classes */
void o_00853830(void);
RT_WRAP(00853830, start_853830)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00853830();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SpaSecond::start_new_season, 1 classes */
void o_00855d80(void);
RT_WRAP(00855d80, start_855d80)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00855d80();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SpaSecondB::start_new_season, 1 classes */
void o_00856f90(void);
RT_WRAP(00856f90, start_856f90)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00856f90();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SpaSuper::start_new_season, 1 classes */
void o_00858cd0(void);
RT_WRAP(00858cd0, start_858cd0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00858cd0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SweCup::start_new_season, 1 classes */
void o_0088d690(void);
RT_WRAP(0088d690, start_88d690)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0088d690();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SweFirst::start_new_season, 1 classes */
void o_0088f150(void);
RT_WRAP(0088f150, start_88f150)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0088f150();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SwePrm::start_new_season, 1 classes */
void o_008905f0(void);
RT_WRAP(008905f0, start_8905f0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_008905f0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* SweSecond::start_new_season, 1 classes */
void o_00892410(void);
RT_WRAP(00892410, start_892410)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00892410();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* TurCup::start_new_season, 1 classes */
void o_008fcdb0(void);
RT_WRAP(008fcdb0, start_8fcdb0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_008fcdb0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* TurFirst::start_new_season, 1 classes */
void o_008fe040(void);
RT_WRAP(008fe040, start_8fe040)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_008fe040();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* TurSecond::start_new_season, 1 classes */
void o_008ff330(void);
RT_WRAP(008ff330, start_8ff330)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_008ff330();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* TurSecondB::start_new_season, 1 classes */
void o_00900500(void);
RT_WRAP(00900500, start_900500)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00900500();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* UefaCup::start_new_season, 1 classes */
void o_00902ee0(void);
RT_WRAP(00902ee0, start_902ee0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00902ee0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* UsaMls::start_new_season, 1 classes */
void o_00908e00(void);
RT_WRAP(00908e00, start_908e00)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00908e00();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* UsaMlsAllStars::start_new_season, 1 classes */
void o_0090a760(void);
RT_WRAP(0090a760, start_90a760)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0090a760();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* UsaOpenCup::start_new_season, 1 classes */
void o_0090b790(void);
RT_WRAP(0090b790, start_90b790)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0090b790();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WcAfricanCup::start_new_season, 1 classes */
void o_00910410(void);
RT_WRAP(00910410, start_910410)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00910410();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WcAsiaLeague::start_new_season, 1 classes */
void o_00913360(void);
RT_WRAP(00913360, start_913360)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00913360();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WcConcacafCup::start_new_season, 1 classes */
void o_00915ff0(void);
RT_WRAP(00915ff0, start_915ff0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00915ff0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WcEuropeLeague::start_new_season, 1 classes */
void o_00919430(void);
RT_WRAP(00919430, start_919430)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00919430();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WcOceaniaLeague::start_new_season, 1 classes */
void o_0091ef40(void);
RT_WRAP(0091ef40, start_91ef40)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0091ef40();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WcSouthAmericanLeague::start_new_season, 1 classes */
void o_00921470(void);
RT_WRAP(00921470, start_921470)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00921470();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WelCup::start_new_season, 1 classes */
void o_00924c50(void);
RT_WRAP(00924c50, start_924c50)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00924c50();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WelFirst::start_new_season, 1 classes */
void o_00925cf0(void);
RT_WRAP(00925cf0, start_925cf0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00925cf0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WelLgeCup::start_new_season, 1 classes */
void o_009268a0(void);
RT_WRAP(009268a0, start_9268a0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_009268a0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WelPrmCup::start_new_season, 1 classes */
void o_009281f0(void);
RT_WRAP(009281f0, start_9281f0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_009281f0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WorldClubChamp::start_new_season, 1 classes */
void o_00929d40(void);
RT_WRAP(00929d40, start_929d40)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_00929d40();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WorldClubCup::start_new_season, 1 classes */
void o_0092bda0(void);
RT_WRAP(0092bda0, start_92bda0)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0092bda0();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

/* WorldCup::start_new_season, 1 classes */
void o_0092d330(void);
RT_WRAP(0092d330, start_92d330)
{
    uint32_t self = cpu.ecx;
    season_change_start(self, 0);
    if (rt_unwinding) return;
    o_0092d330();
    if (rt_unwinding) return;
    season_change_start(self, 1);
}

