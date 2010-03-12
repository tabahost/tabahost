/***
 *  Copyright (C) 2004-2009 Francisco Bischoff
 *  Copyright (C) 2006 MaxMind LLC
 *  Copyright (C) 2000-2003 MySQL AB
 *
 *  This file is part of Tabajara Host Server.
 *
 *  Tabajara Host Server is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tabajara Host Server is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Tabajara Host Server.  If not, see <http://www.gnu.org/licenses/agpl.html>.
 *
 ***/

#include "shared.h"

client_t *clients; // extern
arena_t *arena; // extern
arenaslist_t arenaslist[MAX_ARENASLIST]; // extern
MYSQL my_sock; // extern
MYSQL_RES *my_result= NULL; // extern
MYSQL_ROW my_row= NULL; // extern
u_int32_t my_id = 0; // extern
char my_query[MAX_QUERY]; // extern
u_int32_t factorybuildings[4]; // extern
u_int32_t factorybuildingsup[4]; // extern
u_int32_t maxmuntype = 0; // extern
u_int32_t maxplanes = 0; // extern
u_int8_t mainbuffer[MAX_RECVDATA]; //extern

/*

 {0x0209, 0x0409, 0x08FF}, // pcSEND_SQUAD_INFO *******************************
 {0x030B, 0x030B, 0x06FF}, // arnaREFRESH_FIELD_OBJECTS *******************************
 {0x0909, 0x1D09, 0x3AFF}, // utilSET_AMMO_LOAD *******************************
 {0xFFFF, 0x0E0B, 0x1CFF}, // piNWATTACH_SLOT *******************************
 {0x1800, 0x1B00, 0xFFFF}, // csGET_CHECK_SUM *******************************
 {0x1F07, 0x1607, 0x2CFF}, // ftDISPLAY_FILE *******************************
 {0x1F0B, 0x160B, 0x2CFF}, // ftSEND_FILE_INFO_RESPONSE *******************************

 {0x1B02, 0x1902, 0x32FF}, // wpHIT_SCORED_ON_BADGUY "la10(bbw)" ======= FLAK ===========
 {0x0208, 0x0408, 0x08FF}, // pcREQUEST_SQUAD_INFO ===============================
 {0x020C, 0x040C, 0x08FF}, // pcREQUEST_SPECIFIC_SQUAD_INFO ===============================
 {0x020D, 0x040D, 0x08FF}, // pcREQUEST_PILOT_MEDALS ===============================
 {0x0210, 0x0410, 0x08FF}, // pcREQUEST_GUN_STATUS ===============================
 {0xFFFF, 0x0310, 0x06FF}, // arnaCONFIG_FLIGHTMODEL ===============================
 {0x0907, 0x1D07, 0x3AFF}, // utilATTACH_JUMP ===============================
 {0x090E, 0x1D0E, 0x3AFF}, // utilPLANECOUNTS ===============================
 {0x0C02, 0x0D02, 0x1AFF}, // cnPING_DATA ===============================
 {0x0E04, 0x0E04, 0x1CFF}, // piGUNNER_UPDATE ===============================
 {0x1801, 0x1B01, 0xFFFF}, // csGET_CHECK_SUM_RESPONSE ===============================
 {0x1F0A, 0x160A, 0x2CFF}, // ftSEND_FILE_INFO_REQUEST ===============================

 */

u_int16_t packets_tab[210][3] =
{
		/*// THL
		{ 0xFB00, 0xFB00, 0xFB00 }, // thlGET_CRC
		{ 0xFB01, 0xFB01, 0xFB01 }, // thlGET_CRC_RESPONSE
		{ 0xFB02, 0xFB02, 0xFB02 }, // thl
		{ 0xFB03, 0xFB03, 0xFB03 }, // thl
		*/

		// WB2   WB2007  WB2008
		{ 0x0200, 0x0400, 0xFFFF }, // pcNEW_USER
		{ 0x0201, 0x0401, 0x0809 }, // pcSTART_FLIGHT *
		{ 0x0202, 0x0402, 0x080A }, // pcEXIT_FLIGHT =
		{ 0x0203, 0x0403, 0x080B }, // pcMOVE_TO *
		{ 0x0204, 0x0404, 0x080C }, // pcCONFIG_USER *
		{ 0x0205, 0x0405, 0xFFFF }, // pcSETSQDNINFO
		{ 0x0206, 0x0406, 0x080E }, // pcREQUEST_ONLINE_PLAYERS =
		{ 0x0207, 0x0407, 0x080F }, // pcSEND_ONLINE_PLAYERS *
		{ 0x0208, 0x0408, 0xFFFF }, // pcREQUEST_SQUAD_INFO ===============================
		{ 0x0209, 0x0409, 0xFFFF }, // pcSEND_SQUAD_INFO *******************************
		{ 0x020A, 0x040A, 0xFFFF }, // pcSEND_SQUAD_INVITE
		{ 0x020B, 0x040B, 0xFFFF }, // pcSEND_SQUAD_INVITATION_ACCEPTED
		{ 0x020C, 0x040C, 0xFFFF }, // pcREQUEST_SPECIFIC_SQUAD_INFO ===============================
		{ 0x020D, 0x040D, 0xFFFF }, // pcREQUEST_PILOT_MEDALS ===============================
		{ 0x020E, 0x040E, 0xFFFF }, // pcSEND_PILOT_MEDALS *******************************
		{ 0x020F, 0x040F, 0xFFFF }, // pcSEND_NEW_MEDALS_AWARDED
		{ 0x0210, 0x0410, 0xFFFF }, // pcREQUEST_GUN_STATUS ===============================
		{ 0x0211, 0x0411, 0x0819 }, // pcSEND_GUN_STATUS *
		{ 0x0212, 0x0412, 0x081A }, // pcREQUEST_SPEC_GUN_STATUS =
		{ 0x0213, 0x0413, 0x081B }, // pcSEND_SPEC_GUN_STATUS *
		{ 0x0214, 0x0414, 0x081C }, // pcSEND_GUNNER_STATUS_CHANGE *
		{ 0x0215, 0x0415, 0x081D }, // pcCURRENT_PLANE_SETTINGS *
		{ 0x0216, 0x0416, 0xFFFF }, // pcSEND_SET_RANKS_MEDALS_DISPLAY
		{ 0x0217, 0x0417, 0xFFFF }, // pcSEND_SQUAD_MEMBER_REMOVED
		{ 0x0218, 0x0418, 0xFFFF }, // pcSEND_REQUESTED_TRAINER_HELP
		{ 0x0219, 0x0419, 0x0811 }, // pcSEND_HL_SQUAD
		{ 0x021A, 0x041A, 0xFFFF }, // pcSEND_SYNC_VIEW ===============================
		{ 0x021B, 0x041B, 0xFFFF }, // pcSYNC_VIEW *******************************
		{ 0xFFFF, 0x041C, 0x0814 }, // pcWB3_REQUEST_START_FLIGHT =
		{ 0xFFFF, 0x041D, 0x0815 }, // pcWB3_HEARTBEAT
		{ 0xFFFF, 0x041E, 0xFFFF }, // pcSTART_FLIGHTAIWING
		{ 0xFFFF, 0x041F, 0x0817 }, // pcSTART_FLIGHTSPAWN *
		{ 0xFFFF, 0x0420, 0x0828 }, // pcWB3_REQUEST_MANNEDACK =
		{ 0xFFFF, 0x0421, 0xFFFF }, // pcWB3_NEW_USER
		{ 0xFFFF, 0x0422, 0xFFFF }, // pcEXIT_FLIGHT2
		{ 0xFFFF, 0x0423, 0xFFFF }, // pcWB3_AIJUMP

		{ 0x0300, 0x0300, 0x0606 }, // arnaSET_CONFIG *
		{ 0x0301, 0x0301, 0x0607 }, // arnaSET_TIME *
		{ 0x0302, 0x0302, 0xFFFF }, // arnaSET_TASKGROUP_POSITION *******************************
		{ 0x0303, 0x0303, 0xFFFF }, // arnaSET_TASKGROUP_DESTINATION *******************************
		{ 0x0304, 0x0304, 0xFFFF }, // arnaSINK_BOAT *******************************
		{ 0x0305, 0x0305, 0xFFFF }, // arnaRAISE_BOAT *******************************
		{ 0x0306, 0x0306, 0x0600 }, // arnaSET_FIELD_OWNER *
		{ 0x0307, 0x0307, 0x0601 }, // arnaOBJECT_DAMAGE *
		{ 0x0308, 0x0308, 0xFFFF }, // arnaSET_TERRAIN
		{ 0x0309, 0x0309, 0x060F }, // arnaOBJECT_DAMAGE_RESPONSE =
		{ 0x030A, 0x030A, 0x060C }, // arnaMAP_VECTOR
		{ 0x030B, 0x030B, 0xFFFF }, // arnaREFRESH_FIELD_OBJECTS *******************************
		{ 0xFFFF, 0x030C, 0xFFFF }, // arnaSET_DATE
		{ 0xFFFF, 0x030D, 0x060B }, // arnaGRUNT_CONFIG * 
		{ 0xFFFF, 0x030E, 0x0608 }, // arnaRANDOM_SEED
		{ 0xFFFF, 0x030F, 0xFFFF }, // arnaARENAFLAGS3 *
		{ 0xFFFF, 0x0310, 0xFFFF }, // arnaCONFIG_FLIGHTMODEL *******************************
		{ 0xFFFF, 0x0311, 0xFFFF }, // arnaCONFIG_WEAPONS
		{ 0xFFFF, 0x0312, 0x0614 }, // arnaSET_CONFIG2 *

		// These wb2007 ID's are not tested, and must be checked enabling field info in tower
		{ 0x0800, 0x1400, 0xFFFF }, // cmPLAYER_STARTED_FLIGHT
		{ 0x0801, 0x1401, 0xFFFF }, // cmPLAYER_EXITED_FLIGHT
		{ 0x0802, 0x1402, 0xFFFF }, // cmPLAYER_TALK
		{ 0x0803, 0x1403, 0xFFFF }, // cmPLAYER_DROP
		{ 0xFFFF, 0x1404, 0xFFFF }, // cmPLAYER_HIT
		{ 0xFFFF, 0x1405, 0xFFFF }, // cmPLAYER_KILL
		{ 0xFFFF, 0x1406, 0x1412 }, // cmHOSTVAR
		{ 0xFFFF, 0x1407, 0x1413 }, // cmHOSTVARSTR

		{ 0x0900, 0x1D00, 0xFFFF }, // utilGET_CHECK_SUM
		{ 0x0901, 0x1D01, 0xFFFF }, // utilSETHANDLE
		{ 0x0902, 0x1D02, 0xFFFF }, // utilINFO
		{ 0x0903, 0x1D03, 0xFFFF }, // utilSETVARIABLE
		{ 0x0904, 0x1D04, 0x3A3E }, // utilWATCH_DOG *
		{ 0x0905, 0x1D05, 0x3A3F }, // utilATTACH_REQUEST =
		{ 0x0906, 0x1D06, 0x3A3C }, // utilATTACH_ACCEPT =
		{ 0x0907, 0x1D07, 0xFFFF }, // utilATTACH_JUMP ===============================
		{ 0x0908, 0x1D08, 0xFFFF }, // utilATTACH_NOTIFY
		{ 0x0909, 0x1D09, 0xFFFF }, // utilSET_AMMO_LOAD *******************************
		{ 0x090A, 0x1D0A, 0xFFFF }, // utilENABLED_PLANES_RESPONSE
		{ 0x090B, 0x1D0B, 0xFFFF }, // utilBINARY_PACKET
		{ 0x090C, 0x1D0C, 0x3A36 }, // utilSHANGHAI =
		{ 0x090D, 0x1D0D, 0xFFFF }, // utilPLANECOUNT
		{ 0x090E, 0x1D0E, 0xFFFF }, // utilPLANECOUNTS ===============================
		{ 0x090F, 0x1D0F, 0x3A35 }, // utilENABLED_PLANES_RESPONSE2 *
		{ 0xFFFF, 0x1D10, 0xFFFF }, // utilPLANECOUNTS2
		{ 0xFFFF, 0x1D11, 0xFFFF }, // utilACKSPAWN
		{ 0xFFFF, 0x1D12, 0x3A28 }, // utilACKSPAWNS *
		{ 0xFFFF, 0x1D13, 0x3A29 }, // utilHD_SERIAL *

		{ 0x0C00, 0x0D00, 0x1A1A }, // cnVERSION_DATA *
		{ 0x0C01, 0x0D01, 0x1A1B }, // cnCLIENT_IS_READY =
		{ 0x0C02, 0x0D02, 0xFFFF }, // cnPING_DATA ===============================
		{ 0x0C03, 0x0D03, 0xFFFF }, // cnTIMEOUT_LIMITS
		{ 0xFFFF, 0x0D04, 0x1A1E }, // cnEXIT =
		{ 0xFFFF, 0x0D05, 0xFFFF }, // cnTIMECHECK

		{ 0x0E00, 0x0E00, 0x1C1C }, // piUSER_UPDATE =
		{ 0x0E01, 0x0E01, 0x1C1D }, // piSET_PLAYER_PLANE_DAMAGE =
		{ 0x0E02, 0x0E02, 0x1C1E }, // piPLANE_DAMAGE *
		{ 0x0E03, 0x0E03, 0xFFFF }, // piHIT_RECEIVED
		{ 0x0E04, 0x0E04, 0xFFFF }, // piGUNNER_UPDATE ===============================
		{ 0x0E05, 0x0E05, 0x1C19 }, // piCHUTE_UPDATE =
		{ 0x0E06, 0x0E06, 0xFFFF }, // piMAN_UPDATE
		{ 0xFFFF, 0x0E07, 0x1C1B }, // piGUNNER_ATT_UPDATE =
		{ 0xFFFF, 0x0E08, 0xFFFF }, // piUSERX_UPDATE
		{ 0xFFFF, 0x0E09, 0xFFFF }, // piCMEYESLAVE_UPDATE
		{ 0xFFFF, 0x0E0A, 0xFFFF }, // piUSERY_UPDATE
		{ 0xFFFF, 0x0E0B, 0xFFFF }, // piNWATTACH_SLOT *******************************
		{ 0xFFFF, 0x0E0C, 0x1C10 }, // piMASTER_VIEW "bbbbbbbbb"
		{ 0xFFFF, 0x0E0D, 0xFFFF }, // piFORCE_SPAWN
		{ 0xFFFF, 0x0E0E, 0x1C12 }, // piFIRE_SUPPRESSION = (wb3server reply other clients with 0022)
		{ 0xFFFF, 0x0E0F, 0x1C13 }, // piEXTERNAL_AMMOCNT =
		{ 0xFFFF, 0x0E10, 0xFFFF }, // piUSERREC_UPDATE
		{ 0xFFFF, 0x0E11, 0xFFFF }, // piCONTROL_UPDATE
		{ 0xFFFF, 0x0E12, 0xFFFF }, // piENGINE_UPDATE

		{ 0x0F00, 0x2100, 0x4242 }, // olgSET_PARAMETERS * (otto parameters)
		{ 0x0F01, 0x2101, 0x4243 }, // olgWEAPON_FIRED *= (otto fire in wb2)
		{ 0x0F02, 0x2102, 0x4240 }, // olgACCURACY_MOD
		{ 0xFFFF, 0x2103, 0x4241 }, // olgSET_PARAMETERS2

		{ 0x1200, 0x1200, 0x2424 }, // rmRADIO_TALK *=
		{ 0x1201, 0x1201, 0x2425 }, // rmRADIO_TUNE =
		{ 0x1202, 0x1202, 0xFFFF }, // rmRADIO_TALK_WITH_ACK
		{ 0x1203, 0x1203, 0xFFFF }, // rmRADIO_TALK_RESPONSE
		{ 0xFFFF, 0x1204, 0xFFFF }, // rmRADIO_TALK_ABORT_UL
		{ 0xFFFF, 0x1205, 0x2421 }, // rmRADIO_DOTCMD *

		{ 0x1800, 0x1B00, 0xFFFF }, // csGET_CHECK_SUM *******************************
		{ 0x1801, 0x1B01, 0xFFFF }, // csGET_CHECK_SUM_RESPONSE ===============================

		{ 0x1A00, 0x2000, 0x4040 }, // idSET_PLAYER_ID =
		{ 0x1A01, 0x2001, 0x4041 }, // idSET_PLAYER_ID_RESPONSE *
		{ 0x1A02, 0x2002, 0xFFFF }, // idSET_PERSONA

		{ 0x1B00, 0x1900, 0x3232 }, // wpWEAPON_FIRED =* (including otto fire in wb3)
		{ 0x1B01, 0x1901, 0xFFFF }, // wpGUIDED_WEAPON_FIRED
		{ 0x1B02, 0x1902, 0xFFFF }, // wpHIT_SCORED_ON_BADGUY ===============================
		{ 0x1B03, 0x1903, 0x3231 }, // wpHIT_SCORED_ON_OBJECT =
		{ 0x1B04, 0x1904, 0x3236 }, // wpWEAPON_HIT_BADGUY =
		{ 0x1B05, 0x1905, 0x3237 }, // wpWEAPON_HIT_OBJECT =
		{ 0x1B06, 0x1906, 0x3234 }, // wpWEAPON_DESTROYED =*
		{ 0x1B07, 0x1907, 0x3235 }, // wpHIT_RECEIVED *
		{ 0x1B08, 0x1908, 0xFFFF }, // wpSHOOTING_BULLETS
		{ 0x1B09, 0x1909, 0x323B }, // wpSHOOTING_HARDPOINTS =*
		{ 0x1B0A, 0x190A, 0xFFFF }, // wpDESTROY_PLAYER_WEPS
		{ 0x1B0B, 0x190B, 0x3239 }, // wpHIT_SCORED_ON_BADGUY2 =
		{ 0xFFFF, 0x190C, 0x323E }, // wpGRUNT_DESTROYED =*
		{ 0xFFFF, 0x190D, 0xFFFF }, // wpWEAPON_LAUNCH_SOUND
		{ 0xFFFF, 0x190E, 0xFFFF }, // wpOTTOHIT_SCORED_ON_BADGUY
		{ 0xFFFF, 0x190F, 0x323D }, // wpOTTOHIT_SCORED_ON_BADGUY2 =
		{ 0xFFFF, 0x1910, 0xFFFF }, // wpOTTOHIT_SCORED_ON_OBJECT
		{ 0xFFFF, 0x1911, 0xFFFF }, // wpOTTOWEAPON_HIT_BADGUY
		{ 0xFFFF, 0x1912, 0xFFFF }, // wpOTTOWEAPON_HIT_OBJECT
		{ 0xFFFF, 0x1913, 0x3221 }, // wpTONNAGE_ON_TARGET =
		{ 0xFFFF, 0x1914, 0xFFFF }, // wpSTRIKE_EFFECT
		{ 0xFFFF, 0x1915, 0x3227 }, // wpBULLETS_FIRED =
		{ 0xFFFF, 0x1916, 0x3224 }, // wpFUEL_CONSUMED =
		{ 0xFFFF, 0x1917, 0x3225 }, // wpMAXALT_WEAPON_FIRED =*
		{ 0xFFFF, 0x1918, 0x322A }, // wpDELAYEDFUSE_WEAPON_FIRED
		{ 0xFFFF, 0x1919, 0xFFFF }, // wpAICONTROLLED_WEAPON_FIRED

		{ 0x1F00, 0x1600, 0x2C2C }, // ftDNLOAD_REQUEST =
		{ 0x1F01, 0x1601, 0x2C2D }, // ftDNLOAD_RESPONCE *
		{ 0x1F02, 0x1602, 0x2C2E }, // ftSTART_DNLOAD =
		{ 0x1F03, 0x1603, 0x2C2F }, // ftUPLOAD_REQUEST *
		{ 0x1F04, 0x1604, 0xFFFF }, // ftUPLOAD_RESPONCE
		{ 0x1F05, 0x1605, 0x2C29 }, // ftFILE_DATA *
		{ 0x1F06, 0x1606, 0x2C2A }, // ftDATA_ACK =
		{ 0x1F07, 0x1607, 0xFFFF }, // ftDISPLAY_FILE *******************************
		{ 0x1F08, 0x1608, 0xFFFF }, // ftVERSION_DOWNLOAD_REQUEST
		{ 0x1F09, 0x1609, 0xFFFF }, // ftVERSION_DOWNLOAD_RESPONSE
		{ 0x1F0A, 0x160A, 0xFFFF }, // ftSEND_FILE_INFO_REQUEST ===============================
		{ 0x1F0B, 0x160B, 0xFFFF }, // ftSEND_FILE_INFO_RESPONSE *******************************
		{ 0xFFFF, 0x160C, 0xFFFF }, // ftDOWNLOAD_FILE
		{ 0xFFFF, 0x160D, 0xFFFF }, // ftPIPE

		{ 0x2100, 0x0000, 0x0000 }, // bgFILL_SLOT *
		{ 0x2101, 0x0001, 0xFFFF }, // bgUPDATE * (wb2, same as 0001 in wb3)
		{ 0x2102, 0x0002, 0x0002 }, // bgPLANE_DAMAGE *
		{ 0x2103, 0x0003, 0xFFFF }, // bgRESEND_SLOT_INFO
		{ 0x2104, 0x0004, 0xFFFF }, // bgALL_PLANES_UPDATE
		{ 0x2105, 0x0005, 0x0005 }, // bgRADAR_PLANES_UPDATE
		{ 0x2106, 0x0006, 0x0006 }, // bgCLEAR_RADAR
		{ 0xFFFF, 0x0007, 0x0007 }, // bgGUNNER_ATT *
		{ 0xFFFF, 0x0008, 0x0008 }, // bgAI_FILL_SLOT
		{ 0xFFFF, 0x0009, 0xFFFF }, // bgAI_UPDATE
		{ 0xFFFF, 0x000A, 0xFFFF }, // bgAI_PLANE_DAMAGE
		{ 0xFFFF, 0x000B, 0xFFFF }, // bgAI_RESEND_SLOT_INFO
		{ 0xFFFF, 0x000C, 0xFFFF }, // bgAI_GUNNER_ATT
		{ 0xFFFF, 0x000D, 0xFFFF }, // bgAIHOST_PLANE_DAMAGE
		{ 0xFFFF, 0x000E, 0xFFFF }, // bgAIHOST_UPDATE
		{ 0xFFFF, 0x000F, 0xFFFF }, // bgAI_HIT_SCORED_ON_BADGUY
		{ 0xFFFF, 0x0010, 0xFFFF }, // bgAI_HIT_SCORED_ON_PLAYER
		{ 0xFFFF, 0x0011, 0xFFFF }, // bgAI_WEAPON_HIT_BADGUY
		{ 0xFFFF, 0x0012, 0xFFFF }, // bgAI_WEAPON_HIT_PLAYER
		{ 0xFFFF, 0x0013, 0xFFFF }, // bgAI_HIT_SCORED_ON_OBJECT
		{ 0xFFFF, 0x0014, 0xFFFF }, // bgAI_WEAPON_HIT_OBJECT
		{ 0xFFFF, 0x0015, 0x0015 }, // bgALLAI_PLANES_UPDATE
		{ 0xFFFF, 0x0016, 0xFFFF }, // bgRADARAI_PLANES_UPDATE
		{ 0xFFFF, 0x0017, 0xFFFF }, // bgCLEARAI_RADAR
		{ 0xFFFF, 0x0018, 0xFFFF }, // bgAI_MOUNT
		{ 0xFFFF, 0x0019, 0xFFFF }, // bgAI_ATTACH_UPDATE
		{ 0xFFFF, 0x001A, 0xFFFF }, // bgAI_GUNNER_ENABLE
		{ 0xFFFF, 0x001B, 0xFFFF }, // bgAI_MOUNT_UPDATE
		{ 0xFFFF, 0x001C, 0xFFFF }, // bgCMEYESLAVE_FILL_SLOT
		{ 0xFFFF, 0x001D, 0xFFFF }, // bgAI_CLIENT_UPDATE
		{ 0xFFFF, 0x001E, 0x001E }, // bgUPDATE2 * (wb3, same as 0001 in wb2)
		{ 0xFFFF, 0x001F, 0xFFFF }, // bgAI_UPDATE2
		{ 0xFFFF, 0x0020, 0xFFFF }, // bgAI_UPDATE3
		{ 0xFFFF, 0x0021, 0xFFFF }, // bgAI_WANTSAIHITFLAG
		{ 0xFFFF, 0x0022, 0x0022 }, // bgSUPPRESS_FIRE *
		{ 0xFFFF, 0x0023, 0xFFFF }, // bgAI_SUPPRESS_FIRE
		{ 0xFFFF, 0x0024, 0x0024 }, // bgEXTERNAL_AMMOCNT *
		{ 0xFFFF, 0x0025, 0xFFFF }, // bgAI_EXTERNAL_AMMOCNT
		{ 0xFFFF, 0x0026, 0xFFFF }, // bgEXTRA_ICON_TEXT
		{ 0xFFFF, 0x0027, 0xFFFF }, // bgACMCAM_FILL_SLOT
		{ 0xFFFF, 0x0028, 0xFFFF }, // bgACMCAM_UPDATE
		{ 0xFFFF, 0x0029, 0xFFFF }, // bgACMCAM_PLANE_DAMAGE
		{ 0xFFFF, 0x002A, 0xFFFF }, // bgACMCAM_EXTERNAL_AMMOCNT
		{ 0xFFFF, 0x002B, 0xFFFF }, // bgACMCAM_SUPPRESS_FIRE
		{ 0xFFFF, 0x002C, 0x002C }, // bgUSER_SHAPE
		{ 0xFFFF, 0x002D, 0x002D }, // bgOVERRIDE_SHAPE
		{ 0xFFFF, 0x002E, 0x002E } // bgAI_OVERRIDE_SHAPE
}; // extern
jmp_buf debug_buffer; // debug buffer


/**
 CheckArenaRules

 Make arena lives! :)
 */

void CheckArenaRules(void)
{
	static u_int32_t players_num = 0;
	static u_int16_t players_count = 0;
	u_int8_t close, vitals, dominated;
	int16_t i, j;
	u_int8_t reds, golds, k;
	double tonnage_recover;
	u_int8_t c_cities, totalcities;
	u_int32_t dist, tempdist;
	int32_t posx;
	char file[128];
	//	double angle;

	// Emulated Bombs tick

	if (!setjmp(debug_buffer))
	{
		for (i = 0; i < MAX_BOMBS; i++)
		{
			if (arena->bombs[i].id)
			{
				if (arena->bombs[i].timer > 0)
				{
					arena->bombs[i].timer--;
				}
				else
				{
					HitStructsNear(arena->bombs[i].destx, arena->bombs[i].desty, arena->bombs[i].type, arena->bombs[i].speed, 0, arena->bombs[i].from);
					arena->bombs[i].id = 0;
				}
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, NULL);
	}

	// buildings tick

	if (!setjmp(debug_buffer))
	{
		for (i = 0; i < fields->value; i++)
		{
			if(rebuildtime->value < 9999)
			{
				if(!oldcapt->value && wb3->value && arena->fields[i].tonnage && !(arena->frame % 6000)) // every minute
				{
					c_cities = totalcities = 0;

					for(j = 0; j < fields->value; j++)
					{
						if((arena->fields[j].type >= FIELD_WB3POST) && (arena->fields[j].type <= FIELD_WB3PORT))
						{
							if(arena->fields[j].country == arena->fields[i].country)
							{
								c_cities++;
							}

							totalcities++;
						}
					}

					tonnage_recover = (1.0 + (((double)c_cities / totalcities) - 0.5) / 2.0) * GetTonnageToClose(i+1) / (24.0 * rebuildtime->value / 9.0);

					if(!tonnage_recover)
					{
						Com_Printf(VERBOSE_WARNING, "CheckArenaRules(tonnage_recover = 0) (1.0 + (((double)%u / %u) - 0.5) / 2.0) * %u / (24.0 * %.3f / 9.0)\n",
							c_cities,
							totalcities,
							GetTonnageToClose(i+1),
							rebuildtime->value);
					}

					arena->fields[i].tonnage -= tonnage_recover;

					if(arena->fields[i].tonnage < 0)
						arena->fields[i].tonnage = 0;

					for(k = 0, j = 0; j < MAX_HITBY; j++) // check the number of bombers
					{
						if(arena->fields[i].hitby[j].dbid)
						{
							k++;
						}
					}

					if(k)
					{
						for(j = 0; j < MAX_HITBY; j++) // remove tonnage from bombers
						{
							if(arena->fields[i].hitby[j].dbid)
							{
								arena->fields[i].hitby[j].damage -= (tonnage_recover / k);

								if(arena->fields[i].hitby[j].damage < 0.0) // remove bomber
								{
									Com_Printf(VERBOSE_DEBUG_DAMAGE, "Field damage removed, remove bomber %d from list\n", arena->fields[i].hitby[j].longnick);
									memset(&(arena->fields[i].hitby[j]), 0, sizeof(hitby_t));
								}
							}
						}
					}
				}

				for (j = 0; j < MAX_BUILDINGS; j++)
				{
					if (arena->fields[i].buildings[j].field)
					{
						if (arena->fields[i].buildings[j].status)
						{
							if(!ctf->value)
							{
								if(arena->fields[i].buildings[j].timer > 360000) // 1 hour
								{
									Com_Printf(VERBOSE_DEBUG_DAMAGE, "bugged building timer %d, set to 15min\n", arena->fields[i].buildings[j].timer);
									arena->fields[i].buildings[j].timer = 90000;
								}
							}

							if (arena->fields[i].buildings[j].type < BUILD_CV || arena->fields[i].buildings[j].type > BUILD_SUBMARINE)
								arena->fields[i].buildings[j].timer--;

							if (arena->fields[i].buildings[j].timer <= 0)
							{
								arena->fields[i].buildings[j].timer = 0;
								arena->fields[i].buildings[j].status = 0;
								arena->fields[i].buildings[j].armor = GetBuildingArmor(arena->fields[i].buildings[j].type, NULL);

								if ((arena->fields[i].buildings[j].type >= BUILD_CV) && (arena->fields[i].buildings[j].type <= BUILD_SUBMARINE))
									SinkBoat(TRUE, &arena->fields[i].buildings[j], NULL);

								SetBuildingStatus(&arena->fields[i].buildings[j], arena->fields[i].buildings[j].status, NULL);

								if (arena->fields[i].buildings[j].type == BUILD_TOWER) // to return flag color
								{
									//							close = arena->fields[i].abletocapture;
									Cmd_Capt(i, arena->fields[i].country, NULL);
									//							arena->fields[i].abletocapture = close;
								}
								else if (arena->fields[i].buildings[j].type == BUILD_WARE) // to cancel warehouse effect
								{
									arena->fields[i].warehouse = 0;
								}
							}
							//					else if(arena->fields[i].buildings[j].status == 1 && arena->fields[i].buildings[j].timer < (u_int32_t)(rebuildtime->value/2))
							//					{
							//						arena->fields[i].buildings[j].status = 2;
							//						SetBuildingStatus(&arena->fields[i].buildings[j], arena->fields[i].buildings[j].status);
							//					}
						}
						else
							arena->fields[i].buildings[j].timer = 0;
					}
					else
						break;
				}
			}
		}

		if (!(arena->frame % 100))
		{
			for (i = 0; i < cities->value; i++)
			{
				for (j = 0; j < MAX_BUILDINGS; j++)
				{
					if (arena->cities[i].buildings[j].field)
					{
						if (arena->cities[i].buildings[j].status)
						{
							dist = GetFactoryReupTime(arena->cities[i].buildings[j].country);

							if (arena->frame < arena->cities[i].buildings[j].timer)
								posx = MAX_UINT32 - arena->cities[i].buildings[j].timer + arena->frame;
							else
								posx = arena->frame - arena->cities[i].buildings[j].timer;

							if ((u_int32_t)posx > dist)
							{
								factorybuildingsup[arena->cities[i].buildings[j].country - 1]++;
								arena->cities[i].buildings[j].status = 0;
								arena->cities[i].buildings[j].armor = GetBuildingArmor(arena->cities[i].buildings[j].type,
								NULL);

								SetBuildingStatus(&arena->cities[i].buildings[j], arena->cities[i].buildings[j].status, NULL);
							}
						}
					}
					else
						break;
				}
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, NULL);
	}

	// weather

	if (metar->value)
	{
		if (!(arena->frame % 30000)) // 5 minutes
		{
			if (!setjmp(debug_buffer))
			{
				ProcessMetarWeather();
			}
			else
			{
				DebugClient(__FILE__, __LINE__, TRUE, NULL);
			}
		}
	}
	else if (!(arena->frame % 60000)) // 10 minutes
	{
		if (!setjmp(debug_buffer))
		{
			switch ((int)weather->value)
			{
				case 0: // clouded
					if (rand()%3) // 66.6% keep clouded
						break;

					if (rand()%2) // change weather
						Var_Set("weather", "3"); // partialy clouded
					else
						Var_Set("weather", "2"); // rain
					break;
				case 1: // clear
					if (!rand()%3) // 66.6% keep clear
						Var_Set("weather", "3"); // partialy clouded
					break;
				case 2: // rain
					if (rand()%3) // 33.3% keep raining
						Var_Set("weather", "0"); // clouded
					break;
				case 3: // partialy clouded
					if (rand()%3) // 66.6% keep partialy clouded
						break;

					if (rand()%2) // change weather
						Var_Set("weather", "1"); // clear
					else
						Var_Set("weather", "0"); // clouded
					break;
				default:
					Var_Set("weather", "3"); // partialy clouded
					break;
			}
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}


	// time

	if (!(arena->frame % (int)(6000/arena->multiplier)))
	{
		if (!setjmp(debug_buffer))
		{
			if (arena->minute == 59) // check minutes
			{
				arena->minute = 0;
				if (arena->hour == 23)
					arena->hour = 0;
				else
					arena->hour++;

			}
			else
				arena->minute++;

			if (arena->hour - (7 - ((int)dayhours->value%10)/2) == dayhours->value) // check hours
			{
				arena->hour = (7 - ((int)dayhours->value%10)/2);
				arena->day++;
				Cmd_Time((arena->hour*100)+arena->minute, NULL, NULL);
			}

			if (arena->minute == 0)
			{
				if ((arena->hour - (7 - ((int)dayhours->value%10)/2)) == (dayhours->value - 1)) // change timemult 1h before next day
				{
					CalcTimemultBasedOnTime();
				}
				else if (((7 - ((int)dayhours->value%10)/2) + 1) == arena->hour) // change timemult 1h after a new day
				{
					CalcTimemultBasedOnTime();
				}
			}

			if (arena->minute == 30 && (arena->hour == 5 || arena->hour == 18)) // time to change iconrange
			{
				Com_Printf(VERBOSE_ALWAYS, "Time: %2d:%02d - Changing iconrange\n", arena->hour, arena->minute);
				for (i = 0; i < maxentities->value; i++)
				{
					if (clients[i].inuse && clients[i].ready && !clients[i].drone)
					{
						SendArenaRules(&clients[i]);
					}
				}
			}

			if (arena->day > 28) // check days
			{
				if (arena->month == 2 && (arena->year % 4))
				{
					arena->day = 1;
					arena->month++;
				}

				if (arena->day >= 30)
				{
					if (arena->month == 2)
					{
						arena->day = 1;
						arena->month++;
					}
					else if ((arena->month == 4 || arena->month == 6 || arena->month == 9 || arena->month == 11) && arena->day == 31)
					{
						arena->day = 1;
						arena->month++;
					}
					else if (arena->day == 32)
					{
						arena->day = 1;
						arena->month++;
					}
				}
			}

			if (arena->month > 12)
			{
				arena->month = 1;
				arena->year++;
			}

			if (arena->hour - (7 - ((int)dayhours->value%10)/2) == dayhours->value) // check hours
				BPrintf(RADIO_YELLOW, "Current Date: %04d/%02d/%02d", arena->year, arena->month, arena->day);

			if (wb3->value && !(arena->frame % (int)((360000 * dayhours->value) /timemult->value))) // set date and dayhours every day
			{
				if (arena->month >= 1 && arena->month <= 3) // winter
				{
					Var_Set("dayhours", "10");
				}
				else if (arena->month >= 7 && arena->month <= 9) // summer
				{
					Var_Set("dayhours", "14");
				}
				else // autumn / spring
				{
					Var_Set("dayhours", "12");
				}

				WB3DotCommand(NULL, ".date %u %u %u", arena->month, arena->day, arena->year);
			}

			if (!mapcycle->value)
			{
				if ((arena->year >= endyear->value) && (arena->month >= endmonth->value) && (arena->day >= endday->value))
				{
					arena->year = inityear->value;
					arena->month = initmonth->value;
					arena->day = initday->value;

					if(rps->value)
						UpdateRPS(0);
				}
			}
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}

	if (wb3->value && !(arena->frame % 3000)) // 30 seconds
	{
		if (!setjmp(debug_buffer))
		{
			Cmd_Time((arena->hour*100)+arena->minute, NULL, NULL);
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}


	// Scenario Tick

	if (arena->scenario)
	{
		if (!setjmp(debug_buffer))
		{
			if (!((arena->frame - arena->scenario) % 6000)) // every minute
			{
				// try to execute tX.cfg files
				snprintf(file, sizeof(file), "./arenas/%s/t%u", dirname->string, (arena->frame - arena->scenario) / 6000);
				Cmd_LoadConfig(file, NULL);
			}
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}

	// CTF tick

	if (ctf->value && !(arena->frame % 100) /*1 sec*/)
	{
		if (!setjmp(debug_buffer))
		{
			for (i = 0; i < fields->value; i++)
			{
				dominated = 0;

				for(j = 0; j < maxentities->value; j++)
				{
					if(clients[j].inuse && clients[j].ready && clients[j].infly && !clients[j].drone && IsGround(&clients[j]))
					{
						if(DistBetween(clients[j].posxy[0][0], clients[j].posxy[1][0], 0, arena->fields[i].posxyz[0], arena->fields[i].posxyz[1], 0, ctf->value) >= 0)
						{ // if client in capture range...
							dominated = 1;

							if(arena->fields[i].ctf != clients[j].country)
							{	// if not dominating, dominate and zero counter
								arena->fields[i].ctf = clients[j].country;
								arena->fields[i].ctfwho = j;

								if(arena->fields[i].ctfcount != arena->frame)
								{
									if((arena->frame - arena->fields[i].ctfcount) > 100 /*1 sec*/)
									{
										BPrintf(RADIO_YELLOW, "F%d capture counter reset", i+1);
									}

									arena->fields[i].ctfcount = arena->frame;
								}
							}
							else if(clients[arena->fields[i].ctfwho].country == arena->fields[i].country)
								arena->fields[i].ctfcount = arena->frame;
						}
					}
				}

				if(dominated)
				{
					if(arena->fields[i].ctf != arena->fields[i].country)
					{
						if((arena->frame - arena->fields[i].ctfcount) == 100 /*1 sec*/)
						{
							BPrintf(RADIO_YELLOW, "F%d capture counter started", i+1);
						}

						if((arena->frame - arena->fields[i].ctfcount) >= 3000 /*30 sec*/)
						{
							if(clients[arena->fields[i].ctfwho].inuse && (clients[arena->fields[i].ctfwho].country != arena->fields[i].country))
							{
								CaptureField(i+1, &(clients[arena->fields[i].ctfwho]));
							}
						}
					}
				}
				else
				{
					if((arena->fields[i].ctf != arena->fields[i].country) && (arena->frame - arena->fields[i].ctfcount) > 100 /*1 sec*/)
					{
						BPrintf(RADIO_YELLOW, "F%d capture counter reset", i+1);
					}

					arena->fields[i].ctfcount = arena->frame;
					arena->fields[i].ctf = arena->fields[i].country;
				}
			}
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}

	// field closed, enabletocapture, radioalert, etc...

	if (!setjmp(debug_buffer))
	{
		for (i = 0; i < fields->value; i++)
		{
			if (!(arena->frame % 3000) && !arcade->value) // tank alert
			{
				for (j = 0; j < MAX_BUILDINGS; j++)
				{
					if (!arena->fields[i].buildings[j].field)
					{
						break;
					}
					else if (arena->fields[i].buildings[j].type == BUILD_RADIOHUT || arena->fields[i].buildings[j].type == BUILD_ANTENNA)
					{
						if (arena->fields[i].buildings[j].status)
						{
							arena->fields[i].alert = 0;
							j = -1;
							break;
						}
					}
				}

				if (j > 0)
				{
					for (j = 0; j < maxentities->value; j++)
					{
						if (clients[j].inuse && clients[j].country != arena->fields[i].country &&(clients[j].drone & (DRONE_TANK1 | DRONE_TANK2 | DRONE_AAA | DRONE_KATY)))
						{
							if (sqrt(Com_Pow(clients[j].posxy[0][0] - arena->fields[i].posxyz[0], 2) + Com_Pow(clients[j].posxy[1][0] - arena->fields[i].posxyz[1], 2)) < 9000)
							{
								CPrintf(arena->fields[i].country,
								RADIO_GREEN, "ALERT!!! ALERT!!! Enemy column has been seen near F%d", i+1);
								break;
							}
						}
					}
				}
			}

			if (arena->fields[i].alert && !arcade->value) // attack alert
			{
				if (arena->fields[i].alert + 3000 <= arena->frame)
				{
					for (j = 0; j < MAX_BUILDINGS; j++)
					{
						if (!arena->fields[i].buildings[j].field)
						{
							break;
						}
						else if (arena->fields[i].buildings[j].type == BUILD_RADIOHUT || arena->fields[i].buildings[j].type == BUILD_ANTENNA)
						{
							if (arena->fields[i].buildings[j].status)
							{
								arena->fields[i].alert = 0;
								break;
							}
						}
					}

					if (arena->fields[i].alert)
					{
						CPrintf(arena->fields[i].country, RADIO_GREEN, "ALERT!!! ALERT!!! F%d is under attack!!!", i+1);
						arena->fields[i].alert = 0;
					}
				}
			}

			if (arena->fields[i].warehouse && !arcade->value) // warehouse effect
			{
				if ((arena->fields[i].warehouse + 60000) <= arena->frame)
				{
					for (j = 0; j < MAX_BUILDINGS; j++)
					{
						if (!arena->fields[i].buildings[j].field)
						{
							break;
						}
						else if ((arena->fields[i].buildings[j].type >= BUILD_50CALACK && arena->fields[i].buildings[j].type <= BUILD_88MMFLAK) || (arena->fields[i].buildings[j].type == BUILD_ARTILLERY))
						{
							arena->fields[i].buildings[j].status = 2;
							arena->fields[i].buildings[j].timer += 60000; // 10min // changed = to +=
						}
					}

					SendFieldStatus(i, NULL);
					arena->fields[i].warehouse = 0;
				}
			}

			// Check field closed
			if ((arena->fields[i].type <= FIELD_MAIN) || (arena->fields[i].type >= FIELD_WB3POST)) //!= FIELD_CV && arena->fields[i].type != FIELD_CARGO && arena->fields[i].type != FIELD_DD && arena->fields[i].type != FIELD_SUBMARINE)
			{
				close = 1;
				vitals = arena->fields[i].vitals;
				arena->fields[i].vitals = 0;

				for (j = 0; j < MAX_BUILDINGS; j++)
				{
					if (arena->fields[i].buildings[j].field)
					{
						if (!arena->fields[i].buildings[j].status && IsVitalBuilding(&(arena->fields[i].buildings[j]), oldcapt->value)) // Vital building UP, field not closed
						{
							arena->fields[i].vitals = 1;
							close = 0;
							break;
						}
					}
					else
						break;
				}

				if(vitals && !arena->fields[i].vitals)
				{
					if(!oldcapt->value && wb3->value)
					{
						CPrintf(arena->fields[i].country, RADIO_GREEN, "ALERT!!! ALERT!!! F%d has all defences down!!!", i+1);
						CPrintf(arena->fields[i].country, RADIO_GREEN, "Destroy hangars to avoid planes to be taken by enemies!!!");
					}
				}

				if(!oldcapt->value && wb3->value)
				{
					if(arena->fields[i].tonnage < GetTonnageToClose(i+1))
						close = 0;
				}

				if (!arena->fields[i].closed && close)
				{
					arena->fields[i].closed = 1;
					BPrintf(RADIO_YELLOW, "Field %d closed", i+1);

					Com_Printf(VERBOSE_DEBUG_DAMAGE, "frame %u, close = %d, closed = %d\n", arena->frame, close, arena->fields[i].closed);
					for (j = 0; j < MAX_BUILDINGS; j++)
					{
						if (arena->fields[i].buildings[j].field)
						{
							Com_Printf(VERBOSE_DEBUG_DAMAGE, "building %s, timer %d\n", GetBuildingType(arena->fields[i].buildings[j].type), arena->fields[i].buildings[j].timer);
						}
						else
							break;
					}

					for (j = 0; j < MAX_BUILDINGS; j++)
					{
						if (arena->fields[i].buildings[j].field)
						{
							if(arena->fields[i].buildings[j].status)
								arena->fields[i].buildings[j].timer += 60000;
						}
						else
							break;
					}
				}
				else if (arena->fields[i].closed && !close)
				{
					Com_Printf(VERBOSE_DEBUG_DAMAGE, "frame %u, close = %d, closed = %d\n", arena->frame, close, arena->fields[i].closed);
					for (j = 0; j < MAX_BUILDINGS; j++)
					{
						if (arena->fields[i].buildings[j].field)
						{
							Com_Printf(VERBOSE_DEBUG_DAMAGE, "building %s, timer %d\n", GetBuildingType(arena->fields[i].buildings[j].type), arena->fields[i].buildings[j].timer);
						}
						else
							break;
					}

					for (j = 0; j < MAX_CITYFIELD; j++)
					{
						if (arena->fields[i].city[j] && arena->fields[i].city[j]->needtoclose)
						{
							if (!arena->fields[i].city[j]->closed)
								break;
						}
					}

					if (j == MAX_CITYFIELD)
					{
						if(((int16_t)spawnred->value != (i+1)) && ((int16_t)spawngold->value != (i+1))) // if not a spawnfield
						{
							if(!arena->fields[i].abletocapture)
							{
								arena->fields[i].tonnage *= 0.25; // reduce tonnage to 25% when reopened after captured
							}
							arena->fields[i].abletocapture = 1;
						}
					}

					arena->fields[i].closed = 0;
					arena->fields[i].warehouse = 0; // to avoid field be reclosed by warehouse effect
					BPrintf(RADIO_YELLOW, "Field %d reopened", i+1);
				}
			}
			else
				arena->fields[i].closed = 0;
		}

		for (i = 0; i < cities->value; i++)
		{
			close = 1;

			for (j = 0; j < MAX_BUILDINGS; j++)
			{
				if (arena->cities[i].buildings[j].field)
				{
					/*				if(!arena->cities[i].buildings[j].status &&
					 arena->cities[i].buildings[j].type >= BUILD_BRIDGE && arena->cities[i].buildings[j].type <= BUILD_CRANE)
					 */
					if (!arena->cities[i].buildings[j].status && IsVitalBuilding(&(arena->cities[i].buildings[j]), oldcapt->value)) // Vital building UP, field not closed

					{
						close = 0;
						break;
					}
				}
				else
					break;
			}

			if (close)
			{
				if (!arena->cities[i].closed)
				{
					BPrintf(RADIO_YELLOW, "%s closed (c%d) (f%d)", arena->cities[i].name, i+1, arena->cities[i].field?arena->cities[i].field->number:-1);
					arena->cities[i].closed = 1;

					if (arena->cities[i].field)
					{
						for (j = 0; j < MAX_CITYFIELD; j++)
						{
							if (arena->cities[i].field->city[j] && arena->cities[i].field->city[j]->needtoclose)
							{
								if (!arena->cities[i].field->city[j]->closed)
									break;
							}
						}

						if (j == MAX_CITYFIELD)
							arena->cities[i].field->abletocapture = 1;
					}
				}
			}
			else
			{
				if (arena->cities[i].closed)
				{
					BPrintf(RADIO_YELLOW, "%s reopened (c%d) (f%d)", arena->cities[i].name, i+1, arena->cities[i].field?arena->cities[i].field->number:-1);
					arena->cities[i].closed = 0;

					if (arena->cities[i].field)
					{
						if (arena->cities[i].needtoclose)
							arena->cities[i].field->abletocapture = 0;
					}
				}
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, NULL);
	}

	// CV tick

	if (!setjmp(debug_buffer))
	{
		if (!(arena->frame % 6000)) // Log CVs position every 60sec
			LogCVsPosition();

		for (i = 0; i < cvs->value; i++)
		{
			// CV Route

			if (arena->cv[i].wptotal) // if CV have waypoints
			{
				if (arena->cv[i].outofport && !arena->cv[i].threatened && !(arena->frame % 600)) // check if there are enemies around
				{
					for (j = 0; j < maxentities->value; j++)
					{
						if (clients[j].inuse && clients[j].ready && clients[j].infly && clients[j].country != arena->fields[arena->cv[i].field].country)
						{
							if (DistBetween(clients[j].posxy[0][0], clients[j].posxy[1][0], clients[j].posalt[0], arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1],
									arena->fields[arena->cv[i].field].posxyz[2], 15000) >= 0)
							{
								ChangeCVRoute(&(arena->cv[i]), 0, 0, NULL);
								break;
							}
						}
					}
				}

				if (arena->time >= arena->cv[i].timebase) // if reach next waypoint
				{
					ReadCVWaypoints(i); // reset waypoints

					if (arena->cv[i].threatened)
					{
						arena->cv[i].threatened = 0;

						// set cv next waypoint based on current position
						// this algorithm shall be futurely enhanced when re-reoute is done near last-2nd WP

						dist = MAX_UINT32;

						for (j = 1; j < arena->cv[i].wptotal; j++)
						{
							tempdist = DistBetween(arena->cv[i].wp[j][0], arena->cv[i].wp[j][1], 0, arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1], 0, 60000);

							if (tempdist < dist)
							{
								dist = tempdist;
								arena->cv[i].wpnum = j;
							}
						}
					}
					else
						dist = 0;

					arena->cv[i].wpnum++;

					if (!arena->cv[i].outofport)
					{
						arena->fields[arena->cv[i].field].posxyz[0] = arena->cv[i].wp[0][0];
						arena->fields[arena->cv[i].field].posxyz[1] = arena->cv[i].wp[0][1];
					}

					if (arena->cv[i].wpnum == arena->cv[i].wptotal) // reset waypoint index
					{
						arena->cv[i].wpnum = 1;
					}

					if (dist < MAX_UINT32)
					{
						SetCVSpeed(&(arena->cv[i]));
					}
					else
					{
						CPrintf(arena->fields[arena->cv[i].field].country,
						RADIO_RED, "CV (F%d) is out of route", arena->cv[i].field+1);
					}
				}

				if (!(arena->frame % 100))
				{
					if (arena->cv[i].field >= fields->value)
					{
						Com_Printf(VERBOSE_WARNING, "CheckArenaRules() CV %d not field declared\n", i);
					}
					else
					{
						// adjust field and CV pos, based on route
						arena->fields[arena->cv[i].field].posxyz[0] = GetCVPos( &(arena->cv[i]), 0);
						arena->fields[arena->cv[i].field].posxyz[1] = GetCVPos( &(arena->cv[i]), 1);

						arena->fields[arena->cv[i].field].buildings[0].posx = arena->fields[arena->cv[i].field].posxyz[0];
						arena->fields[arena->cv[i].field].buildings[0].posy = arena->fields[arena->cv[i].field].posxyz[1];
					}
				}
			}
			//		}

			// CV Attack
			if (arena->cv[i].speed > 1 && !(arena->frame % ((u_int32_t) cvdelay->value * 100)) && !(arena->cv[i].field >= fields->value))
			{
				dist = 0;

				j = NearestField(arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1], arena->fields[arena->cv[i].field].country, TRUE, TRUE, &dist);

				if (j >= 0 && dist < (u_int32_t)cvrange->value && j != arena->cv[i].field)
				{
					if (j < fields->value)
					{
						if (!arena->fields[j].closed)
						{
							if (arena->fields[arena->cv[i].field].type == FIELD_SUBMARINE)
							{
								ThrowBomb(FALSE, arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1], arena->fields[arena->cv[i].field].posxyz[2], arena->fields[j].posxyz[0], arena->fields[j].posxyz[1], arena->fields[j].posxyz[2], NULL);
								ThrowBomb(TRUE, arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1], arena->fields[arena->cv[i].field].posxyz[2], arena->fields[j].posxyz[0], arena->fields[j].posxyz[1], arena->fields[j].posxyz[2], NULL);
							}
							else
								CVFire(arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1], arena->fields[arena->cv[i].field].posxyz[2], arena->fields[j].posxyz[0],
										arena->fields[j].posxyz[1], arena->fields[j].posxyz[2]);
						}
					}
					else
					{
						if (!arena->cities[j - (int16_t)fields->value].closed)
						{
							if (arena->fields[arena->cv[i].field].type == FIELD_SUBMARINE)
							{
								ThrowBomb(FALSE, arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1], arena->fields[arena->cv[i].field].posxyz[2], arena->cities[j - (int16_t)fields->value].posxyz[0], arena->cities[j - (int16_t)fields->value].posxyz[1], arena->cities[j - (int16_t)fields->value].posxyz[2], NULL);
								ThrowBomb(TRUE, arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1], arena->fields[arena->cv[i].field].posxyz[2], arena->cities[j - (int16_t)fields->value].posxyz[0], arena->cities[j - (int16_t)fields->value].posxyz[1], arena->cities[j - (int16_t)fields->value].posxyz[2], NULL);
							}
							else
								CVFire(arena->fields[arena->cv[i].field].posxyz[0], arena->fields[arena->cv[i].field].posxyz[1], arena->fields[arena->cv[i].field].posxyz[2], arena->cities[j - (int16_t)fields->value].posxyz[0], arena->cities[j - (int16_t)fields->value].posxyz[1], arena->cities[j - (int16_t)fields->value].posxyz[2]);
						}
					}
				}
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, NULL);
	}

	// RPS Tick

	if (rps->value)
	{
		if (!(arena->frame % 6000)) // every minute
		{
			if (!setjmp(debug_buffer))
			{
				UpdateRPS(1);
			}
			else
			{
				DebugClient(__FILE__, __LINE__, TRUE, NULL);
			}
		}
	}

	// Map dots

	if (!(arena->frame % 200))
	{
		if (!setjmp(debug_buffer))
		{
			SendMapDots();
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}


	// Change Arena

	if (!setjmp(debug_buffer))
	{
		if (arena->mapnum >= 0 && mapcycle->value)
		{
			dist = (arena->year * 10000)+(arena->month * 100) + arena->day;

			if (arena->mapcycle[arena->mapnum].date == dist) // this is the changemap day!
			{
				if (!arena->countdown)
				{
					arena->countdown = 18001; // 3 minutes
				}
			}
		}

		if (mapcycle->value && !(arena->frame % 60000)) // Warn players time left to change arena every 10 minutes
		{
			i = TimetoNextArena();

			if (i > 0 && i < 3)
			{
				if (i > 1)
					BPrintf(RADIO_YELLOW, "Next arena will be loaded in %u virtual days", i);
				else
					BPrintf(RADIO_YELLOW, "Next arena will be loaded next virtual day", i);
			}
		}

		if (arena->countdown)
		{
			arena->countdown--;

			if (arena->countdown < 6000)
			{
				if (!(arena->countdown % 1000)) // each 10 secs
				{
					BPrintf(RADIO_YELLOW, "Arena will be changed in %u seconds", arena->countdown / 100);
				}
			}
			else if (!(arena->countdown % 6000)) // each minute
			{
				BPrintf(RADIO_YELLOW, "Arena will be changed in %s", Com_TimeSeconds(arena->countdown/100), arena->countdown/100);
				BPrintf(RADIO_YELLOW, "Your don't need to land, your score will be saved");
			}

			if (arena->countdown == 1)
			{

				arena->mapnum++;
				arena->countdown = 0;

				if ((arena->mapnum) == MAX_MAPCYCLE || !arena->mapcycle[(arena->mapnum)].date)
				{
					NewWar(); // set mapnum to zero, backup scores, reset scores, etc
				}
				else
				{
					BackupScores(COLLECT_MAP);
					sprintf(my_query, "UPDATE score_common SET killstod = '0', structstod = '0'");

					if (d_mysql_query(&my_sock, my_query)) // query succeeded
					{
						Com_Printf(VERBOSE_WARNING, "MapChange: couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}

				ChangeArena(arena->mapcycle[arena->mapnum].mapname, NULL);
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, NULL);
	}

	// Tatical drones tick

	if (!notanks->value && !arcade->value)
	{
		if (!(arena->frame % 100))
		{
			if (!setjmp(debug_buffer))
			{
				if (rand() % 2)
					close = 1; // red
				else
					close = 3; // gold

				// check all arena for total of drones
				for (i = 0, j = 0; i < maxentities->value; i++)
				{
					if (clients[i].inuse && (clients[i].drone & (DRONE_TANK1 | DRONE_TANK2 | DRONE_AAA | DRONE_KATY)) && clients[i].country == close)
					{
						j++;
					}
				}

				dist = 20;

				if (j < 6)
				{
					while (close)
					{
						i = rand() % (u_int16_t)fields->value;

						if (((arena->fields[i].type <= FIELD_MAIN) || (arena->fields[i].type >= FIELD_WB3POST)) && arena->fields[i].country == close)
						{
							j = NearestField(arena->fields[i].posxyz[0], arena->fields[i].posxyz[1], close, FALSE,
							FALSE, NULL);

							if (j >= 0)
								LaunchTanks(i, j, close, NULL); //close -> country

							close = 0;
						}

						if (dist)
						{
							dist--;
						}
						else
						{
							close = 0;
						}
					}
				}
			}
			else
			{
				DebugClient(__FILE__, __LINE__, TRUE, NULL);
			}
		}
	}

	// Arenalist update

	if (!(arena->frame % 500)) // 5 sec
	{
		if (!setjmp(debug_buffer))
		{
			NoopArenalist();
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}

	// Online Players

	if (!(arena->frame % 100)) // 1 sec
	{
		if (!setjmp(debug_buffer))
		{
			players_num += arena->numplayers;
			players_count++;

			if (!(arena->frame % 30000)) // 5 min
			{
				Com_Printf(VERBOSE_ONLINE, "Online Players: %.3f\n", (double) players_num / players_count);
				players_num = players_count = 0;
			}
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}

	// Country index tick

	if (!(arena->frame % 6000)) // 1 min
	{
		if (!setjmp(debug_buffer))
		{
			reds = golds = 0;

			for(i = 0; i < maxentities->value; i++)
			{
				if (clients[i].inuse && !clients[i].drone)
				{
					if(clients[i].country == COUNTRY_RED)
						reds++;

					if(clients[i].country == COUNTRY_GOLD)
						golds++;
				}
			}

			arena->redindex = 1 + (0.4 * (golds > 10?10:golds) / 10);
			arena->goldindex = 1 + (0.4 * (reds > 10?10:reds) / 10);
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}

	// File descriptors tick

	if (!(arena->frame % 6000)) // 1 min
	{
		if (!setjmp(debug_buffer))
		{
			for(i = 0; i < MAX_LOGFILE; i++)
			{
				if(logfile[i])
				{
					fflush(stdout);
					fclose(logfile[i]);
					logfile[i] = NULL;
				}
			}
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}


	// Backup Tick

	if (!(arena->frame % 30000)) // 5 min
	{
		if (!setjmp(debug_buffer))
		{
			BackupArenaStatus();
			Cmd_CheckBuildings(NULL); // DEBUG
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, NULL);
		}
	}

	// ConnStatistics tick
	if (!setjmp(debug_buffer))
	{
		if (!(arena->frame % 100)) // 1 sec
		{
			ConnStatistics(NULL, 0, 1);

			if (!(arena->frame % 500)) // 5 sec
			{
				ConnStatistics(NULL, 0, 5);

				if (!(arena->frame % 1000)) // 10 sec
				{
					ConnStatistics(NULL, 0, 10);

					if (!(arena->frame % 3000)) // 30 sec
					{
						ConnStatistics(NULL, 0, 30);

						if (!(arena->frame % 6000)) // 60 sec
						{
							ConnStatistics(NULL, 0, 60);
						}
					}
				}
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, NULL);
	}
}

/**
 ProcessMetarWeather

 Process METAR code get by a PHP script and use as WB weather
 */

void ProcessMetarWeather(void)
{
	FILE* fp;
	char buffer[64];
	double wind, angle, xwind, ywind;
	u_int8_t weather;
	char *token;
	char value[8];

	if ((fp = fopen("./cron/weather.cfg", "r")))
	{
		if (!fgets(buffer, sizeof(buffer), fp))
		{
			Com_Printf(VERBOSE_WARNING, "Unexpected end of weather.cfg\n");
			fclose(fp);
			return;
		}
		else
		{
			angle = Com_Atof((char *)strtok(buffer, ";"));
			wind = Com_Atof((char *)strtok(NULL, ";"));

			xwind = wind * sin(Com_Rad(angle));
			snprintf(value, sizeof(value), "%d", (int32_t)(xwind+0.5));
			Var_Set("xwindvelocity", value);

			ywind = wind * cos(Com_Rad(angle));
			snprintf(value, sizeof(value), "%d", (int32_t)(ywind+0.5));
			Var_Set("ywindvelocity", value);

			token = (char *)strtok(NULL, ";");
			strcpy(value, token ? token : "0");
			weather = Com_Atoi(value);
			Var_Set("weather", value);

			BPrintf(RADIO_WEATHER, "Weather: %s, Wind: %.2fKm/h from %s",
			(!weather)?"Cloudy":(weather == 1)?"Clear":(weather == 2)?"Raining":"Partialy Cloudy",
			(wind * 1.09728),
			WBRhumb(angle));

			fclose(fp);
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "Error openning metar file\n");
		return;
	}
}

/**
 ProcessCommands

 Process console input commands or dot commands from clients
 */

void ProcessCommands(char *command, client_t *client)
{
	int32_t i, j, size, permission;
	u_int32_t temp;
	FILE *fp;
	var_t *var;
	u_int8_t argc = 0;
	char *argv[10];
	char file[128];
	double wind, angle;
	client_t *pclient;

	memset(argv, 0, sizeof(argv));

	if (!Com_Strncmp(command, "//", 2)) // break command if commented
		return;

	if (command[0] == '\n')
		return;

	size = strlen(command);

	if(size > 128)
	{
		Com_Printf(VERBOSE_WARNING, "ProcessCommands(size) > 100 (%d)\n", size);
		return;
	}

	while (command[--size] == ' ' || command[size] == '\n' || command[size] == '\t' || command[size] == '\r')
		//Remove ending spaces
		;

	if (size < 0)
		return;

	command[++size] = '\0';

	//Search for args
	for (i=0; i < size; i++)
	{
		if (command[i] == ' ') // break command and args
		{
			command[i++] = '\0';
			while (command[i] == ' ')
				// remove spaces between command, args
				i++;

			if (command[i] == '\"') // quoted string
			{
				i++;

				if (argc < 10)
				{
					argv[argc] = &(command[i]);
					Com_ParseString(argv[argc]);
					argc++;
				}

				while (command[i] != '\"')
					i++;

				command[i] = '\0';
			}
			else
			{
				if (!Com_Strncmp(&(command[i]), "//", 2)) // break args if commented
					break;

				if (argc < 10)
				{
					argv[argc] = &(command[i]);
					Com_ParseString(argv[argc]);
					argc++;
				}
			}
		}
	}

	if (!client) // if not client may (must) be console command
	{
		permission = 3;
	}
	else
	{
		permission = client->attr;
	}

	// commands that everyone can execute

	if (client) // commands that console shall not to execute (players can execute)
	{
		if (!Com_Stricmp(command, "country"))
		{
			if (!client->infly || (client->attr & FLAG_ADMIN))
			{
				if (argv[0])
				{
					if(Com_Atoi(argv[0]) == 1 || Com_Atoi(argv[0]) == 3 || client->attr) // limit to Red and Gold for normal users
					{
						Cmd_Move(NULL, Com_Atoi(argv[0]), client);
						UpdateClientFile(client);
					}
					else
					{
						PPrintf(client, RADIO_YELLOW, "Invalid country, please select Red (1) or Gold (3)");
					}
				}
				else
				{
					PPrintf(client, RADIO_YELLOW, "Your country is %s", GetCountry(client->country));
					PPrintf(client, RADIO_YELLOW, "usage: .country <country number>");
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "You can't change country while flying");
			}
			return;
		}
		else if (!Com_Stricmp(command, "mov") || !Com_Stricmp(command, "move"))
		{
			if (!client->infly || client->attr == 1)
			{
				if (argv[0])
					Cmd_Move(argv[0], client->country, client);
				else
					PPrintf(client, RADIO_YELLOW, "usage: .mov [F<field number>|HQ]");
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "You can't move to another field while flying");
			}
			return;
		}
		else if (!Com_Stricmp(command, "echo"))
		{
			if (argv[0])
				Cmd_Say(argv, argc, client);
			else
				PPrintf(client, RADIO_YELLOW, "usage: .echo <message with max of 10 words>");
			return;
		}
		else if (!Com_Stricmp(command, "plane"))
		{
			if (argv[0])
				Cmd_Plane(Com_Atoi(argv[0]), client);
			else
			{
				Cmd_Plane(999, client);
				PPrintf(client, RADIO_YELLOW, "usage: .plane <plane number>");
			}
			return;
		}
		else if (!Com_Stricmp(command, "warp") && (permission & (FLAG_ADMIN | FLAG_OP)))
		{
			if (argv[0])
			{
				client->warp = Com_Atoi(argv[0]) * 100; // convert seconds to frames
				client->warptimer = client->warp;
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "usage: .warp <seconds>");
			}
			return;
		}
		else if (!Com_Stricmp(command, "fuel"))
		{
			if (argv[0])
				Cmd_Fuel(Com_Atoi(argv[0]), client);
			else
			{
				PPrintf(client, RADIO_YELLOW, "Current fuel: %u", client->fuel);
				PPrintf(client, RADIO_YELLOW, "usage: .fuel <%%fuel>");
			}
			return;
		}
		else if (!Com_Stricmp(command, "conv"))
		{
			if (argv[0])
			{
				Cmd_Conv(Com_Atoi(argv[0]), client);
				UpdateClientFile(client);
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Current convergency: %u", client->conv);
				PPrintf(client, RADIO_YELLOW, "usage: .conv <distance>");
			}
			return;
		}
		else if (!Com_Stricmp(command, "ord"))
		{
			if (argv[0])
				Cmd_Ord(Com_Atoi(argv[0]), client);
			else
			{
				PPrintf(client, RADIO_YELLOW, "Current ordinance: %u", client->ord);
				PPrintf(client, RADIO_YELLOW, "usage: .ord <number>");
			}
			return;
		}
		else if (!Com_Stricmp(command, "forecast"))
		{
			wind = sqrt(Com_Pow(xwindvelocity->value, 2) + Com_Pow(ywindvelocity->value, 2));
			angle = Com_Deg(atan(xwindvelocity->value / ywindvelocity->value));

			PPrintf(client, RADIO_WEATHER, "Weather: %s, Wind: %.2fKm/h from %s",
				(!weather->value)?"Cloudy":(weather->value == 1)?"Clear":(weather->value == 2)?"Raining":"Partialy Cloudy",
				(wind * 1.09728),
				WBRhumb(angle));
			return;
		}
		else if (!Com_Stricmp(command, "rain"))
		{
			if(client->rain)
			{
				PPrintf(client, RADIO_YELLOW, "Rain disabled");
				Com_Printf(VERBOSE_ATTENTION, "%s disabled rain\n", client->longnick);
				WB3DotCommand(client, ".weather 0"); // cloudy
				client->rain = 0;
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Rain enabled");
				Com_Printf(VERBOSE_ATTENTION, "%s enabled rain\n", client->longnick);
				WB3DotCommand(client, ".weather %u", (u_int8_t)weather->value);
				client->rain = 1;
			}
			return;
		}
		else if (!Com_Stricmp(command, "reload"))
		{
			if (!arcade->value)
			{
				Cmd_Reload(client);
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
			}

			return;
		}
		else if (!Com_Stricmp(command, "pingtest"))
		{
			if(argv[0])
			{
				if(pclient = FindLClient(argv[0]))
				{
					PPrintf(client, RADIO_YELLOW, "%s connection quality is currently: %s", pclient->longnick, !(pclient->cancollide)?"Not available":pclient->connection == 0?"Stable":pclient->connection == 1?"Fair":pclient->connection == 2?"Unstable":"Poor");
				}
				else
				{
					PPrintf(client, RADIO_YELLOW, "Player not found");
				}
			}
			else
				PPrintf(client, RADIO_YELLOW, "Your connection quality is currently: %s", !(client->cancollide)?"Not available":client->connection == 0?"Stable":client->connection == 1?"Fair":client->connection == 2?"Unstable":"Poor");
			//Cmd_Pingtest(0, client);
			return;
		}
		else if(!Com_Stricmp(command, "startdrone"))
		{
			if(testarena->value)
			{
				if(!argv[2])
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "usage: .startdrone <field> <plane> <angle>");
					return;
				}

				Cmd_StartDrone(Com_Atoi(argv[0]), Com_Atoi(argv[1]), Com_Atof(argv[2]), client);
				return;
			}
		}
		else if (!Com_Stricmp(command, "easy"))
		{
			if (argv[0])
				Cmd_Easy(Com_Atoi(argv[0]), client);
			else
				PPrintf(client, RADIO_YELLOW, "usage: .easy [1|0]");
			return;
		}
		else if (!Com_Stricmp(command, "kill"))
		{
			if (client->attached && client->attached->drone & (DRONE_HMACK | DRONE_HTANK))
			{
				PPrintf(client, RADIO_YELLOW, "You can't kill yourself in hmack, hit 'bail out'(enter) 3 times");
				return;
			}
			SendForceStatus(STATUS_PILOT, client->status_status, client);
			return;
		}
		else if (!Com_Stricmp(command, "fly"))
		{
			if (!client->infly || client->attr == 1)
			{
				if (!Cmd_Fly(0, client))
				{
					if (client->shanghai) // start shanghai flight
					{
						if (client->shanghai->ready && !client->shanghai->infly)
						{
							client->shanghai->attached = client;
							Com_Printf(VERBOSE_ALWAYS, "%s start flight with %s in shanghai\n", client->longnick, client->shanghai->longnick);
							Cmd_Fly(2, client->shanghai);
						}
					}

					if (HaveGunner(client->plane))
					{
						for (i = 0; i < 7; i++) // start gunners flight
						{
							if (client->gunners[i])
							{
								if (client->gunners[i]->ready && !client->gunners[i]->infly)
								{
									client->gunners[i]->attached = client;
									Cmd_Fly(GunPos(i, 1), client->gunners[i]);
								}
							}
						}
					}
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "You're already flying");
			}
			return;
		}
		else if (!Com_Stricmp(command, "motd"))
		{
			snprintf(file, sizeof(file), "motd.txt");

			if ((fp = fopen(file, "r")) == NULL)
			{
				// Couldn't open general motd.txt, try arena motd
				snprintf(file, sizeof(file), "./arenas/%s/motd.txt", dirname->string);

				if ((fp = fopen(file, "r")) == NULL)
				{
					// Couldn't even open arena motd, print error
					Com_Printf(VERBOSE_WARNING, "Couldn't open \"%s\"\n", file);
					return;
				}
				else
				{
					fclose(fp);
				}
			}
			else
			{
				fclose(fp);
			}

			SendFileSeq1(file, "motd.txt", client);
			return;
		}
		else if (!Com_Stricmp(command, "show"))
		{
			if (!client->infly)
			{
				Cmd_Show(client);
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "You can't use this command while flying");
			}
			return;
		}
		else if (!Com_Stricmp(command, "gunstat"))
		{
			if (client->gunstat)
			{
				client->gunstat = 0;
				PPrintf(client, RADIO_YELLOW, "gunstat disabled");
			}
			else
			{
				client->gunstat = 1;
				PPrintf(client, RADIO_YELLOW, "gunstat enabled");
			}
			return;
		}
		else if (!Com_Stricmp(command, "challenge"))
		{
			return;
		}
		else if (!Com_Stricmp(command, "score"))
		{
			if (!client->infly)
			{
				if (argv[0])
					Cmd_Score(argv[0], client);
				else
					Cmd_Score(NULL, client);
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Current flight score: %.3f", client->score.airscore + client->score.groundscore + client->score.captscore + client->score.rescuescore - client->score.penaltyscore - client->score.costscore);
			}
			return;
		}
		else if (!Com_Stricmp(command, "sqscore") || !Com_Stricmp(command, "medals") || !Com_Stricmp(command, "medal"))
		{
			PClientMedals(NULL, client);
			return;
		}
		else if (!Com_Stricmp(command, "help"))
		{
			SendFileSeq1("help.txt", "helpcommands.asc", client);
			return;
		}
		else if (!Com_Stricmp(command, "startfau"))
		{
			if (!client->infly)
			{
				if (!argv[2])
				{
					PPrintf(client, RADIO_YELLOW, "usage: .startfau <distance> <azimute> <0|1>");
				}
				else
				{
					Cmd_StartFau(Com_Atoi(argv[0]), Com_Atof(argv[1]), Com_Atoi(argv[2]), client);
				}
			}
			else
				PPrintf(client, RADIO_YELLOW, "You can't launch V-1 while flying");
			return;
		}
		else if (!Com_Stricmp(command, "hmack"))
		{
			//			if (wb3->value)
			//			{
			//				PPrintf(client, RADIO_LIGHTYELLOW,
			//						"This command not available in WB3");
			//				return;
			//			}

			if (!argv[0])
			{
				Cmd_Hmack(client, NULL, 0);
			}
			else
			{
				if (!Com_Stricmp(argv[0], "move"))
				{
					if (client->attr == FLAG_OP)
					{
						for (i = 0; i < MAX_RELATED; i++)
						{
							if (client->related[i]->drone == DRONE_HMACK)
							{
								client->related[i]->posxy[0][0] = Com_Atoi(argv[1]);
								client->related[i]->posxy[1][0] = Com_Atoi(argv[2]);
								break;
							}
						}
					}
				}
				else
					Cmd_Hmack(client, argv[0], 0);
			}
			return;
		}
		else if (!Com_Stricmp(command, "tank"))
		{
			if (wb3->value)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "This command not available in WB3");
				return;
			}

			if (!argv[0])
			{
				Cmd_Hmack(client, NULL, 1);
			}
			else
			{
				Cmd_Hmack(client, argv[0], 1);
			}
			return;
		}
		else if (!Com_Stricmp(command, "tanks"))
		{
			if (!notanks->value)
			{
				if (!argv[0])
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "usage: .tanks f<field>");
				}
				else
				{
					if (client->infly && !(client->attr & FLAG_ADMIN))
					{
						PPrintf(client, RADIO_YELLOW, "You can't start tanks while flying");
					}
					else
					{
						Cmd_Tanks(argv[0], client);
					}
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Command disabled by Administrator");
			}
			return;
		}
		else if (!Com_Stricmp(command, "commandos"))
		{
			if (!client->infly)
			{
				PPrintf(client, RADIO_YELLOW, "You can't start commandos at tower");
				return;
			}

			if (client->chute)
			{
				PPrintf(client, RADIO_YELLOW, "You don't have walkie-talkie to contact your commandos");
				return;
			}

			for (j = i = 0; i < MAX_RELATED; i++)
			{
				/*				if(client->related[i] && (client->related[i]->drone & DRONE_FAU))
				 {
				 PPrintf(client, RADIO_YELLOW, "You can't start commandos with a V-1 flying");
				 return;
				 }*/
				if (client->related[i] && (client->related[i]->drone & DRONE_COMMANDOS))
				{
					j++;

					if(j >= 2)
					{
						PPrintf(client, RADIO_YELLOW, "You have 2 Commandos already dropped");
						return;
					}
				}
			}

			Cmd_Commandos(client);
			return;
		}
		else if (!Com_Stricmp(command, "printlogbuffer"))
		{
			Com_PrintLogBuffer(client);
		}
		else if (!Com_Stricmp(command, "traffic"))
		{
			PPrintf(client, RADIO_YELLOW, "Traffic: %d;%d;%d;%d;%d %d;%d;%d;%d;%d",
					client->sendcount[0][0],
					client->sendcount[1][0]/5,
					client->sendcount[2][0]/10,
					client->sendcount[3][0]/30,
					client->sendcount[4][0]/60,
					client->recvcount[0][0],
					client->recvcount[1][0]/5,
					client->recvcount[2][0]/10,
					client->recvcount[3][0]/30,
					client->recvcount[4][0]/60);
		}
		else if (!Com_Stricmp(command, "minen"))
		{
			if (!argv[1])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .minen <distance> <azimuth>");
				return;
			}
			else
			{
				Cmd_Minen(Com_Atou(argv[0]), Com_Atof(argv[1]), client);
				return;
			}
		}
		else if (!Com_Stricmp(command, "pos"))
		{
			if (client->attr & (FLAG_ADMIN | FLAG_OP))
				PPrintf(client, RADIO_LIGHTYELLOW, "X %d Y %d Z %d - P %d R %d Y %d", client->posxy[0][0], client->posxy[1][0], client->posalt[0], client->angles[0][0], client->angles[1][0],
						client->angles[2][0]);

			if (!arcade->value)
			{
				if (!argv[0])
					Cmd_Pos(0, client, NULL);
				else
				{
					if (strlen(argv[0]) >= 6)
					{
						if ((pclient = FindLClient(argv[0])))
						{
							if (client->attr & (FLAG_ADMIN | FLAG_OP))
							{
								if (!((pclient->attr & FLAG_ADMIN) && (client->attr | FLAG_OP) && hideadmin->value))
									Cmd_Pos(0, pclient, client);
							}
							else
							{
								if (client->squadron && client->squadron == pclient->squadron)
								{
									Cmd_Pos(0, pclient, client);
								}
								else
								{
									Cmd_Pos(pclient->shortnick, client, NULL);
								}
							}
						}
						else
							Cmd_Pos(0, client, NULL);
					}
					else
						Cmd_Pos(Com_Atoi(argv[0]), client, NULL);
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
			}

			//			CPrintf(client->country, RADIO_LIGHTYELLOW, "%s Position: %.2f, %.2f, Alt %d [x %d y %d]", client->longnick, (double)(client->posxy[0][0] - 633157) / -105592, (double)(client->posxy[1][0] + 633157) / 105592, client->posalt[0], client->posxy[0][0], client->posxy[1][0]);
			return;
		}
		else if (!Com_Stricmp(command, "thanks"))
		{
			if (!arcade->value)
			{
				if (!client->infly)
				{
					if (!argv[0])
					{
						PPrintf(client, RADIO_LIGHTYELLOW, "usage: .thanks <nick> [<nick> <nick>]");
					}
					else
					{
						Cmd_Thanks(argv, argc, client);
					}
				}
				else
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "You can't use this command while flying");
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
			}

			return;
		}
		else if (!Com_Stricmp(command, "gclear"))
		{
			if (!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .gclear <nick>");
			}
			else
			{
				Cmd_Gclear(argv[0], client);
			}
			return;
		}
		else if (!Com_Stricmp(command, "clear"))
		{
			Cmd_Clear(client);
			return;
		}
		else if (!Com_Stricmp(command, "time") && !(permission & (FLAG_ADMIN | FLAG_OP))) // .time for normal users
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "Current time is: %2d:%02d", arena->hour, arena->minute);
			PPrintf(client, RADIO_LIGHTYELLOW, "%04d/%02d/%02d", arena->year, arena->month, arena->day);
			return;
		}
		else if (!Com_Stricmp(command, "invite") || !Com_Stricmp(command, "inv"))
		{
			if (!argv[0])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .invite <nick>");
			}
			else
			{
				Cmd_Invite(argv[0], client);
			}
			return;
		}
		else if (!Com_Stricmp(command, "jsquad"))
		{
			Cmd_Jsquad(client);
			return;
		}
		else if (!Com_Stricmp(command, "name") || !Com_Stricmp(command, "nam"))
		{
			if (!argv[0])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .name <squadron name>");
			}
			else
			{
				for (i = 1; i < 10; i++)
				{
					if (argv[i])
					{
						*(argv[i] - 1) = 0x20;
					}
					/*					if(argv[i])
					 {
					 argv[i-1][strlen(argv[i-1])] = 0x20;
					 }
					 */
					/*					if(argv[i])
					 {
					 sprintf(argv[0], "%s %s", argv[0], argv[i]);
					 }
					 */
					else
						break;
				}
				Cmd_Name(argv[0], client);
			}
			return;
		}
		else if (!Com_Stricmp(command, "slogan") || !Com_Stricmp(command, "slo"))
		{
			if (!argv[0])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .slogan <squadron motto>");
			}
			else
			{
				for (i = 1; i < 10; i++)
				{
					if (argv[i])
					{
						*(argv[i] - 1) = 0x20;
					}
					else
						break;
				}

				Cmd_Slogan(argv[0], client);
			}
			return;
		}
		else if (!Com_Stricmp(command, "remove"))
		{
			if (!argv[0])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .remove <member nick>");
			}
			else
			{
				Cmd_Remove(argv[0], client);
			}
			return;
		}
		else if (!Com_Stricmp(command, "withdraw") || !Com_Stricmp(command, "with"))
		{
			Cmd_Withdraw(client);
			return;
		}
		else if (!Com_Stricmp(command, "disband"))
		{
			Cmd_Disband(client);
			return;
		}
		else if (!Com_Stricmp(command, "test")) // TODO: Misc: debug dot commands
		{
			WB3ConfigFM(client);

			if (!argv[0])
				PPrintf(client, RADIO_LIGHTYELLOW, "Usage: .test <command> <4 values>");
			else if (!argv[1])
				WB3DotCommand(client, ".%s", argv[0]);
			else if (!argv[2])
				WB3DotCommand(client, ".%s %s", argv[0], argv[1]);
			else if (!argv[3])
				WB3DotCommand(client, ".%s %s %s", argv[0], argv[1], argv[2]);
			else if (!argv[4])
				WB3DotCommand(client, ".%s %s %s %s", argv[0], argv[1], argv[2], argv[3]);
			else if (!argv[5])
				WB3DotCommand(client, ".%s %s %s %s %s", argv[0], argv[1], argv[2], argv[3], argv[4]);

			return;
		}
		else if (!Com_Stricmp(command, "psq"))
		{
			if (!argv[1])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .psq <nick> <permissions>");
			}
			else
			{
				Cmd_Psq(argv[0], Com_Atoi(argv[1]), client);
			}
			return;
		}
		else if (!Com_Stricmp(command, "hls"))
		{
			Cmd_Hls(client);
			return;
		}
		else if (!Com_Stricmp(command, "wings"))
		{
			if (!arcade->value)
			{
				if (!nowings->value)
				{
					if (!argv[0])
					{
						PPrintf(client, RADIO_YELLOW, "usage: .wings <formation>");
						PPrintf(client, RADIO_YELLOW, "0: Disable Wingmen");
						PPrintf(client, RADIO_YELLOW, "1: Vee Wings formation");
						PPrintf(client, RADIO_YELLOW, "2: Line Abreast formation");
						PPrintf(client, RADIO_YELLOW, "3: Line Astern formation");
						PPrintf(client, RADIO_YELLOW, "4: Echelon formation");
					}
					else
						Cmd_Wings(Com_Atoi(argv[0]), client);
				}
				else
				{
					PPrintf(client, RADIO_YELLOW, "Command disabled by Administrator");
				}
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
			}

			return;
		}
		else if (!Com_Stricmp(command, "room"))
		{
			if (client->infly)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "You can't use this command while flying");
			}
			else
			{
				for (i = 0; i < maxentities->value; i++)
				{
					if (clients[i].inuse && !clients[i].drone && clients[i].ready)
					{
						if (clients[i].field == client->field)
							PPrintf(client, RADIO_YELLOW, "%s, country %s", clients[i].longnick, GetCountry(clients[i].country));
					}
				}
			}
			return;
		}
	}

	// commands that console can execute
	if (!Com_Stricmp(command, "version"))
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Tabajara Host Server version %s, build %s %s", VERSION, __DATE__, __TIME__ );
		return;
	}
	if(!Com_Stricmp(command, "license"))
	{
		printf("Tabajara Host Server is free software: you can redistribute it and/or modify\n");
		printf("it under the terms of the GNU Affero General Public License as published by\n");
		printf("the Free Software Foundation, either version 3 of the License, or\n");
		printf("(at your option) any later version.\n\n");
		printf("Tabajara Host Server is distributed in the hope that it will be useful,\n");
		printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
		printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
		printf("GNU Affero General Public License for more details.\n\n");
		printf("You should have received a copy of the GNU Affero General Public License\n");
		printf("along with Tabajara Host Server.  If not, see <http://www.gnu.org/licenses/agpl.html>.\n");
		fflush(stdout);
		return;
	}
	if(!Com_Stricmp(command, "nextarena"))
	{
		if(!arcade->value)
		{
			if(arena->mapnum >= 0 && mapcycle->value)
			{
				if(arena->mapcycle[arena->mapnum + 1].date)
				i = arena->mapnum + 1;
				else
				i = 0;
				PPrintf(client, RADIO_YELLOW, "Next Arena: %s in %04d/%02d/%02d",
				arena->mapcycle[i].mapname,
				arena->mapcycle[arena->mapnum].date / 10000,
				(arena->mapcycle[arena->mapnum].date - ((arena->mapcycle[arena->mapnum].date / 10000) * 10000)) / 100,
				(arena->mapcycle[arena->mapnum].date - ((arena->mapcycle[arena->mapnum].date / 100) * 100)));

				if(mapcycle->value)
				PPrintf(client, RADIO_YELLOW, "%d virtual days left", TimetoNextArena());
			}
			else
			PPrintf(client, RADIO_YELLOW, "Mapcycle not active");
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}

		return;
	}
	if(!Com_Stricmp(command, "maplist"))
	{
		if(!arcade->value)
		{
			if(arena->mapnum >= 0 && mapcycle->value)
			{
				for(i = 0; i < MAX_MAPCYCLE; i++)
				{
					if(arena->mapcycle[i].date)
					{
						PPrintf(client, RADIO_YELLOW, "%u: %s comes out at %04d/%02d/%02d", i+1, arena->mapcycle[i].mapname, arena->mapcycle[i].date / 10000, (arena->mapcycle[i].date - ((arena->mapcycle[i].date / 10000) * 10000)) / 100, (arena->mapcycle[i].date - ((arena->mapcycle[i].date / 100) * 100)));
					}
					else
					break;
				}
			}
			else
			PPrintf(client, RADIO_YELLOW, "Mapcycle not active");
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}

		return;
	}
	if(!Com_Stricmp(command, "info"))
	{
		if(!argv[0])
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "usage: .info <username>");
		}
		else
		{
			Cmd_Info(argv[0], client);
		}
		return;
	}
	if(!Com_Stricmp(command, "listop"))
	{
		if(!arcade->value)
		{
			UpdateIngameClients(2);

			if(client)
			{
				if(!client->infly)
				{
					SendFileSeq1(FILE_OP, "helpop.asc", client);
				}
			}
			else
			Sys_Printfile(FILE_OP);
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}

		return;
	}
	if(!Com_Stricmp(command, "listadmin"))
	{
		if(!arcade->value)
		{
			UpdateIngameClients(1);

			if(client)
			{
				if(!client->infly)
				{
					SendFileSeq1(FILE_ADMIN, "onlineadmins.asc", client);
				}
			}
			else
			Sys_Printfile(FILE_ADMIN);
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}

		return;
	}
	if(!Com_Stricmp(command, "status"))
	{
		if(!argv[0])
		{
			PPrintf(client, RADIO_YELLOW, "usage: .status <fieldnumber>");
		}
		else
		{
			if(!Com_Stricmp(argv[0], "all"))
			{
				Cmd_Field(0, client);
			}
			else
			{
				if(tolower(*argv[0]) == 'f')
				argv[0]++; // ignore compiler error

				Cmd_Field(Com_Atoi(argv[0]), client);
			}
		}
		return;
	}
	if(!Com_Stricmp(command, "utc"))
	{
		if(!arcade->value)
		{
			Cmd_UTC(client);
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}

		return;
	}
	if(!Com_Stricmp(command, "city"))
	{
		if(!argv[0])
		{
			PPrintf(client, RADIO_YELLOW, "usage: .city <citynumber>");
		}
		else
		{
			if(tolower(*argv[0]) == 'c')
			argv[0]++; // ignore compiler error

			Cmd_City(Com_Atoi(argv[0]), client);
		}
		return;
	}
	if(!Com_Stricmp(command, "remdron"))
	{
		if(!arcade->value)
		{
			if(client)
			{
				for(i = 0; i < MAX_RELATED; i++)
				{
					if(client->related[i] && client->related[i]->drone)
					{
						//~ if(client->related[i]->drone & (DRONE_HMACK | DRONE_HTANK))
						//~ client->score.ground.landed++;

						RemoveDrone(client->related[i]);
					}
				}
				PPrintf(client, RADIO_YELLOW, "All drones removed");
			}
			else
			{
				for(i = 0; i < maxentities->value; i++)
				{
					if(clients[i].inuse && clients[i].drone && !clients[i].related[0])
					RemoveDrone(&clients[i]);
				}
				PPrintf(NULL, RADIO_YELLOW, "All unrelated drones removed");
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}
		return;
	}
	if(!Com_Stricmp(command, "ros") || !Com_Stricmp(command, "roster"))
	{
		Cmd_Ros(client);
		return;
	}
	if(!Com_Stricmp(command, "whoare"))
	{
		if(!arcade->value)
		{
			if(argv[0])
			Cmd_Whoare(Com_Atoi(argv[0]), client);
			else if(client)
			Cmd_Whoare(client->radio[0], client);
			else
			{
				printf("usage: whoare <channel 1-99>\n");
				fflush(stdout);
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}

		return;
	}
	if(!Com_Stricmp(command, "listavail"))
	{
		if(!arcade->value)
		{
			if(!argv[0])
			{
				if(client)
					Cmd_Listavail(client->field, client);
				PPrintf(client, RADIO_YELLOW, "usage: .listavail <fieldnumber>");
			}
			else
			{
				if(tolower(*argv[0]) == 'f')
					argv[0]++; // ignore compiler error

				Cmd_Listavail(Com_Atoi(argv[0]), client);
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}
		return;
	}
	if(!Com_Stricmp(command, "lives"))
	{
		if(!arcade->value)
		{
			if(!argv[0] || (client && !client->attr))
			{
				if(client)
				PPrintf(client, RADIO_YELLOW, "You have %d li%s", client->lives, client->lives == 1?"fe":"ves");

				if(!client || client->attr)
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .lives <nick|all> <amount>");
			}
			else if(!argv[1])
			{
				if(!(pclient = FindLClient(argv[0])))
				{
					PPrintf(client, RADIO_YELLOW, "%s not online", argv[0]);
				}
				else
				{
					PPrintf(client, RADIO_YELLOW, "%s has %d li%s", pclient->longnick, pclient->lives, pclient->lives == 1?"fe":"ves");
				}
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .lives <nick|all> <amount>");
			}
			else
			{
				if(!Com_Stricmp(argv[0], "all"))
				{
					Cmd_Lives(NULL, Com_Atoi(argv[1]), client);
				}
				else
				{
					Cmd_Lives(argv[0], Com_Atoi(argv[1]), client);
				}
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "Command disabled in Arcade Mode");
		}

		return;
	}

	//     //     //     //     //     //

	if(permission & (FLAG_ADMIN | FLAG_OP) && arena && arena->frame > 100)
	{
		Com_Printf(VERBOSE_ATTENTION, "OPCMD: %s(%s) - .%s %s %s %s %s %s %s\n", client?client->longnick:"-HOST-", client?client->ip:"", command, argv[0]?argv[0]:"", argv[1]?argv[1]:"", argv[2]?argv[2]:"", argv[3]?argv[3]:"", argv[4]?argv[4]:"", argv[5]?argv[5]:"");
	}

	if (permission & FLAG_ADMIN) // commands that only ADMIN's can execute
	{
		if(!Com_Stricmp(command, "say"))
		{
			if(argv[0])
			Cmd_Say(argv, argc, NULL);
			else
			PPrintf(client, RADIO_YELLOW, "usage: .say <message with max of 10 words>");
			return;
		}
		if(!Com_Stricmp(command, "force"))
		{
			if(!argv[0] || !argv[1])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .force <status> <player's nick>");
			}
			else
			SendForceStatus((1 << Com_Atoi(argv[0])), 0, FindLClient(argv[1]));

			return;
		}
		if(!Com_Stricmp(command, "checkbuildings"))
		{
			Cmd_CheckBuildings(client);
			return;
		}
		if(!Com_Stricmp(command, "checkwaypoints"))
		{
			Cmd_CheckWaypoints(client);
			return;
		}
		if(!Com_Stricmp(command, "end"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .end <player's nick>");
			}
			else
			{
				if(!Com_Stricmp(argv[0], "all"))
				{
					for(i = 0; i < maxentities->value; i++)
					{
						if(clients[i].inuse && clients[i].ready && clients[i].infly && !clients[i].drone && clients[i].attr != 1)
						{
							ForceEndFlight(TRUE, &clients[i]);
						}
					}
				}
				else
					ForceEndFlight(TRUE, FindLClient(argv[0]));
			}

			return;
		}
		if(!Com_Stricmp(command, "set"))
		{

			if(!argv[0] || !argv[1])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .set <variable> <value>");
			}
			else
			{
				if(!Var_Set(argv[0], argv[1]))
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "%s is write protected or out of range", argv[0]);
				}
			}
			return;
		}
		if(!Com_Stricmp(command, "reset"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .reset <filename>");
			}
			else
			LoadArenaStatus(argv[0], client, 1);
			return;
		}
		if(!Com_Stricmp(command, "savearena"))
		{
			if(!argv[0])
			{
				SaveArenaStatus("arena", client);
			}
			else
			{
				SaveArenaStatus(argv[0], client);
			}

			PPrintf(client, RADIO_LIGHTYELLOW, "Arena saved in \"%s.arn\"", argv[0]?argv[0]:"arena");
			return;
		}
		if(!Com_Stricmp(command, "saveconfig"))
		{
			if(!argv[0])
			Var_WriteVariables("config", client);
			else
			Var_WriteVariables(argv[0], client);

			PPrintf(client, RADIO_LIGHTYELLOW, "Config saved in \"%s.cfg\"", argv[0]?argv[0]:"config");
			return;
		}
		if(!Com_Stricmp(command, "quit"))
		{
			BPrintf(RADIO_YELLOW, "Exiting Server...");
			ExitServer(0);
		}
	}

	if(permission & (FLAG_ADMIN | FLAG_OP)) // commands that ADMIN's and OP's can execute
	{
		if(!Com_Stricmp(command, "dbpasswd"))
		{
			if(!argv[0])
			{
				if(!(permission & FLAG_ADMIN))
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "You don't have permission to see DB passwd");
				}
				else
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "Current DB passwd: %s", dbpasswd->string);
				}
				return;
			}
		}
		else if(!Com_Stricmp(command, "dbuser"))
		{
			if(!argv[0])
			{
				if(!(permission & FLAG_ADMIN))
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "You don't have permission to see DB passwd");
				}
				else
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "Current DB user: %s", dbuser->string);
				}
				return;
			}
		}
		else if(!Com_Stricmp(command, "alt"))
		{
			if(!argv[1])
			{
				PPrintf(client, RADIO_YELLOW, "usage: .alt <x> <y>");
				PPrintf(client, RADIO_YELLOW, "Altitude: %d", GetHeightAt(client->posxy[0][0], client->posxy[1][0]));
			}
			else
			{
				PPrintf(client, RADIO_YELLOW, "Altitude: %d", GetHeightAt(Com_Atoi(argv[0]), Com_Atoi(argv[1])));
			}
			return;
		}
		else if(!Com_Stricmp(command, "sink") && client)
		{
			if(argv[1])
			{
				Cmd_Sink(Com_Atoi(argv[0]), Com_Atoi(argv[1]), client);
			}
			return;
		}
		else if(!Com_Stricmp(command, "droneg"))
		{
			//			if(argv[1])
			//			{
			//				if(atoi(argv[0]) > 0 && Com_Atoi(argv[0]) < 5)
			//				{
			//					PPrintf(client, RADIO_YELLOW, "RPS lag %u days\n", GetRPSLag(Com_Atoi(argv[1])));
			//					PPrintf(client, RADIO_YELLOW, "buildings %d, up %d", factorybuildings[Com_Atoi(argv[1]) - 1], factorybuildingsup[atoi(argv[0]) - 1]);
			//				}
			//			}
			PPrintf(client, RADIO_RED, "Start drone plane %u", client->plane);
			AddDrone(DRONE_DEBUG, client->posxy[0][0], client->posxy[1][0], client->posalt[0] + 100, client->country, Com_Atoi(argv[0]), client);
			return;
		}
		else if(!Com_Stricmp(command, "capt"))
		{
			if(!argv[0] || !argv[1])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .capt f|c<number> <country>");
			}
			else
			{
				if(tolower(*argv[0]) == 'c')
				{
					argv[0]++;

					i = Com_Atoi(argv[0]) - 1;

					if(arena->cities[i].field)
					{
						PPrintf(client, RADIO_YELLOW, "You need to capture f%d to capture this city", arena->cities[i].field->number);
						return;
					}

					PPrintf(client, RADIO_YELLOW, "City c%d (%s) captured to %s", arena->cities[i].number, &arena->cities[i].name, GetCountry(Com_Atoi(argv[1])));
					i += fields->value;
				}
				else
				{
					if(tolower(*argv[0]) == 'f')
					{
						argv[0]++;
					}

					i = Com_Atoi(argv[0]) - 1;
				}

				if(Cmd_Capt(i, Com_Atoi(argv[1]), NULL))
				{
					Com_LogEvent(EVENT_ADM_CAPT, client?client->id:0, 0);
					Com_LogDescription(EVENT_DESC_COUNTRY, Com_Atoi(argv[1]), NULL);
					Com_LogDescription(EVENT_DESC_FIELD, i+1, NULL);
					Com_LogDescription(EVENT_DESC_TERRAIN, 0, mapname->string);
				}
			}

			return;
		}
		else if(!Com_Stricmp(command, "ammo"))
		{
			if(!argv[0])
			{
				Cmd_Ammo(client, 0, 0);
				PPrintf(client, RADIO_YELLOW, "usage: .ammo <id> <h|a|d|t[number]>");
			}
			else if (!argv[1])
			{
				munition_t *munition;
				munition = GetMunition(Com_Atoi(argv[0]));

				if(munition)
				PPrintf(client, RADIO_LIGHTYELLOW, "name: %s, he %d, ap %d, decay %d, type %d", munition->name, munition->he, munition->ap, munition->decay, munition->type);
				else
				PPrintf(client, RADIO_LIGHTYELLOW, "Invalid Ammo");
			}
			else
			{
				Cmd_Ammo(client, Com_Atoi(argv[0]), argv[1]);
			}
			return;
		}
		else if(!Com_Stricmp(command, "saveammo"))
		{
			Cmd_Saveammo(client, argv[0]);
			return;
		}
		else if(!Com_Stricmp(command, "chmod"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .chmod <nick> [0|1|2]");
			}
			else if(!argv[1])
			Cmd_Chmod(argv[0], -1, client);
			else
			Cmd_Chmod(argv[0], Com_Atoi(argv[1]), client);

			return;
		}
		else if(!Com_Stricmp(command, "decl"))
		{
			Cmd_Decl(argv, argc, client);
			return;
		}
		else if(!Com_Stricmp(command, "undecl"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .undecl <BuildID | all>");
			}
			else if(!Com_Stricmp(argv[0], "all"))
			{
				Cmd_Undecl(0xffff, client);
			}
			else if(isdigit(*argv[0]))
			{
				if(!Com_Stricmp(argv[0], "65535"))
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "You can't undeclare this structure (reason: program definitions)");
				}
				else
				Cmd_Undecl(Com_Atoi(argv[0]), client);
			}
			else
			PPrintf(client, RADIO_LIGHTYELLOW, "usage: .undecl <BuildID | all>");
			return;
		}
		else if(!Com_Stricmp(command, "varlist"))
		{
			if(argv[0])
			Cmd_VarList(client, argv[0]);
			else
			Cmd_VarList(client, NULL);
			return;
		}
		else if(!Com_Stricmp(command, "loadarena"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .loadarena <filename>");
			}
			else
			LoadArenaStatus(argv[0], client, 0);
			return;
		}
		else if(!Com_Stricmp(command, "loadconfig"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .loadconfig <filename>");
			}
			else
			Cmd_LoadConfig(argv[0], client);
			return;
		}
		else if(!Com_Stricmp(command, "loadbatch") && client)
		{
			Cmd_LoadBatch(client);
			return;
		}
		else if(!Com_Stricmp(command, "scenariostart"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .scenariostart <scenario_dirname>");
				return;
			}

			PPrintf(client, RADIO_YELLOW, "Scenario Timeline Started");
			if(client)
			Com_Printf(VERBOSE_ALWAYS, "Scenario Timeline Started\n");
			arena->scenario = arena->frame;

			snprintf(file, sizeof(file), "./arenas/%s/t0", dirname->string);
			Cmd_LoadConfig(file, NULL);
			BackupScores(COLLECT_EVENT);
			return;
		}
		else if(!Com_Stricmp(command, "scenariostop"))
		{
			PPrintf(client, RADIO_YELLOW, "Scenario Timeline Stopped");
			if(client)
			Com_Printf(VERBOSE_ALWAYS, "Scenario Timeline Stopped\n");
			arena->scenario = 0;
			snprintf(file, sizeof(file), "./arenas/%s/t999", dirname->string);
			Cmd_LoadConfig(file, NULL);
			BackupScores(COLLECT_EVENT);
			return;
		}
		else if(!Com_Stricmp(command, "logstart") && client)
		{
			PPrintf(client, RADIO_YELLOW, "Log started");
			client->lograwdata = 1;
			return;
		}
		else if(!Com_Stricmp(command, "logstop") && client)
		{
			PPrintf(client, RADIO_YELLOW, "Log stopped");
			client->lograwdata = 0;
			return;
		}
		else if(!Com_Stricmp(command, "time")) // .time for OPs and Admins
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .time <hourminute> <multiplier (default:6)>");
			}
			else if(!argv[1])
			{
				Cmd_Time(Com_Atoi(argv[0]), NULL, NULL);
			}
			else
				Cmd_Time(Com_Atoi(argv[0]), argv[1], NULL);

			PPrintf(client, RADIO_LIGHTYELLOW, "Current time is: %2d:%02d", arena->hour, arena->minute);
			PPrintf(client, RADIO_LIGHTYELLOW, "%04d/%02d/%02d", arena->year, arena->month, arena->day);
			return;
		}
		else if(!Com_Stricmp(command, "date"))
		{
			if(!argv[2])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .date <yyyy> <mm> <dd>");
			}
			else
			Cmd_Date(Com_Atoi(argv[0]), Com_Atoi(argv[1]), Com_Atoi(argv[2]), client);

			return;
		}
		else if(!Com_Stricmp(command, "view") && client)
		{
			if(client->attr && (client->attr & (u_int8_t)mview->value))
			{
				if(!argv[0])
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .view [<nick>|exit]");
				else
				Cmd_View(FindLClient(argv[0]), client);
			}
			else
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "You don't have permission to use this command");
			}

			return;
		}
		else if(!Com_Stricmp(command, "gunners") && client)
		{
			for(i = 0; i < 14; i++)
			PPrintf(client, RADIO_LIGHTYELLOW, "%02d: %s, %d", i, client->gunners[i]?client->gunners[i]->longnick:"empty", client->gunners[i]?client->gunners[i]->shortnick:0);
			return;
		}
		else if(!Com_Stricmp(command, "numplayers"))
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "numplayers %d", arena->numplayers);
			return;
		}
		else if(!Com_Stricmp(command, "numdrones"))
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "numdrones %d", arena->numdrones);
			return;
		}
		else if(!Com_Stricmp(command, "kick"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .kick <nick>");
			}
			else if(!Com_Stricmp(argv[0], "all"))
			{
				Com_Printf(VERBOSE_ATTENTION, "All users were kicked out by %s\n", client?client->longnick:"-HOST-");
				BPrintf(RADIO_YELLOW, "ATTENTION: All users were kicked out by %s\n", client?client->longnick:"-HOST-");

				for(i = 0; i < maxentities->value; i++)
				{
					if(clients[i].inuse && !clients[i].drone)
					{
						RemoveClient(&clients[i]);
					}
				}
			}
			else
			{
				Com_Printf(VERBOSE_ATTENTION, "%s was kicked out by %s\n", argv[0], client?client->longnick:"-HOST-");
				BPrintf(RADIO_YELLOW, "ATTENTION: %s was kicked out by %s\n", argv[0], client?client->longnick:"-HOST-");

				RemoveClient(FindLClient(argv[0]));
			}
			return;
		}
		else if(!Com_Stricmp(command, "helpop") && client)
		{
			SendFileSeq1("helpop.txt", "helpop.asc", client);
			return;
		}
		else if(!Com_Stricmp(command, "uptime"))
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "System up since %ud %.2d:%.2d:%.2d",
			arena->frame/8640000,
			(arena->frame/360000)%24,
			(arena->frame/6000)%60,
			(arena->frame/100)%60);
			return;
		}
		else if(!Com_Stricmp(command, "tk"))
		{
			if(!argv[1])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .tk <nick> <0|1>");
			}
			else
			{
				Cmd_TK(argv[0], Com_Atoi(argv[1]), client);
			}

			return;
		}
		else if(!Com_Stricmp(command, "white"))
		{
			if(!argv[1])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .white <nick|all> <0|1>");
			}
			else
			{
				Cmd_White(argv[0], Com_Atoi(argv[1]), client);
			}

			return;
		}
		else if(!Com_Stricmp(command, "flare") && client)
		{
			Cmd_Flare(client);
			return;
		}
		else if(!Com_Stricmp(command, "rocket") && client)
		{
			if(!argv[2])
				return;
			else
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Launched");
				Cmd_Rocket(Com_Atoi(argv[0]), Com_Atof(argv[1]), Com_Atof(argv[2]), client);
			}

			return;
		}
		else if(!Com_Stricmp(command, "startmapping"))
		{
			PPrintf(client, RADIO_LIGHTYELLOW, "Starting Topography mapping");
			client->mapper = 1;
			client->mapperx = Com_Atoi(argv[0]);
			client->mappery = Com_Atoi(argv[1]);
			return;
		}
		else if(!Com_Stricmp(command, "stopmapping"))
		{
			client->mapper = 0;
			PPrintf(client, RADIO_LIGHTYELLOW, "Stopping Topography mapping");
			return;
		}
		else if(!Com_Stricmp(command, "restore"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .restore f[number]");
			}
			else
			{
				if(tolower(*argv[0]) == 'f')
				argv[0]++;

				Cmd_Restore(Com_Atoi(argv[0]), client);
			}
			return;
		}
		else if(!Com_Stricmp(command, "destroy"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .destroy f[number] [time]");
			}
			else if(!argv[1])
			{
				if(tolower(*argv[0]) == 'f')
				argv[0]++;

				Cmd_Destroy(Com_Atoi(argv[0]), 10, client);
			}
			else
			{
				if(tolower(*argv[0]) == 'f')
				argv[0]++;

				Cmd_Destroy(Com_Atoi(argv[0]), Com_Atoi(argv[1]), client);
			}
			return;
		}
		else if(!Com_Stricmp(command, "changecvroute") && client)
		{
			if(argv[1])
			Cmd_ChangeCVRoute(Com_Atof(argv[0]), Com_Atoi(argv[1]), client);
			else if (argv[0])
			Cmd_ChangeCVRoute(Com_Atof(argv[0]), 5000, client);
			else
			PPrintf(client, RADIO_LIGHTYELLOW, "usage: .changecvroute <azimute> [distance]");
			return;
		}
		else if(!Com_Stricmp(command, "changecountry"))
		{
			if(!argv[1])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .changecountry <nick> <country number>");
			}
			else
			{
				if((pclient = FindLClient(argv[0])))
				{
					temp = pclient->countrytime;
					pclient->countrytime = 0; // allow change country w/o timelimit
					Cmd_Move(NULL, Com_Atoi(argv[1]), pclient);
					pclient->countrytime = temp;
					UpdateClientFile(pclient);
				}
				else
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "Player not found");
				}
			}
			return;
		}
		else if(!Com_Stricmp(command, "seta"))
		{
			if(!argv[2] || (tolower(*argv[0]) == 't' && !argv[3]) || (!tolower(*argv[0]) == 'f' && !tolower(*argv[0]) == 't'))
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .seta f<field> <planenum> <amount>");
				PPrintf(client, RADIO_LIGHTYELLOW, "-1 == all in any attribute and -2 == all but CV in <field>");
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .seta t<fieldtype> <country> <planenum> <amount>");
				PPrintf(client, RADIO_LIGHTYELLOW, "<country>: use OR bit to bit. e.g.: Red+Gold = 5");
				PPrintf(client, RADIO_LIGHTYELLOW, "<country>: red = 1, green = 2, gold = 4, purple = 8");
				PPrintf(client, RADIO_LIGHTYELLOW, "<fieldtype>: small = 1, medium = 2, big = 3, CV = 4, Cargo = 5");
			}
			else
			{
				if(tolower(*argv[0]) == 't')
				{
					Cmd_Seta(argv[0], Com_Atoi(argv[1]), Com_Atoi(argv[2]), Com_Atoi(argv[3]));
					if(client)
					BPrintf(RADIO_LIGHTYELLOW, "Amount of plane %s changed to %d at %s fields by %s", Com_Atoi(argv[1]) < 0?"All":GetPlaneName(Com_Atoi(argv[1])), Com_Atoi(argv[2]), GetFieldType(Com_Atoi(argv[0]+1)), client?client->longnick:"-HOST-");
				}
				else
				{
					if(client)
					BPrintf(RADIO_LIGHTYELLOW, "Amount of plane %s changed to %d at %s by %s", Com_Atoi(argv[1]) < 0?"All":GetPlaneName(Com_Atoi(argv[1])), Com_Atoi(argv[2]), argv[0], client?client->longnick:"-HOST-");

					Cmd_Seta(argv[0], 0, Com_Atoi(argv[1]), Com_Atoi(argv[2]));
				}
			}
			return;
		}
		else if(!Com_Stricmp(command, "part"))
		{
			Cmd_Part(argv, argc, client);
			return;
		}
		else if(!Com_Stricmp(command, "vector"))
		{
			if(argv[1])
			{
				*(u_int16_t *)(file) = htons(0x030A);
				*(u_int32_t *)(file + 2) = htonl(Com_Atoi(argv[0]));
				*(u_int32_t *)(file + 6) = htonl(Com_Atoi(argv[1]));

				SendPacket(file, 10, client);
			}
			return;
		}
		else if(!Com_Stricmp(command, "ban"))
		{
			if(!argv[1])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .ban <nick> <0|1>");
			}
			else
			{
				Cmd_Ban(argv[0], Com_Atou(argv[1]), client);
			}
			return;
		}
		else if(!Com_Stricmp(command, "loadrps"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .loadrps <filename>");
			}
			else
			{
				LoadRPS(argv[0], client);
			}
			return;
		}
		else if(!Com_Stricmp(command, "showrps"))
		{
			ShowRPS(client);
			return;
		}
		else if(!Com_Stricmp(command, "loadwaypoints"))
		{
			for(i = 0; i < cvs->value; i++)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Loading CV %u waypoints", i);
				ReadCVWaypoints(i);
			}
			return;
		}
		else if(!Com_Stricmp(command, "loaddm"))
		{
			LoadDamageModel(client);
			return;
		}
		else if(!Com_Stricmp(command, "savedm"))
		{
			SaveDamageModel(client, argv[0]);
			return;
		}
		else if(!Com_Stricmp(command, "loadammo"))
		{
			LoadAmmo(client);
			PPrintf(client, RADIO_YELLOW, "Loading ammo table");
			return;
		}
		else if(!Com_Stricmp(command, "resetcvs"))
		{
			for(i = 0; i < cvs->value; i++)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "Reseting CV %u", i);

				ResetCVPos(&(arena->cv[i]));
			}
			return;
		}
		else if(!Com_Stricmp(command, "struct"))
		{
			if(!argv[1])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .struct <ID> <status>");
			}
			else
			{
				SetBuildingStatus(GetBuilding(Com_Atoi(argv[0])), Com_Atoi(argv[1]), NULL);
			}
			return;
		}
		/*		else if(!Com_Stricmp(command, "saverps"))
		 {
		 if(!argv[0])
		 {
		 SaveRPS("rps", client);
		 }
		 else
		 {
		 SaveRPS(argv[0], client);
		 }
		 return;
		 } */
		else if(!Com_Stricmp(command, "changearena"))
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "usage: .changearena <mapname>");
			}
			else
			{
				ChangeArena(argv[0], client);
			}
			return;
		}

		var = Var_FindVar(command);

		if(var)
		{
			if(!argv[0])
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "\"%s\":\"%s\" (min: %.2f - max %.2f)", var->name, var->string, var->min, var->max);
				PPrintf(client, RADIO_LIGHTYELLOW, "%s", var->description?var->description:"No description");
			}
			else
			{
				if(!(permission & FLAG_ADMIN) && (var->flags & VAR_ADMIN))
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "you don't have permission to change this variable");
					return;
				}

				if(!Var_Set(command, argv[0]))
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "%s is write protected or out of range", command);
				}
				else
				{
					Com_LogEvent(EVENT_ADM_ALTVAR, client?client->id:0, 0);
					sprintf(command, "%s = %s", var->name, var->string);
					Com_LogDescription(EVENT_DESC_VAR, 0, command);

					BPrintf(RADIO_LIGHTYELLOW, "\"%s\" changed to \"%s\" by %s", var->name, var->string, client?client->longnick:"-HOST-");
				}
			}

			return;
		}
	}
	// else unknown command
	PPrintf(client, RADIO_LIGHTYELLOW, "Unknown command \"%s\"", command);
}

/**
 SendFileSeq1

 Begin Sending File Sequence
 */

void SendFileSeq1(char *file, char *clifile, client_t *client)
{
	u_int8_t buffer[128];
	u_int8_t filesize;
	sendfile1_t *sendfile;

	memset(buffer, 0, sizeof(buffer));

	memset(client->file, 0, sizeof(client->file));

	filesize = strlen(file);

	if (filesize > sizeof(client->file))
	{
		Com_Printf(VERBOSE_WARNING, "filename overflowed\n");
		return;
	}

	memcpy(client->file, file, filesize);

	sendfile = (sendfile1_t *)buffer;

	sendfile->packetid = htons(Com_WBhton(0x1603));
	sendfile->transaction = 0x01;
	sendfile->msgsize = filesize;
	memcpy(&(sendfile->msg), file, sendfile->msgsize);

	sendfile = (sendfile1_t *)(buffer+1+filesize);

	sendfile->msgsize = strlen(clifile);

	//	if(wb3->value)
	//	{
	memcpy(&(sendfile->msg), clifile, sendfile->msgsize);
	//	}
	//	else
	//	{
	//memcpy(&(sendfile->msg), "DISPLAY.TXT", sendfile->msgsize);
	//	}

	sendfile = (sendfile1_t *)(buffer+filesize+sendfile->msgsize);

	sendfile->unknown2 = htonl(0x0A);
	sendfile->unknown3 = htonl(0x0200);

	PPrintf(client, RADIO_YELLOW, "Sending file");

	SendPacket(buffer, 13+sendfile->msgsize+filesize, client);
}

void SendFileSeq3(client_t *client)
{
	u_int8_t buffer[12];
	sendfile3_t *sendfile;
	FILE *fp;
	u_int32_t size;

	Sys_WaitForLock(strcat(client->file, ".LOCK"));

	if (Sys_LockFile(client->file) < 0)
	{
		Sys_UnlockFile(client->file);
		client->file[strlen(client->file)-5] = '\0';
		return;
	}

	client->file[strlen(client->file)-5] = '\0';

	if (!(fp = fopen(client->file, "r")))
	{
		Com_Printf(VERBOSE_ALWAYS, "Error opening file %s\n", client->file);
		Sys_UnlockFile(strcat(client->file, ".LOCK"));
		client->file[strlen(client->file)-5] = '\0';
		return;
	}
	else
	{
		//		for(size = 0; getc(fp) != EOF; size++)
		//			;

		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if (size <= 0)
		{
			Com_Printf(VERBOSE_ALWAYS, "Error getting size of file %s\n", client->file);
			return;
		}
	}

	fclose(fp);

	Sys_UnlockFile(strcat(client->file, ".LOCK"));
	client->file[strlen(client->file)-5] = '\0';

	memset(buffer, 0, sizeof(buffer));

	sendfile = (sendfile3_t *)buffer;

	sendfile->packetid = htons(Com_WBhton(0x1601));
	sendfile->transaction = 0x01;
	sendfile->time = htonl(arena->time);
	sendfile->filesize = htonl(size);

	// change this sequence to CeilDiv();
	client->fileframe = FloorDiv(size, 512) + 1;

	if (!(size % (512)))
	{
		client->fileframe--;
	}
	//

	SendPacket(buffer, sizeof(buffer), client);
}

void SendFileSeq5(u_int16_t seek, client_t *client)
{
	u_int8_t buffer[MAX_SENDDATA];
	u_int16_t size;
	FILE *fp;

	sendfile5_t *sendfile;

	if (seek >= client->fileframe)
	{
		if (!wb3->value || !client->loginkey)
			SendFileSeq7(client);
		return;
	}

	memset(buffer, 0, sizeof(buffer));

	sendfile = (sendfile5_t *)buffer;

	sendfile->packetid = htons(Com_WBhton(0x1605));
	sendfile->transaction = 0x01;
	sendfile->unknown1 = 0x01;
	sendfile->frame = htonl(seek);

	Sys_WaitForLock(strcat(client->file, ".LOCK"));
	if (Sys_LockFile(client->file) < 0)
	{
		Sys_UnlockFile(client->file);
		client->file[strlen(client->file)-5] = '\0';
		return;
	}
	client->file[strlen(client->file)-5] = '\0';

	if (!(fp = fopen(client->file, "r")))
	{
		Com_Printf(VERBOSE_ALWAYS, "Error opening file %s\n", client->file);
		Sys_UnlockFile(strcat(client->file, ".LOCK"));
		client->file[strlen(client->file)-5] = '\0';
		return;
	}

	if (fseek(fp, 512*seek, SEEK_SET))
	{
		Com_Printf(VERBOSE_WARNING, "Couldn't fseek() file \"%s\"\n", client->file);
		fclose(fp);

		Sys_UnlockFile(strcat(client->file, ".LOCK"));
		client->file[strlen(client->file)-5] = '\0';
		return;
	}

	sendfile->msgsize = htons(size=fread(&(sendfile->msg), 1, 512, fp));

	fclose(fp);

	Sys_UnlockFile(strcat(client->file, ".LOCK"));
	client->file[strlen(client->file)-5] = '\0';

	SendPacket(buffer, 10+size, client);
}

void SendFileSeq6(u_int8_t *buffer, client_t *client)
{
	sendfile6_t *sendfile;

	sendfile = (sendfile6_t *)buffer;

	SendFileSeq5(ntohl(sendfile->frame)+1, client);
}

void SendFileSeq7(client_t *client)
{
	u_int8_t buffer[16];

	sendfile7_t *sendfile;

	memset(buffer, 0, sizeof(buffer));

	sendfile = (sendfile7_t *)buffer;

	sendfile->packetid = htons(Com_WBhton(0x1607));
	sendfile->unknown1 = htons(0x01);
	sendfile->msgsize = 0x0B;

	memcpy(&(sendfile->msg), "DISPLAY.TXT", sendfile->msgsize);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 ProcessPacket

 Check packet ID and forward to its handler
 */

int ProcessPacket(u_int8_t *buffer, u_int16_t len, client_t *client)
{
	u_int8_t i;
	u_int16_t n;
	char file[128];
	FILE *fp;

	if(!client || !client->inuse)
		return 0;

	if (!client->drone)
	{
		if (client->loginkey)
		{
			if (debug->value)
			{
				Com_Printf(VERBOSE_DEBUG, "<<<--- ");
				Com_Printfhex(buffer, len+1);
			}

			wbcrypt(buffer, client->key, len, FALSE); // decrypting message
		}
		else
		{
			wbcrypt(buffer, client->key, len, TRUE); // decrypting message
		}

		if (debug->value)
		{
			Com_Printf(VERBOSE_DEBUG, "<<<--- ");
			Com_Printfhex(buffer, len+1);
		}
	}

	if ((buffer[len] == CheckSum(buffer, len)) || client->drone) // lenth element == checksum
	{
		//		if(wb3->value && client->loginkey)
		//		{
		//			buffer[0] = Com_WB3ntoh(buffer[0]);
		//		}

		n = (buffer[0] * 0x100) | buffer[1];

		Com_WBntoh(&n);

		if (!client->drone)
		{
			if (n != 0x0E00) // TODO: Bug: THAI still being kicked
				client->awaytimer = 0;
		}

		switch (n)
		{
			case 0x002C:
				if (!setjmp(debug_buffer))
				{
					WB3ClientSkin(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0402:
				if (!setjmp(debug_buffer))
				{
					PEndFlight(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0406:
				if(!setjmp(debug_buffer))
				{
					if(debug->value)
					{
						PPrintf(client, RADIO_RED, "DEBUG: 0x0406: Request Available list (%d)", buffer[2]); // TODO: Otto: fly as otto feature
					}
					PReqBomberList(client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0408:
				if(!setjmp(debug_buffer))
				{
					PSquadInfo(NULL, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x040C:
				if(!setjmp(debug_buffer))
				{
					PSquadLookup(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x040D:
				if(!setjmp(debug_buffer))
				{
					PClientMedals(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0410:
				if(!setjmp(debug_buffer))
				{
					if(debug->value)
					{
						PPrintf(client, RADIO_RED, "DEBUG: 0x0410: Ask for Request/Binded list (0x%0X%0X)", buffer[2], buffer[3]);
					}

					if(client->gunnerview == client || !client->gunnerview)
					{
						SendAttachList(NULL, client);
					}
					else
					{
						reqgunnerlist_t *gunner;
						gunner = (reqgunnerlist_t *) buffer;
						gunner->unknown1 = htons(0x0006);
						gunner->nick = htonl(client->gunnerview->shortnick);
						SendAttachList(buffer, client);
					}
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0412:
				if(!setjmp(debug_buffer))
				{
					SendAttachList(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x041A:
				if(!setjmp(debug_buffer))
				{
					PCurrentView(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x041C: //wb3 start flight
				if(!setjmp(debug_buffer))
				{
					WB3RequestStartFly(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0420: //wb3 start ack
				if(!setjmp(debug_buffer))
				{
					WB3RequestMannedAck(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0309:
				if(!setjmp(debug_buffer))
				{
					PingTest(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break; // FIXME
			case 0x0310:
				if(!setjmp(debug_buffer))
				{
				PPrintf(client, RADIO_GREEN, "DEBUG: arnaCONFIG_FLIGHTMODEL()");
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break; // FIXME
			case 0x1406:
				if(!setjmp(debug_buffer))
				{
				PHostVar(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break; // THAI watchdog
			case 0x1D04:
				if(!setjmp(debug_buffer))
				{
					THAIWatchDog(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1D05:
				if(!setjmp(debug_buffer))
				{
					PRequestGunner(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1D06:
				if(!setjmp(debug_buffer))
				{
					PAcceptGunner(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1D07:
				if(!setjmp(debug_buffer))
				{
					if(client->attached)
					PSwitchOttoPos(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1D0C:
				if(!setjmp(debug_buffer))
				{
					Cmd_Shanghai(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1D13:
				if(!setjmp(debug_buffer))
				{
					ClientHDSerial(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0D01:
				if(!setjmp(debug_buffer))
				{
					client->ready = 1;

					if(!strlen(client->longnick))
					{
						PPrintf(client, RADIO_RED, "ERROR: This account has no registered callsign");
						PPrintf(client, RADIO_RED, "If this is a new account, please login using");
						PPrintf(client, RADIO_RED, "warbirds to register a callsign");

						Com_Printf(VERBOSE_WARNING, "Client tried to login without a callsign\n");
						RemoveClient(client);
						return 0;
					}

					//SendArenaRules(client);
					if(wb3->value)
					{
						WB3SendGruntConfig(client);
						WB3ArenaConfig2(client);
						if(((u_int8_t)weather->value == 2) && !client->rain)
							WB3DotCommand(client, ".weather 0"); // cloudy
						else
							WB3DotCommand(client, ".weather %u", (u_int8_t)weather->value);
						WB3DotCommand(client, ".date %u %u %u", arena->month, arena->day, arena->year);
						WB3DotCommand(client, ".weathereffects 1");
						WB3NWAttachSlot(client);
					}

					SendOttoParams(client);
					//if(!wb3->value)
					SendExecutablesCheck(1, client);
					SendLastConfig(client);

					snprintf(file, sizeof(file), "motd.txt");

					if ((fp = fopen(file, "r")) == NULL)
					{
						// Couldn't open general motd.txt, try arena motd
						snprintf(file, sizeof(file), "./arenas/%s/motd.txt", dirname->string);

						if ((fp = fopen(file, "r")) == NULL)
						{
							// Couldn't even open arena motd, print error
							Com_Printf(VERBOSE_WARNING, "Couldn't open \"%s\"\n", file);
							memset(file, 0, sizeof(file));
						}
						else
						{
							fclose(fp);
						}
					}
					else
					{
						fclose(fp);
					}

					if(strlen(file))
						SendFileSeq1(file, "motd.txt", client);

					PPrintf(client, RADIO_YELLOW, "Tabajara Host version %s, build %s %s", VERSION, __DATE__, __TIME__);

					if(!(client->attr == 1 && hideadmin->value))
					{
						if(client->loginkey == 1)
							BPrintf(RADIO_YELLOW, "%s entered the game using THChat", client->longnick);
						else
							BPrintf(RADIO_YELLOW, "%s entered the game", client->longnick);

						Com_LogEvent(EVENT_LOGIN, client->id, 0);
						Com_LogDescription(EVENT_DESC_PLIP, 0, client->ip);
						Com_LogDescription(EVENT_DESC_PLCTRID, client->ctrid, 0);

						for(i = 0; i < maxentities->value; i++)
						{
							if(clients[i].inuse && !clients[i].drone && clients[i].ready && client != &clients[i])
							{
								if(!strcmp(clients[i].ip, client->ip))
								{
									Com_Printf(VERBOSE_ALWAYS, "%s and %s are sharing the same IP (%s)\n", client->longnick, clients[i].longnick, client->ip);
									BPrintf(RADIO_YELLOW, "%s and %s are sharing the same IP (%s)\n", client->longnick, clients[i].longnick, client->ip);
									break;
								}
							}
						}
					}

					ClientIpaddr(client);
					CheckTK(client);

					sprintf(my_query, "INSERT INTO online_players SET player_id = '%u', country = '%u', login_time = FROM_UNIXTIME(%u)", client->id, client->country, (u_int32_t)time(NULL));

					if(d_mysql_query(&my_sock, my_query)) // query succeeded

					{
						if(mysql_errno(&my_sock) != 1062)
						Com_Printf(VERBOSE_WARNING, "LOGIN: couldn't query INSERT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0D02:
				if(!setjmp(debug_buffer))
				{
					SendPacket(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0D04:
				if(!setjmp(debug_buffer))
				{
					Com_Printf(VERBOSE_DEBUG, "Printing request disco packet:\n");
					Com_Printfhex(buffer, len);
					client->disconnect = 1;
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0E00:
				if(!setjmp(debug_buffer))
				{
					PPlanePosition(buffer, client, 0);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0E01:
				if(!setjmp(debug_buffer))
				{
					if(client->infly)
					PPlaneStatus(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0E04:
				if(!setjmp(debug_buffer))
				{
					PPlanePosition(buffer, client, 1);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0E05:
				if(!setjmp(debug_buffer))
				{
					PChutePos(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0E07:
				if(!setjmp(debug_buffer))
				{
					WB3GunnerUpdate(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0E0E:
				if(!setjmp(debug_buffer))
				{
					WB3FireSuppression(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x0E0F:
				if(!setjmp(debug_buffer))
				{
					WB3ExternalAmmoCnt(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x2101:
				if(!setjmp(debug_buffer))
				{
					POttoFiring(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1200:
				if(!setjmp(debug_buffer))
				{
					PRadioMessage(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1201:
				if(!setjmp(debug_buffer))
				{
					PSetRadioChannel(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1B01:
				if(!setjmp(debug_buffer))
				{
					PFileCheck(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x2000:
				if(!setjmp(debug_buffer))
				{
					PNewNick(buffer+2, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1900:
				if(!setjmp(debug_buffer))
				{
					PDropItem(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1902:
				if(!setjmp(debug_buffer))
				{
					// TODO: FIXME: are you sure it's only flak hit?
					PFlakHit(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1903:
				if(!setjmp(debug_buffer))
				{
					PHitStructure(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1904:
				if(!setjmp(debug_buffer))
				{
					PHardHitPlane(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1905:
				if(!setjmp(debug_buffer))
				{
					PHardHitStructure(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1906:
				if(!setjmp(debug_buffer))
				{
					PRemoveDropItem(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1909:
				if(!setjmp(debug_buffer))
				{
					PFireMG(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x190B:
				if(!setjmp(debug_buffer))
				{
					if(client->infly)
					PHitPlane(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x190C:
				if(!setjmp(debug_buffer))
				{
					PRemoveDropItem(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x190F: // wpOTTOHIT_SCORED_ON_BADGUY2
				if(!setjmp(debug_buffer))
				{
					if(client->infly)
					PHitPlane(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1913:
				if(!setjmp(debug_buffer))
				{
					WB3TonnageOnTarget(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1915:
				if(!setjmp(debug_buffer))
				{
					PFireMG(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1916:
				if(!setjmp(debug_buffer))
				{
					WB3FuelConsumed(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1917:
				if(!setjmp(debug_buffer))
				{
					WB3DelayedFuse(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1918:
				if(!setjmp(debug_buffer))
				{
					PDropItem(buffer, len, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1600:
				if(!setjmp(debug_buffer))
				{
					if(client->ready)
					SendFileSeq3(client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1602:
				if(!setjmp(debug_buffer))
				{
					if(client->ready)
						SendFileSeq5(0, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x1606:
				if(!setjmp(debug_buffer))
				{
					if(client->ready)
					SendFileSeq6(buffer, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			case 0x160A:
				if(!setjmp(debug_buffer))
				{
					if(debug->value)
					Com_Printf(VERBOSE_DEBUG, "160A\n");
					memset(buffer+2, 0, 8);
					buffer[1] = 0x0B; // FRANZ verificar WB2008
					SendPacket(buffer, 10, client);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
			default:
				if(!setjmp(debug_buffer))
				{
					if(client->ready && client->attr & (FLAG_ADMIN | FLAG_OP))
					PPrintf(client, RADIO_LIGHTYELLOW, "Unknown packet %04X, WB %d", n, (u_int8_t)wb3->value);
					Com_Printf(VERBOSE_WARNING, "Unknown packet %04X, WB %d", n, (u_int8_t)wb3->value);
					Com_Printfhex(buffer, len);
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
				}
				break;
		}
	}
	else
	{
		Com_Printf(VERBOSE_WARNING, "%s(%s) invalid checksum\n", client->longnick, client->ip);
		return -1;
	}
	return 0;
}

/**
 PingTest

 Calculate PingTest response
 */

void PingTest(u_int8_t *buffer, client_t *client)
{
	pingtest_t *pingtest;
	u_int16_t frame;

	pingtest = (pingtest_t *)buffer;
	frame = htons(pingtest->frame);

	if ((++frame) < 12)
	{
		Cmd_Pingtest(frame, client);
	}
	else
	{
		PPrintf(client, RADIO_RED, "Ping Average: %ums", ((arena->time - client->pingtest) / 24));
	}
}

/**
 PReqBomberList

 Send a list of all players that are with buffer and still in tower
 */

void PReqBomberList(client_t *client)
{
	u_int8_t buffer[MAX_SENDDATA];
	u_int8_t i, j;
	reqbomberlist_t *list;

	memset(buffer, 0, sizeof(buffer));

	list = (reqbomberlist_t *)buffer;

	list->packetid = htons(Com_WBhton(0x0407));
	list->unknown1 = 3;

	for (j = 0, i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && clients[i].ready && !clients[i].infly && HaveGunner(clients[i].plane) && clients[i].country == client->country)
		{
			list = (reqbomberlist_t *)(buffer+(4*j));
			list->nick = htonl(clients[i].shortnick);
			j++;
		}
	}

	list = (reqbomberlist_t *)buffer;
	list->numofnicks = j;

	if (debug->value)
		PPrintf(client, RADIO_RED, "DEBUG: 0x0407: Available list (%d players)", j);

	SendPacket(buffer, (4*j)+4, client);
}

/**
 PEndFlight

 Process the end of flight of a client
 */

void PEndFlight(u_int8_t *buffer, u_int16_t len, client_t *client)
{
	char field[8];
	u_int16_t i, j, gunused, totalhits;
	int8_t land = 0;
	u_int16_t end = 0;
	u_int32_t dist = 0;
	endflight_t *endflight = 0;
	client_t *nearplane;

	if (!setjmp(debug_buffer))
	{
		endflight = (endflight_t *)buffer;

		end = ntohs(endflight->type);
		gunused = ntohs(endflight->gunused);
		land = ntohs(endflight->field);

		if (end != 0x01)
		{
			land = NearestField(client->posxy[0][0], client->posxy[1][0], 0, FALSE, TRUE, &dist);

			if (land < 0)
			{
				land = FirstFieldCountry(client->country);

				if (land < 0)
				{
					land = 1;
				}
				else
				{
					land += 1;
				}
			}
			else
			{
				land += 1;
			}

			if (land <= fields->value && dist > arena->fields[land-1].radius) // no fields found, or too far
			{
				if (arena->fields[client->field - 1].country == client->country)
				{
					snprintf(field, sizeof(field), "f%d", client->field);
				}
				else
				{
					land = FirstFieldCountry(client->country);

					if (land < 0)
					{
						land = 1;
					}
					else
					{
						land += 1;
					}

					snprintf(field, sizeof(field), "f%d", land);
				}

				land = 0;
			}
			else // found a field near and is in boundaries
			{
				snprintf(field, sizeof(field), "f%d", land);
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	if (!setjmp(debug_buffer))
	{
		totalhits = client->hits[0] + client->hits[1] + client->hits[2] + client->hits[3] + client->hits[4] + client->hits[5];

		if (totalhits && !gunused)
			gunused = totalhits * 2;

		if (!client->attached)
		{
			if (client->attr == 2)
				PPrintf(client, RADIO_RED, "DEBUG: end flight code %d", end);

			switch (end)
			{
				case ENDFLIGHT_LANDED:
					Com_LogEvent(EVENT_LAND, client->id, 0);
					Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
					Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);

					if (land)
					{
						snprintf(field, sizeof(field), "f%d", land);

						Com_LogDescription(EVENT_DESC_FIELD, land, NULL);

						Com_Printf(VERBOSE_ALWAYS, "%s landed at %s\n", client->longnick, field);
						PPrintf(client, RADIO_YELLOW, "%s landed %s", client->longnick, field);

						if (landingcapture->value && IsBomber(client))
						{
							if ((arena->fields[land - 1].country != client->country) && arena->fields[land - 1].abletocapture && arena->fields[land - 1].closed)
							{
								i = 1;

								for (j = 0; j < MAX_RELATED; j++) // dont capture if bomber has wingmen
								{
									if (client->related[j] && (client->related[j]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
									{
										i = 0;
										break;
									}
								}

								// dont capture field if plane is damaged
								if (client->status_damage & (STATUS_RWING | STATUS_LWING | STATUS_CENTERFUSE | STATUS_REARFUSE | STATUS_LGEAR | STATUS_RGEAR))
								{
									i = 0;
								}

								for (j = 0; j < MAX_BUILDINGS; j++)
								{
									if (arena->fields[land - 1].buildings[j].field)
									{
										if (arena->fields[land - 1].buildings[j].status)
										{
											if (arena->fields[land - 1].buildings[j].type == BUILD_TOWER)
											{
												PPrintf(client, RADIO_RED, "Distance %d", DistBetween(client->posxy[0][0], client->posxy[1][0], 0, arena->fields[land - 1].buildings[j].posx, arena->fields[land - 1].buildings[j].posy, 0, 1000));

												j = DistBetween(client->posxy[0][0], client->posxy[1][0], 0, arena->fields[land - 1].buildings[j].posx, arena->fields[land - 1].buildings[j].posy, 0, 1000);

												if(j > 300 /* D1 */)
													i = 0;

												break;
											}
										}
									}
									else
										break;
								}

								if (i)
								{
									CaptureField(land, client);
								}
							}
						}
					}
					else
					{
						land = NearestField(client->posxy[0][0], client->posxy[1][0], 0, FALSE, FALSE, NULL);

						if (land < 0)
						{
							land = FirstFieldCountry(client->country);

							if (land < 0)
							{
								land = 1;
							}
							else
							{
								land += 1;
							}
						}
						else
						{
							land += 1;
						}

						snprintf(field, sizeof(field), "f%d", land);

						Com_Printf(VERBOSE_ALWAYS, "%s landed out of runway at %s\n", client->longnick, field);
						PPrintf(client, RADIO_YELLOW, "%s landed out of %s runway", client->longnick, field);
					}
					break;
				case ENDFLIGHT_PILOTKILL:
					client->status_damage |= STATUS_PILOT;
					Com_Printf(VERBOSE_ALWAYS, "%s is killed in flight at %s\n", client->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
					PPrintf(client, RADIO_YELLOW, "%s is killed in flight", client->longnick);
					Kamikase(client);
					break;
				case ENDFLIGHT_CRASHED:
					if (client->chute)
					{
						Com_Printf(VERBOSE_ALWAYS, "%s's plane crashed at %s\n", client->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
						PPrintf(client, RADIO_YELLOW, "%s's plane crashed", client->longnick);
					}
					else
					{
						Com_Printf(VERBOSE_ALWAYS, "%s crashed at %s\n", client->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
						PPrintf(client, RADIO_YELLOW, "%s crashed", client->longnick);
					}

					Kamikase(client);

					if (client->chute)
						client->infly = 0;
					break;
				case ENDFLIGHT_DITCHFAILED:
					Com_Printf(VERBOSE_ALWAYS, "%s failed to ditch at %s\n", client->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
					PPrintf(client, RADIO_YELLOW, "%s failed to ditch", client->longnick);
					break;
				case ENDFLIGHT_BAILED:
					Com_Printf(VERBOSE_ALWAYS, "%s sucessfully bailed at %s\n", client->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));

					Com_LogEvent(EVENT_BAIL, client->id, 0);
					Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
					Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
					PPrintf(client, RADIO_YELLOW, "%s bailed", client->longnick);
					break;
				case ENDFLIGHT_ZERO:
					Com_Printf(VERBOSE_WARNING, "PEndFlight(zero) %s\n", client->longnick);
					PPrintf(client, RADIO_GREEN, "End Flight ZERO, please report your crash/ditch to admin");
				case ENDFLIGHT_DITCHED:
					Com_Printf(VERBOSE_ALWAYS, "%s ditched at %s\n", client->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));

					Com_LogEvent(EVENT_DITCH, client->id, 0);
					Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
					Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
					PPrintf(client, RADIO_YELLOW, "%s ditched", client->longnick);
					break;
				case ENDFLIGHT_COLLIDED:
					PPrintf(client, RADIO_YELLOW, "%s collided", client->longnick);

					client->cancollide = -1;

					if (!emulatecollision->value || arcade->value)
					{
						Com_Printf(VERBOSE_ALWAYS, "%s collided at %s\n", client->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));

						Com_LogEvent(EVENT_COLLIDED, client->id, 0);
						Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
						Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);

						client->damaged = 1;
					}
					else
					{
						nearplane = NearPlane(client, client->country, 300);

						if (nearplane)
						{
							Com_Printf(VERBOSE_ALWAYS, "%s collided with %s at %s\n", client->longnick, nearplane->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));

							Com_LogEvent(EVENT_COLLIDED, client->id, nearplane->id);
							Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
							Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
							Com_LogDescription(EVENT_DESC_VCPLANE, nearplane->plane, NULL);
							Com_LogDescription(EVENT_DESC_VCCTRY, nearplane->country, NULL);

							if (!nearplane->drone) // TODO: collision: Score fix
							{
								PPrintf(nearplane, RADIO_DARKGREEN, "%s collided with you!!!", client->longnick);
								/*
								if (IsFighter(client))
								{
									sprintf(my_query, "UPDATE score_fighter");
								}
								if (IsBomber(client))
								{
									sprintf(my_query, "UPDATE score_bomber");
								}
								else if (IsGround(client))
								{
									sprintf(my_query, "UPDATE score_ground");
								}
								else
								{
									Com_Printf(VERBOSE_WARNING, "Plane not classified (N%d)\n", client->plane);
									sprintf(my_query, "UPDATE score_fighter");
								}

								sprintf(my_query, "%s SET collided = collided + '1' WHERE player_id = '%u'", my_query, nearplane->id);

								if (d_mysql_query(&my_sock, my_query)) // query succeeded
								{
									Com_Printf(VERBOSE_WARNING, "Collided(near): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
								}
								*/
							}

							nearplane->cancollide = -1;

							nearplane->hitby[0].dbid = client->id;
							nearplane->hitby[0].damage = (double)MAX_UINT32;  // TODO: Score: collision: change this
							client->hitby[0].dbid = nearplane->id;
							client->hitby[0].damage = (double)MAX_UINT32;  // TODO: Score: collision: change this

							client->damaged = 1;
							nearplane->damaged = 1;

							if (rand()%2)
								SendForceStatus(STATUS_LWING, nearplane->status_status, nearplane);
							else if (rand()%2)
								SendForceStatus(STATUS_RWING, nearplane->status_status, nearplane);
							else
								SendForceStatus(STATUS_REARFUSE, nearplane->status_status, nearplane);
						}
					}
					break;
				case ENDFLIGHT_PANCAKE:
					Com_Printf(VERBOSE_ALWAYS, "%s became a pancake at %s\n", client->longnick, land ? field : Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
					PPrintf(client, RADIO_YELLOW, "%s became pancake", client->longnick);
					break;
				default:
					Com_Printf(VERBOSE_WARNING, "%s(%s) PEndFlight() type unknown %d\n", client->longnick, client->ip, end);
					break;
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	if (!setjmp(debug_buffer))
	{
		for (i = 0; i < MAX_RELATED; i++)
		{
			if (client->related[i])
			{
				if (client->related[i]->drone & (DRONE_WINGS1 | DRONE_WINGS2 | DRONE_HMACK | DRONE_HTANK | DRONE_EJECTED))
				{
					RemoveDrone(client->related[i]);
				}
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	if (!setjmp(debug_buffer))
	{
		if (!client->chute || (client->chute && ((end == ENDFLIGHT_PILOTKILL) || (end == ENDFLIGHT_BAILED) || (end == ENDFLIGHT_PANCAKE))))
		{
			ScoresEndFlight(end, land, gunused, totalhits, client);

			if (land && end == ENDFLIGHT_LANDED && arena->fields[land - 1].rps[client->plane] > -1 && client->plane < maxplanes && !client->tkstatus && !(client->plane >= 131 && client->plane <= 134))
				arena->fields[land - 1].rps[client->plane]++;

			if (!client->attached)
			{
				if (HaveGunner(client->plane)) // ends gunners flight
				{
					for (i = 0; i < 7; i++)
					{
						if (client->gunners[i])
						{
							if (client->gunners[i]->attached == client)
							{
								PEndFlight(buffer, len, client->gunners[i]);
							}
						}
					}
				}

				if (client->shanghai) // ends shanghai flight
				{
					if (client->shanghai->attached == client)
						PEndFlight(buffer, len, client->shanghai);
				}

				if (client->view) // ends view flight
				{
					if (client->view->attached == client)
						PEndFlight(buffer, len, client->view);
				}
			}

			client->score.costscore = client->score.airscore = client->score.groundscore = client->score.captscore = client->score.rescuescore = client->killssortie = client->status_damage = client->status_status = client->infly
					= client->hits[0] = client->hits[1] = client->hits[2] = client->hits[3] = client->hits[4] = client->hits[5] = client->hitstaken[0] = client->hitstaken[1] = client->hitstaken[2]
					= client->hitstaken[3] = client->hitstaken[4] = client->hitstaken[5] = client->chute = client->obradar = client->mortars = client->cancollide = client->fueltimer
					= client->score.penaltyscore = client->commandos = client->damaged = client->conn_sum = client->conn_count = client->conn_curravg = client->conn_lastavg = 0;

			memset(client->skin, 0, sizeof(client->skin));

			ClearKillers(client); // TODO: Clear Killers, for debug only

			client->view = client->shanghai = client->attached = NULL;
			client->speedxyz[0][0] = client->speedxyz[1][0] = client->speedxyz[2][0] = 0;
			UpdateIngameClients(0);

			SendPacket(buffer, len, client);
			SendGunnerStatusChange(client, 0, client);
			SendAttachList(NULL, client); // TODO: TODO: FIXME: send actual list or emptylist?

			if (land && end != 0x01)
			{
				if (arena->fields[client->field - 1].country == client->country)
				{
					snprintf(field, sizeof(field), "f%d", client->field);
				}
				else
				{
					land = FirstFieldCountry(client->country);

					if (land < 0)
					{
						land = 1;
					}
					else
					{
						land += 1;
					}

					snprintf(field, sizeof(field), "f%d", land);
				}
			}
			Cmd_Move(field, client->country, client);
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}
}

/**
 PPlanePosition

 Fill client structure with current position
 */

void PPlanePosition(u_int8_t *buffer, client_t *client, u_int8_t attached)
{
	u_int8_t i;
	planeposition_t *plane;
	wb3planeposition_t *wb3plane;
	planeposition2_t *plane2;
	int32_t field;
	u_int32_t distance, oldpostimer, basetimer;
	int16_t clientoffset;
	int16_t posoffset;
	double conn_avgdiff;
	client_t *near = NULL;

	if (wb3->value)
	{
		wb3plane = (wb3planeposition_t *) buffer;
	}
	else
	{
		plane = (planeposition_t *) buffer;
	}

	oldpostimer = client->postimer;
	client->postimer = arena->time; // set the time when last position packet has been received

	if (attached)
	{
		plane2 = (planeposition2_t *) buffer;
		clientoffset = client->clienttimer - ntohl(plane2->timer);
		client->clienttimer = ntohl(plane2->timer);

		if(oldpostimer == client->postimer) // if this packet was received in bolus
			client->offset += clientoffset;
		else
			client->offset = clientoffset;
		//		client->atradar = ntohs(0x10);

		/*		if(client->attached && client->view)
		 {
		 if(client->attached->infly)
		 {
		 client->posxy[0][0] = client->attached->posxy[0][0];
		 client->posxy[1][0] = client->attached->posxy[1][0];
		 client->posalt[0] = client->attached->posalt[0];
		 SendDronePos(client->attached, client);
		 }
		 }
		 */
	}
	else// if(!client->chute)
	{
		BackupPosition(client, FALSE);

		if (wb3->value)
		{
			if (!client->mapper)
			{
				client->posxy[0][0] = ntohl(wb3plane->posx);
				client->posxy[1][0] = ntohl(wb3plane->posy);
			}
			client->posalt[0] = ntohl(wb3plane->alt);
			//		client->atradar = ntohs(wb3plane->radar);
			if (!client->predict)
			{
				clientoffset = client->clienttimer - ntohl(wb3plane->timer);
				client->clienttimer = ntohl(wb3plane->timer);

				if(oldpostimer == client->postimer) // if this packet was received in bolus
					client->offset += clientoffset;
				else
					client->offset = clientoffset;

				if(client->cancollide && !arena->overload)
				{
					if(clientoffset < -600)
					{
						near = NearPlane(client, client->country, planerangelimit->value);
						Com_Printf(VERBOSE_DEBUG, "%s possible client-side overload (offset = %d) %s\n", client->longnick, clientoffset, near?"enemy near":"");
					}

					if(client->postimer != oldpostimer) // not received in bolus
					{
						posoffset = (client->postimer - oldpostimer + client->offset);
						posoffset = MODULUS(posoffset);

						client->conn_sum += posoffset;
						client->conn_count++;

						if(client->conn_count >= 40) // each 15 seconds in flight
						{
							if(!client->conn_lastavg)
							{
								client->conn_lastavg = (double)client->conn_sum / client->conn_count;
							}
							else
							{
								client->conn_lastavg = 0.5 * client->conn_lastavg + 0.5 * (double)client->conn_sum / client->conn_count;
							}

							client->conn_sum = 10 * client->conn_lastavg;
							client->conn_count = 10;

							// if(client->conn_curravg)
							// {
							// 	conn_avgdiff = client->conn_curravg - client->conn_lastavg;
							// 	Com_Printf(VERBOSE_DEBUG, "%s avgdiff %.3f\n", client->longnick, conn_avgdiff);
							// 	//if(MODULUS(conn_avgdiff) > 100)
							// }

							client->conn_curravg = (double)client->conn_sum / client->conn_count;
							// Com_Printf(VERBOSE_DEBUG, "%s current avg: %.3f\n", client->longnick, client->conn_curravg);

							if(client->conn_curravg > 175)
							{
								client->connection = 3; // poor
							}
							else if(client->conn_curravg > 125)
							{
								client->connection = 2; // unstable
							}
							else if(client->conn_curravg > 75)
							{
								client->connection = 1; // fair
							}
							else
								client->connection = 0; // stable
							UpdateIngameClients(0);
						}
					}
				}
			}
		}
		else
		{
			client->posxy[0][0] = ntohl(plane->posx);
			client->posxy[1][0] = ntohl(plane->posy);
			client->posalt[0] = ntohl(plane->alt);
			//		client->atradar = ntohs(plane->radar);
			if (!client->predict)
			{
				clientoffset = client->clienttimer - ntohl(plane->timer);
				client->clienttimer =  ntohl(plane->timer);

				if(oldpostimer == client->postimer) // if this packet was received in bolus
					client->offset += clientoffset;
				else
					client->offset = clientoffset;
			}
		}

		i = 0;

		if (client->predict)
		{
			if (client->predict > 5)
				i = 5;
			else
				i = client->predict;

			client->predict--;
		}

		if (client->infly)
		{
			if (wb3->value)
			{
				client->speedxyz[0][i] = ntohs(wb3plane->xspeed);
				client->speedxyz[1][i] = ntohs(wb3plane->yspeed);
				client->speedxyz[2][i] = ntohs(wb3plane->climbspeed);
				client->accelxyz[0][i] = ntohs(wb3plane->sideaccel);
				client->accelxyz[1][i] = ntohs(wb3plane->forwardaccel);
				client->accelxyz[2][i] = ntohs(wb3plane->climbaccel);
				client->angles[0][i] = ntohs(wb3plane->pitchangle);
				client->angles[1][i] = ntohs(wb3plane->rollangle);
				client->angles[2][i] = ntohs(wb3plane->yawangle);
				client->aspeeds[0][i] = ntohs(wb3plane->pitchangspeed);
				client->aspeeds[1][i] = ntohs(wb3plane->rollangspeed);
				client->aspeeds[2][i] = ntohs(wb3plane->yawangspeed);
			}
			else
			{
				client->speedxyz[0][i] = ntohs(plane->xspeed);
				client->speedxyz[1][i] = ntohs(plane->yspeed);
				client->speedxyz[2][i] = ntohs(plane->climbspeed);
				client->accelxyz[0][i] = ntohs(plane->sideaccel);
				client->accelxyz[1][i] = ntohs(plane->forwardaccel);
				client->accelxyz[2][i] = ntohs(plane->climbaccel);
				client->angles[0][i] = ntohs(plane->pitchangle);
				client->angles[1][i] = ntohs(plane->rollangle);
				client->angles[2][i] = ntohs(plane->yawangle);
				client->aspeeds[0][i] = ntohs(plane->pitchangspeed);
				client->aspeeds[1][i] = ntohs(plane->rollangspeed);
				client->aspeeds[2][i] = ntohs(plane->yawangspeed);
			}

			if ((client->lograwdata || lograwposition->value) && client->infly)
			{
				LogRAWPosition(FALSE, client);
			}
		}
		else
		{
			client->speedxyz[0][0] = 0;
			client->speedxyz[1][0] = 0;
			client->speedxyz[2][0] = 0;
			client->accelxyz[0][0] = 0;
			client->accelxyz[1][0] = 0;
			client->accelxyz[2][0] = 0;
			client->angles[0][0] = 0;
			client->angles[1][0] = 0;
			client->angles[2][0] = 0;
			client->aspeeds[0][0] = 0;
			client->aspeeds[1][0] = 0;
			client->aspeeds[2][0] = 0;
		}

		if (!wb3->value)
		{
			if ((arena->hour >= 6 && arena->hour <= 18) && (client->posalt[0] > contrail->value)) // allow contrail during day
			{
				client->status_damage |= (STATUS_LFUEL | STATUS_RFUEL);

				if (!client->contrail)
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "You are in contrail area");
					client->contrail = 1;
					PPlaneStatus(NULL, client);
				}
			}
			else // remove contrail if night
			{ // TODO: WB2: remove fuel leak at night
				if (client->armor.points[PLACE_LFUEL] > 0) // 0x4000
				{
					client->status_damage &= 0xFFFFBFFF;
				}

				if (client->armor.points[PLACE_RFUEL] > 0) // 0x8000
				{
					client->status_damage &= 0xFFFF7FFF;
				}

				if (client->contrail)
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "Your contrail is not visible anymore");
					client->contrail = 0;
					PPlaneStatus(NULL, client);
				}
			}
		}

		// ackstar rules
		if(ackstardisable->value)
		{
			if (client->infly && ((client->posalt[0] - GetHeightAt(client->posxy[0][0], client->posxy[1][0])) < 1000) && IsBomber(client))
			{
				if (client->ackstarcount > 4)
				{
					for (i = 0; i < MAX_RELATED; i++)
					{
						if (client->related[i] && (client->related[i]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
						{
							field = NearestField(client->posxy[0][0], client->posxy[1][0], (client->country == 1) ? 3 : 1, FALSE, FALSE, &distance);

							if (field >= 0 && field < fields->value && distance < arena->fields[field].radius)
							{
								if (!client->ackstar)
								{
									client->ackstar = 1;
									SendOttoParams(client);
									PPrintf(client, RADIO_YELLOW, "Ackstar rules applied, your ottos are Safe!");
								}
							}
							else
							{
								if (client->ackstar)
								{
									client->ackstar = 0;
									SendOttoParams(client);
									PPrintf(client, RADIO_YELLOW, "Ackstar rules removed, your ottos are Hot!");
								}
							}
							break;
						}
					}

					client->ackstarcount = 0;
				}
				else
					client->ackstarcount++;
			}
			else
			{
				if (client->ackstar)
				{
					client->ackstarcount = client->ackstar = 0;
					SendOttoParams(client);
					PPrintf(client, RADIO_YELLOW, "Ackstar rules removed, your ottos are Hot!");
				}
			}
		}

		if (client->infly && !client->attached && !IsGround(client))
		{
			if (!client->cancollide && !arcade->value)
			{
				if ((FLIGHT_TIME(client)/1000) > 10)
				{
					if ((client->posalt[0] - arena->fields[client->field - 1].posxyz[2]) > 15)
					{
						if(midairs->value)
						{
							if(!wb3->value)
							{
								PPrintf(client, RADIO_YELLOW, "Friendly collision enabled");
							}
							else
							{
								if(IsBomber(client) || IsCargo(client)) // TODO: Collision: this is a temporary fix to wings collision
									WB3DotCommand(client, ".midairs_frndly 1");
							}
						}

						client->cancollide = 1;
						if(IsFighter(client))
						{
							SendArenaRules(client);
							WB3DotCommand(client, ".handler toggleexternview");
							WB3DotCommand(client, ".handler toggleexternview");
						}

						client->dronetimer = arena->time; // stores time when client took off
					}
				}
			}

			if (((client->armor.points[PLACE_LWING] < arena->planedamage[client->plane].points[PLACE_LWING]) || (client->armor.points[PLACE_RWING]
					< arena->planedamage[client->plane].points[PLACE_RWING])))
				CheckMaxG(client);
		}
	}

	basetimer = arena->time - client->clienttimer;
	if(client->basetimer > basetimer)
	{
		client->basetimer -= 2;
	}
	if(client->basetimer < basetimer)
	{
		client->basetimer += 2;
	}

	if(MODULUS(basetimer - client->basetimer) > 500)
	{
		client->basediff++;

		if(client->basediff > 10)
		{
			client->basetimer = basetimer;
			client->basediff = 0;
		}
	}

	client->timer = client->clienttimer + client->basetimer;

	if (client->mapper)
	{
		if (!(client->mapper % 5))
			WB3MapTopography(client);
		client->mapper++;
	}

	if (client->infly)
	{
		if (HaveGunner(client->plane))
		{
			for (i = 0; i < 7; i++) // start gunners flight
			{
				if (client->gunners[i])
				{
					if (client->gunners[i]->attached == client)
					{
						client->gunners[i]->posxy[0][0] = client->posxy[0][0];
						client->gunners[i]->posxy[1][0] = client->posxy[1][0];
						client->gunners[i]->posalt[0] = client->posalt[0];
						SendDronePos(client, client->gunners[i]);
					}
				}
			}
		}

		if (client->shanghai)
		{
			if (client->shanghai->attached == client)
			{
				client->shanghai->posxy[0][0] = client->posxy[0][0];
				client->shanghai->posxy[1][0] = client->posxy[1][0];
				client->shanghai->posalt[0] = client->posalt[0];
				SendDronePos(client, client->shanghai);
			}
		}

		if (client->view)
		{
			if (client->view->attached == client)
			{
				client->view->posxy[0][0] = client->posxy[0][0];
				client->view->posxy[1][0] = client->posxy[1][0];
				client->view->posalt[0] = client->posalt[0];
				SendDronePos(client, client->view);
			}

		}
	}
}

/**
 CheckMaxG

 Check if plane has G overloaded
 */

void CheckMaxG(client_t *client)
{
	double g, maxg, ming;

	g = 0;

	if (!(client->status_damage & STATUS_LWING) || !(client->status_damage & STATUS_RWING))
	{
		g = ClientG(client);
	}

	if (!(client->status_damage & STATUS_LWING))
	{
		maxg = (double)(arena->planedamage[client->plane].positiveG * client->armor.points[PLACE_LWING] / arena->planedamage[client->plane].points[PLACE_LWING]) + 1.5;
		ming = (double)((arena->planedamage[client->plane].negativeG * client->armor.points[PLACE_LWING] / arena->planedamage[client->plane].points[PLACE_LWING]) * -1) - 1.5;

		//		PPrintf(client, RADIO_WHITE, "left min %.3f max %.3f", ming, maxg);

		if ((g > maxg) || (g < ming))
		{
			if (client->armor.points[PLACE_LWING] > 0)
				PPrintf(client, RADIO_DARKGREEN, "Your left wing blown off due G overload");
			SendForceStatus((client->status_damage | STATUS_LWING), client->status_status, client);
			client->damaged = 1;
		}
		else
		{
			if ((maxg - g) < gwarning->value || (g - ming) < gwarning->value)
			{
				if (client->armor.points[PLACE_LWING] > 0)
				{
					WB3DotCommand(client, ".playsoundfile sounds/gexceed.wav");
					//PPrintf(client, RADIO_DARKGREEN, "Warning! Your left wing is about to fall!");
				}
			}
		}
	}

	if (!(client->status_damage & STATUS_RWING))
	{
		maxg = (double)(arena->planedamage[client->plane].positiveG * client->armor.points[PLACE_RWING] / arena->planedamage[client->plane].points[PLACE_RWING]) + 1.5;
		ming = (double)((arena->planedamage[client->plane].negativeG * client->armor.points[PLACE_RWING] / arena->planedamage[client->plane].points[PLACE_RWING]) * -1) - 1.5;

		//		PPrintf(client, RADIO_WHITE, "right min %.3f max %.3f", ming, maxg);

		if ((g > maxg) || (g < ming))
		{
			if (client->armor.points[PLACE_RWING] > 0)
				PPrintf(client, RADIO_DARKGREEN, "Your right wing blown off due G overload");
			SendForceStatus((client->status_damage | STATUS_RWING), client->status_status, client);
			client->damaged = 1;
		}
		else
		{
			if ((maxg - g) < gwarning->value || (g - ming) < gwarning->value)
			{
				if (client->armor.points[PLACE_RWING] > 0)
				{
					WB3DotCommand(client, ".playsoundfile sounds/gexceed.wav");
					//PPrintf(client, RADIO_DARKGREEN, "Warning! Your right wing is about to fall!");
				}
			}
		}
	}
}

/**
 ClientG

 Calculates actual client's plane wings' G load
 */

double ClientG(client_t *client)
{
	double Ax, Ay, Az, g, pitch, roll, yaw, hyaw;

	pitch = (double)((double)client->angles[0][0] / 10);
	roll = (double)((double)client->angles[1][0] / 10);

	yaw = WBtoHdg(client->angles[2][0]);

	////////// Az
	Az = Com_Deg(acos(cos(Com_Rad(pitch)) * cos(Com_Rad(MODULUS(roll)))));
	///////////
	////////// Ay

//	PPrintf(client, RADIO_GREEN, "Pitch %.2f, Roll %.2f, Yaw %.2f", pitch, roll, yaw);
//	PPrintf(client, RADIO_RED, "AccX %d, AccY %d, Accz %d", client->accelxyz[0][0], client->accelxyz[1][0], client->accelxyz[2][0]);

	if(wb3->value)
		pitch *= -1;

	hyaw = yaw;

	if (yaw > 180)
		hyaw = 360 - yaw;

	// pitch influence
	if (pitch > 0)
		Ay = ((90 - hyaw) * 2) - Com_Deg(asin(sin(Com_Rad(90+pitch)) * cos(Com_Rad(hyaw))));
	else
		Ay = Com_Deg(asin(sin(Com_Rad(90+pitch)) * cos(Com_Rad(hyaw))));

	if (hyaw > 90)
	{
		Ay = MODULUS((90 - hyaw) * 2) + Ay;
	}

	if (MODULUS(roll)> 90)
	{
		Ay = MODULUS((90 - hyaw) * 2) - Ay;
	}
	///
	// roll influence

	if (roll > 0)
		if (hyaw > 90)
			g = ((180 - hyaw) * 2) - Com_Deg(asin(MODULUS(sin(Com_Rad(90+roll))) * sin(Com_Rad(hyaw))));
		else
			g = (hyaw * 2) - Com_Deg(asin(MODULUS(sin(Com_Rad(90+roll))) * sin(Com_Rad(hyaw))));
	else
		g = Com_Deg(asin(MODULUS(sin(Com_Rad(90+roll))) * sin(Com_Rad(hyaw))));

	if ((yaw > 180) && (yaw < 360))
	{
		if (hyaw > 90)
			g = ((180 - hyaw) * 2) - g;
		else
			g = (hyaw * 2) - g;
	}

	Ay += g;
	///////////
	////////// Ax
	hyaw = yaw + 90;

	if (hyaw >= 360)
		hyaw -= 360;

	yaw = hyaw;

	if (yaw > 180)
		hyaw = 360 - yaw;

	// pitch influence

	if (pitch > 0)
		Ax = ((90 - hyaw) * 2) - Com_Deg(asin(sin(Com_Rad(90+pitch)) * cos(Com_Rad(hyaw))));
	else
		Ax = Com_Deg(asin(sin(Com_Rad(90+pitch)) * cos(Com_Rad(hyaw))));

	if (hyaw > 90)
	{
		Ax = MODULUS((90 - hyaw) * 2) + Ax;
	}

	if (MODULUS(roll)> 90)
	{
		Ax = MODULUS((90 - hyaw) * 2) - Ax;
	}
	///
	// roll influence

	if (roll > 0)
		if (hyaw > 90)
			g = ((180 - hyaw) * 2) - Com_Deg(asin(MODULUS(sin(Com_Rad(90+roll))) * sin(Com_Rad(hyaw))));
		else
			g = (hyaw * 2) - Com_Deg(asin(MODULUS(sin(Com_Rad(90+roll))) * sin(Com_Rad(hyaw))));
	else
		g = Com_Deg(asin(MODULUS(sin(Com_Rad(90+roll))) * sin(Com_Rad(hyaw))));

	if ((yaw > 180) && (yaw < 360))
	{
		if (hyaw > 90)
			g = ((180 - hyaw) * 2) - g;
		else
			g = (hyaw * 2) - g;
	}

	Ax += g;

	///////////

	if(wb3->value)
		Ay = 180 - Ay;

	g = (((double)client->accelxyz[0][0]/31) * cos(Com_Rad(Ax))) + (((double)client->accelxyz[1][0]/31) * cos(Com_Rad(Ay))) + ((((double)client->accelxyz[2][0]/31) + 1) * cos(Com_Rad(Az)));

//	PPrintf(client, RADIO_WHITE, "Ax %.3f Ay %.3f Az %.3f G %.3f", Ax, Ay, Az, g);

	return g;
}

/**
 PChutePos

 Process chute position
 */

void PChutePos(u_int8_t *buffer, client_t *client)
{
	u_int8_t i, j, num;
	chutepos_t *chute;
	u_int32_t basetimer;

	chute = (chutepos_t *)buffer;

	if (client->infly)
	{
		if (!client->chute)
		{
			PPrintf(client, RADIO_YELLOW, "%s ejected", client->longnick);

			if (printeject->value)
			{
				BPrintf(RADIO_YELLOW, "%s ejected at %s",
				client->longnick,
				Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
			}
			else
			{
				Com_Printf(VERBOSE_ALWAYS, "%s ejected at %s\n", client->longnick, Com_Padloc(client->posxy[0][0], client->posxy[1][0]));
			}

			client->chute = 1;

			Com_LogEvent(EVENT_EJECT, client->id, 0);
			Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
			Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);

			if (!client->attached)
			{
				AddDrone(DRONE_EJECTED, client->posxy[0][0], client->posxy[1][0], client->posalt[0], client->country, client->plane, client);

				if (!wb3->value)
				{
					for (i = 0; i < maxentities->value; i++)
					{
						if (clients[i].inuse && !clients[i].drone)
						{
							for (j = 0; j < MAX_SCREEN; j++)
							{
								if (!clients[i].visible[j].client)
									continue;

								if (client == clients[i].visible[j].client)
								{
									AddRemovePlaneScreen(client, &clients[i], TRUE);
									num = client->country;
									client->country = 0; // neutral
									//client->status_damage = client->status_status = 0;
									AddRemovePlaneScreen(client, &clients[i], FALSE);
									client->country = num;
									break;
								}
							}
						}
					}
				}
			}
			else if (!client->attached->drone)
			{
				for (i = 0; i < 7; i++)
				{
					if (client->attached->gunners[i] == client)
					{
						client->attached->gunners[i] = NULL;
						break;
					}
				}

				if (client->attached->view == client)
					client->attached->view = NULL;

				client->attached = NULL;
			}

			/*			for(i = 0; i < MAX_RELATED; i++)
			 {
			 if(client->related[i])
			 if(client->related[i]->drone & (DRONE_WINGS1 | DRONE_WINGS2))
			 RemoveDrone(client->related[i]); // TODO: FIXME: don't remove drones, but make it related with DRONE_EJECTED
			 }
			 */
		}

		for (i = 0; i < MAX_RELATED; i++)
		{
			if (client->related[i])
				if (client->related[i]->drone & DRONE_EJECTED)
					break;
		}

		if (i < MAX_RELATED)
		{
			client->related[i]->posxy[0][0] = ntohl(chute->posx);
			client->related[i]->posxy[1][0] = ntohl(chute->posy);
			client->related[i]->posalt[0] = ntohl(chute->alt);
			client->related[i]->speedxyz[0][0] = client->related[i]->speedxyz[1][0] = client->related[i]->speedxyz[2][0] = client->related[i]->accelxyz[0][0] = client->related[i]->accelxyz[1][0]
					= client->related[i]->accelxyz[2][0] = client->related[i]->angles[0][0] = client->related[i]->angles[1][0] = client->related[i]->angles[2][0] = client->related[i]->aspeeds[0][0]
							= client->related[i]->aspeeds[1][0] = client->related[i]->aspeeds[2][0] = 0;

			client->related[i]->offset = client->clienttimer - ntohl(chute->timer);

			client->clienttimer = ntohl(chute->timer);

			basetimer = arena->time - client->clienttimer;
			if(client->basetimer > basetimer)
			{
				client->basetimer -= 2;
			}
			if(client->basetimer < basetimer)
			{
				client->basetimer += 2;
			}

			if(MODULUS(basetimer - client->basetimer) > 500)
			{
				client->basediff++;

				if(client->basediff > 10)
				{
					client->basetimer = basetimer;
					client->basediff = 0;
				}
			}

			client->timer = client->clienttimer + client->basetimer;
			client->related[i]->timer = client->timer;
		}
	}
}

/**
 WB3GunnerUpdate

 Process client gunner update
 */

void WB3GunnerUpdate(u_int8_t *buffer, client_t *client)
{
	u_int8_t packet[6];
	wb3gunnerupdate_t *gunnerupdate;
	wb3updategunner_t *updategunner;
	u_int8_t i, j;

	memset(packet, 0, sizeof(packet));

	gunnerupdate = (wb3gunnerupdate_t *)buffer;
	updategunner = (wb3updategunner_t *)packet;

	updategunner->packetid = htons(Com_WBhton(0x0007));
	updategunner->gun = ntohs(gunnerupdate->gun);
	updategunner->pitch = gunnerupdate->pitch;
	updategunner->yaw = gunnerupdate->yaw;

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{
					updategunner->slot = j;
					SendPacket(packet, sizeof(packet), &clients[i]);
					break;
				}
			}
		}
	}
}

/**
 PPlaneStatus

 Process client plane status
 */

void PPlaneStatus(u_int8_t *buffer, client_t *client)
{
	planestatus1_t *status;
	u_int8_t i, j;

	if (client->attached || client->chute)
		return;

	if (buffer)
	{
		status = (planestatus1_t *)buffer;

		if(client->status_damage != htonl(status->status_damage)) // TODO: Collision: check when new damage from client, possible collision detection
		{
			client->status_damage = htonl(status->status_damage);
		}

		client->status_status = htonl(status->status_status);
	}

	if (!wb3->value && (arena->hour >= 6 && arena->hour <= 18) && (client->posalt[0] > contrail->value))
		client->status_damage |= (STATUS_LFUEL | STATUS_RFUEL);

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{
					SendPlaneStatus(client, &clients[i]);
					break;
				}
			}
		}
	}
}

/**
 WB3FireSuppression

 Some strange WB3 packet
 */

void WB3FireSuppression(u_int8_t *buffer, client_t *client)
{
	u_int8_t packet[7];
	wb3firesupression_t *firesupression;
	wb3supressfire_t *supressfire;
	u_int8_t i, j;

	memset(packet, 0, sizeof(packet));

	firesupression = (wb3firesupression_t *)buffer;
	supressfire = (wb3supressfire_t *)packet;

	supressfire->packetid = htons(Com_WBhton(0x0022));
	supressfire->supress = firesupression->supress;

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{
					supressfire->slot = j;
					SendPacket(packet, sizeof(packet), &clients[i]);
					break;
				}
			}
		}
	}
}

/**
 WB3SupressFire

 Some strange WB3 packet
 */

void WB3SupressFire(u_int8_t slot, client_t *client)
{
	u_int8_t packet[7];
	wb3supressfire_t *supressfire;

	memset(packet, 0, sizeof(packet));

	supressfire = (wb3supressfire_t *)packet;

	supressfire->packetid = htons(Com_WBhton(0x0022));
	supressfire->slot = slot;
	supressfire->supress = htonl(0);

	SendPacket(packet, sizeof(packet), client);
}


/**
 WB3ExternalAmmoCnt

 Some strange WB3 packet
 */

void WB3ExternalAmmoCnt(u_int8_t *buffer, u_int16_t len, client_t *client)
{
	wb3externalammocnt_t *ammocnt;
	u_int8_t i, j;

	ammocnt = (wb3externalammocnt_t *)buffer;

	ammocnt->packetid = htons(Com_WBhton(0x0024));

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{
					SendPacket(buffer, len, &clients[i]);
					break;
				}
			}
		}
	}
}

/**
 PSetRadioChannel

 Set which channel client is talking/earing
 */

void PSetRadioChannel(u_int8_t *buffer, client_t *client)
{
	setradiochannel_t *radio;

	radio = (setradiochannel_t *)buffer;

	client->radio[ntohs(radio->radionum) - 1] = ntohs(radio->channel);
}

/**
 PDropItem

 Process client informing it dropped some item
 */

void PDropItem(u_int8_t *buffer, u_int8_t len, /*u_int8_t fuse,*/ client_t *client)
{
	rocketbomb_t *drop;
	u_int8_t i;
	int16_t j;
	u_int32_t time, dist;
	int16_t speed, z, x, y;
	int32_t destx, desty, alt;

	drop = (rocketbomb_t *)buffer;

	/*
	 if(client->drone & DRONE_COMMANDOS)
	 {
	 item = drop->item;
	 }
	 */

	drop->packetid = htons(Com_WBhton(0x1900));

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{
					SendPacket(buffer, len, &clients[i]);

					if (wb3->value)
					{
						if (drop->item == 103 /* 75mm M1897 */)
						{
							drop->item = 149 /* Flak */;
							SendPacket(buffer, len, &clients[i]);
							drop->item = 103 /* 75mm M1897 */;
						}

						if (drop->item == 88) /* 250kg AP */
						{
							drop->item = 149 /* Flak */;
							SendPacket(buffer, len, &clients[i]);
							drop->item = 88 /* 75mm M1897 */;
						}
					}
					else
					{
						if (drop->item == 113 /* Flare */)
						{
							drop->item = 150 /* Hvy Flak */;
							SendPacket(buffer, len, &clients[i]);
							drop->item = 113 /* Flare */;
						}

						if (drop->item == 112 /* Bomb */)
						{
							drop->item = 150 /* Hvy Flak */;
							SendPacket(buffer, len, &clients[i]);
							drop->item = 112 /* Bomb */;
						}
					}

					/*
					 if(client->drone & DRONE_COMMANDOS)
					 {
					 drop->item = 150; // Hvy Flak;
					 SendPacket(buffer, len, &clients[i]);
					 drop->item = item;
					 }
					 */

					break;
				}
			}
		}
	}

	for (i = 0; i < MAX_RELATED; i++)
	{
		if (client->related[i] && (client->related[i]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
		{
			drop->posx = htonl(client->related[i]->posxy[0][0]);
			drop->posy = htonl(client->related[i]->posxy[1][0]);

			drop->id = htons(ntohs(drop->id) + (500 * (i+1)));

			if (drop->item != 132 /*barrage*/ && drop->item != 114 /*para*/&& (drop->item < 137 || drop->item > 140)/*torpedoes (avoid nuke effect)*/)
			{
				dist = 0;

				j = NearestField(client->posxy[0][0], client->posxy[1][0], 0, TRUE, TRUE, &dist);

				if(j >= 0 && j < fields->value && dist < arena->fields[j].radius)
				{
					if (j < fields->value)
						alt = ntohl(drop->alt) - arena->fields[j].posxyz[2]; // TODO: FIXME: calculate destx desty and GetHeightAt()
					else
						alt = ntohl(drop->alt) - arena->cities[j - (int16_t)fields->value].posxyz[2]; // TODO: FIXME: calculate destx desty and GetHeightAt()
				}
				else
					alt = ntohl(drop->alt);

				if (alt < 0)
					alt = 0;

				x = ntohs(drop->xspeed);
				y = ntohs(drop->yspeed);
				z = ntohs(drop->zspeed);

				if (drop->item > 56 && drop->item < 65) // TODO: FIXME: this is a tentative to correct rocket hit
				{
					x *= 4;
					y *= 4;
					z *= 4;
				}

				speed = sqrt(Com_Pow(z, 2) + (double)(2 * GRAVITY * alt));
				time = (z + speed) / GRAVITY;

				destx = ntohl(drop->posx);
				desty = ntohl(drop->posy);

				destx += x * time;
				desty += y * time;

				time *= 100; // convert to frames


				//				PPrintf(client, RADIO_WHITE, "Vx %d Vy %d Vz %d speed %d", x, y, z, speed);
				AddBomb(ntohs(drop->id), destx, desty, drop->item, speed, time, client->related[i]);
			}
			PDropItem(buffer, len, client->related[i]);
		}
	}

	//score

	if (!client->drone)
	{
		Com_LogEvent(EVENT_DROP, client->id, 0);
		Com_LogDescription(EVENT_DESC_PLPLANE, client->plane, NULL);
		Com_LogDescription(EVENT_DESC_AMMO, drop->item, NULL);

		ScoresEvent(SCORE_DROPITEM, client, drop->item);
	}
}

/**
 WB3TonnageOnTarget

 misterious packet from ien
 */

void WB3TonnageOnTarget(u_int8_t *buffer, client_t *client)
{
	munition_t *ammo;
	u_int16_t field, distance;
	wb3tonnage_t *wb3tonnage;

	wb3tonnage = (wb3tonnage_t *)buffer;

	if(!client)
		return;

	if (client->attr)
		PPrintf(client, RADIO_GREEN, "DEBUG: tonnage ammoID: %d, field: %d, distance: %d", wb3tonnage->ammo, ntohs(wb3tonnage->field), ntohs(wb3tonnage->distance));

	if(!oldcapt->value)
	{
		if((ammo = GetMunition(wb3tonnage->ammo)))
		{
			field = ntohs(wb3tonnage->field);
			distance = ntohs(wb3tonnage->distance);

			if(field && field <= fields->value && distance < arena->fields[field-1].radius)
			{
				if(client->country != arena->fields[field-1].country)
				{
					AddFieldDamage(field-1, ammo->he, client);
					arena->fields[field-1].tonnage += (ammo->he / 50);

					if(arena->fields[field-1].tonnage >= (GetTonnageToClose(field) * 2.4))
						arena->fields[field-1].tonnage = (GetTonnageToClose(field) * 2.4);
				}
			}
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "WB3TonnageOnTarget() unknown munition %d from plane %d", wb3tonnage->ammo, client->plane);
		}
	}
}

/**
 PRemoveDropItem

 Remove dropped item from screen
 */

void PRemoveDropItem(u_int8_t *buffer, u_int8_t len, client_t *client)
{
	u_int8_t i, j;
	weapondestroy_t *weapondestroy;
	weapondestroy = (weapondestroy_t *)buffer;

	weapondestroy->packetid = htons(Com_WBhton(0x1906));

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{

					SendPacket(buffer, len, &clients[i]);
					break;
				}
			}
		}
	}
}

/**
 PFlakHit

 Process Flak hits
 */

void PFlakHit(u_int8_t *buffer, client_t *client)
{
	flakhit_t *flakhit;
	munition_t *munition;
	client_t *pvictim;
	u_int8_t i, hits;
	char header[128];
	char heb[128];
	char gunstatsb[512];
	int8_t killer = 0;

	flakhit = (flakhit_t *)buffer;

	hits = flakhit->hits;

	Com_Printf(VERBOSE_DEBUG_DAMAGE, "PFlakHit hits %d\n", hits);

	if (!(pvictim = FindSClient(ntohl(flakhit->victim))))
		return;

	munition = GetMunition(flakhit->type);

	if (!munition)
	{
		Com_Printf(VERBOSE_WARNING, "Unknown munition ID %d, plane %d\n", flakhit->type, client->plane);
		return;
	}

	PPrintf(pvictim, RADIO_DARKGREEN, "You are hit by %s (%d)", munition->name, hits);

	if (!(pvictim->drone && pvictim->related[0] == client)) // allow to kill own drones (no penalties, no score, etc)
		killer = AddKiller(pvictim, client);

	//	for(i = 0; i < hits; i++)
	//	{
	i = 0;
	flakhit = (flakhit_t *)(buffer+(4*i));

	if (!pvictim->inuse)
		return;

	if (killer >= 0)
		pvictim->hitby[killer].damage += munition->he;

	if (client != pvictim)
	{
		Com_Printf(VERBOSE_WARNING, "PFlakHit(client!=pvictim) client = %s, pvictim = %s\n", client->longnick, pvictim->longnick);
	}

	Com_Printf(VERBOSE_ALWAYS, "-HOST- Flakhit %s with %s\n", pvictim->longnick, munition->abbrev);

	if (gunstats->value)
	{
		memset(heb, 0, sizeof(heb));
		AddPlaneDamage(PLACE_CENTERFUSE, munition->he, 0, heb, NULL, pvictim);

		if (gunstats->value > 1 && client->gunstat && (client != pvictim))
		{
			memset(header, 0, sizeof(header));
			snprintf(header, sizeof(header), "%s(%u)[%de%dp]%s%s", munition->abbrev, flakhit->type, munition->he, munition->ap, pvictim->longnick, GetSmallPlaneName(pvictim->plane));
			memset(gunstatsb, 0, sizeof(gunstatsb));
			snprintf(gunstatsb, sizeof(gunstatsb), "%s;%s;", header, heb);
			PPrintf(client, RADIO_GREEN, "%s", gunstatsb);
		}

		memset(header, 0, sizeof(header));
		snprintf(header, sizeof(header), "%s(%s)->%s(%s)%s(%u)[%de%dp]", client==pvictim?"-HOST-":client->longnick, client==pvictim?"":GetSmallPlaneName(client->plane), pvictim->longnick, GetSmallPlaneName(pvictim->plane), munition->abbrev, flakhit->type, munition->he, munition->ap);
		memset(gunstatsb, 0, sizeof(gunstatsb));
		snprintf(gunstatsb, sizeof(gunstatsb), "%s;%s;", header, heb);
		Com_Printf(VERBOSE_DEBUG_DAMAGE, "DM: %s\n", gunstatsb);
		if (gunstats->value > 1 && pvictim->gunstat)
			PPrintf(pvictim, RADIO_PURPLE, "%s", gunstatsb);
	}
	else
	{
		AddPlaneDamage(PLACE_CENTERFUSE, munition->he, 0, NULL, NULL, pvictim);
	}
	//	}
}

/**
 PHitStructure

 Process client information about hit some structure
 */

void PHitStructure(u_int8_t *buffer, client_t *client)
{
	hitstructure_t *hitstructure;
	u_int8_t i = 0, j = 0, hits = 0, damaged = 0;
	u_int16_t ap;
	double distance;
	munition_t *munition;
	building_t *building;

	hitstructure = (hitstructure_t *)buffer;

	hits = hitstructure->hits;

	building = GetBuilding(htons(hitstructure->build));

	if (!building)
	{
		if (gunstats->value > 1 && client->gunstat)
			PPrintf(client, RADIO_LIGHTYELLOW, "Building %d not declared", htons(hitstructure->build));

		Com_Printf(VERBOSE_WARNING, "Building %d not declared", htons(hitstructure->build));

		return;
	}

	if (building->country == client->country)
	{
		if(oldcapt->value || !wb3->value || arena->fields[building->field - 1].vitals || building->type != BUILD_HANGAR)
		{
			if (arcade->value || !friendlyfire->value || !teamkillstructs->value)
			{
				return;
			}
			else if (client->tkstatus)
			{
				PPrintf(client, RADIO_YELLOW, "You've hit again a friendly structure, please don't do that...");
				if (client->infly)
					ForceEndFlight(TRUE, client);
				return;
			}
		}

		Com_Printf(VERBOSE_ALWAYS, "%s hit friendly %s\n", client->longnick, GetBuildingType(building->type));
	}

	if (building->fieldtype >= FIELD_CV && building->fieldtype <= FIELD_SUBMARINE)
	{
		distance = DistBetween(client->posxy[0][0], client->posxy[1][0], client->posalt[0], arena->fields[building->field - 1].posxyz[0], arena->fields[building->field - 1].posxyz[1],
				arena->fields[building->field - 1].posxyz[2], -1);
	}
	else
		distance = DistBetween(client->posxy[0][0], client->posxy[1][0], client->posalt[0], building->posx, building->posy, building->posz, -1);

	if (distance < 0)
	{
		Com_Printf(VERBOSE_WARNING, "PHitStructure(): %s(%s) sent invalid distance\n", client->longnick, client->ip);
		distance = 0;
	}

	distance /= 300; // convert feets to yards

//	j = hits; // to avoid log hits several times

	for (i = 0; i < hits; i++)
	{
		hitstructure = (hitstructure_t *)(buffer+4*i);

		munition = GetMunition(hitstructure->munition);

		if (!munition)
		{
			Com_Printf(VERBOSE_WARNING, "Unknown munition ID %d, plane %d\n", hitstructure->munition, client->plane);
			return;
		}

		if((((double)munition->decay * distance) + munition->ap) > 0)
			ap = ((double)munition->decay * distance) + munition->ap;
		else
			ap = 0;

		damaged = AddBuildingDamage(building, munition->he, ap, client);

//		if (j == hits)
//		{
//			Com_Printf(VERBOSE_ALWAYS, "%s %shit %u rounds at %s with %s\n", client->longnick, building->country==client->country ? "friendly " : "", hits, GetBuildingType(building->type), munition->abbrev);
//			j = 0;
//		}

		if (IsBomber(client) && client->wings)
		{
			for (j = 0, hits = 0; j < MAX_RELATED; j++)
			{
				if (client->related[j] && (client->related[j]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
					hits++;
			}

			if (hits)
				hits = (rand() % hits) + 1;

			while (hits)
			{
				damaged += AddBuildingDamage(building, munition->he, ap, client);
				hits--;
			}
		}

		if (gunstats->value)
		{
			if (gunstats->value > 1 && client->gunstat)
				PPrintf(client, RADIO_LIGHTYELLOW, "%s(%u)[%de,%da];%s(%d)[%d]", munition->name, hitstructure->munition, munition->he, munition->ap, GetBuildingType(building->type), building->id, building->armor);
			Com_Printf(VERBOSE_DEBUG_DAMAGE, "DM: %s %s(%u)[%de,%da];%s(%d)[%d]\n", client?client->longnick:"-HOST-", munition->name, hitstructure->munition, munition->he, munition->ap, GetBuildingType(building->type), building->id, building->armor);
		}
//		else if (client->gunstat)
//			PPrintf(client, RADIO_GREEN, "Hit %s with %s", GetBuildingType(building->type), munition->name);
	}

	if (building->field <= fields->value)
	{
		if (building->fieldtype >= FIELD_CV && building->fieldtype <= FIELD_SUBMARINE && damaged)
		{
			CheckBoatDamage(building, client);
		}
	}
}

/**
 PHardHitStructure

 Process client information about hard hit some structure
 */

void PHardHitStructure(u_int8_t *buffer, client_t *client)
{
	hardhitstructure_t *hardhitstructure;
	// u_int8_t paras;
	int16_t i;
	int32_t he;
	munition_t *munition;
	building_t *building;
	building_t tempbuild;

	hardhitstructure = (hardhitstructure_t *)buffer;

	if (hardhitstructure->munition >= maxmuntype)
	{
		Com_Printf(VERBOSE_WARNING, "PHardHitStructure(): Munition ID overflow %d. maxmuntype=%d\n", hardhitstructure->munition, MAX_MUNTYPE);
		return;
	}

	building = GetBuilding(htons(hardhitstructure->build));

	if (!building)
	{
		if (gunstats->value > 1 && client->gunstat)
			PPrintf(client, RADIO_LIGHTYELLOW, "Building %d not declared", htons(hardhitstructure->build));

		Com_Printf(VERBOSE_WARNING, "Building %d not declared\n", htons(hardhitstructure->build));

		tempbuild.id = htons(hardhitstructure->build);
		SetBuildingStatus(&tempbuild, 1, NULL);

		return;
	}

	munition = GetMunition(hardhitstructure->munition);

	if (!munition)
	{
		Com_Printf(VERBOSE_WARNING, "Unknown munition ID %d, plane %d\n", hardhitstructure->munition, client->plane);
		return;
	}

	if (building->country == client->country)
	{
		if(oldcapt->value || !wb3->value || arena->fields[building->field - 1].vitals || building->type != BUILD_HANGAR)
		{
			if (arcade->value || !friendlyfire->value || !teamkillstructs->value)
			{
				return;
			}
			else if (client->tkstatus)
			{
				PPrintf(client, RADIO_YELLOW, "You've hit again a friendly structure, please don't do that...");
				if (client->infly)
					ForceEndFlight(TRUE, client);
				return;
			}
		}
	}

	Com_LogEvent(EVENT_HITSTRUCT, client->id, 0);
	Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
	Com_LogDescription(EVENT_DESC_COUNTRY, building->country, NULL);
	Com_LogDescription(EVENT_DESC_STRUCT, building->type, NULL);
	Com_LogDescription(EVENT_DESC_FIELD, building->field, NULL);
	Com_LogDescription(EVENT_DESC_AMMO, hardhitstructure->munition, NULL);

	if (hardhitstructure->munition == 114 /*paras*/&& ((building->fieldtype <= FIELD_MAIN) || (building->fieldtype >= FIELD_WB3POST)))
	{
		if ((arena->fields[building->field - 1].country != client->country) && arena->fields[building->field - 1].abletocapture && arena->fields[building->field - 1].closed)
		{
			arena->fields[building->field - 1].paras++;

			PPrintf(client, RADIO_YELLOW, "Paras count: %d", arena->fields[building->field - 1].paras);
			Com_Printf(VERBOSE_ALWAYS, "%s paras count f%d = %d\n", client->longnick, building->field, arena->fields[building->field - 1].paras);

			AddFieldDamage(building->field-1, GetBuildingArmor(BUILD_TOWER, client), client);

			if (arena->fields[building->field - 1].paras >= GetFieldParas(arena->fields[building->field - 1].type))
			{
				AddBuildingDamage(building, GetBuildingArmor(BUILD_TOWER, client), 1, client);
				CaptureField(building->field, client);
			}
			return;
		}
		else
		{
			return;
		}
	}
	else
	{
		if(GetBuildingCost(building->type))
		{
			HardHit(hardhitstructure->munition, (client->country == building->country), client);

			if (client->country == building->country && !client->msgtimer)
			{
				CPrintf(client->country,
				RADIO_GREEN, "ALERT!!! ALERT!!! %s hit friendly structure at %s%d", client->longnick, (building->fieldtype > FIELD_SUBMARINE && building->fieldtype < FIELD_WB3POST) ? "C" : "F",
						building->field);

				// client->msgtimer = 1000;
			}
		}
	}

	he = munition->he;

	if ((building->type == BUILD_CV) && (hardhitstructure->munition > 136) && (hardhitstructure->munition < 140) && (ntohs(hardhitstructure->speed) < 100)) // to avoid powerfull torpedoes that can down buildings for a long time
		he *= 79;

	i = AddBuildingDamage(building, he, munition->ap, client);

	if (gunstats->value)
	{
		if (gunstats->value > 1 && client->gunstat)
			PPrintf(client, RADIO_LIGHTYELLOW, "%s(%u)[%de,%da];%s(%d)[%d]", munition->name, hardhitstructure->munition, munition->he, munition->ap, GetBuildingType(building->type), building->id, building->armor);
		Com_Printf(VERBOSE_DEBUG_DAMAGE, "DM: %s %s(%u)[%de,%da];%s(%d)[%d]\n", client?client->longnick:"-HOST-", munition->name, hardhitstructure->munition, munition->he, munition->ap, GetBuildingType(building->type), building->id, building->armor);
	}
//	else if (client->gunstat)
//		PPrintf(client, RADIO_GREEN, "Hit %s with %s", GetBuildingType(building->type), munition->name);

	Com_Printf(VERBOSE_ALWAYS, "%s %shit %s with %s\n", client->longnick, client->country ==building->country ? "friendly " : "", GetBuildingType(building->type), munition->abbrev);

	if (building->field <= fields->value)
	{
		if (building->fieldtype >= FIELD_CV && building->fieldtype <= FIELD_SUBMARINE && i)
		{
			CheckBoatDamage(building, client);
		}
	}
}

/**
 PHitPlane

 Process client information about hit some plane and where
 */

void PHitPlane(u_int8_t *buffer, client_t *client)
{
	hitplane_t *hitplane;
	u_int8_t i, j, k, hits, hits2;
	int8_t val;
	u_int16_t he, ap;
	int8_t needle[5];
	u_int32_t dist;
	double distance, sdamage, totaldamage;
	int8_t killer = 0;
	int32_t damage;
	munition_t *munition;
	client_t *pvictim;
	char header[128];
	char heb[128];
	char ign[64];
	char apb[128];
	char gunstatsb[512];

	hitplane = (hitplane_t *)buffer;

	if (!(pvictim = FindSClient(ntohl(hitplane->victim))))
		return;

	if(!pvictim->drone && pvictim->chute) /// Ignore ChuteKill
		return;

	if (!pvictim->inuse)
		return;

	if ((client == pvictim)) // ack hit
	{
		if ((val = NearestField(client->posxy[0][0], client->posxy[1][0], 0, TRUE, TRUE, &dist)) >= 0)
		{
			distance = (double) dist;
			distance = DistBetween(client->posxy[0][0], client->posxy[1][0], client->posalt[0], (client->posxy[0][0] > 0 ? client->posxy[0][0] - distance : client->posxy[0][0] + distance),
					client->posxy[1][0], 0, -1);
		}
		else
		{
			distance = 0;
			Com_Printf(VERBOSE_WARNING, "PHitPlane(): couldn't calculate distance from ack hit (val = %d, dist = %u)\n", val, dist);
		}
	}
	else
	{
		if (client->attached)
		{
			FireAck(client, pvictim, 1);
		}

		distance = DistBetween(client->posxy[0][0], client->posxy[1][0], client->posalt[0], pvictim->posxy[0][0], pvictim->posxy[1][0], pvictim->posalt[0], MAX_INT16);
	}

	if (distance < 0)
	{
		Com_Printf(VERBOSE_WARNING, "PHitPlane(): %s(%s) sent invalid distance\n", client->longnick, client->ip);
		distance = 0;
	}

	if (arcade->value || !friendlyfire->value || client->tkstatus) //&& pvictim != client)
	{
		if (pvictim->country == client->country)
		{
			if (!(!client->tkstatus && pvictim->tkstatus))
				pvictim = client;
		}
	}

	distance /= 300; // convert feets to yards

	if (!pvictim->drone)
		SendPings(hitplane->hits, hitplane->type, pvictim); // Send hit pings

	hits2 = hits = hitplane->hits;

	munition = GetMunition(hitplane->type);

	if (!munition)
	{
		Com_Printf(VERBOSE_WARNING, "Unknown munition ID %d, plane %d\n", hitplane->type, client->plane);
		return;
	}

	if(munition->caliber && munition->caliber <= 6)
	{
		if(client != pvictim)
		{
			client->hits[munition->caliber - 1] += hits;
			client->hitsstat[munition->caliber - 1] += hits;
		}

		pvictim->hitstaken[munition->caliber - 1] += hits;
		pvictim->hitstakenstat[munition->caliber - 1] += hits;
	}

	if (!(pvictim->drone && pvictim->related[0] == client)) // allow to kill own drones (no penalties, no score, etc)
	{
		if (pvictim != client) //not a ack hit
		{
			for (i = 0; i < MAX_RELATED; i++)
			{
				if (pvictim->related[i] && (pvictim->related[i]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
					break;
			}

			if (i < MAX_RELATED)
				pvictim = pvictim->related[i]; // send damage to first wingman
			//	return; // dont hit plane if with wingmans
		}

		killer = AddKiller(pvictim, client);
	}

	// Random damage while there are wingmans
	if (IsBomber(client) && client->wings)
	{
		for (i = 0, j = 0; i < MAX_RELATED; i++)
		{
			if (client->related[j] && (client->related[j]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
				j++;
		}

		if (j)
			hits *= (rand() % j) + 1; // random damage multiply according with wings number
	}

	memset(buffer, 0, sizeof(buffer));


	for (i = 0; i < hits; i++)
	{
		for (j = i; j >= hits2;)
		{
			j -= hits2;
		}

		hitplane = (hitplane_t *)(buffer + (8 * j));

		munition = GetMunition(hitplane->type);

		if (!munition)
		{
			Com_Printf(VERBOSE_WARNING, "Unknown munition ID %d, plane %d\n", hitplane->type, client->plane);
			return;
		}

		// Begin Needle pre-processing

		for (j = 0; j < 5; j++)
		{
			needle[j] = -1;
		}

		memset(ign, 0, sizeof(ign));

		for (j = 0, k = 0; j < 5; j++)
		{
			if (hitplane->place[j] < 0)
			{
				continue;
			}

			if (hitplane->place[j] >= MAX_PLACE)
			{
				PPrintf(client, RADIO_LIGHTYELLOW, "invalid place %d", hitplane->place[j]);
				Com_Printf(VERBOSE_WARNING, "PHitPlane(): invalid place\n");
				continue;
			}

			if (pvictim->armor.points[hitplane->place[j]] < 0)
			{
//				PPrintf(client, RADIO_LIGHTYELLOW, "unused place %d, plane %s(%d)", hitplane->place[j], GetSmallPlaneName(pvictim->plane), pvictim->plane);
				Com_Printf(VERBOSE_WARNING, "PHitPlane(): unused place %d plane %d\n", hitplane->place[j], pvictim->plane);
				continue; // unused part
			}

//			if (!IsGround(pvictim))
//			{
//				if (hitplane->place[j] >= PLACE_ELEVATOR && hitplane->place[j] <= PLACE_VSTAB) /* thin element */
//				{
//					if (j < 4) // not last layer
//					{
//						if (hitplane->place[j + 1] >= 0) // there is anything after thin element
//						{
//							sprintf((ign + strlen(ign)), "(%s=%d)", GetSmallHitSite(hitplane->place[j]), pvictim->armor.points[hitplane->place[j]]);
//							continue;
//						}
//					}
//				}
//			}

			if (munition->he < 100 && distance > 2.0 && pvictim->plane != 61 /*CHUTE*/)
			{
				if (hitplane->place[j] == PLACE_PILOTARMOR || hitplane->place[j] == PLACE_PILOT)
				{
					sprintf((ign + strlen(ign)), "(%s=%d)", GetSmallHitSite(hitplane->place[j]), pvictim->armor.points[hitplane->place[j]]);
					continue;
				}
			}

			needle[k++] = hitplane->place[j];
		}

		// End Needle pre-processing

		he = (u_int16_t)munition->he;

		if((((double)munition->decay * distance) + munition->ap) > 0)
			ap = ((double)munition->decay * distance) + munition->ap;
		else
			ap = 0;

		if (gunstats->value)
		{
			memset(heb, 0, sizeof(heb));
			memset(apb, 0, sizeof(apb));
		}

		for (j = 0; j < 5; j++)
		{
			if (needle[j] < 0)
				break;

			if (needle[j] >= MAX_PLACE || pvictim->armor.points[needle[j]] <= 0)
				continue;

			damage = (he + ap);

			if(damage < 0)
			{
				Com_Printf(VERBOSE_DEBUG_DAMAGE, "PHitPlane(damage he+ap) < 0, he = %d, ap = %d\n", he, ap);
			}

			if (!setjmp(debug_buffer))
			{
				if (gunstats->value)
				{
					ap = AddPlaneDamage(needle[j], he, ap, (heb + strlen(heb)), (apb + strlen(apb)), pvictim);
				}
				else
				{
					ap = AddPlaneDamage(needle[j], he, ap, NULL, NULL, pvictim);
				}
			}
			else
			{
				DebugClient(__FILE__, __LINE__, TRUE, client);
				return;
			}

			damage -= ap;

			if(damage < 0)
			{
				Com_Printf(VERBOSE_DEBUG_DAMAGE, "PHitPlane(damage -= ap) < 0, damage = %d, ap = %d\n", damage, ap);
				damage = 0;
			}

			if(needle[j] >= 0 && needle[j] < MAX_PLACE && killer >= 0 && killer < MAX_HITBY)
			{
				sdamage = (double)(10.0 * logf(1.0 + 100.0 * (double)damage / (double)(((pvictim->armor.points[needle[j]] <= 0) ? 0 : pvictim->armor.points[needle[j]]) + 1.0)));

//				if(pvictim->drone == DRONE_COMMANDOS)
//				{
//					Com_Printf(VERBOSE_DEBUG_DAMAGE, "Commandos damage = %u, sdamage = %.2f, part = %u\n", damage, sdamage, needle[j]);
//					sdamage = 59.0;
//				}

				if(sdamage >= 0)
				{
					pvictim->hitby[killer].damage += sdamage;
				}
				else
				{
					Com_Printf(VERBOSE_DEBUG_DAMAGE, "PHitPlane(sdamage) < 0, (1.0 + 100.0 * %d / (%d + 1.0))\n", damage, ((pvictim->armor.points[needle[j]] <= 0) ? 0 : pvictim->armor.points[needle[j]]));
				}
			}

			if (ap == 0)
				break;

			he = 0;
		}

		if (gunstats->value)
		{
//			hitplane = (hitplane_t *)buffer;

//			munition = GetMunition(hitplane->type);

//			if (!munition)
//			{
//				Com_Printf(VERBOSE_WARNING, "Unknown munition ID %d, plane %d\n", hitplane->type, client->plane);
//				return;
//			}

			if((((double)munition->decay * distance) + munition->ap) > 0)
				ap = ((double)munition->decay * distance) + munition->ap;
			else
				ap = 0;

			if (gunstats->value > 1 && client->gunstat && (client != pvictim))
			{
				memset(header, 0, sizeof(header));
				snprintf(header, sizeof(header), "%s(%u)[%de%dp]%s%s", munition->abbrev, hitplane->type, munition->he, ap, pvictim->longnick, GetSmallPlaneName(pvictim->plane));
				memset(gunstatsb, 0, sizeof(gunstatsb));
				snprintf(gunstatsb, sizeof(gunstatsb), "%s;%s;%s%s", header, heb, ign, apb);
				PPrintf(client, RADIO_GREEN, "%s", gunstatsb);
			}

			memset(header, 0, sizeof(header));
			snprintf(header, sizeof(header), "%s(%s)->%s(%s)%s(%u)[%de%dp]", client==pvictim?"-HOST-":client->longnick, client==pvictim?"":GetSmallPlaneName(client->plane), pvictim->longnick, GetSmallPlaneName(pvictim->plane), munition->abbrev, hitplane->type, munition->he, ap);
			memset(gunstatsb, 0, sizeof(gunstatsb));
			snprintf(gunstatsb, sizeof(gunstatsb), "%s;%s;%s%s", header, heb, ign, apb);
			Com_Printf(VERBOSE_DEBUG_DAMAGE, "DM: %s\n", gunstatsb);
			if (gunstats->value > 1 && pvictim->gunstat)
				PPrintf(pvictim, RADIO_PURPLE, "%s", gunstatsb);
		}
	}

	if (killer >=0 && pvictim->chute && (pvictim->status_damage & (1 << PLACE_PILOT)))
	{
		for (i = 0; i < MAX_HITBY; i++) // check who hit player
		{
			if (pvictim->hitby[i].dbid && pvictim->hitby[i].damage && pvictim->hitby[i].dbid < DRONE_DBID_BASE)
			{
				totaldamage += pvictim->hitby[i].damage;
			}
		}

		pvictim->hitby[killer].damage += totaldamage * 0.25;
	}
}

/**
 WB3FuelConsumed

 Fuel Consumed by player
 */
void WB3FuelConsumed(u_int8_t *buffer, client_t *client)
{
	wb3fuelconsumed_t *fuelconsumed;

	fuelconsumed = (wb3fuelconsumed_t *)buffer;

	if (client->attr) // TODO: Scores: debite fuel consumed from scores
		PPrintf(client, RADIO_RED, "fuel = %u", ntohl(fuelconsumed->amount));
}

/**
 WB3DelayedFuse

 Fire flaks with delayed fuse
 */
void WB3DelayedFuse(u_int8_t *buffer, client_t *client)
{
	wb3delayedfuse_t *wb3delayedfuse;
	u_int16_t i, j;

	wb3delayedfuse = (wb3delayedfuse_t *)buffer;

	wb3delayedfuse->packetid = htons(Com_WBhton(0x1917));

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{
					SendPacket(buffer, 35, &clients[i]);
					break;
				}
			}
		}
	}
}

/**
 SendPings

 Send bullet pings
 */
void SendPings(u_int8_t hits, u_int8_t type, client_t *client)
{
	pings_t *pings;
	u_int8_t bufferping[4];

	memset(bufferping, 0, sizeof(bufferping));

	pings = (pings_t *)bufferping;

	pings->packetid = htons(Com_WBhton(0x1907));
	pings->hits = hits;
	pings->type = type;

	SendPacket(bufferping, sizeof(bufferping), client);
}

/**
 PHardHitPlane

 Process client information about hard hit some plane and where
 */

void PHardHitPlane(u_int8_t *buffer, client_t *client)
{
	hardhitplane_t *hardhitplane;
	client_t *pvictim;
	u_int8_t i;
	int8_t killer = 0;
	int32_t he;
	double sdamage, totaldamage;
	munition_t *munition;
	char header[128];
	char heb[128];
	char gunstatsb[512];

	hardhitplane = (hardhitplane_t *) buffer;

	if (!(pvictim = FindSClient(ntohl(hardhitplane->victim))))
		return;

	if (arcade->value || !friendlyfire->value || client->tkstatus)// && pvictim != client)
	{
		if (pvictim->country == client->country)
		{
			if (!(!client->tkstatus && pvictim->tkstatus))
				pvictim = client;
		}
	}

	if (!pvictim->drone)
		SendPings(1, hardhitplane->munition, pvictim);

	munition = GetMunition(hardhitplane->munition);

	if (!munition)
	{
		Com_Printf(VERBOSE_WARNING, "Unknown munition ID %d, plane %d\n", hardhitplane->munition, client->plane);
		return;
	}
	else
		he = munition->he;

	if(munition->caliber && munition->caliber <= 6)
	{
		if(client != pvictim)
		{
			client->hits[munition->caliber - 1]++;
			client->hitsstat[munition->caliber - 1]++;
		}

		pvictim->hitstaken[munition->caliber - 1]++;
		pvictim->hitstakenstat[munition->caliber - 1]++;
	}

	Com_Printf(VERBOSE_ALWAYS, "%s %shardhit %s with %s (%u damage)\n", client->longnick, client->country == pvictim->country ? "friendly " : "", pvictim->longnick, munition->abbrev, he);

	HardHit(hardhitplane->munition, !((pvictim->country != client->country) || pvictim->tkstatus), client);

	if (!wb3->value && (pvictim->plane == 101 /*TANK*/|| pvictim->plane == 85 /*JEEP*/) && hardhitplane->munition == 56)
	{
		he /= 3;
	}

	if (!(client == pvictim && hardhitplane->munition < 99 && hardhitplane->munition > 55)) // avoid bomb killers
	{
		if (!(pvictim->drone && pvictim->related[0] == client)) // allow to kill own drones (no penalties, no score, etc)
		{
			killer = AddKiller(pvictim, client);

			if (hardhitplane->place >= 0 && hardhitplane->place < 32 && killer >= 0 && killer < MAX_HITBY)
			{
				sdamage = (double)(10.0 * logf(1.0 + 100.0 * (double)he / (double)(((pvictim->armor.points[hardhitplane->place] <= 0) ? 0 : pvictim->armor.points[hardhitplane->place]) + 1.0)));

				if(sdamage >= 0)
				{
					pvictim->hitby[killer].damage += sdamage;
				}
				else
				{
					Com_Printf(VERBOSE_DEBUG_DAMAGE, "PHardHitPlane(sdamage) < 0, (1.0 + 100.0 * %d / (%d + 1.0))\n", he, ((pvictim->armor.points[hardhitplane->place] <= 0) ? 0 : pvictim->armor.points[hardhitplane->place]));
				}
			}
		}
	}

	for (i = 0; i < MAX_RELATED; i++)
	{
		if (pvictim->related[i] && (pvictim->related[i]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
			return; // not hit plane if with wingmans
	}

	if (gunstats->value)
	{
		memset(heb, 0, sizeof(heb));
		AddPlaneDamage(hardhitplane->place, he, 0, heb, NULL, pvictim);

		if (gunstats->value > 1 && client->gunstat && (client != pvictim))
		{
			memset(header, 0, sizeof(header));
			snprintf(header, sizeof(header), "%s(%u)[%de%dp]%s%s", munition->abbrev, hardhitplane->munition, munition->he, munition->ap, pvictim->longnick, GetSmallPlaneName(pvictim->plane));
			memset(gunstatsb, 0, sizeof(gunstatsb));
			snprintf(gunstatsb, sizeof(gunstatsb), "%s;%s;", header, heb);
			PPrintf(client, RADIO_GREEN, "%s", gunstatsb);
		}

		memset(header, 0, sizeof(header));
		snprintf(header, sizeof(header), "%s(%s)->%s(%s)%s(%u)[%de%dp]", client==pvictim?"-HOST-":client->longnick, client==pvictim?"":GetSmallPlaneName(client->plane), pvictim->longnick, GetSmallPlaneName(pvictim->plane), munition->abbrev, hardhitplane->munition, munition->he, munition->ap);
		memset(gunstatsb, 0, sizeof(gunstatsb));
		snprintf(gunstatsb, sizeof(gunstatsb), "%s;%s;", header, heb);
		Com_Printf(VERBOSE_DEBUG_DAMAGE, "DM: %s\n", gunstatsb);
		if (gunstats->value > 1 && pvictim->gunstat)
			PPrintf(pvictim, RADIO_PURPLE, "%s", gunstatsb);
	}
	else
	{
		AddPlaneDamage(hardhitplane->place, he, 0, NULL, NULL, pvictim);
	}

	if (killer >=0 && pvictim->chute && (pvictim->status_damage & (1 << PLACE_PILOT)))
	{
		for (i = 0; i < MAX_HITBY; i++) // check who hit player
		{
			if (pvictim->hitby[i].dbid && pvictim->hitby[i].damage && pvictim->hitby[i].dbid < DRONE_DBID_BASE)
			{
				totaldamage += pvictim->hitby[i].damage;
			}
		}

		pvictim->hitby[killer].damage += totaldamage * 0.25;
	}
}

/**
 GetHitSite

 Returns the name of where client were hit
 */

char *GetHitSite(u_int8_t id)
{
	switch (id)
	{
		case PLACE_RIGHTGUN:
			return "right gun";
		case PLACE_TOPGUN:
			return "top gun";
		case PLACE_BOTTOMGUN:
			return "bottom gun";
		case PLACE_LWING:
			return "left wing";
		case PLACE_RWING:
			return "right wing";
		case PLACE_RGEAR:
			return "right gear";
		case PLACE_LGEAR:
			return "left gear";
		case PLACE_FLAPS:
			return "flaps";
		case PLACE_CENTERFUEL:
			return "center fuel";
		case PLACE_REARFUSE:
			return "rear fuselage";
		case PLACE_CENTERFUSE:
			return "center fuselage";
		case PLACE_PILOTARMOR:
			return "pilot armor";
		case PLACE_PILOT:
			return "pilot";
		case PLACE_TAILGUN:
			return "tail gun";
		case PLACE_NOSEGUN:
			return "nose gun";
		case PLACE_LEFTGUN:
			return "left gun";
		case PLACE_ELEVATOR:
			return "elevator";
		case PLACE_HSTAB:
			return "horizontal stabilizator";
		case PLACE_RUDDER:
			return "rudder";
		case PLACE_VSTAB:
			return "vertical stabilizator";
		case PLACE_LAILERON:
			return "left aileron";
		case PLACE_RAILERON:
			return "right aileron";
		case PLACE_LFUEL:
			return "left fuel";
		case PLACE_RFUEL:
			return "right fuel";
		case PLACE_ENGINE1:
			return "engine 1";
		case PLACE_ENGINE2:
			return "engine 2";
		case PLACE_ENGINE3:
			return "engine 3";
		case PLACE_ENGINE4:
			return "engine 4";
		case PLACE_ENGINE1S:
			return "engine 1 oil lines";
		case PLACE_ENGINE2S:
			return "engine 2 oil lines";
		case PLACE_ENGINE3S:
			return "engine 3 oil lines";
		case PLACE_ENGINE4S:
			return "engine 4 oil lines";
		default:
			return "unknown place";
	}
}

/**
 GetSmallHitSite

 Returns the abreviated name of where client were hit
 */

char *GetSmallHitSite(u_int8_t id)
{
	switch (id)
	{
		case PLACE_RIGHTGUN:
			return "rgn";
		case PLACE_TOPGUN:
			return "ugn";
		case PLACE_BOTTOMGUN:
			return "bgn";
		case PLACE_LWING:
			return "lwi";
		case PLACE_RWING:
			return "rwi";
		case PLACE_RGEAR:
			return "ruc";
		case PLACE_LGEAR:
			return "luc";
		case PLACE_FLAPS:
			return "flp";
		case PLACE_CENTERFUEL:
			return "cft";
		case PLACE_REARFUSE:
			return "rfz";
		case PLACE_CENTERFUSE:
			return "cfz";
		case PLACE_PILOTARMOR:
			return "arm";
		case PLACE_PILOT:
			return "pil";
		case PLACE_TAILGUN:
			return "6gn";
		case PLACE_NOSEGUN:
			return "ngn";
		case PLACE_LEFTGUN:
			return "lgn";
		case PLACE_ELEVATOR:
			return "ele";
		case PLACE_HSTAB:
			return "hst";
		case PLACE_RUDDER:
			return "rud";
		case PLACE_VSTAB:
			return "vst";
		case PLACE_LAILERON:
			return "lai";
		case PLACE_RAILERON:
			return "rai";
		case PLACE_LFUEL:
			return "lft";
		case PLACE_RFUEL:
			return "rft";
		case PLACE_ENGINE1:
			return "en1";
		case PLACE_ENGINE2:
			return "en2";
		case PLACE_ENGINE3:
			return "en3";
		case PLACE_ENGINE4:
			return "en4";
		case PLACE_ENGINE1S:
			return "oi1";
		case PLACE_ENGINE2S:
			return "oi2";
		case PLACE_ENGINE3S:
			return "oi3";
		case PLACE_ENGINE4S:
			return "oi4";
		default:
			return "unk";
	}
}

/**
 GetMunition

 Returns the pointer for munition from given ID
 */

munition_t *GetMunition(u_int8_t id)
{
	if (id < maxmuntype)
	{
		if (arena->munition[id].he < 0)
		{
			Com_Printf(VERBOSE_WARNING, "GetMunition(): Unused weapon (he = -1) ID %u\n", id);

			return NULL;
		}

		return (munition_t *) &arena->munition[id];
	}

	return NULL;
}

/**
 AddPlaneDamage

 Add damage to given and parent parts, returns AP left
 */

u_int16_t AddPlaneDamage(int8_t place, u_int16_t he, u_int16_t ap, char *phe, char *pap, client_t *client)
{
	int32_t apabsorb, dmgprobe;
	//static u_int8_t depth = 0;

	//	if(++depth > 10)
	//	{
	//		Com_Printf(VERBOSE_DEBUG, "AddPlaneDamage() Possible Infinite Loop, %s - %d\n", client, client->armor.parent[place]);
	//		PPrintf(client, RADIO_YELLOW, "Damage Model error, please inform admins");
	//		depth--;
	//		return 0;
	//	}
	if(!client)
	{
		Com_Printf(VERBOSE_DEBUG_DAMAGE, "AddPlaneDamage(client) == NULL\n");
		return 0;
	}

	if (place >= MAX_PLACE)
	{
		PPrintf(client, RADIO_LIGHTYELLOW, "Unknown hit place 0x%X", place);
		//		depth--;
		return 0;
	}

	if (!setjmp(debug_buffer))
	{
		apabsorb = (ap > client->armor.apstop[place]) ? client->armor.apstop[place] : ap;
		if(apabsorb < 0)
		{
			Com_Printf(VERBOSE_DEBUG_DAMAGE, "AddPlaneDamage(apabsorb) < 0 at line %u\n", __LINE__);
			apabsorb = 0;
		}
		dmgprobe = he + apabsorb;

		if(dmgprobe < 0 || dmgprobe > 1000000)
		{
			Com_Printf(VERBOSE_DEBUG_DAMAGE, "AddPlaneDamage(dmgprobe) out of range (%u) at line %u\n", dmgprobe, __LINE__);
			dmgprobe = 0;
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
		return 0;
	}

	if (!(dmgprobe < client->armor.imunity[place])) // hit makes damage
	{
		if (dmgprobe >= client->armor.points[place]) // hit destroy part
		{
			if (he)
			{
				he = dmgprobe - client->armor.points[place];
			}

			if (client->armor.points[place])
			{
				if (!setjmp(debug_buffer))
				{
					if (place >= PLACE_LFUEL && place <= PLACE_CENTERFUEL) // fuel will leak
					{
						if (!client->fueltimer)
							client->fueltimer = 1;
					}

					client->damaged = 1;
					client->armor.points[place] = 0;
					SendForceStatus((1 << place), client->status_status, client);

					if (!client->inuse || !client->infly) // drone killed
					{
						// depth--;
						return 0;
					}
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
					return 0;
				}
			}
			else if (place >= PLACE_LFUEL && place <= PLACE_CENTERFUEL && (dmgprobe - apabsorb)) // fuel is leaking
			{
				if (!setjmp(debug_buffer))
				{
					if (client->fueltimer && client->fueltimer > 1000)
					{
						client->damaged = 1;

						if (place == PLACE_LFUEL)
						{
							client->armor.points[PLACE_LWING] = 0;
							SendForceStatus((1 << PLACE_LWING), client->status_status, client);
						}
						else if (place == PLACE_RFUEL)
						{
							client->armor.points[PLACE_RWING] = 0;
							SendForceStatus((1 << PLACE_RWING), client->status_status, client);
						}
						else if (place == PLACE_CENTERFUEL)
						{
							client->armor.points[PLACE_CENTERFUSE] = 0;
							SendForceStatus((1 << PLACE_CENTERFUSE), client->status_status, client);
						}

						PPrintf(client, RADIO_YELLOW, "Your %s exploded", GetHitSite(place));

						client->fueltimer = he = 0;
					}
				}
				else
				{
					DebugClient(__FILE__, __LINE__, TRUE, client);
					return 0;
				}
			}
		}
		else // hit damage part but not destroy it
		{
			if (!setjmp(debug_buffer))
			{
				if (gunstats->value)
				{
					if (he && phe)
						sprintf(phe, "|%s=%d-(%d+%d)|", GetSmallHitSite(place), client->armor.points[place], he, apabsorb/*dmgprobe*/);
				}

				client->armor.points[place] -= dmgprobe;

				client->damaged = 1; // FIXME: this is a temporary fix for kills attribution

//				if (gunstats->value)
//				{
//					if (he && phe)
//						sprintf(phe, "%s=%d", GetSmallHitSite(place), client->armor.points[place]);
//				}

				he = 0;
			}
			else
			{
				DebugClient(__FILE__, __LINE__, TRUE, client);
				return 0;
			}
		}

		if (!setjmp(debug_buffer))
		{
			if (gunstats->value)
			{
				if (ap && pap)
					sprintf(pap, "%s=%d", GetSmallHitSite(place), client->armor.points[place]);

				if (he && phe)
					sprintf(phe, "%s=%d", GetSmallHitSite(place), client->armor.points[place]);
			}
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, client);
			return 0;
		}

		if (!setjmp(debug_buffer))
		{
			if (he)
			{
				if (client->armor.parent[place] >= 0)
				{
					if (client->armor.parent[place] >= MAX_PLACE)
						BPrintf(RADIO_RED, "DEBUG: invalid parent %d, place %d", client->armor.parent[place], place);
					else
					{
						if (gunstats->value)
						{
							if (phe)
								AddPlaneDamage(client->armor.parent[place], he, 0, (phe + strlen(phe)), 0, client);
						}
						else
						{
							AddPlaneDamage(client->armor.parent[place], he, 0, NULL, NULL, client);
						}
					}
				}
			}

			if(apabsorb > ap)
			{
				Com_Printf(VERBOSE_DEBUG_DAMAGE, "AddPlaneDamage(apabsorb) > ap at line %u (apabsorb = %d, ap = %u)\n", __LINE__, apabsorb, ap);
				apabsorb = ap;
			}

			ap -= apabsorb;
		}
		else
		{
			DebugClient(__FILE__, __LINE__, TRUE, client);
			return 0;
		}
	}
	else
	{
		ap = 0;
	}

	//	depth--;
	return ap;
}

/**
 RebuildTime

 Calculate the rebuiding time
 */

double RebuildTime(building_t *building)
{
	static u_int8_t villages, towns, ports, posts;
	static double const_v, const_t, const_por, const_pos;
	static u_int8_t calc = 0;
	double c_villages, c_towns, c_ports, c_posts;
	u_int16_t total;
	u_int8_t i;
	double rebuild;

	if(!building)
	{
		calc = 0;
		return 0;
	}

	if(building->type >= BUILD_MAX)
		return 0;

	if(!calc)
	{
		villages = towns = ports = 0;

		for(i = 0; i < fields->value; i++)
		{
			if(arena->fields[i].type == FIELD_WB3PORT)
				ports++;
			else if(arena->fields[i].type == FIELD_WB3TOWN)
				towns++;
			else if(arena->fields[i].type == FIELD_WB3VILLAGE)
				villages++;
			else if(arena->fields[i].type == FIELD_WB3POST)
				posts++;
		}

		total = ports * 9 + towns * 15 + villages * 3 + posts * 1;

		if(total)
		{
			const_v = 0.6 * villages * 3 / total;
			const_t = 0.6 * towns * 15 / total;
			const_por = 0.6 * ports * 9 / total;
			const_pos = 0.6 * posts * 1 / total;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "RebuildTime(total) == 0\n");
		}

		calc = 1;
	}

	c_posts = c_villages = c_towns = c_ports = 0;

	for(i = 0; i < fields->value; i++)
	{
		if(arena->fields[i].country == building->country)
		{
			if(arena->fields[i].type == FIELD_WB3PORT)
				c_ports++;
			else if(arena->fields[i].type == FIELD_WB3TOWN)
				c_towns++;
			else if(arena->fields[i].type == FIELD_WB3VILLAGE)
				c_villages++;
			else if(arena->fields[i].type == FIELD_WB3POST)
				c_posts++;
		}
	}

	rebuild = 6000 * rebuildtime->value * Com_Log(GetBuildingArmor(building->type, NULL), 40) *
		(1 - (posts?(const_pos * c_posts / posts):0) - (ports?(const_por * c_ports / ports):0) - (villages?(const_v * c_villages / villages):0) - (towns?(const_t * c_towns / towns):0));

	if(rebuild < 10000 /*10 seconds*/|| rebuild > 360000 /*50min*/)
	{
		Com_Printf(VERBOSE_WARNING, "RebuildTime(rebuild) rebuild error %d\n", rebuild);
		Com_Printf(VERBOSE_WARNING, "RebuildTime(rebuild) post: %d, %f, %f, port: %d, %f, %f, village: %d, %f, %f, town: %d, %f, %f\n", posts, const_pos, c_posts, ports, const_por, c_ports, villages, const_v, c_villages, towns, const_t, c_towns);
		Com_Printf(VERBOSE_WARNING, "RebuildTime(rebuild) %f, Log %f, %f\n", rebuildtime->value, Com_Log(GetBuildingArmor(building->type, NULL), 40), (1 - (posts?(const_pos * c_posts / posts):0) - (ports?(const_por * c_ports / ports):0) - (villages?(const_v * c_villages / villages):0) - (towns?(const_t * c_towns / towns):0)));
		rebuild = 90000; // 15min
	}

	return rebuild;
}

/**
 AddBuildingDamage

 Set the max damage all structure can support in selected airplane
 */

u_int8_t AddBuildingDamage(building_t *building, u_int16_t he, u_int16_t ap, client_t *client)
{
	int32_t apabsorb, dmgprobe; //, armor;
	int32_t score;
	u_int16_t i;

	if (!building)
		return 0;

	if (building->status)
		return 0;

	if(!building->field)
	{
		Com_Printf(VERBOSE_DEBUG_DAMAGE, "AddBuildingDamage() building %d has no field", building->id);
		return 0;
	}

	if(oldcapt->value || !wb3->value || arena->fields[building->field - 1].vitals || building->type != BUILD_HANGAR)
	{
		if (client && (client->country == building->country) && !teamkillstructs->value)
			return 0;
	}

	if (!setjmp(debug_buffer))
	{
		dmgprobe = GetBuildingAPstop(building->type, NULL);
		apabsorb = (ap > dmgprobe) ? dmgprobe : ap;
		dmgprobe = he + apabsorb;
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	if (!setjmp(debug_buffer))
	{
		if (dmgprobe < (int32_t)GetBuildingImunity(building->type, NULL))
			return 0;
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	if (!setjmp(debug_buffer))
	{
		if (building->type == BUILD_CARGO || building->type == BUILD_DESTROYER || building->type == BUILD_SUBMARINE)
		{
			if (building->fieldtype >= FIELD_CARGO && building->fieldtype <= FIELD_SUBMARINE) // if is cargo, dd or submarine convoy
			{
				for (i = 0; i < MAX_BUILDINGS; i++)
				{
					if (arena->fields[building->field - 1].buildings[i].field)
					{
						if (arena->fields[building->field - 1].buildings[i].type == BUILD_CARGO || arena->fields[building->field - 1].buildings[i].type == BUILD_DESTROYER
								|| arena->fields[building->field - 1].buildings[i].type == BUILD_SUBMARINE)
						{ // found the 1st ship (that carries the number)
							if (building == &(arena->fields[building->field - 1].buildings[i]))
							{
								for (i++; i < MAX_BUILDINGS; i++)
								{
									if (arena->fields[building->field - 1].buildings[i].field)
									{
										if (arena->fields[building->field - 1].buildings[i].type == BUILD_CARGO || arena->fields[building->field - 1].buildings[i].type == BUILD_DESTROYER
												|| arena->fields[building->field - 1].buildings[i].type == BUILD_SUBMARINE)
										{
											if (!arena->fields[building->field - 1].buildings[i].status) // check if another ship is alive, so 1st ship cannot be sunk
												return 0;
										}
									}
									else
									{
										Com_Printf(VERBOSE_ALWAYS, "Main Ship (%d) destroyed (F%d) by %s %s\n", building->id, building->field, client ? (client->drone ? "drone" : "player") : "-HOST-",
												client ? client->longnick : "");
										break;
									}
								}
							}
							else
								break;
						}
					}
					else
						break;
				}
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	if (!setjmp(debug_buffer))
	{
		if (!oldcapt->value /*ToT enabled*/ || IsVitalBuilding(building, oldcapt->value)) // TODO: Score: unbeta: DM: add AP to all type 1 guns
		{
			if(oldcapt->value || (ap)) // this adds damage by bombs if oldcapt or by bullets if ToT enabled
			{
				if(!client || (client->country != building->country))
				{
					AddFieldDamage(building->field-1, dmgprobe, client);
					arena->fields[building->field-1].tonnage += (dmgprobe / 50);

					if(arena->fields[building->field-1].tonnage >= (GetTonnageToClose(building->field) * 2.4))
						arena->fields[building->field-1].tonnage = (GetTonnageToClose(building->field) * 2.4);
				}
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	if (!setjmp(debug_buffer))
	{
		score = dmgprobe < building->armor ? dmgprobe : building->armor;
		score = score * 100 * GetBuildingCost(building->type) / GetBuildingArmor(building->type, NULL);
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	if (!setjmp(debug_buffer))
	{
		if(score && client)
		{
			if (client->country != building->country)
			{
				ScoresEvent(SCORE_STRUCTDAMAGE, client, score);
			}
			else
			{
				ScoresEvent(SCORE_STRUCTDAMAGE, client, -1 * score);
			}
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}


	if (!setjmp(debug_buffer))
	{
		if ((int32_t)building->armor <= dmgprobe)
		{
			if ((building->type == BUILD_FENCE) || (building->type == BUILD_ROCK) || (building->type == BUILD_TREE))
			{
				building->status = 2;
			}
			else
			{
				building->status = 1;
			}

			if ((building->fieldtype <= FIELD_SUBMARINE) || (building->fieldtype >= FIELD_WB3POST))
			{
				//armor = GetBuildingArmor(building->type, client);
				building->timer = (int32_t)(RebuildTime(building) / ((building->country == COUNTRY_RED)?arena->redindex:arena->goldindex));//(u_int32_t) (Com_Log(dmgprobe, 17) * Com_Log(armor, 17) * (rebuildtime->value * 100)); // (double)((double)(armor + dmgprobe - building->armor)/(double)armor) * (double)(rebuildtime->value * 100);
				//if (building->timer > (u_int32_t)(rebuildtime->value * 1200))
				//	building->timer = (rebuildtime->value * 1200);
				building->armor = 0;
			}
			else
			{
				building->armor = 0;
				building->timer = arena->frame;
			}

			if (client && !client->drone)
			{
				Com_LogEvent(EVENT_KILLSTRUCT, client->id, 0);
				Com_LogDescription(EVENT_DESC_PLCTRY, client->country, NULL);
				Com_LogDescription(EVENT_DESC_COUNTRY, building->country, NULL);
				Com_LogDescription(EVENT_DESC_STRUCT, building->type, NULL);
				Com_LogDescription(EVENT_DESC_FIELD, building->field, NULL);

				if(oldcapt->value || !wb3->value || arena->fields[building->field - 1].vitals || building->type != BUILD_HANGAR)
				{
					if (client->country == building->country)
					{
						client->structstod--;

						CPrintf(client->country,
						RADIO_GREEN, "ALERT!!! ALERT!!! %s destroyed friendly structure at %s%d", client->longnick, (building->fieldtype > FIELD_SUBMARINE && building->fieldtype < FIELD_WB3POST) ? "C" : "F",
								building->field);

						if (!client->tkstatus) // if player is not TK yet, increase tklimit
						{
							if (teamkiller->value)
								client->tklimit++;
						}

						if (client->tklimit > 5)
						{
							if (!client->tkstatus)
								Cmd_TK(client->longnick, TRUE, NULL);
							else
								; // TODO: FIXME: BAN CLIENT UNTIL END OF TOD
						}
					}
					else
					{
						client->structstod++;
					}

					if (client->country != building->country)
					{
						ScoresEvent(SCORE_STRUCTURE, client, building->type);
					}
					else
					{
						ScoresEvent(SCORE_STRUCTURE, client, (int32_t)(-1 * building->type));
					}
				}

				if ((building->fieldtype <= FIELD_SUBMARINE) || (building->fieldtype >= FIELD_WB3POST))
				{
					if (building->type >= BUILD_CV && building->type <= BUILD_CARGO)
					{
						SinkBoat(FALSE, building, NULL);
						PPrintf(client, RADIO_YELLOW, "You destroyed %s", GetBuildingType(building->type));

						Com_Printf(VERBOSE_ALWAYS, "%s destroyed %s at F%d\n", client->longnick, GetBuildingType(building->type), building->field);
					}
					else if (IsVitalBuilding(building, oldcapt->value))
					{
						PPrintf(client, RADIO_YELLOW, "You destroyed %s for %s", GetBuildingType(building->type), Com_TimeSeconds(building->timer/100));

						Com_Printf(VERBOSE_ALWAYS, "%s destroyed %s at F%d for %s\n", client->longnick, GetBuildingType(building->type), building->field, Com_TimeSeconds(building->timer /100));
					}

					if (!arcade->value)
					{
						if (building->type == BUILD_HANGAR)
						{
							ReducePlanes(building->field);
						}
						else if (building->type == BUILD_WARE)
						{
							arena->fields[building->field - 1].warehouse = arena->frame;
							// IncreaseAcksReup(building->field); // obsolete
						}
					}
				}
				else
				{
					factorybuildingsup[building->country - 1]--;
					PPrintf(client, RADIO_YELLOW, "You destroyed %s", GetBuildingType(building->type));
					Com_Printf(VERBOSE_ALWAYS, "%s destroyed %s at C%d\n", client->longnick, GetBuildingType(building->type), building->field);
				}
			}

			SetBuildingStatus(building, building->status, NULL);
		}
		else
		{
			building->armor -= dmgprobe;
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}
	// radio alert

	if (!setjmp(debug_buffer))
	{
		if (((building->fieldtype <= FIELD_SUBMARINE) || (building->fieldtype >= FIELD_WB3POST)) && !arena->fields[building->field - 1].alert)
		{
			arena->fields[building->field - 1].alert = arena->frame;
		}
	}
	else
	{
		DebugClient(__FILE__, __LINE__, TRUE, client);
	}

	return 1;
}

/**
 SendFieldStatus

 Send status of all buildings in the field/city
 */

void SendFieldStatus(u_int16_t field, client_t *client)
{
	u_int16_t i;

	if (field < fields->value)
	{
		for (i = 0; i < MAX_BUILDINGS; i++)
		{
			if (arena->fields[field].buildings[i].field)
			{
				if (arena->fields[field].buildings[i].status)
				{
					if ((arena->fields[field].buildings[i].type > BUILD_CV) && (arena->fields[field].buildings[i].type <= BUILD_SUBMARINE))
						SinkBoat(FALSE, &arena->fields[field].buildings[i], client);

					SetBuildingStatus(&arena->fields[field].buildings[i], 2, client);
				}
				else
				{
					SetBuildingStatus(&arena->fields[field].buildings[i], 0, client);
				}
			}
			else
				break;
		}
	}
	else
	{
		field -= fields->value;

		if (field < cities->value)
		{
			for (i = 0; i < MAX_BUILDINGS; i++)
			{
				if (arena->cities[field].buildings[i].field)
				{
					if (arena->cities[field].buildings[i].status)
					{
						SetBuildingStatus(&arena->cities[field].buildings[i], 2, client);
					}
					else
					{
						SetBuildingStatus(&arena->cities[field].buildings[i], 0, client);
					}
				}
				else
					break;
			}
		}
	}
}

/**
 SetBuildingStatus

 Destroy an structure
 */

void SetBuildingStatus(building_t *building, u_int8_t status, client_t *client)
{
	buildstatus_t *buildstatus;
	u_int8_t i;
	u_int8_t buffer[6];

	memset(buffer, 0, sizeof(buffer));

	buildstatus = (buildstatus_t *) buffer;

	buildstatus->packetid = htons(Com_WBhton(0x0307));
	buildstatus->building = htons(building->id);
	buildstatus->status = status;
	buildstatus->country = building->country;

	if (client)
	{
		SendPacket(buffer, sizeof(buffer), client);
	}
	else
	{
		memset(arena->thaisent, 0, sizeof(arena->thaisent));

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone && clients[i].ready)
			{
				if(clients[i].thai) // SetBuildingStatus
				{
					if(arena->thaisent[clients[i].thai].b)
						continue;
					else
						arena->thaisent[clients[i].thai].b = 1;
				}

				SendPacket(buffer, sizeof(buffer), &clients[i]);
			}
		}
	}
}

/**
 SetPlaneDamage

 Set the max damage all structure can support in selected airplane
 */

void SetPlaneDamage(u_int16_t plane, client_t *client)
{
	u_int8_t i;

	for (i = 0; i < MAX_PLACE; i++)
	{
		client->armor.points[i] = arena->planedamage[plane].points[i];
		client->armor.apstop[i] = arena->planedamage[plane].apstop[i];
		client->armor.imunity[i] = arena->planedamage[plane].imunity[i];
		client->armor.parent[i] = arena->planedamage[plane].parent[i];
	}
}

/**
 PFireMG

 Process client informing he's firing a MG
 */

void PFireMG(u_int8_t *buffer, u_int8_t len, client_t *client)
{
	firemg_t *firemg;
	wb3firemg_t *wb3firemg;
	u_int32_t guntype;
	u_int8_t i, j;

	wb3firemg = (wb3firemg_t *)buffer;
	firemg = (firemg_t *)buffer;

	firemg->packetid = htons(Com_WBhton(0x1909));

	if (wb3->value)
	{
		i = wb3firemg->slot;
		guntype = ntohl(wb3firemg->sumguntype);
		firemg->sumguntype = htonl(guntype);
		firemg->slot = i;
	}

	for (i = 0; i < MAX_RELATED; i++)
	{
		if (client->related[i] && (client->related[i]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
		{
			PFireMG(buffer, len, client->related[i]);
		}
	}

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{
					firemg->slot = j;

					SendPacket(buffer, len, &clients[i]);

					break;
				}
			}
		}
	}
}

/**
 POttoFiring

 Process client informing its ottos are firing
 */

void POttoFiring(u_int8_t *buffer, u_int8_t len, client_t *client)
{
	ottofiring_t *otto;
	u_int8_t i, j, k;

	otto = (ottofiring_t *)buffer;

	otto->packetid = htons(Com_WBhton(0x1900));

	for (i = 0; i < maxentities->value; i++)
	{
		if (clients[i].inuse && !clients[i].drone)
		{
			for (j = 0; j < MAX_SCREEN; j++)
			{
				if (!clients[i].visible[j].client)
					continue;

				if (client == clients[i].visible[j].client)
				{
					SendPacket(buffer, len, &clients[i]);

					for (k = 0; k < MAX_RELATED; k++)
					{
						if (client->related[k] && (client->related[k]->drone & (DRONE_WINGS1 | DRONE_WINGS2)))
						{
							otto->posx = htonl(client->related[k]->posxy[0][0]);
							otto->posy = htonl(client->related[k]->posxy[1][0]);
							otto->alt = htonl(client->related[k]->posalt[0]);
							otto->shortnick = htonl(client->related[k]->shortnick);
							SendPacket(buffer, len, &clients[i]);
						}
					}
					otto->shortnick = htonl(client->shortnick);

					break;
				}
			}
		}
	}
}

/**
 SendForceStatus

 Force client plane status
 */

void SendForceStatus(u_int32_t status_damage, u_int32_t status_status, client_t *client)
{
	u_int8_t buffer[10];
	u_int8_t i;
	planestatus1_t *status;

	if (!client || !client->infly)
		return;

	// DM log

	for(i = 0; i < 32; i++)
	{
		if(status_damage & (1 << i))
		{
			Com_Printf(VERBOSE_ALWAYS, "%s lost %s\n", client->longnick, GetHitSite(i));
		}
	}

	// Begin parse status_damage
	if (status_damage & STATUS_REARFUSE)
	{
		status_damage |= STATUS_HSTAB | STATUS_VSTAB;

		client->armor.points[PLACE_HSTAB] = 0;
		client->armor.points[PLACE_VSTAB] = 0;
	}

	if (status_damage & STATUS_HSTAB)
	{
		status_damage |= STATUS_ELEVATOR;

		client->armor.points[PLACE_ELEVATOR] = 0;
	}

	if (status_damage & STATUS_VSTAB)
	{
		status_damage |= STATUS_RUDDER;

		client->armor.points[PLACE_RUDDER] = 0;
	}

	if (status_damage & STATUS_LWING)
	{
		status_damage |= STATUS_LAILERON;

		client->armor.points[PLACE_LAILERON] = 0;
	}

	if (status_damage & STATUS_RWING)
	{
		status_damage |= STATUS_RAILERON;

		client->armor.points[PLACE_RAILERON] = 0;
	}
	// End parse status_damage

	client->status_damage |= status_damage;
	client->status_status |= status_status;

//	if (client->drone)
//	{
		PPlaneStatus(NULL, client);
//	}
//	else
	if(!client->drone)
	{
		memset(buffer, 0, sizeof(buffer));

		status = (planestatus1_t *)buffer;

		status->packetid = htons(Com_WBhton(0x0E02));
		status->status_damage = htonl(status_damage);
		status->status_status = htonl(status_status);

		SendPacket(buffer, sizeof(buffer), client);
	}
}

/**
 SendPlaneStatus

 Update status of a visible air plane
 */

void SendPlaneStatus(client_t *plane, client_t *client)
{
	u_int8_t buffer[11];
	planestatus2_t *status;

	memset(buffer, 0, sizeof(buffer));

	status = (planestatus2_t *)buffer;

	status->packetid = htons(Com_WBhton(0x0002));
	status->slot = GetSlot(plane, client);

	status->status_damage = ntohl(plane->status_damage);
	status->status_status = ntohl(plane->status_status);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 PRadioMessage

 Process message incomming from radio (that can be a dot command)
 */

void PRadioMessage(u_int8_t *buffer, client_t *client)
{
	radiomessage_t *radiomessage;
	char *message;
	u_int8_t size;
	u_int32_t msgto, msgfrom;

	radiomessage = (radiomessage_t *)buffer;

	if (!radiomessage->msgsize)
		return;

	msgto = ntohl(radiomessage->msgto);
	msgfrom = ntohl(radiomessage->msgfrom);
	message = &(radiomessage->message);
	size = radiomessage->msgsize;

	if (!size)
	{
		Com_Printf(VERBOSE_WARNING, "%s(%s) sent message with size Zero\n", client->longnick, client->ip);
		return;
	}

	if (msgfrom < 1000)
	{
		Com_Printf(VERBOSE_WARNING, "%s(%s) invalid nick (%X)\n", client->longnick, client->ip, msgfrom);
		return;
	}

	if (msgfrom == client->shortnick)
	{
		if (size > (MAX_RADIOMSG - 11))
			size = (MAX_RADIOMSG - 12);

		if (!strncasecmp(client->lastmsg, message, size)) // possible flood
		{
			if ((arena->time - client->lastmsgtime) < 2000)
				return;
		}

		memset(client->lastmsg, 0, MAX_RADIOMSG);
		strncpy(client->lastmsg, message, size);
		client->lastmsgtime = arena->time;

		if (*message == '.')
		{
			if (!setjmp(debug_buffer))
			{
				PRadioCommand(message, size, client);
			}
			else
			{
				DebugClient(__FILE__, __LINE__, TRUE, client);
			}
		}
		else
		{
			if(client->tkstatus)
				{
					size += 4;
					message -= 4;
					message[0] = '(';
					message[1] = 'T';
					message[2] = 'K';
					message[3] = ')';
				}

				PrintRadioMessage(msgto, msgfrom, message, size, client);
			}
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "%s(%s) message nick (%X) doesnt match with ingame nick (%X)\n", client->longnick, client->ip, msgfrom, client->shortnick);
		}
	}

	/**
	 WB3DotCommand

	 Send a Dot command to client
	 */

void WB3DotCommand(client_t *client, char *fmt, ...)
{
	va_list argptr;
	char msg[MAX_PRINTMSG];
	u_int8_t buffer[128];
	wb3dotcmd_t *wb3dotcmd;
	u_int8_t i;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	wb3dotcmd = (wb3dotcmd_t *)buffer;

	memset(buffer, 0, sizeof(buffer));

	wb3dotcmd->packetid = htons(Com_WBhton(0x1205));
	wb3dotcmd->msgsize = strlen(msg);

	memcpy(&(wb3dotcmd->message), msg, wb3dotcmd->msgsize);

	if (client)
	{
		SendPacket(buffer, wb3dotcmd->msgsize + 7, client);
	}
	else
	{
		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone)
			{
				SendPacket(buffer, wb3dotcmd->msgsize + 7, &clients[i]);
			}
		}
	}
}

/**
 PrintRadioMessage

 Prints the message to all clients on frequency 'msgto' or to a single player
 */

void PrintRadioMessage(u_int32_t msgto, u_int32_t msgfrom, char *message, u_int8_t msgsize, client_t *client)
{
	u_int8_t buffer[MAX_RADIOMSG]; // 74 = 2 bytes (packet id -> 0x1200) maximum msgsize (64) + 4 bytes (msgto) + 4 bytes (msgfrom) + 1 byte (msgsize)
	u_int8_t i;
	short int n;
	client_t *toClient;
	radiomessage_t *radiomessage;

	if(!msgto)
	{
		PPrintf(client, RADIO_YELLOW, "You are talking to radio Zero, please change channel using [.radio X Y]");
		return;
	}

	if (msgto == 201 && !(client->attr & (FLAG_ADMIN | FLAG_OP)))
	{
		PPrintf(client, RADIO_YELLOW, "Superuser channel - not permitted");
		return;
	}

	if (!broadcast->value && msgto == 100)
	{
		PPrintf(client, RADIO_YELLOW, "Broadcast channel disabled");
		return;
	}

	memset(buffer, 0, sizeof(buffer));

	//	buffer[0] = 0x12; // set packet ID
	radiomessage = (radiomessage_t *)buffer;

	radiomessage->packetid = htons(Com_WBhton(0x1200));
	radiomessage->msgto = htonl(msgto);
	radiomessage->msgfrom = htonl(msgfrom);
	radiomessage->msgsize = ((msgsize>64) ? 64 : msgsize);
	memcpy(&(radiomessage->message), message, radiomessage->msgsize);

	if (msgto > 0xFF) // Private Message
	{
		radiomessage->msgto = htonl(0xC8);

		if ((toClient = FindSClient(msgto)))
		{
			if (toClient->drone)
			{
				PPrintf(client, RADIO_YELLOW, "You can't send a message to drones");
				return;
			}

			if(!(client->squadron == 732 /*vlamik*/ && toClient->squadron == 1581 /*robtec*/) &&
				!(client->squadron == 1581 /*robtec*/ && toClient->squadron == 732 /*vlamik*/))
			{
				SendPacket(buffer, radiomessage->msgsize + 11, toClient);
				SendPacket(buffer, radiomessage->msgsize + 11, client);

				message[msgsize] = '\0';

				Com_Printf(VERBOSE_CHAT, "%s:(%s)%s\n", client->longnick, toClient->longnick, message);
			}
		}
		else
		{
			PPrintf(client, RADIO_YELLOW, "User is not online");
		}

	}
	else // General Message
	{
		if ((msgto == 110) && !client->squadron)
		{
			PPrintf(client, RADIO_YELLOW, "You are not in any squadron");
			return;
		}

		n = 0;

		if (msgto == 111)
		{
			n = 1;

			if (client->attached)
			{
				if (client->attached->drone)
				{
					PPrintf(client, RADIO_YELLOW, "You can't chat with drones");
					return;
				}
				else
				{
					for (i = 0; i < 7; i++)
					{
						if (client->attached->gunners[i] && client->attached->gunners[i]->infly)
						{
							SendPacket(buffer, radiomessage->msgsize + 11, client->attached->gunners[i]);
						}
					}

					SendPacket(buffer, radiomessage->msgsize + 11, client->attached);
				}
			}
			else
			{
				if (HaveGunner(client->plane) && client->infly)
				{
					for (i = 0; i < 7; i++)
					{
						if (client->gunners[i] && client->gunners[i]->infly)
						{
							SendPacket(buffer, radiomessage->msgsize + 11, client->gunners[i]);
							n = 2;
						}
					}

					if (n == 2)
					{
						SendPacket(buffer, radiomessage->msgsize + 11, client);
					}
					else
					{
						PPrintf(client, RADIO_YELLOW, "There are no players attached");
					}
				}
			}
		}

		if (!n)
		{
			memset(arena->thaisent, 0, sizeof(arena->thaisent));

			for (i = 0; i < maxentities->value; i++)
			{
				if (clients[i].inuse && !clients[i].drone && clients[i].ready)
				{
					if ((n=CanHear(client, &clients[i], msgto)) > 0)
					{
						if((client->squadron == 732 /*vlamik*/ && clients[i].squadron == 1581 /*robtec*/) ||
							(clients[i].squadron == 1581 /*robtec*/ && client->squadron == 732 /*vlamik*/))
							continue;

						if(clients[i].thai) // PrintRadioMessage
						{
							if(arena->thaisent[clients[i].thai].b)
								continue;
							else
								arena->thaisent[clients[i].thai].b = 1;
						}

						SendPacket(buffer, radiomessage->msgsize + 11, &clients[i]);
					}
				}
				if (n<0)
					break;
			}
		}

		if (n >= 0)
		{
			message[msgsize] = '\0';
			Com_Printf(VERBOSE_CHAT, "%s:(%d)%s\n", client->longnick, msgto, message);
		}
	}
}

/**
 PNewNick

 Get player nick and update user profile
 */

void PNewNick(u_int8_t *buffer, client_t *client)
{
	char loginname[32];
	char nickname[7];
	char tempnick[7];
	u_int8_t packet[32];
	u_int8_t found = 0;
	FILE *fp;
	insertnick_t *nickpacket;

	memset(loginname, 0, sizeof(loginname));
	memset(packet, 0, sizeof(packet));

	nickpacket = (insertnick_t *)packet;

	memcpy(loginname, buffer+1, buffer[0]);
	loginname[31] = '\0';

	Com_Printf(VERBOSE_ALWAYS, "LOGINNAME %s\n", loginname);

	memset(nickname, 0, sizeof(nickname));
	memcpy(nickname, buffer+2+buffer[0], 6);

	strncpy(nickname, wbnick2ascii(ascii2wbnick(nickname, 0)), 6);

	Com_Printf(VERBOSE_ALWAYS, "NICK %s\n", nickname);

	nickpacket->packetid = htons(Com_WBhton(0x2001));

	if (!(strstr(nickname, "=") || (ascii2wbnick(nickname, 0) < 1000)))
	{
		sprintf(my_query, "SELECT longnick FROM players WHERE longnick = '%s'", nickname);

		if (!d_mysql_query(&my_sock, my_query)) // query succeeded
		{
			if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
			{
				if (!mysql_num_rows(my_result))
				{
					mysql_free_result(my_result);
					my_result = NULL;

					// Search in DroneNicks

					Sys_WaitForLock(FILE_DRONENICKS_LOCK);

					if (!(Sys_LockFile(FILE_DRONENICKS_LOCK) < 0))
					{
						if ((fp = fopen(FILE_DRONENICKS, "r")))
						{
							while (fgets(tempnick, sizeof(tempnick), fp) != NULL)
							{
								if (!Com_Strncmp(tempnick, nickname, 6))
								{
									found = 1;
									break;
								}
							}
							fclose(fp);
							Sys_UnlockFile(FILE_DRONENICKS_LOCK);

							if (!found)
							{
								sprintf(my_query, "SELECT id FROM players WHERE loginuser = '%s'", client->loginuser);

								if (!d_mysql_query(&my_sock, my_query)) // query succeeded
								{
									if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
									{
										if ((my_row = mysql_fetch_row(my_result)))
										{
											client->id = Com_Atou(Com_MyRow("id"));

											mysql_free_result(my_result);
											my_result = NULL;

											client->shortnick = ascii2wbnick(nickname, 0);
											strcpy(client->longnick, wbnick2ascii(client->shortnick));

											sprintf(my_query, "UPDATE players SET longnick = '%s' WHERE id = '%u'", client->longnick, client->id);

											if (!d_mysql_query(&my_sock, my_query))
											{
												Com_LogEvent(EVENT_CREATE, client->id, 0);
												Com_LogDescription(EVENT_DESC_PLIP, 0, client->ip);
												Com_LogDescription(EVENT_DESC_PLCTRID, client->ctrid, 0);

												nickpacket->allow = 1;
												nickpacket->msgsize = 0;
												SendPacket(packet, 4, client);
												SendCopyright(client);
											}
											else
											{
												Com_Printf(VERBOSE_WARNING, "PNewNick(): couldn't query UPDATE error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
												nickpacket->msgsize = 23;
												memcpy( &(nickpacket->msg), "SQL Error query(update)", 23);
											}
										}
										else
										{
											mysql_free_result(my_result);
											my_result = NULL;

											Com_Printf(VERBOSE_WARNING, "PNewNick(id): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
											nickpacket->msgsize = 19;
											memcpy(&(nickpacket->msg), "SQL Error fetch(id)", 19);
										}
									}
									else
									{
										Com_Printf(VERBOSE_WARNING, "PNewNick(id): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

										nickpacket->msgsize = 20;
										memcpy(&(nickpacket->msg), "SQL Error result(id)", 20);
									}
								}
								else
								{
									Com_Printf(VERBOSE_WARNING, "PNewNick(id): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

									nickpacket->msgsize = 19;
									memcpy(&(nickpacket->msg), "SQL Error query(id)", 19);
								}
							}
							else
							{
								nickpacket->msgsize = 19;
								memcpy(&(nickpacket->msg), "Nick used by drones", 19);
							}
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "Couldn't open file \"%s\"\n",
							FILE_DRONENICKS);
							Sys_UnlockFile(FILE_DRONENICKS_LOCK);

							nickpacket->msgsize = 21;
							memcpy(&(nickpacket->msg), "Error open drone nick", 21);
						}
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "Couldn't lock file \"%s\"\n",
						FILE_DRONENICKS);

						nickpacket->msgsize = 21;
						memcpy(&(nickpacket->msg), "Error lock drone nick", 21);
					}
				}
				else
				{
					mysql_free_result(my_result);
					my_result = NULL;

					nickpacket->msgsize = 19;
					memcpy(&(nickpacket->msg), "Nick already in use", 19);
				}
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "PNewNick(nick): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

				nickpacket->msgsize = 22;
				memcpy(&(nickpacket->msg), "SQL Error result(nick)", 22);
			}
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "PNewNick(nick): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));

			nickpacket->msgsize = 21;
			memcpy(&(nickpacket->msg), "SQL Error query(nick)", 21);
		}
	}
	else
	{
		nickpacket->msgsize = 12;
		memcpy(&(nickpacket->msg), "Invalid Nick", 12);
	}

	nickpacket->allow = 0;
	SendPacket(packet, 4+nickpacket->msgsize, client);
}

/**
 SendCopyright

 Sends copyright packet to client
 */

int32_t SendCopyright(client_t *client)
{
	copyrighta_t *copyrighta;
	copyrightb_t *copyrightb;
	u_int8_t buffer[100];
	u_int8_t offset;

	memset(buffer, 0, sizeof(buffer));

	copyrighta = (copyrighta_t *)(buffer);

	if (wb3->value && client->loginkey)
	{
		copyrighta->packetid = htons(Com_WBhton(0x0D00));// htons(0x0D00); // 2007
		copyrighta->gameversion = htonl(0x4B54B);// 2008 //htonl(0x4B0F4); // 2007
	}
	else
	{
		copyrighta->packetid = htons(0x0C00);
		copyrighta->gameversion = htonl(0x043B55);
	}
	copyrighta->nicksize = strlen(client->longnick);
	if (copyrighta->nicksize)
		memcpy(&(copyrighta->nick), client->longnick, copyrighta->nicksize);

	offset = 7+copyrighta->nicksize;
	buffer[offset++] = 0x3D;

	if (wb3->value && client->loginkey)
		memcpy( &(buffer[offset]), "Copyright (C) 2000 iEntertainment Network All Rights Reserved", 61);
	else
		memcpy( &(buffer[offset]), "Copyright (C) 2001 iEntertainment Network All Rights Reserved", 61);

	copyrightb = (copyrightb_t *)(buffer+offset+61);
	copyrightb->cryptkey = htonl(client->key);
	copyrightb->mapnamesize = strlen(mapname->string); // mapnamesize
	memcpy(&(copyrightb->mapname), mapname->string, copyrightb->mapnamesize); // mapname

	offset = offset+66+copyrightb->mapnamesize;

	if (wb3->value && client->loginkey) // gameversion
	{
		buffer[offset++] = 0x00;
		buffer[offset++] = 0x04;
		buffer[offset++] = 0xB5;
		buffer[offset] = 0x4B;
	}
	else // gameversion
	{
		buffer[offset++] = 0x00;
		buffer[offset++] = 0x04;
		buffer[offset++] = 0x3B;
		buffer[offset] = 0x55;
	}

	return SendPacket(buffer, 78+copyrighta->nicksize+copyrightb->mapnamesize, client);
}

/**
 UpdateIngameClients

 Update file with all ingame clients / OP's or Admins
 */

void UpdateIngameClients(u_int8_t attr)
{
	u_int8_t i = 0, j = 0, k = 0;
	char file[128];
	FILE *fp;

	if (attr & FLAG_OP)
	{
		strcpy(file, FILE_OP);
	}
	else if (attr & FLAG_ADMIN)
	{
		strcpy(file, FILE_ADMIN);
	}
	else
	{
		strcpy(file, FILE_INGAME);
	}

	strcat(file, ".LOCK");

	Sys_WaitForLock(file);

	if (Sys_LockFile(file) < 0)
		return;

	file[strlen(file) - 5] = '\0';

	if (!(fp = fopen(file, "wb")))
	{
		Com_Printf(VERBOSE_WARNING, "Couldn't create file \"%s\"\n", file);
	}
	else
	{
		fprintf(fp, " Callsign   Side    Status     Country   Connection\n");
		fprintf(fp, "====================================================\n");

		for (i = 0, j = 0; i < maxentities->value; i++)
		{
			if (clients[i].attr == 1 && hideadmin->value)
				continue;

			if (clients[i].inuse && !clients[i].drone && (attr ? clients[i].attr & attr : 1))
			{
				j++;
				fprintf(fp, "  %-10s%-7s", clients[i].longnick, GetCountry(clients[i].country));

				if(clients[i].loginkey == 1)
				{
					fprintf(fp, "In Chat     ");
				}
				else if (clients[i].infly)
				{
					if (IsGround(&clients[i]))
						fprintf(fp, "In Ground   ");
					else
						fprintf(fp, "In Flight   ");
				}
				else
				{
					if (clients[i].field)
					{
						if (!clients[i].hq)
							fprintf(fp, "FIELD F%03d  ", clients[i].field);
						else
							fprintf(fp, "HQ          ");
					}
					else
						fprintf(fp, "CONNECTING  ");
				}

				fprintf(fp, "%-10s%s\n", GeoIP_country_name_by_addr(gi, clients[i].ip),
						!(clients[i].cancollide)?"":clients[i].connection == 0?"Stable":clients[i].connection == 1?"Fair":clients[i].connection == 2?"Unstable":"Poor");

				for (k = 0; k < maxentities->value; k++) // add sharing IP
				{
					if (clients[k].inuse && !clients[k].drone && clients[k].ready && &clients[i] != &clients[k] && !clients[k].thai)
					{
						if (!strcmp(clients[k].ip, clients[i].ip))
						{
							fprintf(fp, "\\--> Sharing IP with %s\n", clients[k].longnick);
							break;
						}
					}
				}
			}
		}
	}

	if (!j)
	{
		fprintf(fp, "             There is no player online\n");
	}
	fclose(fp);

	strcat(file, ".LOCK");

	Sys_UnlockFile(file);
}

/**
 GetCountry

 Return country string using country number
 */

char *GetCountry(u_int8_t country)
{
	switch (country)
	{
		case 1:
			return "Red";
		case 2:
			return "Green";
		case 3:
			return "Gold";
		case 4:
			return "Purple";
		default:
			return "Unknown";
	}
}

/**
 GetRanking

 Return ranking string using ranking number
 */

char *GetRanking(u_int8_t ranking)
{
	switch (ranking)
	{
		case 0:
			return "Novice";
		case 1:
			return "Amateur";
		case 2:
			return "Average";
		case 3:
			return "Veteran";
		case 4:
			return "Ace";
		default:
			return "Unknown";
	}
}

/**
 SendArenaNames

 Sends All opened arena's names
 */

int32_t SendArenaNames(client_t *client)
{
	char buffer[1024]; // MAX_ARENASLIST * X
	arenaslist_t *p_arenas_temp, *p_arenas[MAX_ARENASLIST];
	u_int16_t offset = 0;
	u_int8_t i, j;
	int32_t n = 0;

	memset(buffer, 0, sizeof(buffer));

	/*
	 Packet layout:

	 l - packet size
	 s20 - mapname
	 b   - \0
	 s40 - hostdomain
	 b   - \0
	 s50 - hostname
	 b   - \0
	 l - numplayers
	 l - maxclients
	 ----
	 l - shortnick
	 b - country
	 ----

	 struct
	 {
	 char		mapname[20];
	 char		hostdomain[40];
	 char		hostname[50];
	 u_int32_t	numplayers;
	 u_int32_t	maxclients;
	 u_int8_t	array;
	 }
	 */

	//	memcpy(&(buffer[offset]), hostname->string, strlen(hostname->string));
	//	offset += strlen(hostname->string);
	//	buffer[offset++] = ':';
	//	memcpy(&(buffer[offset]), hostdomain->string, strlen(hostdomain->string));
	//	offset += strlen(hostdomain->string);
	//	buffer[offset++] = ':';
	//	memcpy(&(buffer[offset]), dirname->string, strlen(dirname->string));
	//	offset += strlen(dirname->string);
	//	buffer[offset++] = ':';
	//	memcpy(&(buffer[offset]), "Free", 4);
	//	offset += 4;
	//	buffer[offset++] = ':';
	//	memcpy(&(buffer[offset]), maxclients->string, strlen(maxclients->string));
	//	offset += strlen(maxclients->string);
	//	buffer[offset++] = ':';
	//	sprintf((char*)&(buffer[offset]), "%d", arena->numplayers);
	//	while(buffer[offset] != 0)
	//		offset++;
	//	buffer[offset++] = ':';
	//	memcpy(&(buffer[offset]), "00000000", 8);
	//	offset += 8;
	//	buffer[offset++] = ':';
	//	if(wb3->value)
	//	{
	//		memcpy(&(buffer[offset]), "arnalst3", 8);
	//		offset += 8;
	//	}
	//	else
	//	{
	//		memcpy(&(buffer[offset]), "arnalst2", 8);
	//		offset += 8;
	//	}
	//	buffer[offset++] = ':';
	//	if(wb3->value)
	//	{
	//		memcpy(&(buffer[offset]), mapname->string, strlen(mapname->string));
	//		offset += strlen(mapname->string);
	//	}
	//	buffer[offset++] = 0x0a;
	//
	//	buffer[offset++] = 0x2a;
	//	buffer[offset++] = 0x0a;
	//
	//	n = Com_Send(client->socket, buffer, offset);
	//
	//	return n;

	/*
	 hostname->string,
	 hostdomain->string,
	 dirname->string,
	 (u_int16_t)maxclients->value,
	 arena->numplayers,
	 wb3->value?"arnalst3":"arnalst2",
	 wb3->value?mapname->string:"\n");
	 */

	for (i = 0; i < MAX_ARENASLIST; i++)
	{
		p_arenas[i] = &(arenaslist[i]);
	}

	do // Bubble sort
	{
		i = 0;
		for (j = 1; j < MAX_ARENASLIST; j++)
		{
			if (p_arenas[j-1]->logintime > p_arenas[j]->logintime)
			{
				p_arenas_temp = p_arenas[j-1];
				p_arenas[j-1] = p_arenas[j];
				p_arenas[j] = p_arenas_temp;
				i++;
			}
		}
	} while (i);

	// make compatible with WB 2008, remove to WB2007
	if (wb3->value == 2)
	{
		sprintf( &(buffer[offset]), "Copyright (C) 2000 iEntertainment Network All Rights Reserved\n$Copyright (C) 2004-2009 Tabajara Host\n$Welcome to the Free Public Arenas!\n$\n");
		offset += 139;
	}

	for (i = 0; i < MAX_ARENASLIST; i++)
	{
		if (p_arenas[i]->time)
		{
			// 2008: MAIN:72.232.165.203:wb3abmain:0:300:025:3004:arnabob:atoll
			//       Practice:72.232.165.203:wfprac:0:300:000:3005:arnalst3:atoll
			// %s:%s:%d:Free:%d:%d:00000000:%s:%s%s

			sprintf(&(buffer[offset]), "%s:%s:%d:Free:%d:%d:00000000:%s:%s%s", p_arenas[i]->hostname, p_arenas[i]->hostdomain, p_arenas[i]->logintime, // dirname->string,
					p_arenas[i]->maxclients, p_arenas[i]->numplayers, wb3->value ? "arnalst3" : "arnalst2", wb3->value ? p_arenas[i]->mapname : "\n", wb3->value ? "\n" : "");

			for (; offset < 1024; offset++)
			{
				if (!buffer[offset])
					break;
			}
		}
	}

	sprintf(&(buffer[offset]), "*\n");
	offset += 2;

	n = Com_Send(client, buffer, (int)offset);

	/*
	 for(i = 0, i < 4, i++)
	 {
	 memcpy(&(buffer[offset]), arenalist[i].string, arenalist[i].size);
	 offset += arenalist[i].size;
	 }
	 */

	return n;
}

/**
 SendPlayerNames

 Sends players names
 */

void SendPlayersNames(client_t *client)
{
	u_int8_t buffer[1024 * MAX_ARENASLIST]; // supports 204 nicks/players * 5 arenas (5120)
	u_int16_t offset, temp;
	//	u_int16_t nplayers;
	//	playerslist_t *Plist;
	arenaslist_t *p_arenas_temp, *p_arenas[MAX_ARENASLIST];
	u_int8_t i, j;

	for (i = 0; i < MAX_ARENASLIST; i++)
	{
		p_arenas[i] = &(arenaslist[i]);
	}

	do // Bubble sort
	{
		i = 0;
		for (j = 1; j < MAX_ARENASLIST; j++)
		{
			if (p_arenas[j-1]->logintime > p_arenas[j]->logintime)
			{
				p_arenas_temp = p_arenas[j-1];
				p_arenas[j-1] = p_arenas[j];
				p_arenas[j] = p_arenas_temp;
				i++;
			}
		}
	} while (i);

	memset(buffer, 0, sizeof(buffer));
	temp = offset = 0;

	for (i = 0, j = 0; i < MAX_ARENASLIST; i++)
	{
		if (p_arenas[i]->time)
		{
			buffer[offset++] = j; // arena index
			*(int16_t *) (buffer+offset) = htons(p_arenas[i]->numplayers);
			offset += 2;

			memcpy(&buffer[offset], p_arenas[i]->array, ((p_arenas[i]->numplayers * 5) > 1000) ? 1000 : (p_arenas[i]->numplayers * 5)); // nicks+country list

			offset += ((p_arenas[i]->numplayers * 5) > 1000) ? 1000 : (p_arenas[i]->numplayers * 5);
			j++;
		}
	}

	buffer[offset++] = 0xff;

	Com_Send(client, buffer, offset);
}

/**
 SendIdle

 Send a keep alive packet
 */

void SendIdle(client_t *client)
{
	u_int8_t buffer[6];
	idle_t *idle;

	memset(buffer, 0, sizeof(buffer));
	idle = (idle_t *) buffer;

	idle->packetid = htons(Com_WBhton(0x1D04));
	idle->timer = htonl(arena->time);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 SendPlayersNear

 Updates all near planes status
 */

void SendPlayersNear(client_t *client)
{
	register int16_t mid, i, j, k;
	client_t *carray[MAX_ENTITIES];
	client_t *temp;

	memset(carray, 0, sizeof(carray));
	//	for(i = 0; i < maxentities->value; i++)
	//		carray[i] = NULL;

	/* calculate distance */
	for (k = i = 0; i < maxentities->value; i++)
	{
		if (client != &clients[i] && clients[i].inuse && clients[i].infly && !clients[i].attached && !(clients[i].drone == DRONE_EJECTED && clients[i].related[0] == client))
		{
			if ((clients[i].reldist = DistBetween(client->posxy[0][0], client->posxy[1][0], client->posalt[0], clients[i].posxy[0][0], clients[i].posxy[1][0], clients[i].posalt[0], MAX_INT16)) >= 0)
				carray[k++] = &clients[i];
		}
	}

	/* sort by dist*/
	for (mid = k / 2; mid > 0; mid /= 2)
	{
		for (i = mid; i < k; i++)
		{
			for (j = i - mid; j >= 0; j -= mid)
			{
				if (carray[j]->reldist <= carray[j+mid]->reldist) /* compare */
					break;
				temp = carray[j]; /* permute */
				carray[j] = carray[j+mid];
				carray[j+mid] = temp;
			}
		}
	}

	k = (k < (MAX_SCREEN - 1) ? k : (MAX_SCREEN - 1));

	for (j = 0; j < MAX_SCREEN; j++) /* check for removing from list */
	{
		if ((j != (MAX_SCREEN - 1)) || (FLIGHT_TIME(client) > 15000))
		{
			if (client->visible[j].client)
			{
				for (i = 0; i < k; i++)
				{
					if (client->visible[j].client == carray[i])
					{
						i = -1;
						break;
					}
				}
				if (i >= 0) // not found on 'nearplanes' array
				{
					AddRemovePlaneScreen(client->visible[j].client, client, TRUE); // remove plane from screen
					client->visible[j].client = NULL; // remove plane from array
				}
			}
		}
	}

	for (i = 0; i < k; i++) /* check for entering in list */
	{
		for (j = 0; j < (MAX_SCREEN - 1); j++) // MAX_SCREEN - 1 => Last slot is reserved for attach plane
		{
			if (client->attached == carray[i])
			{
				if (client->visible[MAX_SCREEN - 1].client)
				{
					j = -1;
					break;
				}
			}
			else if (carray[i] == client->visible[j].client)
			{
				j = -1;
				break;
			}
		}
		if (j >= 0) // not found on visible array
		{
			if (client->attached == carray[i])
			{
				client->visible[MAX_SCREEN - 1].client = carray[i];
			}
			else
			{
				for (j = 0; j < (MAX_SCREEN - 1); j++) // add plane to array
				{
					if (!client->visible[j].client)
					{
						client->visible[j].client = carray[i];
						break;
					}
				}
			}
			AddRemovePlaneScreen(carray[i], client, FALSE); // add plane to screen
		}
	}

	for (i = 0; i < (MAX_SCREEN - 1); i++)
	{
		if (client->visible[i].client && (client->visible[i].client->timer != client->visible[i].timer)) // if any client has been updated, send screen update
		{
			SendScreenUpdates(client);
			return;
		}
	}

	if (!((arena->frame - client->frame) % 150))
	{
		SendIdle(client);
	}
}

/**
 SendOttoParams

 Send Otto parameters
 */

void SendOttoParams(client_t *client)
{
	u_int8_t buffer[21];
	ottoparams_t *otto;

	memset(buffer, 0, sizeof(buffer));
	otto = (ottoparams_t *)buffer;

	otto->packetid = htons(Com_WBhton(0x2100));
	otto->accuracy = ottoaccuracy->value;
	otto->unknown1 = htons(0x0000);

	if (client->ackstar)
		otto->range = htons(0);
	else
		otto->range = htons((u_int16_t)ottorange->value);

	otto->burston = htons((u_int16_t)(ottoburston->value * 100));
	otto->burstonmax = htons((u_int16_t)(ottoburstonmax->value * 100));
	otto->burstoff = htons((u_int16_t)(ottoburstoff->value * 100));
	otto->retarget = htons((u_int16_t)(ottoretarget->value * 100));
	otto->adjust = htons((u_int16_t)(ottoadjust->value));
	otto->override = htonl((u_int32_t)ottooverrides->value);

	SendPacket(buffer, sizeof(buffer), client);

	SendOttoParams2(client);
}

/**
 SendOttoParams2

 Send Otto parameters
 */

void SendOttoParams2(client_t *client)
{
	u_int8_t buffer[21];
	ottoparams2_t *otto;

	memset(buffer, 0, sizeof(buffer));
	otto = (ottoparams2_t *)buffer;

	otto->packetid = htons(Com_WBhton(0x2103));
	otto->unk1 = htons(0x0000);
	otto->ottoacquirerange = htons((u_int16_t)ottoacquirerange->value);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 SendLastConfig

 Send player last configuration
 */

void SendLastConfig(client_t *client)
{
	u_int8_t buffer[34];
	lastconfig_t *lastconfig;
	wb3lastconfig_t *wb3lastconfig;

	memset(buffer, 0, sizeof(buffer));

	if (wb3->value)
	{
		wb3lastconfig = (wb3lastconfig_t *)buffer;
		wb3lastconfig->packetid = htons(Com_WBhton(0x0404));
	}
	else
	{
		lastconfig = (lastconfig_t *)buffer;
		lastconfig->packetid = htons(Com_WBhton(0x0404));
	}

	if (wb3->value)
	{
		wb3lastconfig->unk1 = htonl(0x4A00);
		wb3lastconfig->plane = htonl(0x01);
		wb3lastconfig->unk2 = htonl(0x01);
		wb3lastconfig->unk3 = htonl(0x2000);
		wb3lastconfig->unk4 = htonl(0x12);
		wb3lastconfig->country = htonl(client->country);
		wb3lastconfig->nick = htonl(client->shortnick);
		wb3lastconfig->unk5 = htonl(0);
	}
	else
	{
		lastconfig->plane = htonl(client->plane);
		lastconfig->country = htonl(client->country);
		lastconfig->nick = htonl(client->shortnick);
		lastconfig->fuel = htonl(client->fuel);
		lastconfig->conv = htonl(client->conv*3);
		lastconfig->ord = htonl(client->ord);
	}

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 AddRemovePlaneScreen

 Adds or remove a plane to client screen
 */

void AddRemovePlaneScreen(client_t *plane, client_t *client, u_int8_t remove)
{
	u_int8_t buffer[19];
	addplane_t *addplane;

	memset(buffer, 0, sizeof(buffer));
	addplane = (addplane_t *)buffer;

	addplane->packetid = htons(Com_WBhton(0x0000));

	addplane->slot = GetSlot(plane, client);

	if (remove)
	{
		addplane->remove = htons(0x0D);
	}
	else
	{
		addplane->shortnick = htonl(plane->shortnick);
		addplane->country = htonl(plane->country);
		addplane->plane = htons(plane->plane);
	}

	SendPacket(buffer, sizeof(buffer), client);
	WB3SupressFire(addplane->slot, client);
	SendPlaneStatus(plane, client);

	if(skins->value)
	{
		Com_Printf(VERBOSE_DEBUG, "WB3OverrideSkin() at AddRemovePlaneScreen()\n");
		WB3OverrideSkin(addplane->slot, client);
	}
}

/**
 SendScreenUpdates

 Update client screen with planes status
 */

void SendScreenUpdates(client_t *client)
{
	u_int8_t buffer[MAX_SCREENBUF];
	u_int8_t i, j;
	updateplane_t *updateplane;
	updateplane2_t *updateplane2;
	wb3updateplane2_t *wb3updateplane2;
	char file[128];
	FILE *fp= NULL;

	memset(buffer, 0, sizeof(buffer));
	updateplane = (updateplane_t *)buffer;

	if (wb3->value)
	{
		updateplane->packetid = htons(Com_WBhton(0x001E));
	}
	else
	{
		updateplane->packetid = htons(Com_WBhton(0x0001));
	}

	updateplane->timer = htonl(arena->time);
	updateplane->posx = htonl(((client->posxy[0][0] >> 11) << 11));
	updateplane->posy = htonl(((client->posxy[1][0] >> 11) << 11));
	updateplane->alt = htonl(((client->posalt[0] >> 9) << 9));

	snprintf(file, sizeof(file), "./logs/players/%s.screen", client->longnick);

	if ((client->lograwdata || lograwposition->value) && client->infly)
	{
		if (!(fp = fopen(file, "a")))
		{
			Com_Printf(VERBOSE_WARNING, "Couldn't append file \"%s\"\n", file);
		}
	}

	for (i = 0, j = 0; i < (MAX_SCREEN - 1); i++)
	{
		if (client->visible[i].client && (client->visible[i].client->timer != client->visible[i].timer))
		{
			if (wb3->value)
			{
				wb3updateplane2 = (wb3updateplane2_t *)(buffer+19+(22*j));

				wb3updateplane2->timeoffset = htons(arena->time - client->visible[i].client->timer);//htons(0xFFFC);

				wb3updateplane2->slot = i;
				wb3updateplane2->unk1 = 0x10;
				wb3updateplane2->relposx = htons(client->visible[i].client->posxy[0][0] - ((client->posxy[0][0] >> 11) << 11));
				wb3updateplane2->relposy = htons(client->visible[i].client->posxy[1][0] - ((client->posxy[1][0] >> 11) << 11));
				wb3updateplane2->relalt = htons(client->visible[i].client->posalt[0] - ((client->posalt[0] >> 9) << 9));

				wb3updateplane2->pitch = client->visible[i].client->angles[0][0] / 14; // Pitch Position
				wb3updateplane2->xaccel = client->visible[i].client->accelxyz[0][0] >> 2; // X Acceleration
				wb3updateplane2->prxspeed = htons(((client->visible[i].client->aspeeds[0][0] >> 6) << 9) ^ 0x8000); // Pitch Angular Speed // 7
				wb3updateplane2->prxspeed |= htons(((client->visible[i].client->speedxyz[0][0] >> 2) & 0x1FF) ^ 0x0100); // X Speed // 9
				wb3updateplane2->roll = client->visible[i].client->angles[1][0] / 14; // Roll Position
				wb3updateplane2->yaccel = client->visible[i].client->accelxyz[1][0] >> 2; // Y Acceleration
				wb3updateplane2->bryspeed = htons(((client->visible[i].client->aspeeds[1][0] >> 6) << 9) ^ 0x8000); // Roll Angular Speed // 7
				wb3updateplane2->bryspeed |= htons(((client->visible[i].client->speedxyz[1][0] >> 2) & 0x1FF) ^ 0x0100); // Y Speed // 9
				wb3updateplane2->yaw = client->visible[i].client->angles[2][0] / 14; // Yaw Position
				wb3updateplane2->zaccel = client->visible[i].client->accelxyz[2][0] >> 2; // Z Acceleration
				wb3updateplane2->yrzspeed = htons(((client->visible[i].client->aspeeds[2][0] >> 6) << 9) ^ 0x8000); // Yaw Angular Speed // 7
				wb3updateplane2->yrzspeed |= htons(((client->visible[i].client->speedxyz[2][0] >> 2) & 0x1FF) ^ 0x0100); // Z Speed // 9

//				if(IsGround(client->visible[i].client))
//				{
//					wb3updateplane2->prxspeed = htons(0x8000); // zero Pitch Angular Speed
//					wb3updateplane2->bryspeed = htons(0x8000); // zero Roll Angular Speed
//					wb3updateplane2->zaccel = 0; // zero Z Acceleration
//					wb3updateplane2->yrzspeed |= htons(0x0100); // zero Z Speed
//				}

				if (fp)
				{
					fprintf(fp, "%u;%d;%d;%d;%d;%u;%d;%d;%d;%d;%u\n",
							ntohl(updateplane->timer),
							(int32_t)ntohl(updateplane->posx),
							(int32_t)ntohl(updateplane->posy),
							(int32_t)ntohl(updateplane->alt),
							(int16_t)ntohs(wb3updateplane2->timeoffset),
							wb3updateplane2->slot,
							wb3updateplane2->unk1,
							(int16_t)ntohs(wb3updateplane2->relposx),
							(int16_t)ntohs(wb3updateplane2->relposy),
							(int16_t)ntohs(wb3updateplane2->relalt),
							Sys_Milliseconds());
				}
			}
			else
			{
				updateplane2 = (updateplane2_t *)(buffer+19+(21*j));

				updateplane2->timeoffset = htons(arena->time - client->visible[i].client->timer);//htons(0xFFFC);

				updateplane2->slot = i;
				updateplane2->relposx = htons(client->visible[i].client->posxy[0][0] - ((client->posxy[0][0] >> 11) << 11));
				updateplane2->relposy = htons(client->visible[i].client->posxy[1][0] - ((client->posxy[1][0] >> 11) << 11));
				updateplane2->relalt = htons(client->visible[i].client->posalt[0] - ((client->posalt[0] >> 9) << 9));
				updateplane2->pitch = client->visible[i].client->angles[0][0] / 14; // >> 4;
				updateplane2->xaccel = client->visible[i].client->accelxyz[0][0] >> 2;
				updateplane2->prxspeed = htons(((client->visible[i].client->aspeeds[0][0] >> 6) << 9) ^ 0x8000); // 7
				updateplane2->prxspeed |= htons(((client->visible[i].client->speedxyz[0][0] >> 2) & 0x1FF) ^ 0x0100); // 9
				updateplane2->roll = client->visible[i].client->angles[1][0] / 14; // >> 4;
				updateplane2->yaccel = client->visible[i].client->accelxyz[1][0] >> 2;
				updateplane2->bryspeed = htons(((client->visible[i].client->aspeeds[1][0] >> 6) << 9) ^ 0x8000); // 7
				updateplane2->bryspeed |= htons(((client->visible[i].client->speedxyz[1][0] >> 2) & 0x1FF) ^ 0x0100); // 9
				updateplane2->yaw = client->visible[i].client->angles[2][0] / 14; //>> 4;
				updateplane2->zaccel = client->visible[i].client->accelxyz[2][0] >> 2;
				updateplane2->yrzspeed = htons(((client->visible[i].client->aspeeds[2][0] >> 6) << 9) ^ 0x8000); // 7
				updateplane2->yrzspeed |= htons(((client->visible[i].client->speedxyz[2][0] >> 2) & 0x1FF) ^ 0x0100); // 9

				//			updateplane2->pitch = (double) client->visible[i].client->angles[0][0] * 128 / 1800;
				//			updateplane2->xaccel = (double) client->visible[i].client->accelxyz[0][0] / 4;
				//			updateplane2->prxspeed = htons(((int8_t)((double) client->visible[i].client->aspeeds[0][0] / 64) << 9) ^ 0x8000); // 7
				//			updateplane2->prxspeed |= htons(((int8_t)((double) client->visible[i].client->speedxyz[0][0] / 4) & 0x1FF) ^ 0x0100); // 9
				//			updateplane2->roll = (double) client->visible[i].client->angles[1][0] * 128 / 1800;
				//			updateplane2->yaccel = (double) client->visible[i].client->accelxyz[1][0] / 4;
				//			updateplane2->bryspeed = htons(((int8_t)((double) client->visible[i].client->aspeeds[1][0] / 64) << 9) ^ 0x8000); // 7
				//			updateplane2->bryspeed |= htons(((int8_t)((double) client->visible[i].client->speedxyz[1][0] / 4) & 0x1FF) ^ 0x0100); // 9
				//			updateplane2->yaw = (double) client->visible[i->client]->angles[2][0] * 128 / 1800;
				//			updateplane2->zaccel = (double) client->visible[i].client->accelxyz[2][0] / 4;
				//			updateplane2->yrzspeed = htons(((int8_t)((double) client->visible[i].client->aspeeds[2][0] / 64) << 9) ^ 0x8000); // 7
				//			updateplane2->yrzspeed |= htons(((int8_t)((double) client->visible[i].client->speedxyz[2][0] / 4) & 0x1FF) ^ 0x0100); // 9
			}

			client->visible[i].timer = client->visible[i].client->timer;
			j++;
		}
	}

	if (fp)
		fclose(fp);

	if (!j)
		return;

	updateplane->num = j;
	SendPacket(buffer, 20+(22*j), client);
}

/**
 CanHear

 Check if a player can hear another in radio message
 */
int CanHear(client_t *client1, client_t *client2, u_int32_t msgto)
{

	if (client2->attr & FLAG_ADMIN) // admin can hear any radio


		return 1;

	if (msgto == 100 || msgto == 201)
		return 1;

	if (msgto == 110)
	{
		if (client1->squadron == client2->squadron)
			return 1;
	}

	if (msgto > 100 && msgto <= 104)
	{
		if ((u_int32_t)client1->country != (msgto % 100) && !(client1->attr & FLAG_ADMIN))
		{
			PPrintf(client1, RADIO_YELLOW, "Channel of other country - not permitted");
			return -1;
		}
		else if ((u_int32_t)client2->country != (msgto % 100))
			return 0;
		else
			return 1;
	}

	if (msgto < 100)
	{
		if (msgto >= 1 && msgto < 50)
		{
			if (client1->country != client2->country)
				return 0;
			else if (client1->radio[0] == client2->radio[0])
				return 1;
		}
		else if (client1->radio[0] == client2->radio[0])
			return 1;
	}

	return 0;
}

/**
 SendArenaRules

 Send arena rules to client
 */

void SendArenaRules(client_t *client)
{
	u_int8_t buffer[83];
	u_int8_t buffersize;
	u_int32_t flags = 0;
	arenarules_t *arenarules;
	wb3arenarules_t *wb3arenarules;

	memset(buffer, 0, sizeof(buffer));

	Com_Printf(VERBOSE_DEBUG, "Printing Arena Rules to %s\n", client->longnick);

	if (client->attr & FLAG_ADMIN)
	{
		flags = (FLAG_MAPFLAGSFLY | FLAG_MAPFLAGSTWR | FLAG_MAPFLAGSOWN | FLAG_MAPFLAGSENEMY | FLAG_PLANEATRADAR);
	}
	else if (mapflags->value)
	{
		flags |= (u_int32_t)mapflags->value;

		if (mapflagsfly->value)
			flags |= FLAG_MAPFLAGSFLY;
		if (mapflagstwr->value)
			flags |= FLAG_MAPFLAGSTWR;
		if (mapflagsown->value)
			flags |= FLAG_MAPFLAGSOWN;
		if (mapflagsenemy->value)
			flags |= FLAG_MAPFLAGSENEMY;
		if (planeatradar->value)
			flags |= FLAG_PLANEATRADAR;
		if (!friendlydotsfly->value)
			flags |= FLAG_FRIENDLYDOTSFLY;
		if (!enemydotsfly->value)
			flags |= FLAG_ENEMYDOTSFLY;
		if (!friendlydotstwr->value)
			flags |= FLAG_FRIENDLYDOTSTWR;
		if (!enemydotstwr->value)
			flags |= FLAG_ENEMYDOTSTWR;
	}
	else
	{
		flags = 0;
	}

	if (wb3->value)
	{
		buffersize = 83;
		wb3arenarules = (wb3arenarules_t *)buffer;
		wb3arenarules->packetid = htons(Com_WBhton(0x0300));
		wb3arenarules->radaralt = htonl(radaralt->value);
		wb3arenarules->mapflags = htonl(flags);
		wb3arenarules->ammomult = htonl(ammomult->value);
		wb3arenarules->maxpilotg = htons(maxpilotg->value);
		wb3arenarules->xwindvelocity = htonl(xwindvelocity->value);
		wb3arenarules->ywindvelocity = htonl(ywindvelocity->value);
		wb3arenarules->zwindvelocity = client->plane == 54 ? htonl(zwindvelocity->value - 9):htonl(zwindvelocity->value);
		wb3arenarules->structlim = (u_int8_t) structlim->value;
		wb3arenarules->unknown1 = 0x2D;
		wb3arenarules->unknown2 = 0xA0;
		wb3arenarules->ackmaxtrav = (u_int8_t) ackmaxtrav->value;
		wb3arenarules->altv = altv->value ? altv->value : (IsBomber(client) || !client->cancollide) ? 1 : 0;
		wb3arenarules->fueldiv = htonl(fueldiv->value);
		wb3arenarules->flakmax = htonl(flakmax->value);
		wb3arenarules->radarrange0 = htonl(radarrange0->value);
		wb3arenarules->radarrange1 = htonl(radarrange1->value);
		wb3arenarules->radarrange2 = htonl(radarrange2->value);
		wb3arenarules->radarrange3 = htonl(radarrange3->value);
		wb3arenarules->radarrange4 = htonl(radarrange4->value);
		wb3arenarules->ackshrinkco = (u_int8_t) ackshrinkco->value;
		wb3arenarules->ackgrowco = (u_int8_t) (ackgrowco->value * 10);
		wb3arenarules->arenaflags2 = arenaflags2->value; // 0x80
		wb3arenarules->arenaflags1 = arenaflags1->value; // 0x20
		wb3arenarules->unknown5 = 0x10;
		wb3arenarules->clickrangelim = htonl(clickrangelim->value);
		wb3arenarules->unknown6 = 0x05;
	}
	else
	{
		buffersize = 80;
		arenarules = (arenarules_t *)buffer;
		arenarules->packetid = htons(0x0300);
		arenarules->radaralt = htonl(radaralt->value);
		arenarules->mapflags = htonl(flags);
		arenarules->ammomult = htonl(ammomult->value);
		arenarules->xwindvelocity = htonl(xwindvelocity->value);
		arenarules->ywindvelocity = htonl(ywindvelocity->value);
		arenarules->zwindvelocity = htonl(zwindvelocity->value);
		arenarules->maxpilotg = htons(maxpilotg->value);
		arenarules->structlim = structlim->value;
		arenarules->unknown1 = 0x5A;
		arenarules->unknown2 = htons(0x2828);
		arenarules->altv = altv->value ? altv->value : IsBomber(client) ? 1 : 0;
		arenarules->fueldiv = htonl(fueldiv->value);
		arenarules->flakmax = htonl(flakmax->value);
		arenarules->radarrange0 = htonl(radarrange0->value);
		arenarules->radarrange1 = htonl(radarrange1->value);
		arenarules->radarrange2 = htonl(radarrange2->value);
		arenarules->radarrange3 = htonl(radarrange3->value);
		arenarules->radarrange4 = htonl(radarrange4->value);
		arenarules->unknown3 = 0x55;
		arenarules->unknown4 = 0x30;
		arenarules->arenaflags2 = arenaflags2->value; // 00
		arenarules->arenaflags1 = arenaflags1->value; // 03
		arenarules->unknown7 = 0x10;
		arenarules->unknown8 = 0x01;
		arenarules->unknown9 = 0x07;
	}

	if (client->attr & FLAG_ADMIN)
	{
		if (wb3->value)
		{
			wb3arenarules->planerangelimit = htonl(16000);
			wb3arenarules->enemyidlim = htonl(16000);
			wb3arenarules->friendlyidlim = htonl(16000);
		}
		else
		{
			arenarules->planerangelimit = htonl(16000);
			arenarules->enemyidlim = htonl(16000);
			arenarules->friendlyidlim = htonl(16000);
		}
	}
	else if (IsFighter(client) || ((IsBomber(client) || IsGround(client)) && !iconbombersoverride->value))
	{
		if ((arena->hour <= 5 && arena->minute < 30) || (arena->hour >= 18 && arena->minute >= 30))
		{
			if (wb3->value)
			{
				wb3arenarules->planerangelimit = htonl(planerangelimit->value / 2);
				wb3arenarules->enemyidlim = htonl(enemyidlim->value / 2);
				wb3arenarules->friendlyidlim = htonl(friendlyidlim->value / 2);
			}
			else
			{
				arenarules->planerangelimit = htonl(planerangelimit->value / 2);
				arenarules->enemyidlim = htonl(enemyidlim->value / 2);
				arenarules->friendlyidlim = htonl(friendlyidlim->value / 2);
			}
		}
		else
		{
			if (wb3->value)
			{
				wb3arenarules->planerangelimit = htonl(planerangelimit->value);
				wb3arenarules->enemyidlim = htonl(enemyidlim->value);
				wb3arenarules->friendlyidlim = htonl(friendlyidlim->value);
			}
			else
			{
				arenarules->planerangelimit = htonl(planerangelimit->value);
				arenarules->enemyidlim = htonl(enemyidlim->value);
				arenarules->friendlyidlim = htonl(friendlyidlim->value);
			}
		}
	}
	else
	{
		if ((arena->hour < 6 && arena->minute < 30) || (arena->hour > 17 && arena->minute > 29))
		{
			if (wb3->value)
			{
				wb3arenarules->planerangelimit = htonl(planerangelimitbomber->value / 2);
				wb3arenarules->enemyidlim = htonl(enemyidlimbomber->value / 2);
				wb3arenarules->friendlyidlim = htonl(friendlyidlimbomber->value / 2);
			}
			else
			{
				arenarules->planerangelimit = htonl(planerangelimitbomber->value / 2);
				arenarules->enemyidlim = htonl(enemyidlimbomber->value / 2);
				arenarules->friendlyidlim = htonl(friendlyidlimbomber->value / 2);
			}
		}
		else
		{
			if (wb3->value)
			{
				wb3arenarules->planerangelimit = htonl(planerangelimitbomber->value);
				wb3arenarules->enemyidlim = htonl(enemyidlimbomber->value);
				wb3arenarules->friendlyidlim = htonl(friendlyidlimbomber->value);
			}
			else
			{
				arenarules->planerangelimit = htonl(planerangelimitbomber->value);
				arenarules->enemyidlim = htonl(enemyidlimbomber->value);
				arenarules->friendlyidlim = htonl(friendlyidlimbomber->value);
			}
		}
	}

	SendPacket(buffer, buffersize, client);
}

/**
 WB3SendGruntConfig

 Send arena rules to client
 */

void WB3SendGruntConfig(client_t *client)
{
	u_int8_t buffer[8];
	wb3gruntconfig_t *gruntconfig;

	memset(buffer, 0, sizeof(buffer));

	gruntconfig = (wb3gruntconfig_t *)buffer;

	gruntconfig->packetid = htons(Com_WBhton(0x030D));
	gruntconfig->gruntsmaxd = htonl(gruntsmaxd->value);
	gruntconfig->shootgrunts = gruntshoot->value;
	gruntconfig->capturegrunts = gruntcapture->value;

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 WB3SendArenaFlags3

 Send arena flags 3 to client
 */

void WB3SendArenaFlags3(client_t *client)
{
	u_int8_t buffer[6];
	wb3arenaflags3_t *wb3arenaflags3;

	memset(buffer, 0, sizeof(buffer));

	wb3arenaflags3 = (wb3arenaflags3_t *)buffer;

	wb3arenaflags3->packetid = htons(Com_WBhton(0x030F));
	wb3arenaflags3->flags = htonl(arenaflags3->value);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 WB3RandomSeed

 Send random seed to client
 */

void WB3RandomSeed(client_t *client)
{
	u_int8_t buffer[6];
	wb3randomseed_t *wb3randomseed;

	memset(buffer, 0, sizeof(buffer));

	wb3randomseed = (wb3randomseed_t *)buffer;

	wb3randomseed->packetid = htons(Com_WBhton(0x030E));
	wb3randomseed->seed = htonl(0x1F002ABF);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 WB3ConfigFM

 Send Flight Model Config
 */

void WB3ConfigFM(client_t *client)
{
	u_int8_t buffer[50];
	wb3configfm_t *wb3configfm;

	memset(buffer, 0, sizeof(buffer));

	wb3configfm = (wb3configfm_t *)buffer;

	wb3configfm->packetid = htons(Com_WBhton(0x0310));
	wb3configfm->unk1 = htonl(dpitch->value);
	wb3configfm->unk2 = htonl(droll->value);
	wb3configfm->unk3 = htonl(dyaw->value);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 WB3ArenaConfig2

 Send arena config 2 to client
 */

void WB3ArenaConfig2(client_t *client)
{
	u_int8_t buffer[10];
	wb3arenaconfig2_t *arenaconfig2;

	memset(buffer, 0, sizeof(buffer));

	arenaconfig2 = (wb3arenaconfig2_t *)buffer;

	arenaconfig2->packetid = htons(Com_WBhton(0x0312));
	arenaconfig2->arnaflags3 = htonl(arenaflags3->value);
	arenaconfig2->wingstrikerng = htonl(wingstrikerng->value);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 WB3NWAttachSlot

 Send new? attach slot
 */

void WB3NWAttachSlot(client_t *client)
{
	u_int8_t buffer[6];
	wb3nwattachslot_t *attachslot;

	memset(buffer, 0, sizeof(buffer));

	attachslot = (wb3nwattachslot_t *)buffer;

	attachslot->packetid = htons(Com_WBhton(0x0E0B));
	attachslot->slot = htonl(0x3F);

	SendPacket(buffer, sizeof(buffer), client);
}

/**
 PRadioCommand

 Parse radio command and send to process it
 */

void PRadioCommand(char *message, u_int8_t size, client_t *client)
{
	if (size > 0 && size <= 64)
	{
		message++;
		message[size - 1] = '\0';
		ProcessCommands(message, client);
	}
}

/**
 PFileCheck
 Check clients file integrity
 */

void PFileCheck(u_int8_t *buffer, client_t *client)
{
	checkclientfiles_t *check;
	char *message;
	u_int8_t i;
	u_int32_t crc;

	check = (checkclientfiles_t *)buffer;

	crc = ntohl(check->crc);
	message = &check->msg;
	message[check->msgsize] = 0;

	switch (ntohl(check->exemiscview))
	{
		case 0x01:
			if ((crc != CRC_WARBIRDS) && (crc != CRC_WBD3D) && (crc != CRC_WBD3D_MOD))
			{
				PPrintf(client, RADIO_BLUE, "Invalid Executable CRC, error logged");
				ForceEndFlight(TRUE, client);
				Com_Printf(VERBOSE_WARNING, "Invalid Executable CRC: %X (%s)\n", crc, client->longnick);
			}
			break;
		case 0x02:
			if (crc != CRC_MISCBIN)
			{
				PPrintf(client,
				RADIO_BLUE, "Invalid MISC.BIN CRC, please download last FHL version", crc);

				if (strcmp(client->longnick, "-exec-") || strcmp(client->longnick, "franz-"))
					ForceEndFlight(TRUE, client);
				Com_Printf(VERBOSE_WARNING, "Invalid MISC.BIN CRC: %X (%s)\n", crc, client->longnick);
			}
			break;
		case 0x05:
			// TODO: FIXME: Make server known which pos client is and how many bullets pos have
			if (client->attached)
			{
				if (client->attached->drone & DRONE_HMACK)
					ReloadWeapon(1, 5000, client);
				else if (client->attached->drone & DRONE_HTANK)
					ReloadWeapon(1, 60, client);
				else
				{
					for (i = 0; i < 7; i++)
					{
						if (client->attached->gunners[i] == client)
							break;
					}

					if (i < 7)
					{
						ReloadWeapon(GunPos(i, 1), 1000, client);
					}
				}
			}

			CheckCockpitCRC(message, crc, client);

			if (crcview->value)
			{
				Com_Printf(VERBOSE_ALWAYS, "Cockpit CRC:0x%0X, %s\n", crc, message);
				PPrintf(client, RADIO_GREEN, "Cockpit CRC:0x%0X, %s", crc, message);
			}
			break;
		default:
			Com_Printf(VERBOSE_WARNING, "packet 0x1801 invalid value (%X)\n", ntohl(check->exemiscview));
			break;
	}
}

/**
 CheckCockpitCRC

 Check Cockpit CRC
 */

void CheckCockpitCRC(char *path, u_int32_t crc, client_t *client)
{
	char *name;
	u_int8_t i;

	if (!path)
		return;

	name = strrchr(path, '\\');

	if (!name)
	{
		name = strrchr(path, '/');

		if (!name)
			return;
	}

	name++; // now name == B171.VEW

	if ((i = strlen(name)) < 5)
		return;

	name[i - 4] = '\0'; // now just B171

	i = 1;

	switch (tolower(name[0]))
	{
		case 'b':
			if (!Com_Stricmp(name, "b171"))
			{
				if (crc == 0xFDA5A06C)
					i = 0;
			}
			else if (!Com_Stricmp(name, "b173"))
			{
				if (crc == 0x41B2D37A) // transparent
					i = 2;
				else if (crc == 0x4803D2B9)
					i = 0;
			}
			else if (!Com_Stricmp(name, "b241"))
			{
				if (crc == 0x2D920664)
					i = 0;
			}
			else if (!Com_Stricmp(name, "b243"))
			{
				if (crc == 0xC0FA1202) // transparent
					i = 2;
				else if (crc == 0xE5EB032C)
					i = 0;
			}
			else if (!Com_Stricmp(name, "b25h1"))
			{
				if (crc == 0x73EDB9ED)
					i = 0;
			}
			else if (!Com_Stricmp(name, "b25h3"))
			{
				if (crc == 0x76335E42) // transparent BUGGED
					i = 2;
				else if (crc == 0x76335E38)
					i = 0;
			}
			else if (!Com_Stricmp(name, "bf109e1"))
			{
				if (crc == 0x9C2B2643)
					i = 0;
			}
			else if (!Com_Stricmp(name, "bf109e3"))
			{
				if (crc == 0x00D0D4A3) // transparent
					i = 2;
				else if (crc == 0x5FDC1D70 || crc == 0x320F1EEF)
					i = 0;
			}
			else if (!Com_Stricmp(name, "bf109g1"))
			{
				if (crc == 0x8FE9E3B3)
					i = 0;
			}
			else if (!Com_Stricmp(name, "bf109g3"))
			{
				if (crc == 0x7FCAD160) // transparent
					i = 2;
				else if (crc == 0xFC02B870)
					i = 0;
			}
			else if (!Com_Stricmp(name, "bf109k1"))
			{
				if (crc == 0xC5BB866E)
					i = 0;
			}
			else if (!Com_Stricmp(name, "bf109k3"))
			{
				if (crc == 0xBCB15BE6 || crc == 0xA4EE71AD)
					i = 0;
			}
			else if (!Com_Stricmp(name, "bf110g1"))
			{
				if (crc == 0xF60444F6)
					i = 0;
			}
			else if (!Com_Stricmp(name, "bf110g3"))
			{
				if (crc == 0xA49E89AB) // transparent
					i = 2;
				else if (crc == 0x644AD191 || 0xC9EB9FEA)
					i = 0;
			}
			break;

		case 'c':
			if (!Com_Stricmp(name, "chute1"))
			{
				if (crc == 0xFAFACE21)
					i = 0;
			}
			else if (!Com_Stricmp(name, "chute3"))
			{
				if (crc == 0xFAFACC21)
					i = 0;
			}
			else if (!Com_Stricmp(name, "cv1"))
			{
				if (crc == 0x955D83DE || crc == 0x45A0780C)
					i = 0;
			}
			else if (!Com_Stricmp(name, "cv3"))
			{
				if (crc == 0xB7BA1B74 || crc == 0xDDCCBAEE)
					i = 0;
			}

			break;

		case 'f':
			if (!Com_Stricmp(name, "f4u11"))
			{
				if (crc == 0x3B5BBD4B)
					i = 0;
			}
			else if (!Com_Stricmp(name, "f4u13"))
			{
				if (crc == 0x83486051) // transparent
					i = 2;
				else if (crc == 0xB1620A88 || crc == 0xA8CA2B1B)
					i = 0;
			}
			else if (!Com_Stricmp(name, "f6f51"))
			{
				if (crc == 0x9E362FA1)
					i = 0;
			}
			else if (!Com_Stricmp(name, "f6f53"))
			{
				if (crc == 0xE3AB7030) // transparent
					i = 2;
				else if (crc == 0xD0F86534 || crc == 0x091542BB)
					i = 0;
			}
			else if (!Com_Stricmp(name, "foo1"))
			{
				if (crc == 0x02010772)
					i = 0;
			}
			else if (!Com_Stricmp(name, "foo3"))
			{
				if (crc == 0xC0E8768C)
					i = 0;
			}
			else if (!Com_Stricmp(name, "fw19041"))
			{
				if (crc == 0xD87307BF)
					i = 0;
			}
			else if (!Com_Stricmp(name, "fw19043"))
			{
				if (crc == 0xD5011BBC) // transparent
					i = 2;
				else if (crc == 0xFAA64893)
					i = 0;
			}
			break;

		case 'g':
			if (!Com_Stricmp(name, "g4m21"))
			{
				if (crc == 0x30D342E2)
					i = 0;
			}
			else if (!Com_Stricmp(name, "g4m23"))
			{
				if (crc == 0x58C9CAEB || crc == 0x85005047)
					i = 0;
			}
			else if (!Com_Stricmp(name, "gunner1"))
			{
				if (crc == 0x5A451267)
					i = 0;
			}
			else if (!Com_Stricmp(name, "gunner3"))
			{
				if (crc == 0x0CE40B33 || crc == 0xD7FFD5DB)
					i = 0;
			}
			break;

		case 'h':
			if (!Com_Stricmp(name, "he1771"))
			{
				if (crc == 0x093E925E)
					i = 0;
			}
			else if (!Com_Stricmp(name, "he1773"))
			{
				if (crc == 0xC5C01727)
					i = 0;
			}
			else if (!Com_Stricmp(name, "hq1"))
			{
				if (crc == 0x4F1DF561)
					i = 0;
			}
			else if (!Com_Stricmp(name, "hq3"))
			{
				if (crc == 0x4F1BF561)
					i = 0;
			}
			else if (!Com_Stricmp(name, "hurri11"))
			{
				if (crc == 0x790B1BC5)
					i = 0;
			}
			else if (!Com_Stricmp(name, "hurri13"))
			{
				/*if(crc == 0x324B45F6) // transparent BUGGED
				 i = 2;
				 else */if (crc == 0x324B45F6)
					i = 0;
			}
			break;

		case 'i':
			if (!Com_Stricmp(name, "i161"))
			{
				if (crc == 0xAB1B8713 || crc == 0xAFC52F5A)
					i = 0;
			}
			else if (!Com_Stricmp(name, "i163"))
			{
				if (crc == 0x3CF56605 || crc == 0x1DFCA9F5)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ijndef1"))
			{
				if (crc == 0x288D7242)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ijndef3"))
			{
				if (crc == 0x203A52EA) // transparent
					i = 2;
				else if (crc == 0x46401B6D)
					i = 0;
			}
			else if (!Com_Stricmp(name, "il21"))
			{
				if (crc == 0x13BD38FD || crc == 0xF89C8D51)
					i = 0;
			}
			else if (!Com_Stricmp(name, "il23"))
			{
				if (crc == 0x754EA2CD || crc == 0x522463C1 || crc == 0xF0B8E6AB)
					i = 0;
			}
			else if (!Com_Stricmp(name, "il43"))
			{
				if (crc == 0xD56A53E4)
					i = 0;
			}
			break;

		case 'j':
			if (!Com_Stricmp(name, "ju521"))
			{
				if (crc == 0xC62DDB82)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ju523"))
			{
				if (crc == 0xD2374FB7) // transparent
					i = 2;
				else if (crc == 0xF6F943EB || crc == 0xD8F74253)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ju87d1"))
			{
				if (crc == 0x3FF385F8)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ju87d3"))
			{
				if (crc == 0x1812D54C || crc == 0x8C1184BD)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ju881"))
			{
				if (crc == 0x2A429157)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ju883"))
			{
				if (crc == 0xC2BF0F4A)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ju88a1"))
			{
				if (crc == 0x7CF40711)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ju88a3"))
			{
				if (crc == 0x6AE3492B || crc == 0x5A9AF04F)
					i = 0;
			}
			break;

		case 'k':
			if (!Com_Stricmp(name, "ki611"))
			{
				if (crc == 0xAA62F413)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ki613"))
			{
				if (crc == 0x2FED63E6) // transparent
					i = 2;
				else if (crc == 0x55882BE0)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ki841"))
			{
				if (crc == 0x9842A8E8)
					i = 0;
			}
			else if (!Com_Stricmp(name, "ki843"))
			{
				if (crc == 0xA906BC0B) // transparent
					i = 2;
				else if (crc == 0xCE7DB636)
					i = 0;
			}
			break;

		case 'l':
			if (!Com_Stricmp(name, "la51"))
			{
				if (crc == 0xCF762CF5 || crc == 0xF0498C56)
					i = 0;
			}
			else if (!Com_Stricmp(name, "la53"))
			{
				if (crc == 0x1D9FCD01 || crc == 0xF0498C56 || crc == 0x824DAC62)
					i = 0;
			}
			else if (!Com_Stricmp(name, "lagg3"))
			{
				if (crc == 0x6A782E52 || crc == 0xA118784D)
					i = 0;
			}
			break;

		case 'm':
			if (!Com_Stricmp(name, "mc2021"))
			{
				if (crc == 0xC88C7A91)
					i = 0;
			}
			else if (!Com_Stricmp(name, "mc2023"))
			{
				if (crc == 0x45C8811F || crc == 0x8541F62F)
					i = 0;
			}
			else if (!Com_Stricmp(name, "me2621"))
			{
				if (crc == 0x0CD917DF)
					i = 0;
			}
			else if (!Com_Stricmp(name, "me2623"))
			{
				if (crc == 0xDABF54D5)
					i = 0;
			}
			else if (!Com_Stricmp(name, "me4101"))
			{
				if (crc == 0x53664309)
					i = 0;
			}
			else if (!Com_Stricmp(name, "me4103"))
			{
				if ((crc == 0x0B3E3AB8) || (crc == 0xAC189CBF))
					i = 0;
			}
			else if (!Com_Stricmp(name, "mig31"))
			{
				if (crc == 0x01B1628A)
					i = 0;
			}
			else if (!Com_Stricmp(name, "mig33"))
			{
				if (crc == 0xE11B0CE3)
					i = 0;
			}
			else if (!Com_Stricmp(name, "mosq1"))
			{
				if (crc == 0xFEF34326)
					i = 0;
			}
			else if (!Com_Stricmp(name, "mosq3"))
			{
				if (crc == 0x0BA72831) // transparent
					i = 2;
				else if (crc == 0x36936DC4 || crc == 0x30B8BE28)
					i = 0;
			}
			break;

		case 'p':
			if (!Com_Stricmp(name, "p381"))
			{
				if (crc == 0xDDB88DAE)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p383"))
			{
				if (crc == 0x18A05716) // transparent
					i = 2;
				else if (crc == 0x3DD2E1ED || crc == 0x28D2B9D9)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p39d1"))
			{
				if (crc == 0x0D3D1A9D)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p39d3"))
			{
				if (crc == 0x970A1001) // transparent
					i = 2;
				else if (crc == 0xBC4996E3)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p401"))
			{
				if (crc == 0x8E2ED1C7)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p403"))
			{
				if (crc == 0x963C06BD) // transparent
					i = 2;
				else if (crc == 0xBB5AF3F4)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p47c1"))
			{
				if (crc == 0xADDBCB11)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p47c3"))
			{
				if (crc == 0x960970B0 || crc == 0xFF3F675E)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p47d1"))
			{
				if (crc == 0x0B66ADD1)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p47d3"))
			{
				if (crc == 0x49A3E951) // transparent
					i = 2;
				else if (crc == 0x6F10C28D)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p511"))
			{
				if (crc == 0x0473291D)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p513"))
			{
				if (crc == 0xB4D72159) // transparent
					i = 2;
				else if (crc == 0xB53A7322 || crc == 0xBB78E75B)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p51b1"))
			{
				if (crc == 0x4FF1C3D0)
					i = 0;
			}
			else if (!Com_Stricmp(name, "p51b3"))
			{
				if (crc == 0x802ED5C)
					i = 0;
			}
			else if (!Com_Stricmp(name, "pe21"))
			{
				if (crc == 0x0C96FF1A)
					i = 0;
			}
			else if (!Com_Stricmp(name, "pe23"))
			{
				if (crc == 0x141D614E)
					i = 0;
			}
			break;

		case 's':
			if (!Com_Stricmp(name, "sbd1"))
			{
				if (crc == 0xA83DFE94)
					i = 0;
			}
			else if (!Com_Stricmp(name, "sbd3"))
			{
				if (crc == 0x6170E147)
					i = 0;
			}
			else if (!Com_Stricmp(name, "spit51"))
			{
				if (crc == 0x1B9713F4)
					i = 0;
			}
			else if (!Com_Stricmp(name, "spit53"))
			{
				if (crc == 0x0EF82BE0) // transparent
					i = 2;
				else if (crc == 0x34806D57)
					i = 0;
			}
			break;

		case 't':
			if (!Com_Stricmp(name, "twr1"))
			{
				if (crc == 0xB83412F5 || crc == 0xEE3637B1)
					i = 0;
			}
			else if (!Com_Stricmp(name, "twr3"))
			{
				if (crc == 0xA6FB98F0 || crc == 0x16B08A32)
					i = 0;
			}
			else if (!Com_Stricmp(name, "temp3"))
			{
				if (crc == 0x91A50E84)
					i = 0;
			}
			if (!Com_Stricmp(name, "typh1"))
			{
				if (crc == 0xFAB13550)
					i = 0;
			}
			if (!Com_Stricmp(name, "typh3"))
			{
				if (crc == 0x84008B99 || crc == 0x162B9A77)
					i = 0;
			}
			break;

		case 'y':
			if (!Com_Stricmp(name, "yak11"))
			{
				if (crc == 0x6B8D289C || crc == 0x7D4D1F42)
					i = 0;
			}
			else if (!Com_Stricmp(name, "yak13"))
			{
				if (crc == 0x78B56A33 || crc == 0xC5BAE4D5 || crc == 0x474F7F89)
					i = 0;
			}
			else if (!Com_Stricmp(name, "yak31"))
			{
				if (crc == 0xF22B15D8)
					i = 0;
			}
			else if (!Com_Stricmp(name, "yak33"))
			{
				if (crc == 0x8CC4F556)
					i = 0;
			}
			else if (!Com_Stricmp(name, "yak9d1"))
			{
				if (crc == 0x5CF8CEFE || crc == 0x8FAA4EB5)
					i = 0;
			}
			else if (!Com_Stricmp(name, "yak9d3"))
			{
				if (crc == 0x3D5D12D5 || crc == 0x192B0769)
					i = 0;
			}

			else if (!Com_Stricmp(name, "yak9d3"))
			{
				if (crc == 0x340BCFFA) // transparent
					i = 2;
				else if (crc == 0x3D5D12D5)
					i = 0;
			}
			break;

		case 'z':
			if (!Com_Stricmp(name, "zero1"))
			{
				if (crc == 0xA187CC83)
					i = 0;
			}
			else if (!Com_Stricmp(name, "zero3"))
			{
				if (crc == 0x715E21BA) // transparent
					i = 2;
				else if (crc == 0x969AA6B7 || crc == 0x3B6015DF)
					i = 0;
			}
			break;
		default:
			PPrintf(client, RADIO_GREEN, "Cockpit is not in CRC list");
	}

	if (i)
	{
		if (i == 2)
		{
			BPrintf(RADIO_BLUE, "%s used transparent cockpit and will be banned", client->longnick);
			Cmd_Ban(client->longnick, 1, NULL);
		}
		else
		{
			PPrintf(client,
			RADIO_BLUE, "Invalid Cockpit CRC (%s.VEW) please download correct file", name);
			ForceEndFlight(TRUE, client);
		}
		Com_Printf(VERBOSE_WARNING, "Invalid Cockpit CRC: %X %s.VEW (%s)\n", crc, name, client->longnick);
	}
}

/**
 SendExecutablesCheck

 Send Request of file integrity
 */

void SendExecutablesCheck(u_int32_t exemisc, client_t *client)
{
	u_int8_t buffer[32];
	checkexecutables_t *check;

	memset(buffer, 0, sizeof(buffer));

	check = (checkexecutables_t *)buffer;

	check->packetid = htons(Com_WBhton(0x1B00));
	check->exemisc = htonl(exemisc);
	if (exemisc == 1)
	{
		check->filesize = htonl(EXEC_SIZE);
		check->msgsize = strlen(EXEC_FILE);
		memcpy(&(check->msg), EXEC_FILE, check->msgsize);
	}
	else if (exemisc == 2)
	{
		check->filesize = htonl(MISC_SIZE);
		check->msgsize = strlen(MISC_FILE);
		memcpy(&(check->msg), MISC_FILE, check->msgsize);
	}

	SendPacket(buffer, 15+check->msgsize, client);
}

/**
 SendFieldCountries

 Send all fields' countries
 */

void SendFieldsCountries(client_t *client)
{
	u_int8_t i;

	for (i = 0; i < fields->value; i++)
	{
		Cmd_Capt(i, arena->fields[i].country, client);
	}
}

/**
 FirstFieldCountry

 Return the number of first field that match with country
 */

int8_t FirstFieldCountry(u_int8_t country)
{
	int8_t i;

	for (i = 0; i < fields->value; i++)
	{
		if (arena->fields[i].country == country && !arena->fields[i].closed)
			return i;
	}

	return -1;
}

/**
 SendGunnerStatusChange

 Send the nick of who client will be gunner
 */

void SendGunnerStatusChange(client_t *gunner, u_int16_t status, client_t *client)
{
	u_int8_t buffer[8];
	gunnerstatus_t *gunnerstatus;

	memset(buffer, 0, sizeof(buffer));

	gunnerstatus = (gunnerstatus_t *)buffer;
	gunnerstatus->packetid = htons(Com_WBhton(0x0414));
	gunnerstatus->shortnick = htonl(gunner->shortnick);
	gunnerstatus->status = htons(status);
	SendPacket(buffer, sizeof(buffer), client);
}

/**
 SendAttachList

 Send to client current gunner list
 */

void SendAttachList(u_int8_t *packet, client_t *client)
{
	u_int8_t buffer[60];
	client_t *dest;
	reqgunnerlist_t *gunner;
	gunnerlist_t *pattach;
	u_int8_t i;

	memset(buffer, 0, sizeof(buffer));

	pattach = (gunnerlist_t *)buffer;

	if (packet)
	{
		gunner = (reqgunnerlist_t *) packet;

		dest = FindSClient(ntohl(gunner->nick));

		client->gunnerview = dest;

		if (!dest)
			return;

		pattach->packetid = htons(Com_WBhton(0x0413));
		pattach->unknown1 = gunner->unknown1;
		if (debug->value)
			PPrintf(client, RADIO_RED, "DEBUG: 0x0412: Ask for Unk nick list (0x%0X, 0x0X)", ntohs(gunner->unknown1), ntohl(gunner->nick));
	}
	else
	{
		dest = client;
		client->gunnerview = client;
		pattach->packetid = htons(Com_WBhton(0x0411));
	}

	for (i = 0; i < 14; i++)
	{
		if (dest->gunners[i] && dest->gunners[i]->longnick)
			pattach->gunners[i] = htonl(dest->gunners[i]->shortnick);
	}

	if (packet)
		i = sizeof(buffer);
	else
		i = sizeof(buffer) - 2;

	SendPacket(buffer, i, client);
}

/**
 PCurrentView

 Send current client's view to who is attached on observer position
 */

void PCurrentView(u_int8_t *buffer, client_t *client)
{
	currentview_t *view;

	view = (currentview_t *) buffer;

	if (client->shanghai || client->view)
	{
		view->packetid = htons(Com_WBhton(0x041B));
		view->unknown2 = view->view;
		view->view = 0;
	}
	else
		return;

	if (client->shanghai)
	{
		if (client->shanghai->attached == client)
			SendPacket(buffer, 6, client->shanghai);
	}

	if (client->view)
	{
		if (client->view->attached == client)
			SendPacket(buffer, 6, client->view);
	}
}

/**
 PSquadLookup

 Show info from another squad from given nick
 */

void PSquadLookup(u_int8_t *buffer, client_t *client)
{
	char *nick;
	squadlookup_t *lookup;

	lookup = (squadlookup_t *)buffer;

	nick = wbnick2ascii(htonl(lookup->nick));

	PSquadInfo(nick, client);
}

/**
 PClientMedals

 Send all medals player got
 */

void PClientMedals(u_int8_t *buffer, client_t *client)
{
	u_int8_t offset, i, ranking = 0;
	u_int32_t buddy, playerid = 0;
	client_t *pclient;
	int16_t num_rows;
	u_int8_t bufmedals[256];
	char filename[128];
	FILE *fp;
	requestmedals_t *medals;
	clientmedals_t *medals1;
	clientmedals2_t *medals2;
	clientmedals3_t *medals3;

	if (buffer)
	{
		medals = (requestmedals_t *)buffer;
		buddy = ntohl(medals->nick);

		if ((pclient = FindSClient(buddy)))
		{
			playerid = pclient->id;
			ranking = pclient->ranking;
		}
		else // not found online, must search in database
		{
			sprintf(my_query, "SELECT players.id, score_common.ranking FROM players INNER JOIN score_common ON players.id = score_common.player_id WHERE players.shortnick = '%u'", buddy);

			if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
			{
				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if (mysql_num_rows(my_result) > 0)
					{
						if ((my_row = mysql_fetch_row(my_result)))
						{
							playerid = Com_Atou(Com_MyRow("id"));
							ranking = Com_Atou(Com_MyRow("ranking"));
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "PClientMedals(id): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						}
					}
					else
					{
						PPrintf(client, RADIO_YELLOW, "Player not found");

						mysql_free_result(my_result);
						my_result = NULL;
						my_row = NULL;
						return;
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;

				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "PClientMedals(id): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				if (mysql_errno(&my_sock))
				{
					Com_Printf(VERBOSE_WARNING, "PClientMedals(id): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			client->lastsql = 0;
		}
	}
	else
	{
		playerid = client->id;
		ranking = client->ranking;
	}

	sprintf(my_query, "SELECT what, why, howmuch, UNIX_TIMESTAMP(date_time) as date_time FROM medals WHERE player_id = '%u'", playerid);

	if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
	{
		if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if ((num_rows = mysql_num_rows(my_result)) >= 0)
			{
				offset = 3;

				memset(bufmedals, 0, sizeof(bufmedals));

				medals1 = (clientmedals_t *)bufmedals;

				medals1->packetid = htons(Com_WBhton(0x040E));
				medals1->amount = num_rows;

				if (num_rows)
				{
					if (wb3->value)
					{
						snprintf(filename, sizeof(filename), "./players/%s.medal.LOCK", client->longnick);

						Sys_WaitForLock(filename);

						if (Sys_LockFile(filename) < 0)
							return;

						filename[strlen(filename) - 5] = '\0';

						if (!(fp = fopen(filename, "wb")))
						{
							Com_Printf(VERBOSE_WARNING, "PClientMedals(): Couldn't open file \"%s\"\n", filename);
							PPrintf(client,
							RADIO_YELLOW, "PClientMedals(): Couldn't open score file, please contact admin", mysql_errno(&my_sock));
							Sys_UnlockFile(strcat(filename, ".LOCK"));
							return;
						}
					}

					for (i = 0; i < num_rows; i++)
					{
						if ((my_row = mysql_fetch_row(my_result)))
						{
							if (wb3->value)
							{
								fprintf(fp, "[IMG]offln\\common\\med%s.tga 0 0 77 153[/IMG]\n", Com_MyRow("what"));

								switch (Com_Atou(Com_MyRow("why")))
								{
									case 1:
										fprintf(fp, "streak of %s kills\n", Com_MyRow("howmuch"));
										break;
									case 2:
										fprintf(fp, "%s kills in a tour of duty\n", Com_MyRow("howmuch"));
										break;
									case 3:
										fprintf(fp, "%s career kills\n", Com_MyRow("howmuch"));
										break;
									case 4:
										fprintf(fp, "%s kills in a sortie\n", Com_MyRow("howmuch"));
										break;
									case 5:
										fprintf(fp, "bomber streak of %s kills\n", Com_MyRow("howmuch"));
										break;
									case 6:
										fprintf(fp, "%s structures destroyed in a tour\n", Com_MyRow("howmuch"));
										break;
									case 7:
										fprintf(fp, "%s career structures destroyed\n", Com_MyRow("howmuch"));
										break;
									case 8:
										fprintf(fp, "%s career fields captured\n", Com_MyRow("howmuch"));
										break;
									default:
										fprintf(fp, "unknown\n");
										break;
								}
							}
							else
							{
								medals2 = (clientmedals2_t *)(bufmedals+offset);

								medals2->medal = Com_Atou(Com_MyRow("what"));
								medals2->deed = Com_Atou(Com_MyRow("why"));
								medals2->time = htonl(Com_Atou(Com_MyRow("date_time")));
								medals2->num = htons(Com_Atou(Com_MyRow("howmuch")));

								offset += 8;

								if (i == 29)
									break;
							}
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "PClientMedals(): Couldn't Fetch Row(%u), error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
							break;
						}
					}

					if (wb3->value)
					{
						fclose(fp);
						SendFileSeq1(filename, "medals.txt", client);
						Sys_UnlockFile(strcat(filename, ".LOCK"));
						return;
					}
				}

				medals3 = (clientmedals3_t *)(bufmedals+offset);

				medals3->unknown1 = htons(3);
				medals3->msgsize = strlen(GetRanking(ranking));
				memcpy(&(medals3->ranking), GetRanking(ranking), medals3->msgsize);

				offset += (medals3->msgsize + 3);

				SendPacket(bufmedals, offset, client);
			}
			else
			{
				Com_Printf(VERBOSE_WARNING, "PClientMedals(): Error num_rows, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
			}

			mysql_free_result(my_result);
			my_result = NULL;
			my_row = NULL;
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "PClientMedals(): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		if (mysql_errno(&my_sock))
		{
			Com_Printf(VERBOSE_WARNING, "PClientMedals(): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
}

/**
 GunPos

 Convert gunner position (a WB stupid thing)
 */

u_int16_t GunPos(u_int16_t pos, u_int8_t reverse)
{
	if (!reverse)
	{
		switch (pos)
		{
			case 0: /* observer */
				return 6;
			case 1: /* tail */
				return 4;
			case 2: /* nose */
				return 3;
			case 3: /* left */
				return 1;
			case 4: /* right */
				return 2;
			case 5: /* top */
				return 5;
			case 6: /* bottom */
				return 0;
			default:
				return 0;
		}
	}
	else
	{
		switch (pos)
		{
			case 0:
				return 6;
			case 1:
				return 3;
			case 2:
				return 4;
			case 3:
				return 2;
			case 4:
				return 1;
			case 5:
				return 5;
			case 6:
				return 0;
			default:
				return 0;
		}
	}
}

/**
 WB3RequestStartFly

 Client request start flight
 */

void WB3RequestStartFly(u_int8_t *buffer, client_t *client)
{
	wb3requestfly_t *reqfly;
	u_int8_t i;

	reqfly = (wb3requestfly_t *) buffer;

	if (client->attr)
	{
		PPrintf(client, RADIO_GREEN, "field = %d, unk1 = %u, unk2 = %u", ntohl(reqfly->field), ntohl(reqfly->unk1), ntohl(reqfly->unk2));
		PPrintf(client, RADIO_GREEN, "ord = %d, spawnpoint = %u, unk4 = %u, unk5 = %u", ntohl(reqfly->ord), ntohl(reqfly->spawnpoint), ntohl(reqfly->unk4), ntohl(reqfly->unk5));
	}

	client->plane = ntohl(reqfly->plane);
	client->field = ntohl(reqfly->field);
	client->ord = ntohl(reqfly->ord);

	if (!Cmd_Fly(0, client))
	{
		if (client->shanghai) // start shanghai flight
		{
			if (client->shanghai->ready && !client->shanghai->infly)
			{
				client->shanghai->attached = client;
				Com_Printf(VERBOSE_DEBUG, "Start Shanghai Fly\n");
				Com_Printf(VERBOSE_ALWAYS, "%s start flight with %s in shanghai\n", client->longnick, client->shanghai->longnick);
				SendGunnerStatusChange(client, 2, client->shanghai); // define position to be attached in client
				SendAttachList(NULL, client->shanghai);
				client->shanghai->visible[MAX_SCREEN - 1].client = client;
				AddRemovePlaneScreen(client, client->shanghai, FALSE);
				Cmd_Fly(2, client->shanghai);
			}
		}

		if (HaveGunner(client->plane))
		{
			for (i = 0; i < 7; i++) // start gunners flight
			{
				if (client->gunners[i])
				{
					if (client->gunners[i]->ready && !client->gunners[i]->infly)
					{
						client->gunners[i]->attached = client;
						Cmd_Fly(GunPos(i, 1), client->gunners[i]);
					}
				}
			}
		}
	}
}

/**
 WB3RequestMannedAck

 Client request start flight
 */

void WB3RequestMannedAck(u_int8_t *buffer, client_t *client)
{
	u_int8_t packet[119];
	u_int32_t rules = 0;
	wb3requestmannedack_t *reqack;
	wb3startack_t *startack;
	building_t *building;

	memset(packet, 0, sizeof(packet));

	reqack = (wb3requestmannedack_t *) buffer;
	startack = (wb3startack_t *) packet;

	PPrintf(client, RADIO_GREEN, "plane %u; country %u; field %u", ntohl(reqack->plane), ntohl(reqack->country), ntohl(reqack->field));
	PPrintf(client, RADIO_GREEN, "unk1 %u; unk2 %u; unk3 %u", ntohl(reqack->unk1), ntohl(reqack->unk2), ntohl(reqack->unk3));
	PPrintf(client, RADIO_GREEN, "unk4 %u; ackid %u", ntohl(reqack->unk4), ntohl(reqack->ackid));

	building = GetBuilding(ntohl(reqack->ackid));

	if (building->status)
	{
		PPrintf(client, RADIO_YELLOW, "This ack is destroyed");
		return;
	}

	if (client->attr & FLAG_ADMIN)
	{
		rules = (FLAG_AIRSHOWSMOKE | FLAG_ENEMYNAMES | FLAG_ENEMYPLANES);
	}
	else
	{
		if (midairs->value)
			rules |= FLAG_MIDAIRS;
		if (blackout->value)
			rules |= FLAG_BLACKOUT;
		if (airshowsmoke->value)
			rules |= FLAG_AIRSHOWSMOKE;
		if (client->easymode)
			rules |= FLAG_EASYMODE;
		if (enemynames->value)
			rules |= FLAG_ENEMYNAMES;
		if (enemyplanes->value)
			rules |= FLAG_ENEMYPLANES;
		if (enableottos->value)
			rules |= FLAG_ENABLEOTTOS;
	}

	startack->packetid = htons(Com_WBhton(0x041F));
	startack->country = reqack->country;
	startack->field = htonl(ntohl(reqack->field) - 1);
	startack->bulletradius1 = htons((u_int16_t)(bulletradius->value * 10));
	startack->bulletradius2 = htons((u_int16_t)(bulletradius->value * 10));
	startack->gunrad = htons((u_int16_t)(gunrad->value * 10));
	startack->bulletradius3 = htons((u_int16_t)(bulletradius->value * 10));
	startack->rules = htonl(rules);
	startack->plane = htons(13);//ntohl(reqack->plane));
	startack->numofarrays = 1;
	startack->ord = 0;
	startack->posx = htonl(building->posx ? building->posx : client->posxy[0][0]);
	startack->posy = htonl(building->posy ? building->posy : client->posxy[1][0]);
	startack->posz = htonl(building->posz ? building->posz : client->posalt[0]);

	client->plane = ntohl(reqack->plane);
	client->dronetimer = arena->time; // stores time when client started flight
	client->infly = 1;
	client->status_damage = 0; // to force player not appear damaged after takeoff
	client->chute = 0; // to assure client is not flagged as chute
	client->obradar = 0;

	SendArenaRules(client);
	WB3SendGruntConfig(client);
	WB3ArenaConfig2(client);

	SendPacket(packet, sizeof(packet), client);

	client->lastscore = 0;

	ClearMapDots(client);
	SetPlaneDamage(client->plane, client);
	UpdateIngameClients(0);
	SendOttoParams(client);
	//if (!wb3->value)
		SendExecutablesCheck(2, client);
}

/**
 PHostVar

 Get a HostVar packet
 */

void PHostVar(u_int8_t *buffer, client_t *client)
{
	u_int8_t i;
	hostvar_t *hostvar;
	char message[32];

	hostvar = (hostvar_t *) buffer;

	strncpy(message, &(hostvar->var), hostvar->size);

	PPrintf(client, RADIO_LIGHTYELLOW, "%s", message);

	for (i = 0; i < 20; i++)
	{
		if (message[i] == ':')
		{
			message[i] = '\0';
			break;
		}
	}

	buffer[0] = 0x14;
	buffer[1] = 0x13;
	buffer[2] = strlen(message);
	strcpy((buffer+3), message);
	if (!strcmp(message, "TOTENABLED"))
	{
		buffer[buffer[2]+3] = strlen(dpitch->string);
		strcpy((buffer+(buffer[2]+4)), dpitch->string);
	}
	else
	{
		buffer[buffer[2]+3] = strlen(droll->string);
		strcpy((buffer+(buffer[2]+4)), droll->string);
	}

	Com_Printfhex(buffer, buffer[2]+buffer[buffer[2]+3]+4);
	SendPacket(buffer, buffer[2]+buffer[buffer[2]+3]+4, client);
}

/**
 THAIWatchDog

 THAI Watch Dog, used to identify THAI Drones
 */

void THAIWatchDog(u_int8_t *buffer, client_t *client)
{
	thaiwatchdog_t *watchdog;

	if(client->loginkey == 1)
	{
		watchdog = (thaiwatchdog_t *) buffer;
		client->thai = watchdog->group;
	}
}


/**
 PRequestGunner

 Request gunner position
 */

void PRequestGunner(u_int8_t *buffer, client_t *client)
{
	u_int8_t i;
	requestgunner_t *request;
	client_t *dest;
	u_int16_t pos;

	request = (requestgunner_t *) buffer;
	pos = ntohs(request->pos);
	dest = FindSClient(ntohl(request->nick));

	if (!dest)
	{
		PPrintf(client, RADIO_YELLOW, "Player not found");
		return;
	}

	if (dest == client)
	{
		PPrintf(client, RADIO_YELLOW, "You can't attach to yourself");
		return;
	}

	for (i = 0; i < 14; i++)
	{
		if (dest->gunners[i])
			if (dest->gunners[i]->shortnick == client->shortnick)
			{
				PPrintf(client, RADIO_YELLOW, "You're already in %s gunner's list", dest->longnick);
				return;
			}
	}

	pos = GunPos(pos, 0);

	if (dest->gunners[pos + 7] || dest->gunners[pos])
	{
		PPrintf(client, RADIO_YELLOW, "This position is unavailable");
	}
	else
	{
		dest->gunners[pos + 7] = client;
		if (debug->value)
			PPrintf(client, RADIO_RED, "DEBUG: 0x1D05: Request %0X, pos %0X", ntohl(request->nick), ntohs(request->pos));
		PPrintf(client, RADIO_YELLOW, "Your request has been sent");
		PPrintf(dest, RADIO_YELLOW, "%s requested a gunner position", client->longnick);
	}
}

/**
 PAcceptGunner

 Check if clients has been captured on land/ditch/bail
 */

void PAcceptGunner(u_int8_t *buffer, client_t *client)
{
	u_int8_t i;
	u_int32_t nick;
	acceptgunner_t *accept;
	reqgunnerlist_t *gunner;

	accept = (acceptgunner_t *) buffer;

	nick = ntohl(accept->nick);

	for (i = 7; i < 14; i++)
	{
		if (client->gunners[i])
		{
			if (client->gunners[i]->shortnick == nick)
			{
				client->gunners[i - 7] = client->gunners[i];
				client->gunners[i] = 0;
				break;
			}
		}
	}

	if (debug->value)
		PPrintf(client, RADIO_RED, "DEBUG: 0x1D06: Accept %0X, unk1 %0X", ntohl(accept->nick), ntohs(accept->unknown1));

	for (i = 7; i < 14; i++)
	{
		if (client->gunners[i])
			break;
	}

	if (i == 14) // no gunners found in request list
	{
		gunner = (reqgunnerlist_t *) buffer;
		gunner->unknown1 = htons(0x0006);
		gunner->nick = htonl(client->shortnick);
		SendAttachList(buffer, client);
	}
}

/**
 PSwitchOttoPos

 Try to jump to other otto position
 */

void PSwitchOttoPos(u_int8_t *buffer, client_t *client)
{
	switchottopos_t *otto;
	u_int8_t i;
	int16_t pos;

	otto = (switchottopos_t *) buffer;

	if (!client->attached->drone && client->attached->infly)
	{
		if ((pos = htons(otto->pos)) < 0)
			return;

		if ((client->attached->view != client) && (client->attached->shanghai != client))
		{
			if (client->attached->gunners[GunPos(pos, 0)])
			{
				PPrintf(client, RADIO_YELLOW, "This position is occupied");
				return;
			}
			else
			{
				for (i = 0; i < 7; i++)
				{
					if (client->attached->gunners[i] == client)
						client->attached->gunners[i] = NULL;
				}

				client->attached->gunners[GunPos(pos, 0)] = client;
				Cmd_Fly(pos, client);
			}
		}
		else if (client->attached->view == client)
		{
			Cmd_Fly(pos, client);
		}
	}
}

/**
 PSquadInfo

 Send squad information
 */

void PSquadInfo(char *nick, client_t *client)
{
	u_int8_t i, j;
	int16_t num_rows;
	u_int32_t squadowner = 0;
	client_t *cnick= NULL;
	u_int8_t buffer[512];
	squadmembers1_t *squad1;
	squadmembers2_t *squad2;

	if (!nick) // request info about own squad
	{
		squadowner = client->squadron;
	}
	else
	{
		if ((cnick = FindLClient(nick))) // try to find player in game
		{
			squadowner = cnick->squadron;
		}
		else // ok, so, lets search in database
		{
			sprintf(my_query, "SELECT squad_owner FROM players WHERE longnick = '%s'", nick);

			if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
			{
				if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
				{
					if ((my_row = mysql_fetch_row(my_result)))
					{
						squadowner = Com_Atou(Com_MyRow("squad_owner"));
						client->lastsql = 0;
					}
					else
					{
						if (!mysql_errno(&my_sock))
						{
							PPrintf(client, RADIO_LIGHTYELLOW, "Player %s not found", nick);
							nick = NULL;
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "PSquadInfo(find): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						}
					}

					mysql_free_result(my_result);
					my_result = NULL;
					my_row = NULL;

					if (!nick)
						return;
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "PSquadInfo(find): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				if (mysql_errno(&my_sock))
				{
					Com_Printf(VERBOSE_WARNING, "PSquadInfo(find): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
				else
					return; // SQL Flood
			}

			if (mysql_errno(&my_sock))
			{
				PPrintf(client,
				RADIO_YELLOW, "PSquadInfo(find): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
				return;
			}
		}
	}

	if (!squadowner)
	{
		PPrintf(client, RADIO_YELLOW, "%s is not in any squadron", nick ? nick : client->longnick);
		return;
	}

	sprintf(my_query, "SELECT longnick, squad_flag FROM players WHERE id = '%u'", squadowner); // find squadron leader info

	if (!Com_MySQL_Query(client, &my_sock, my_query)) // query succeeded
	{
		if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
		{
			if ((my_row = mysql_fetch_row(my_result)))
			{
				memset(buffer, 0, sizeof(buffer));

				squad2 = (squadmembers2_t *) (buffer+5);
				squad2->nick = htonl(ascii2wbnick(Com_MyRow("longnick"), 0));
				squad2->attr = htonl(Com_Atou(Com_MyRow("squad_flag")));

				sprintf(my_query, "SELECT longnick, squad_flag FROM players WHERE squad_owner = '%u' AND id != '%u' ORDER BY longnick LIMIT 35", squadowner, squadowner); // find other members than leader

				mysql_free_result(my_result);
				my_result = NULL;
				my_row = NULL;

				if (!d_mysql_query(&my_sock, my_query)) // query succeeded
				{
					if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
					{
						squad1 = (squadmembers1_t *) buffer;

						squad1->packetid = htons(Com_WBhton(0x0409));

						if ((num_rows = mysql_num_rows(my_result)) > 0)
						{
							squad1->nicks = htons(num_rows + 1);

							for (i = 0; i < num_rows; i++)
							{
								if ((my_row = mysql_fetch_row(my_result)))
								{
									squad2 = (squadmembers2_t *) (buffer+13+(8 *i));

									squad2->nick = htonl(ascii2wbnick(Com_MyRow("longnick"), 0));
									squad2->attr = htonl(Com_Atou(Com_MyRow("squad_flag")));
								}
								else
								{
									mysql_free_result(my_result);
									my_result = NULL;
									Com_Printf(VERBOSE_WARNING, "PSquadInfo(members): Couldn't Fetch Row %d, error %d: %s\n", i, mysql_errno(&my_sock), mysql_error(&my_sock));
									break;
								}
							}
						}
						else
						{
							squad1->nicks = htons(1);
							Com_Printf(VERBOSE_WARNING, "PSquadInfo(members): num_rows = %u\n", num_rows);
						}

						mysql_free_result(my_result);
						my_result = NULL;
						my_row = NULL;

						sprintf(my_query, "SELECT name, motto FROM squads WHERE owner = '%u'", squadowner);

						if (!d_mysql_query(&my_sock, my_query)) // query succeeded
						{
							if ((my_result = mysql_store_result(&my_sock))) // returned a non-NULL value
							{
								if ((my_row = mysql_fetch_row(my_result)))
								{
									j = i = 0;

									if (Com_MyRow("name"))
									{
										buffer[13+(8*num_rows)] = i = strlen(Com_MyRow("name"));
										memcpy(&(buffer[14+(8*num_rows)]), Com_MyRow("name"), i);
									}
									if (Com_MyRow("motto"))
									{
										buffer[14+(8*num_rows) + i] = j = strlen(Com_MyRow("motto"));
										memcpy(&(buffer[15+(8*num_rows) + i]), Com_MyRow("motto"), j);
									}

									SendPacket(buffer, 15 + i + j + (8*num_rows), client);
								}
								else
								{
									if (!mysql_errno(&my_sock))
									{
										PPrintf(client, RADIO_LIGHTYELLOW, "Squadron info not found, contact admin");
										Com_Printf(VERBOSE_ATTENTION, "PSquadInfo(name): Squadron info not found, owner = %u", squadowner);
									}
									else
									{
										Com_Printf(VERBOSE_WARNING, "PSquadInfo(name): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
									}
								}

								mysql_free_result(my_result);
								my_result = NULL;
								my_row = NULL;
							}
							else
							{
								Com_Printf(VERBOSE_WARNING, "PSquadInfo(name): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
							}
						}
						else
						{
							Com_Printf(VERBOSE_WARNING, "PSquadInfo(name): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
						}
					}
					else
					{
						Com_Printf(VERBOSE_WARNING, "PSquadInfo(members): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
					}
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "PSquadInfo(members): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}
			}
			else
			{
				if (!mysql_errno(&my_sock))
				{
					PPrintf(client, RADIO_LIGHTYELLOW, "Squadron leader not found, contact admin");
				}
				else
				{
					Com_Printf(VERBOSE_WARNING, "PSquadInfo(owner): Couldn't Fetch Row, error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
				}

				mysql_free_result(my_result);
				my_result = NULL;
				my_row = NULL;
			}
		}
		else
		{
			Com_Printf(VERBOSE_WARNING, "PSquadInfo(owner): my_result == NULL error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
	}
	else
	{
		if (mysql_errno(&my_sock))
		{
			Com_Printf(VERBOSE_WARNING, "PSquadInfo(owner): couldn't query SELECT error %d: %s\n", mysql_errno(&my_sock), mysql_error(&my_sock));
		}
		else
			return;
	}

	if (mysql_errno(&my_sock))
	{
		PPrintf(client, RADIO_YELLOW, "PSquadInfo(): SQL Error (%d), please contact admin", mysql_errno(&my_sock));
	}
}


/**
 CalcDamage

 Calculate damage
 */

double CalcDamage(u_int32_t mass, u_int16_t vel)
{
	double energy, speed;

	speed = (double)vel * 0.3048;

	energy = (double) mass/1000 * Com_Pow(speed, 2) * lethality->value / 2;

	return energy;
}

/**
 Kamikase

 Checks if client made a kamikase crash
 */

void Kamikase(client_t *client)
{
	int16_t i;

	// don't allow TK clients to make kamikase attacks
	if (client->tkstatus)
		return;

	if (client->cancollide)
	{
		i = client->posalt[0] + (client->speedxyz[2][0] / 2) - GetHeightAt(client->posxy[0][0], client->posxy[1][0]);

		if (i < 100) // supposed to made a kamikase
		{
			PPrintf(client, RADIO_YELLOW, "You made a Kamikase hit!");
			AddBomb(0x01F9, client->posxy[0][0] + (client->speedxyz[0][0] / 2), client->posxy[1][0] + (client->speedxyz[1][0] / 2), 85/*800Kg*/, client->speedxyz[2][0], 0, client);
		}
	}
}

/**
 SinkBoat

 Sink or Raise boats
 */

void SinkBoat(u_int8_t raise, building_t* building, client_t *client)
{
	sinkboat_t *sink;
	u_int16_t i;
	u_int8_t boat;
	u_int8_t buffer[6];

	for (i = 0, boat = 0; i < MAX_BUILDINGS; i++)
	{
		if (!arena->fields[building->field - 1].buildings[i].field)
		{
			break;
		}
		else if ((arena->fields[building->field - 1].buildings[i].type >= BUILD_CV) && (arena->fields[building->field - 1].buildings[i].type <= BUILD_SUBMARINE))
		{
			if (building->id == arena->fields[building->field - 1].buildings[i].id)
				break;
			else
				boat++;
		}
	}

	memset(buffer, 0, sizeof(buffer));

	sink = (sinkboat_t *) buffer;

	if (raise)
	{
		sink->packetid = htons(Com_WBhton(0x0305));
	}
	else
	{
		sink->packetid = htons(Com_WBhton(0x0304));
	}
	sink->task = htons(arena->fields[building->field - 1].cv->id);
	sink->boat = htons(boat);

	if (client)
		SendPacket(buffer, sizeof(buffer), client);
	else
	{
		memset(arena->thaisent, 0, sizeof(arena->thaisent));

		for (i = 0; i < maxentities->value; i++)
		{
			if (clients[i].inuse && !clients[i].drone && clients[i].ready)
			{
				if(clients[i].thai) // SinkBoat
				{
					if(arena->thaisent[clients[i].thai].b)
						continue;
					else
						arena->thaisent[clients[i].thai].b = 1;
				}

				SendPacket(buffer, sizeof(buffer), &clients[i]);
			}
		}
	}
}

/**
 GetFactoryReupTime

 Returns factory reup time based on numplayers
 */

u_int32_t GetFactoryReupTime(u_int8_t country)
{
	u_int16_t players, i;
	static u_int32_t delay= MAX_UINT32;

	if (!(arena->frame % 1000))
	{
		for (players = i = 0; i < maxentities->value; i++)
		{
			if (clients[i].attr == 1 && hideadmin->value)
				continue;

			if (clients[i].inuse && !clients[i].drone && clients[i].country != country)
			{
				players++; // TODO: FIXME: use this value to some thing!!!
			}
		}

		delay = (900.0 / ((1.0 / 10.0) + 4.0)) - 60;
		delay *= 6000; // convert to minutes
	}

	return delay;
}

/**
 GetRPSLag

 Returns RPS lag based on factories health and numplayers
 */

u_int32_t GetRPSLag(u_int8_t country)
{
	u_int16_t i;
	u_int32_t RPSlag;
	u_int16_t players;
	double RPScoeff;
	double industrylack;
	double playerspart;

	if (wb3->value)
		return 0;

	for (players = i = 0; i < maxentities->value; i++)
	{
		if (clients[i].attr == 1 && hideadmin->value)
			continue;

		if (clients[i].inuse && !clients[i].drone && clients[i].country == country)
		{
			players++;
		}
	}

	playerspart = (double)players / (arena->numplayers ? arena->numplayers : 1);

	industrylack = 1 - (double)factorybuildingsup[country - 1] / (double)(factorybuildings[country - 1] ? factorybuildings[country - 1] : 1);

	RPScoeff = industrylack * playerspart;

	RPSlag = (double)RPScoeff * 365 * 2;

	return RPSlag > 365 ? 365 : RPSlag;
}
