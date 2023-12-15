//#####################################
//##                                 ##
//## MaxxFan Wemos Remote Controller ##
//## By Caschy 8/2021                ##
//##                                 ##
//#####################################
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "index.h"
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "MillisTimer.h"

const uint16_t kIrLed = 12;

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

//wifi variables
const char* ssid = "MaxxFanCtrl";
const char* password = "caschy1980";
// Set web server port number to 80
ESP8266WebServer webserver(80);

//timer variables
bool timerSet = false;
String timerVal = "";
unsigned long DELAY_TIME = 0; // 1500 = 1.5 sec
unsigned long TIME_LEFT = 0;
bool delayRunning = false; // true if still waiting for delay to finish
MillisTimer timer15 = MillisTimer(900000);
MillisTimer timer30 = MillisTimer(1800000);
MillisTimer timer1h = MillisTimer(3600000);
MillisTimer timer2h = MillisTimer(7200000);
MillisTimer timer3h = MillisTimer(10800000);
MillisTimer timer4h = MillisTimer(14400000);

//captured ir signals from Maxxfan remote
uint16_t fanOpen[81] = {1646, 842,  854, 1634,  854, 802,  852, 1630,  852, 808,  854, 804,  1680, 808,  852, 2460,  6624, 2498,  854, 5776,  854, 1632,  5832, 804,  854, 1630,  854, 4950,  854, 2456,  5010, 800,  1680, 1634,  854, 4120,  852, 3288,  4182, 798,  2506, 1640,  2472, 1678,  1678, 3288,  854, 806,  854, 1632,  3320, 1652,  1646, 846,  854, 800,  3298, 1676,  854, 3290,  1680, 806,  854, 1630,  854, 8262,  854, 1632,  2472, 844,  1682, 1632,  2518, 804,  856, 800,  1682, 2458,  854, 806,  5836};  // UNKNOWN 969C31E2
uint16_t fanClose[77] = {1652, 840,  824, 1660,  824, 834,  854, 1626,  826, 836,  824, 832,  1650, 836,  850, 2462,  6628, 2494,  852, 5780,  824, 1660,  5804, 832,  852, 1632,  854, 4950,  852, 2460,  5006, 804,  1678, 1636,  824, 4148,  824, 3316,  4182, 802,  2502, 1642,  2474, 1674,  1678, 3288,  852, 2468,  4176, 1624,  1650, 844,  852, 804,  3298, 1674,  852, 3292,  1682, 804,  856, 1628,  850, 8264,  852, 1634,  2476, 842,  1678, 1634,  1650, 3326,  1678, 2462,  852, 810,  5832};  // UNKNOWN 5E8FC705
uint16_t fanIn[79] = {1664, 834,  834, 1652,  836, 824,  834, 1652,  834, 828,  834, 826,  1662, 830,  834, 2480,  6654, 2490,  862, 5770,  834, 1654,  5856, 800,  862, 1622,  862, 4942,  862, 2452,  5030, 796,  1690, 1628,  862, 4114,  860, 3278,  4200, 796,  2518, 1632,  2488, 1668,  1688, 3282,  860, 2462,  4168, 1646,  2510, 816,  1688, 1636,  858, 1624,  860, 806,  2488, 834,  858, 802,  858, 1628,  832, 8284,  856, 1630,  2486, 838,  1686, 1632,  1686, 2466,  856, 4112,  858, 806,  5852};  // UNKNOWN AB185103
uint16_t fanOut[81] = {1656, 864,  802, 1686,  826, 806,  854, 1656,  828, 834,  828, 806,  1682, 812,  826, 2486,  6648, 2494,  854, 5780,  850, 1636,  5822, 858,  826, 1634,  852, 4954,  848, 2488,  4994, 804,  1680, 1662,  828, 4148,  828, 3314,  4168, 802,  2508, 1668,  2458, 1698,  1654, 3292,  852, 836,  828, 1660,  3310, 1672,  2484, 846,  1630, 1692,  828, 1656,  826, 840,  2482, 840,  826, 832,  828, 1658,  826, 8290,  826, 1664,  2480, 818,  1654, 1688,  2480, 850,  1652, 4146,  826, 836,  5794};  // UNKNOWN A18FAEF
uint16_t fan10p[87] = {1662, 836,  860, 1628,  858, 800,  858, 1630,  856, 804,  858, 802,  1688, 806,  860, 2456,  6654, 2490,  860, 5774,  860, 1628,  5854, 804,  858, 1628,  860, 4946,  858, 2458,  5000, 828,  1660, 1658,  860, 4118,  858, 3284,  4198, 798,  2514, 1638,  2488, 1670,  1686, 3288,  858, 806,  856, 1632,  856, 804,  1686, 1632,  1686, 814,  856, 802,  3314, 1670,  832, 832,  2488, 836,  856, 804,  830, 1656,  856, 8262,  854, 1634,  2484, 840,  1686, 1634,  1684, 812,  856, 804,  854, 804,  856, 2460,  856, 808,  5852};  // UNKNOWN 8BABFCCF
uint16_t fan20p[83] = {1662, 836,  834, 1654,  834, 824,  836, 1652,  834, 828,  836, 826,  1664, 830,  834, 2480,  6658, 2490,  862, 5770,  862, 1626,  5860, 798,  864, 1622,  862, 4942,  864, 2452,  5032, 796,  1692, 1626,  862, 4114,  862, 3280,  4204, 794,  2518, 1632,  2490, 1670,  1690, 3282,  862, 802,  862, 1628,  3340, 1646,  2510, 818,  832, 826,  2490, 1662,  860, 804,  2490, 836,  858, 802,  858, 1628,  832, 8286,  858, 1630,  2488, 838,  1686, 1634,  2488, 844,  832, 824,  1686, 2462,  856, 806,  5854};  // UNKNOWN 60A51449
uint16_t fan30p[79] = {1660, 836,  860, 1628,  860, 800,  860, 1626,  860, 802,  860, 802,  1688, 806,  860, 2454,  6656, 2490,  860, 5774,  858, 1630,  5856, 802,  860, 1626,  860, 4944,  860, 2454,  5028, 800,  1688, 1630,  858, 4118,  860, 3282,  4200, 798,  2516, 1634,  2488, 1668,  1688, 3284,  858, 804,  860, 1628,  3340, 1646,  1688, 3292,  2490, 1662,  860, 804,  2488, 834,  860, 802,  860, 1626,  860, 8258,  860, 1628,  2488, 836,  1688, 1632,  1688, 3292,  1688, 2458,  860, 804,  5856};  // UNKNOWN BF4BC811
uint16_t fan40p[83] = {1688, 806,  860, 1628,  860, 802,  860, 1624,  860, 804,  860, 800,  1688, 804,  860, 2454,  6656, 2488,  860, 5772,  860, 1628,  5856, 800,  860, 1626,  860, 4946,  860, 2454,  5028, 800,  1686, 1630,  860, 4116,  860, 3284,  4198, 796,  2516, 1636,  2488, 1666,  1688, 3284,  860, 804,  860, 1628,  3340, 1642,  3344, 816,  860, 800,  1688, 1630,  860, 806,  2516, 808,  860, 800,  860, 1626,  860, 8258,  860, 1630,  2490, 836,  1688, 1630,  3370, 792,  860, 800,  860, 2454,  860, 804,  5856};  // UNKNOWN 4F0FB452
uint16_t fan50p[83] = {1712, 782,  834, 1654,  834, 824,  858, 1628,  834, 828,  834, 826,  1662, 830,  834, 2480,  6656, 2490,  834, 5798,  834, 1654,  5830, 826,  834, 1650,  834, 4968,  836, 2478,  5002, 826,  1664, 1654,  834, 4142,  834, 3306,  4174, 822,  2492, 1658,  2490, 1666,  1664, 3308,  834, 828,  834, 1654,  3340, 1644,  1664, 834,  1662, 1656,  1662, 1656,  834, 830,  2490, 832,  834, 826,  834, 1650,  836, 8282,  834, 1654,  2488, 836,  1662, 1656,  1662, 832,  1662, 1658,  834, 2480,  834, 830,  5828};  // UNKNOWN 4305A19F
uint16_t fan60p[79] = {1660, 832,  834, 1654,  834, 824,  836, 1650,  834, 828,  834, 826,  1662, 830,  834, 2478,  6656, 2488,  862, 5770,  862, 1624,  5858, 798,  862, 1622,  862, 4942,  862, 2450,  5030, 794,  1690, 1628,  862, 4114,  862, 3280,  4202, 794,  2518, 1634,  2518, 1638,  1690, 3282,  862, 802,  862, 1626,  3340, 1642,  2514, 3300,  1688, 1628,  860, 804,  2516, 806,  860, 798,  860, 1626,  860, 8256,  858, 1630,  2488, 836,  1686, 1630,  2488, 3324,  858, 2456,  858, 806,  5852};  // UNKNOWN A52D45FC
uint16_t fan70p[81] = {1662, 834,  834, 1654,  834, 824,  862, 1626,  860, 802,  860, 800,  1690, 802,  862, 2452,  6654, 2490,  860, 5770,  862, 1626,  5858, 798,  860, 1624,  860, 4944,  860, 2452,  5026, 800,  1688, 1630,  860, 4116,  860, 3282,  4198, 798,  2514, 1636,  2486, 1668,  1686, 3284,  858, 806,  860, 1628,  3338, 1646,  1686, 1638,  2488, 840,  858, 1624,  858, 806,  2488, 834,  832, 828,  858, 1628,  856, 8260,  858, 1628,  2486, 838,  1686, 1634,  1684, 1642,  2512, 3292,  856, 806,  5850};  // UNKNOWN 25AEEB33
uint16_t fan80p[81] = {1662, 834,  834, 1654,  834, 824,  860, 1626,  860, 800,  834, 826,  1690, 802,  862, 2452,  6658, 2488,  862, 5770,  862, 1626,  5858, 798,  862, 1624,  862, 4942,  864, 2450,  5030, 796,  1692, 1626,  862, 4114,  862, 3280,  4202, 794,  2518, 1632,  2488, 1668,  1688, 3282,  860, 804,  860, 1628,  3340, 1644,  4200, 792,  860, 804,  860, 1624,  832, 832,  2488, 834,  858, 802,  860, 1626,  858, 8258,  860, 1628,  2488, 838,  1686, 1632,  4198, 796,  856, 3284,  858, 806,  5826};  // UNKNOWN F5510885
uint16_t fan90p[85] = {1662, 834,  834, 1654,  834, 826,  832, 1652,  862, 800,  862, 798,  1692, 802,  862, 2452,  6656, 2490,  860, 5772,  862, 1626,  5858, 798,  862, 1622,  864, 4940,  862, 2452,  5030, 796,  1690, 1628,  862, 4114,  862, 3280,  4200, 796,  2518, 1632,  2488, 1668,  1688, 3282,  860, 802,  860, 1628,  3340, 1644,  1688, 810,  858, 1626,  860, 804,  862, 1622,  860, 806,  2488, 834,  858, 802,  858, 1628,  858, 8258,  858, 1630,  2486, 838,  1660, 1658,  1686, 808,  832, 1654,  860, 3282,  858, 806,  5852};  // UNKNOWN 23D3B23
uint16_t fan100p[83] = {1686, 810,  834, 1654,  834, 826,  834, 1650,  834, 828,  834, 824,  1662, 830,  836, 2478,  6656, 2488,  834, 5796,  836, 1654,  5828, 826,  834, 1650,  834, 4970,  834, 2478,  5002, 824,  1664, 1656,  834, 4140,  834, 3308,  4174, 820,  2492, 1660,  2488, 1668,  1662, 3308,  834, 828,  834, 1654,  834, 824,  1662, 1656,  2512, 816,  1662, 1660,  834, 1648,  834, 832,  2490, 832,  834, 824,  834, 1652,  834, 8282,  834, 1654,  2490, 836,  1662, 1654,  2488, 842,  2490, 3314,  834, 830,  5828};  // UNKNOWN 1076EC6C
uint16_t fanOff[81] = {1654, 838,  854, 1658,  802, 858,  828, 1634,  848, 836,  828, 808,  1678, 838,  802, 2486,  6648, 2518,  802, 5830,  802, 1660,  5848, 832,  828, 1656,  828, 4978,  826, 2462,  5018, 830,  1654, 1638,  850, 4148,  828, 3314,  4166, 828,  2456, 1670,  2504, 1674,  1656, 3314,  2482, 848,  4138, 1672,  2508, 820,  1656, 1666,  802, 1682,  826, 838,  2482, 840,  826, 832,  826, 1658,  828, 8288,  828, 1660,  2458, 868,  1628, 1688,  800, 864,  826, 1660,  824, 4144,  826, 836,  5820};  // UNKNOWN 34233BDA
uint16_t fanOn[81] = {1686, 806,  862, 1630,  830, 828,  832, 1652,  860, 802,  832, 828,  1686, 804,  832, 2482,  6652, 2490,  860, 5772,  860, 1626,  5828, 828,  858, 1626,  858, 4946,  858, 2454,  5024, 800,  1688, 1632,  858, 4116,  858, 3284,  4170, 824,  2512, 1638,  2488, 1668,  1686, 3284,  858, 804,  858, 1630,  3338, 1644,  2510, 818,  1686, 1636,  830, 1654,  854, 810,  2514, 810,  854, 804,  856, 1628,  856, 8260,  856, 1632,  2486, 838,  1684, 1634,  2484, 844,  1684, 4116,  856, 808,  5850};  // UNKNOWN A18FAEF
uint16_t fanAutoOn[81] = {1674, 814,  822, 1662,  822, 836,  822, 1658,  850, 808,  850, 806,  1674, 812,  822, 2488,  6624, 2498,  822, 5804,  850, 1634,  5826, 808,  850, 1632,  822, 4980,  822, 2488,  4974, 832,  1648, 1666,  822, 4148,  824, 3316,  4150, 830,  2498, 1644,  2472, 1676,  1648, 3316,  1648, 1672,  848, 806,  2472, 1670,  2496, 824,  1646, 1670,  822, 1658,  850, 814,  848, 3294,  1646, 1662,  848, 8264,  848, 1640,  2470, 846,  1646, 1664,  822, 1666,  1646, 840,  1648, 2492,  850, 810,  5800};  // UNKNOWN 68A410D6
uint16_t fanAutoOff[77] = {1650, 838,  826, 1660,  824, 832,  824, 1656,  824, 836,  826, 832,  1650, 836,  826, 2484,  6626, 2494,  824, 5802,  824, 1660,  5802, 832,  826, 1658,  824, 4976,  824, 2484,  4976, 830,  1650, 1660,  826, 4146,  824, 3314,  4182, 798,  2502, 1640,  2472, 1674,  1676, 3288,  2472, 850,  4148, 1650,  2496, 824,  1676, 1638,  850, 1630,  824, 838,  850, 3290,  1676, 1636,  846, 8262,  824, 1662,  2474, 842,  1648, 1664,  822, 836,  4974, 2484,  822, 838,  5800};  // UNKNOWN EF77808A
uint16_t fan60t[73] = {1646, 844,  824, 1662,  824, 832,  824, 1660,  824, 834,  824, 832,  1650, 836,  824, 2486,  6626, 2496,  824, 5804,  824, 1660,  5802, 834,  824, 1658,  824, 4976,  824, 2486,  4978, 832,  1648, 1662,  824, 4150,  824, 3314,  4152, 828,  2476, 1666,  2472, 1676,  1650, 3314,  2472, 850,  4148, 1652,  2472, 846,  1650, 1668,  824, 1654,  2474, 3334,  1648, 1660,  824, 8290,  824, 1660,  2474, 844,  1650, 1664,  6624, 2496,  824, 836,  5802};  // UNKNOWN D505127A
uint16_t fan65t[77] = {1648, 840,  824, 1662,  824, 832,  826, 1656,  824, 836,  824, 832,  1652, 836,  826, 2486,  6626, 2496,  824, 5806,  824, 1660,  5804, 832,  824, 1658,  826, 4976,  826, 2486,  4978, 830,  1650, 1662,  824, 4148,  826, 3316,  4154, 828,  2476, 1668,  2474, 1674,  1650, 3316,  2474, 848,  4150, 1650,  2498, 824,  1650, 1666,  826, 1656,  824, 838,  4152, 822,  824, 1658,  824, 8290,  824, 1662,  2474, 844,  1650, 1662,  824, 836,  826, 6628,  824, 836,  5804};  // UNKNOWN E898C3CB
uint16_t fan70t[77] = {1650, 842,  824, 1660,  824, 832,  824, 1658,  826, 834,  824, 834,  1678, 810,  852, 2460,  6626, 2496,  852, 5778,  852, 1632,  5830, 808,  824, 1658,  854, 4948,  824, 2486,  5006, 804,  1678, 1636,  852, 4122,  852, 3286,  4152, 832,  2502, 1640,  2472, 1676,  1678, 3288,  2474, 848,  4150, 1650,  2496, 824,  1678, 1642,  852, 1628,  1648, 1670,  2472, 844,  824, 1660,  824, 8290,  852, 1632,  2474, 844,  1676, 1638,  1676, 812,  850, 5774,  852, 810,  5802};  // UNKNOWN DE5E8E44
uint16_t fan75t[79] = {1646, 842,  824, 1662,  824, 832,  824, 1658,  824, 836,  824, 832,  1650, 838,  822, 2488,  6624, 2498,  822, 5806,  822, 1660,  5804, 832,  824, 1658,  822, 4980,  824, 2486,  4976, 830,  1650, 1664,  824, 4148,  824, 3316,  4152, 828,  2476, 1668,  2472, 1676,  1650, 3316,  2472, 850,  4150, 1650,  2474, 846,  1648, 1668,  824, 1656,  824, 1664,  824, 834,  1650, 836,  824, 1658,  824, 8288,  824, 1660,  2474, 844,  1650, 1664,  824, 2494,  824, 4968,  824, 836,  5802};  // UNKNOWN 96EFBDC3
uint16_t fan80t[77] = {1650, 838,  826, 1662,  824, 832,  824, 1656,  824, 836,  826, 832,  1650, 836,  824, 2486,  6628, 2496,  824, 5804,  824, 1660,  5804, 832,  824, 1658,  824, 4978,  824, 2486,  4978, 830,  1650, 1662,  824, 4148,  824, 3316,  4154, 828,  2476, 1666,  2474, 1674,  1650, 3314,  2474, 848,  4150, 1648,  2498, 824,  1650, 1668,  824, 1656,  4154, 826,  824, 832,  824, 1658,  826, 8288,  824, 1662,  2474, 842,  1650, 1662,  2474, 1674,  824, 4142,  826, 834,  5804};  // UNKNOWN A19E5CC3
uint16_t fan85t[83] = {1650, 840,  824, 1662,  824, 834,  824, 1658,  824, 836,  824, 832,  1650, 836,  826, 2486,  6626, 2494,  824, 5806,  824, 1662,  5802, 834,  824, 1658,  824, 4978,  824, 2486,  4978, 830,  1650, 1662,  824, 4148,  824, 3316,  4152, 830,  2474, 1668,  2474, 1674,  1650, 3318,  2472, 850,  4148, 1650,  2496, 824,  1650, 1666,  824, 1656,  826, 838,  824, 830,  824, 834,  826, 832,  824, 1658,  824, 8290,  824, 1662,  2474, 844,  1650, 1662,  824, 836,  1650, 836,  824, 4142,  824, 836,  5804};  // UNKNOWN AC74A565
uint16_t fanOff_f[35] = {1620, 860, 820, 1760, 740, 1040, 660, 1740, 740, 1000, 240, 140, 380, 960, 1480, 5860, 460, 140, 400, 2040, 280, 1220, 300, 140, 240, 3080, 200, 6640, 200, 2120, 240, 140, 300, 140, 260};  // UNKNOWN
//--------------------------------------------

void handle_OnConnect() {
  String s = MAIN_page; //Read HTML contents
  webserver.send(200, "text/html", s); //Send web page
}

void handle_readDATA() {
  if (timerSet) {
    webserver.send(200, "text/html", timerVal+"|"+String(TIME_LEFT));
  } else {
    webserver.send(200, "text/html", "Timer Not Set|0");
  }
}

void handle_timer15m() {
  timerSet = true;
  timerVal = "15 minutes";
  DELAY_TIME = 900000;
  // start delay
  delayRunning = true;
  timer15.setInterval(900000);
  timer15.expiredHandler(myTimerFunction);
  timer15.setRepeats(1);
  timer15.start();
  Serial.println("Timer 15m");
}

void handle_timer30m() {
  timerSet = true;
  timerVal = "30 minutes";
  DELAY_TIME = 1800000;
  // start delay
  delayRunning = true;
  timer30.setInterval(1800000);
  timer30.expiredHandler(myTimerFunction);
  timer30.setRepeats(1);
  timer30.start();
  Serial.println("Timer 30m");
}

void handle_timer1h() {
  timerSet = true;
  timerVal = "1 hour";
  DELAY_TIME = 3600000;
  // start delay
  delayRunning = true;
  timer1h.setInterval(3600000);
  timer1h.expiredHandler(myTimerFunction);
  timer1h.setRepeats(1);
  timer1h.start();
  Serial.println("Timer 1h");
}

void handle_timer2h() {
  timerSet = true;
  timerVal = "2 hours";
  DELAY_TIME = 7200000;
  // start delay
  delayRunning = true;
  timer2h.setInterval(7200000);
  timer2h.expiredHandler(myTimerFunction);
  timer2h.setRepeats(1);
  timer2h.start();
  Serial.println("Timer 2h");
}

void handle_timer3h() {
  timerSet = true;
  timerVal = "3 hours";
  DELAY_TIME = 10800000;
  // start delay
  delayRunning = true;
  timer3h.setInterval(10800000);
  timer3h.expiredHandler(myTimerFunction);
  timer3h.setRepeats(1);
  timer3h.start();
  Serial.println("Timer 3h");
}

void handle_timer4h() {
  timerSet = true;
  timerVal = "4 hours";
  DELAY_TIME = 14400000;
  // start delay
  delayRunning = true;
  timer4h.setInterval(14400000);
  timer4h.expiredHandler(myTimerFunction);
  timer4h.setRepeats(1);
  timer4h.start();
  Serial.println("Timer 4h");
}

void handle_timerOff() {
  timerSet = false;
  timerVal = " ";
  delayRunning = false;
  webserver.send(200, "text/plane", "Timer Not Set");
  Serial.println("Timer OFF");
  timer15.stop();
  timer15.reset();
  timer30.stop();
  timer30.reset();
  timer1h.stop();
  timer1h.reset();
  timer2h.stop();
  timer2h.reset();
  timer3h.stop();
  timer3h.reset();
  timer4h.stop();
  timer4h.reset();
  
}

// This is the function that is called when the timer expires.
void myTimerFunction(MillisTimer &mt)
{
  if (DELAY_TIME = 900000) {
    timer15.stop();
    Serial.print("Timer DONE!");
    timerSet = false;
    timerVal = " ";
    delayRunning = false;
  } else if (DELAY_TIME = 1800000) {
    timer30.stop();
    Serial.print("Timer DONE!");
    timerSet = false;
    timerVal = " ";
    delayRunning = false;
  } else if (DELAY_TIME = 3600000) {
    timer1h.stop();
    Serial.print("Timer DONE!");
    timerSet = false;
    timerVal = " ";
    delayRunning = false;
  }  else if (DELAY_TIME = 7200000) {
    timer2h.stop();
    Serial.print("Timer DONE!");
    timerSet = false;
    timerVal = " ";
    delayRunning = false;
  } else if (DELAY_TIME = 10800000) {
    timer3h.stop();
    Serial.print("Timer DONE!");
    timerSet = false;
    timerVal = " ";
    delayRunning = false;
  } else if (DELAY_TIME = 14400000) {
    timer4h.stop();
    Serial.print("Timer DONE!");
    timerSet = false;
    timerVal = " ";
    delayRunning = false;
  }
  IRsignal_Off();
}

void handle_fanOFF() {
  IRsignal_Off();
  Serial.println("OFF");
}

void handle_fanON() {
  IRsignal_On();
  Serial.println("ON");
}

void handle_fanOPEN() {
  IRsignal_Open();
  Serial.println("OPEN");
}

void handle_fanCLOSE() {
  IRsignal_Closed();
  Serial.println("CLOSE");
}

void handle_fanIN() {
  IRsignal_OutIn();
  Serial.println("IN");
}

void handle_fanOUT() {
  IRsignal_InOut();
  Serial.println("OUT");
}

void handle_fan10p() {
  IRsignal_10pct();
  Serial.println("10% speed");
}

void handle_fan20p() {
  IRsignal_20pct();
  Serial.println("20% speed");
}

void handle_fan30p() {
  IRsignal_30pct();
  Serial.println("30% speed");
}

void handle_fan40p() {
  IRsignal_40pct();
  Serial.println("40% speed");
}

void handle_fan50p() {
  IRsignal_50pct();
  Serial.println("50% speed");
}

void handle_fan60p() {
  IRsignal_60pct();
  Serial.println("60% speed");
}

void handle_fan70p() {
  IRsignal_70pct();
  Serial.println("70% speed");
}

void handle_fan80p() {
  IRsignal_80pct();
  Serial.println("80% speed");
}

void handle_fan90p() {
  IRsignal_90pct();
  Serial.println("90% speed");
}

void handle_fan100p() {
  IRsignal_100pct();
  Serial.println("100% speed");
}

void setup() {
  irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266

  // Wi-Fi connection
  WiFi.softAP(ssid, password);
  Serial.println("MaxxFanCtrl WiFi AP Ready!");

  webserver.on("/", handle_OnConnect);
  webserver.on("/fanOPEN", handle_fanOPEN);
  webserver.on("/fanCLOSE", handle_fanCLOSE);
  webserver.on("/fanON", handle_fanON);
  webserver.on("/fanOFF", handle_fanOFF);
  webserver.on("/fanIN", handle_fanIN);
  webserver.on("/fanOUT", handle_fanOUT);
  webserver.on("/fan10p", handle_fan10p);
  webserver.on("/fan20p", handle_fan20p);
  webserver.on("/fan30p", handle_fan30p);
  webserver.on("/fan40p", handle_fan40p);
  webserver.on("/fan50p", handle_fan50p);
  webserver.on("/fan60p", handle_fan60p);
  webserver.on("/fan70p", handle_fan70p);
  webserver.on("/fan80p", handle_fan80p);
  webserver.on("/fan90p", handle_fan90p);
  webserver.on("/fan100p", handle_fan100p);
  webserver.on("/timer15m", handle_timer15m);
  webserver.on("/timer30m", handle_timer30m);
  webserver.on("/timer1h", handle_timer1h);
  webserver.on("/timer2h", handle_timer2h);
  webserver.on("/timer3h", handle_timer3h);
  webserver.on("/timer4h", handle_timer4h);
  webserver.on("/timerOff", handle_timerOff);
  webserver.on("/readDATA", handle_readDATA);

  webserver.begin();
}

void loop() {
  webserver.handleClient();

  if (delayRunning && (DELAY_TIME == 900000)) {
    timer15.run();
    //Serial.println(timer15.getRemainingTime());
    TIME_LEFT = timer15.getRemainingTime();
  } else if (delayRunning && (DELAY_TIME == 1800000)) {
    timer30.run();
    //Serial.println(timer30.getRemainingTime());
    TIME_LEFT = timer30.getRemainingTime();
  } if (delayRunning && (DELAY_TIME == 3600000)) {
    timer1h.run();
    //Serial.println(timer1h.getRemainingTime());
    TIME_LEFT = timer1h.getRemainingTime();
  } if (delayRunning && (DELAY_TIME == 7200000)) {
    timer2h.run();
    //Serial.println(timer2h.getRemainingTime());
    TIME_LEFT = timer2h.getRemainingTime();
  } if (delayRunning && (DELAY_TIME == 10800000)) {
    timer3h.run();
    //Serial.println(timer3h.getRemainingTime());
    TIME_LEFT = timer3h.getRemainingTime();
  } if (delayRunning && (DELAY_TIME == 14400000)) {
    timer4h.run();
    //Serial.println(timer4h.getRemainingTime());
    TIME_LEFT = timer4h.getRemainingTime();
  } else {
    delay(50);
  }

}

void IRsignal_Off() {
  irsend.sendRaw(fanOff, 81, 38);  // Send a raw data capture at 38kHz.
  delay(3000);
  irsend.sendRaw(fanOff_f, 35, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_On() {
  irsend.sendRaw(fanOn, 81, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_Closed() {
  irsend.sendRaw(fanClose, 77, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_Open() {
  irsend.sendRaw(fanOpen, 81, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_InOut() {
  irsend.sendRaw(fanIn, 79, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_OutIn() {
  irsend.sendRaw(fanOut, 81, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_Auto() {
  irsend.sendRaw(fanAutoOn, 81, 38);  // Send a raw data capture at 38kHz.
}

void IRsignalAutoOff() {
  irsend.sendRaw(fanAutoOff, 77, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_10pct() {
  irsend.sendRaw(fan10p, 87, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_20pct() {
  irsend.sendRaw(fan20p, 83, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_30pct() {
  irsend.sendRaw(fan30p, 79, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_40pct() {
  irsend.sendRaw(fan40p, 83, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_50pct() {
  irsend.sendRaw(fan50p, 83, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_60pct() {
  irsend.sendRaw(fan60p, 79, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_70pct() {
  irsend.sendRaw(fan70p, 81, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_80pct() {
  irsend.sendRaw(fan80p, 81, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_90pct() {
  irsend.sendRaw(fan90p, 85, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_100pct() {
  irsend.sendRaw(fan100p, 83, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_60t() {
  irsend.sendRaw(fan60t, 73, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_65t() {
  irsend.sendRaw(fan65t, 77, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_70t() {
  irsend.sendRaw(fan70t, 77, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_75t() {
  irsend.sendRaw(fan75t, 79, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_80t() {
  irsend.sendRaw(fan80t, 77, 38);  // Send a raw data capture at 38kHz.
}

void IRsignal_85t() {
  irsend.sendRaw(fan85t, 83, 38);  // Send a raw data capture at 38kHz.
}
