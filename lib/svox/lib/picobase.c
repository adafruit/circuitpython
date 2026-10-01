/*
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file picobase.c
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#include "picoos.h"
#include "picodbg.h"
#include "picodefs.h"
#include "picobase.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/**
 * @addtogroup picobase
 *
 * @b Unicode_UTF8_functions
 *
 * UTF8
 * scalar value                1st Byte    2nd Byte    3rd Byte    4th Byte
 * 00000000 0xxxxxxx           0xxxxxxx
 * 00000yyy yyxxxxxx           110yyyyy    10xxxxxx
 * zzzzyyyy yyxxxxxx           1110zzzz    10yyyyyy    10xxxxxx
 * 000uuuuu zzzzyyyy yyxxxxx   11110uuu    10uuzzzz    10yyyyyy    10xxxxxx
 *
*/
picoos_int32 picobase_utf8_length(const picoos_uint8 *utf8str,
                                  const picoos_uint16 maxlen) {

    picoos_uint16 i;
    picoos_uint16 len;
    picoos_uint8 follow;
    picoos_uint8 ok;

    ok = TRUE;
    i = 0;
    len = 0;
    follow = 0;
    while (ok && (i < maxlen) && (utf8str[i] != '\000')) {
        if (follow > 0) {
            if ((utf8str[i] >= (picoos_uint8)'\200') &&
                (utf8str[i] < (picoos_uint8)'\300')) {
                follow--;
            } else {
                ok = FALSE;
            }
        } else if (utf8str[i] < (picoos_uint8)'\200') {
            len++;
        } else if (utf8str[i] >= (picoos_uint8)'\370') {
            ok = FALSE;
        } else if (utf8str[i] >= (picoos_uint8)'\360') {
            follow = 3;
            len++;
        } else if (utf8str[i] >= (picoos_uint8)'\340') {
            follow = 2;
            len++;
        } else if (utf8str[i] >= (picoos_uint8)'\300') {
            follow = 1;
            len++;
        } else {
            ok = FALSE;
        }
        i++;
    }
    if (ok) {
        return len;
    } else {
        return -1;
    }
}


static picoos_uint32 base_utf32_lowercase (picoos_uint32 utf32)
{

    picoos_uint32 lc;

    lc = utf32;
    if (((utf32 >= 65313) && (utf32 <= 65338))) {
        lc = (utf32 + 32);
    } else if (((utf32 >= 66560) && (utf32 <= 66599))) {
        lc = (utf32 + 40);
    } else if (((utf32 >= 7680) && (utf32 <= 9423))) {
        switch (utf32) {
            case 7680:   case 7681:   case 7682:   case 7683:   case 7684:   case 7685:   case 7686:   case 7687:   case 7688:   case 7689:
            case 7690:   case 7691:   case 7692:   case 7693:   case 7694:   case 7695:   case 7696:   case 7697:   case 7698:   case 7699:   case 7700:   case 7701:
            case 7702:   case 7703:   case 7704:   case 7705:   case 7706:   case 7707:   case 7708:   case 7709:   case 7710:   case 7711:   case 7712:   case 7713:
            case 7714:   case 7715:   case 7716:   case 7717:   case 7718:   case 7719:   case 7720:   case 7721:   case 7722:   case 7723:   case 7724:   case 7725:
            case 7726:   case 7727:   case 7728:   case 7729:   case 7730:   case 7731:   case 7732:   case 7733:   case 7734:   case 7735:   case 7736:   case 7737:
            case 7738:   case 7739:   case 7740:   case 7741:   case 7742:   case 7743:   case 7744:   case 7745:   case 7746:   case 7747:   case 7748:   case 7749:
            case 7750:   case 7751:   case 7752:   case 7753:   case 7754:   case 7755:   case 7756:   case 7757:   case 7758:   case 7759:   case 7760:   case 7761:
            case 7762:   case 7763:   case 7764:   case 7765:   case 7766:   case 7767:   case 7768:   case 7769:   case 7770:   case 7771:   case 7772:   case 7773:
            case 7774:   case 7775:   case 7776:   case 7777:   case 7778:   case 7779:   case 7780:   case 7781:   case 7782:   case 7783:   case 7784:   case 7785:
            case 7786:   case 7787:   case 7788:   case 7789:   case 7790:   case 7791:   case 7792:   case 7793:   case 7794:   case 7795:   case 7796:   case 7797:
            case 7798:   case 7799:   case 7800:   case 7801:   case 7802:   case 7803:   case 7804:   case 7805:   case 7806:   case 7807:   case 7808:   case 7809:
            case 7810:   case 7811:   case 7812:   case 7813:   case 7814:   case 7815:   case 7816:   case 7817:   case 7818:   case 7819:   case 7820:   case 7821:
            case 7822:   case 7823:   case 7824:   case 7825:   case 7826:   case 7827:   case 7828:   case 7840:   case 7841:   case 7842:   case 7843:
            case 7844:   case 7845:   case 7846:   case 7847:   case 7848:   case 7849:   case 7850:   case 7851:   case 7852:   case 7853:   case 7854:   case 7855:
            case 7856:   case 7857:   case 7858:   case 7859:   case 7860:   case 7861:   case 7862:   case 7863:   case 7864:   case 7865:   case 7866:   case 7867:
            case 7868:   case 7869:   case 7870:   case 7871:   case 7872:   case 7873:   case 7874:   case 7875:   case 7876:   case 7877:   case 7878:   case 7879:
            case 7880:   case 7881:   case 7882:   case 7883:   case 7884:   case 7885:   case 7886:   case 7887:   case 7888:   case 7889:   case 7890:   case 7891:
            case 7892:   case 7893:   case 7894:   case 7895:   case 7896:   case 7897:   case 7898:   case 7899:   case 7900:   case 7901:   case 7902:   case 7903:
            case 7904:   case 7905:   case 7906:   case 7907:   case 7908:   case 7909:   case 7910:   case 7911:   case 7912:   case 7913:   case 7914:   case 7915:
            case 7916:   case 7917:   case 7918:   case 7919:   case 7920:   case 7921:   case 7922:   case 7923:   case 7924:   case 7925:   case 7926:   case 7927:
            case 7928:
                if ( !(((utf32) % 2 == 1))) {
                    lc = (utf32 + 1);
                }
                break;
            case 7944:   case 7945:   case 7946:   case 7947:   case 7948:   case 7949:   case 7950:   case 7951:   case 7960:
            case 7961:   case 7962:   case 7963:   case 7964:   case 7965:   case 7976:   case 7977:   case 7978:   case 7979:   case 7980:   case 7981:
            case 7982:   case 7983:   case 7992:   case 7993:   case 7994:   case 7995:   case 7996:   case 7997:   case 7998:   case 7999:
            case 8008:   case 8009:   case 8010:   case 8011:   case 8012:   case 8013:   case 8040:   case 8041:   case 8042:   case 8043:   case 8044:
            case 8045:   case 8046:   case 8047:   case 8072:   case 8073:   case 8074:   case 8075:   case 8076:   case 8077:   case 8078:   case 8079:
            case 8088:   case 8089:   case 8090:   case 8091:   case 8092:   case 8093:   case 8094:   case 8095:   case 8104:   case 8105:
            case 8106:   case 8107:   case 8108:   case 8109:   case 8110:   case 8111:
                lc = (utf32 - 8);
                break;
            case 8025:   case 8026:   case 8027:   case 8028:   case 8029:   case 8030:   case 8031:
                if (((utf32) % 2 == 1)) {
                    lc = (utf32 - 8);
                }
                break;
            case 8544:   case 8545:   case 8546:   case 8547:   case 8548:   case 8549:   case 8550:   case 8551:   case 8552:   case 8553:
            case 8554:   case 8555:   case 8556:   case 8557:   case 8558:   case 8559:
                lc = (utf32 + 16);
                break;
            case 9398:   case 9399:   case 9400:   case 9401:   case 9402:   case 9403:   case 9404:   case 9405:   case 9406:   case 9407:
            case 9408:   case 9409:   case 9410:   case 9411:   case 9412:   case 9413:   case 9414:   case 9415:   case 9416:   case 9417:   case 9418:   case 9419:
            case 9420:   case 9421:   case 9422:   case 9423:
                lc = (utf32 + 26);
                break;
            case 8120:
                lc = 8112;
                break;
            case 8121:
                lc = 8113;
                break;
            case 8122:
                lc = 8048;
                break;
            case 8123:
                lc = 8049;
                break;
            case 8124:
                lc = 8115;
                break;
            case 8136:
                lc = 8050;
                break;
            case 8137:
                lc = 8051;
                break;
            case 8138:
                lc = 8052;
                break;
            case 8139:
                lc = 8053;
                break;
            case 8140:
                lc = 8131;
                break;
            case 8152:
                lc = 8144;
                break;
            case 8153:
                lc = 8145;
                break;
            case 8154:
                lc = 8054;
                break;
            case 8155:
                lc = 8055;
                break;
            case 8168:
                lc = 8160;
                break;
            case 8169:
                lc = 8161;
                break;
            case 8170:
                lc = 8058;
                break;
            case 8171:
                lc = 8059;
                break;
            case 8172:
                lc = 8165;
                break;
            case 8184:
                lc = 8056;
                break;
            case 8185:
                lc = 8057;
                break;
            case 8186:
                lc = 8060;
                break;
            case 8187:
                lc = 8061;
                break;
            case 8188:
                lc = 8179;
                break;
            case 8486:
                lc = 969;
                break;
            case 8490:
                lc = 107;
                break;
            case 8491:
                lc = 229;
                break;
        default:
            break;
        }
    } else {
        switch (utf32) {
            case 65:   case 66:   case 67:   case 68:   case 69:   case 70:   case 71:   case 72:   case 73:   case 74:
            case 75:   case 76:   case 77:   case 78:   case 79:   case 80:   case 81:   case 82:   case 83:   case 84:   case 85:   case 86:
            case 87:   case 88:   case 89:   case 90:   case 192:   case 193:   case 194:   case 195:   case 196:   case 197:   case 198:
            case 199:   case 200:   case 201:   case 202:   case 203:   case 204:   case 205:   case 206:   case 207:   case 208:   case 209:   case 210:
            case 211:   case 212:   case 213:   case 214:   case 216:   case 217:   case 218:   case 219:   case 220:   case 221:   case 222:
            case 913:   case 914:   case 915:   case 916:   case 917:   case 918:   case 919:   case 920:   case 921:   case 922:   case 923:
            case 924:   case 925:   case 926:   case 927:   case 928:   case 929:   case 931:   case 932:   case 933:   case 934:   case 935:
            case 936:   case 937:   case 938:   case 939:   case 1040:   case 1041:   case 1042:   case 1043:   case 1044:   case 1045:   case 1046:
            case 1047:   case 1048:   case 1049:   case 1050:   case 1051:   case 1052:   case 1053:   case 1054:   case 1055:   case 1056:   case 1057:   case 1058:
            case 1059:   case 1060:   case 1061:   case 1062:   case 1063:   case 1064:   case 1065:   case 1066:   case 1067:   case 1068:   case 1069:   case 1070:
            case 1071:
                lc = (utf32 + 32);
                break;
            case 256:   case 257:   case 258:   case 259:   case 260:   case 261:   case 262:   case 263:   case 264:   case 265:
            case 266:   case 267:   case 268:   case 269:   case 270:   case 271:   case 272:   case 273:   case 274:   case 275:   case 276:   case 277:
            case 278:   case 279:   case 280:   case 281:   case 282:   case 283:   case 284:   case 285:   case 286:   case 287:   case 288:   case 289:
            case 290:   case 291:   case 292:   case 293:   case 294:   case 295:   case 296:   case 297:   case 298:   case 299:   case 300:   case 301:
            case 302:   case 303:   case 305:   case 306:   case 307:   case 308:   case 309:   case 310:   case 330:   case 331:
            case 332:   case 333:   case 334:   case 335:   case 336:   case 337:   case 338:   case 339:   case 340:   case 341:   case 342:   case 343:
            case 344:   case 345:   case 346:   case 347:   case 348:   case 349:   case 350:   case 351:   case 352:   case 353:   case 354:   case 355:
            case 356:   case 357:   case 358:   case 359:   case 360:   case 361:   case 362:   case 363:   case 364:   case 365:   case 366:   case 367:
            case 368:   case 369:   case 370:   case 371:   case 372:   case 373:   case 374:   case 416:   case 417:   case 418:   case 419:
            case 420:   case 478:   case 479:   case 480:   case 481:   case 482:   case 483:   case 484:   case 485:   case 486:   case 487:
            case 488:   case 489:   case 490:   case 491:   case 492:   case 493:   case 494:   case 504:   case 505:   case 506:   case 507:
            case 508:   case 509:   case 510:   case 511:   case 512:   case 513:   case 514:   case 515:   case 516:   case 517:   case 518:   case 519:
            case 520:   case 521:   case 522:   case 523:   case 524:   case 525:   case 526:   case 527:   case 528:   case 529:   case 530:   case 531:
            case 532:   case 533:   case 534:   case 535:   case 536:   case 537:   case 538:   case 539:   case 540:   case 541:   case 542:
            case 546:   case 547:   case 548:   case 549:   case 550:   case 551:   case 552:   case 553:   case 554:   case 555:   case 556:   case 557:
            case 558:   case 559:   case 560:   case 561:   case 562:   case 984:   case 985:   case 986:   case 987:   case 988:   case 989:
            case 990:   case 991:   case 992:   case 993:   case 994:   case 995:   case 996:   case 997:   case 998:   case 999:   case 1000:   case 1001:
            case 1002:   case 1003:   case 1004:   case 1005:   case 1006:   case 1120:   case 1121:   case 1122:   case 1123:   case 1124:   case 1125:
            case 1126:   case 1127:   case 1128:   case 1129:   case 1130:   case 1131:   case 1132:   case 1133:   case 1134:   case 1135:   case 1136:   case 1137:
            case 1138:   case 1139:   case 1140:   case 1141:   case 1142:   case 1143:   case 1144:   case 1145:   case 1146:   case 1147:   case 1148:   case 1149:
            case 1150:   case 1151:   case 1152:   case 1162:   case 1163:   case 1164:   case 1165:   case 1166:   case 1167:   case 1168:   case 1169:
            case 1170:   case 1171:   case 1172:   case 1173:   case 1174:   case 1175:   case 1176:   case 1177:   case 1178:   case 1179:   case 1180:   case 1181:
            case 1182:   case 1183:   case 1184:   case 1185:   case 1186:   case 1187:   case 1188:   case 1189:   case 1190:   case 1191:   case 1192:   case 1193:
            case 1194:   case 1195:   case 1196:   case 1197:   case 1198:   case 1199:   case 1200:   case 1201:   case 1202:   case 1203:   case 1204:   case 1205:
            case 1206:   case 1207:   case 1208:   case 1209:   case 1210:   case 1211:   case 1212:   case 1213:   case 1214:   case 1232:   case 1233:
            case 1234:   case 1235:   case 1236:   case 1237:   case 1238:   case 1239:   case 1240:   case 1241:   case 1242:   case 1243:   case 1244:   case 1245:
            case 1246:   case 1247:   case 1248:   case 1249:   case 1250:   case 1251:   case 1252:   case 1253:   case 1254:   case 1255:   case 1256:   case 1257:
            case 1258:   case 1259:   case 1260:   case 1261:   case 1262:   case 1263:   case 1264:   case 1265:   case 1266:   case 1267:   case 1268:
            case 1280:   case 1281:   case 1282:   case 1283:   case 1284:   case 1285:   case 1286:   case 1287:   case 1288:   case 1289:   case 1290:   case 1291:
            case 1292:   case 1293:   case 1294:
                if ( !(((utf32) % 2 == 1))) {
                    lc = (utf32 + 1);
                }
                break;
            case 313:   case 314:   case 315:   case 316:   case 317:   case 318:   case 319:   case 320:   case 321:   case 322:
            case 323:   case 324:   case 325:   case 326:   case 327:   case 377:   case 378:   case 379:   case 380:   case 381:
            case 459:   case 460:   case 461:   case 462:   case 463:   case 464:   case 465:   case 466:   case 467:   case 468:   case 469:   case 470:
            case 471:   case 472:   case 473:   case 474:   case 475:   case 1217:   case 1218:   case 1219:   case 1220:   case 1221:   case 1222:
            case 1223:   case 1224:   case 1225:   case 1226:   case 1227:   case 1228:   case 1229:
                if (((utf32) % 2 == 1)) {
                    lc = (utf32 + 1);
                }
                break;
            case 1024:   case 1025:   case 1026:   case 1027:   case 1028:   case 1029:   case 1030:   case 1031:   case 1032:   case 1033:
            case 1034:   case 1035:   case 1036:   case 1037:   case 1038:   case 1039:
                lc = (utf32 + 80);
                break;
            case 1329:   case 1330:   case 1331:   case 1332:   case 1333:   case 1334:   case 1335:   case 1336:   case 1337:   case 1338:
            case 1339:   case 1340:   case 1341:   case 1342:   case 1343:   case 1344:   case 1345:   case 1346:   case 1347:   case 1348:   case 1349:   case 1350:
            case 1351:   case 1352:   case 1353:   case 1354:   case 1355:   case 1356:   case 1357:   case 1358:   case 1359:   case 1360:   case 1361:   case 1362:
            case 1363:   case 1364:   case 1365:   case 1366:
                lc = (utf32 + 48);
                break;
            case 304:
                lc = 105;
                break;
            case 376:
                lc = 255;
                break;
            case 385:
                lc = 595;
                break;
            case 386:
                lc = 387;
                break;
            case 388:
                lc = 389;
                break;
            case 390:
                lc = 596;
                break;
            case 391:
                lc = 392;
                break;
            case 393:
                lc = 598;
                break;
            case 394:
                lc = 599;
                break;
            case 395:
                lc = 396;
                break;
            case 398:
                lc = 477;
                break;
            case 399:
                lc = 601;
                break;
            case 400:
                lc = 603;
                break;
            case 401:
                lc = 402;
                break;
            case 403:
                lc = 608;
                break;
            case 404:
                lc = 611;
                break;
            case 406:
                lc = 617;
                break;
            case 407:
                lc = 616;
                break;
            case 408:
                lc = 409;
                break;
            case 412:
                lc = 623;
                break;
            case 413:
                lc = 626;
                break;
            case 415:
                lc = 629;
                break;
            case 422:
                lc = 640;
                break;
            case 423:
                lc = 424;
                break;
            case 425:
                lc = 643;
                break;
            case 428:
                lc = 429;
                break;
            case 430:
                lc = 648;
                break;
            case 431:
                lc = 432;
                break;
            case 433:
                lc = 650;
                break;
            case 434:
                lc = 651;
                break;
            case 435:
                lc = 436;
                break;
            case 437:
                lc = 438;
                break;
            case 439:
                lc = 658;
                break;
            case 440:
                lc = 441;
                break;
            case 444:
                lc = 445;
                break;
            case 452:
                lc = 454;
                break;
            case 453:
                lc = 454;
                break;
            case 455:
                lc = 457;
                break;
            case 456:
                lc = 457;
                break;
            case 458:
                lc = 460;
                break;
            case 497:
                lc = 499;
                break;
            case 498:
                lc = 499;
                break;
            case 500:
                lc = 501;
                break;
            case 502:
                lc = 405;
                break;
            case 503:
                lc = 447;
                break;
            case 544:
                lc = 414;
                break;
            case 902:
                lc = 940;
                break;
            case 904:
                lc = 941;
                break;
            case 905:
                lc = 942;
                break;
            case 906:
                lc = 943;
                break;
            case 908:
                lc = 972;
                break;
            case 910:
                lc = 973;
                break;
            case 911:
                lc = 974;
                break;
            case 1012:
                lc = 952;
                break;
            case 1015:
                lc = 1016;
                break;
            case 1017:
                lc = 1010;
                break;
            case 1018:
                lc = 1019;
                break;
            case 1272:
                lc = 1273;
                break;
        default:
            break;
        }
    }
    return lc;
}

/**
 * Converts utf32 input to uppercase
 * @param    utf32 : a single character encoded in UTF32
 * @return   a single uppercase character encoded in UTF32
*/
static picoos_uint32 base_utf32_uppercase (picoos_uint32 utf32)
{
    picoos_uint32 lc;

    lc = utf32;
    if (((utf32 >= 65345) && (utf32 <= 65370))) {
        lc = (utf32 - 32);
    } else if (((utf32 >= 66600) && (utf32 <= 66639))) {
        lc = (utf32 - 40);
    } else if (((utf32 >= 7681) && (utf32 <= 9449))) {
        switch (utf32) {
            case 7681:   case 7682:   case 7683:   case 7684:   case 7685:   case 7686:   case 7687:   case 7688:   case 7689:   case 7690:
            case 7691:   case 7692:   case 7693:   case 7694:   case 7695:   case 7696:   case 7697:   case 7698:   case 7699:   case 7700:   case 7701:   case 7702:
            case 7703:   case 7704:   case 7705:   case 7706:   case 7707:   case 7708:   case 7709:   case 7710:   case 7711:   case 7712:   case 7713:   case 7714:
            case 7715:   case 7716:   case 7717:   case 7718:   case 7719:   case 7720:   case 7721:   case 7722:   case 7723:   case 7724:   case 7725:   case 7726:
            case 7727:   case 7728:   case 7729:   case 7730:   case 7731:   case 7732:   case 7733:   case 7734:   case 7735:   case 7736:   case 7737:   case 7738:
            case 7739:   case 7740:   case 7741:   case 7742:   case 7743:   case 7744:   case 7745:   case 7746:   case 7747:   case 7748:   case 7749:   case 7750:
            case 7751:   case 7752:   case 7753:   case 7754:   case 7755:   case 7756:   case 7757:   case 7758:   case 7759:   case 7760:   case 7761:   case 7762:
            case 7763:   case 7764:   case 7765:   case 7766:   case 7767:   case 7768:   case 7769:   case 7770:   case 7771:   case 7772:   case 7773:   case 7774:
            case 7775:   case 7776:   case 7777:   case 7778:   case 7779:   case 7780:   case 7781:   case 7782:   case 7783:   case 7784:   case 7785:   case 7786:
            case 7787:   case 7788:   case 7789:   case 7790:   case 7791:   case 7792:   case 7793:   case 7794:   case 7795:   case 7796:   case 7797:   case 7798:
            case 7799:   case 7800:   case 7801:   case 7802:   case 7803:   case 7804:   case 7805:   case 7806:   case 7807:   case 7808:   case 7809:   case 7810:
            case 7811:   case 7812:   case 7813:   case 7814:   case 7815:   case 7816:   case 7817:   case 7818:   case 7819:   case 7820:   case 7821:   case 7822:
            case 7823:   case 7824:   case 7825:   case 7826:   case 7827:   case 7828:   case 7829:   case 7841:   case 7842:   case 7843:   case 7844:
            case 7845:   case 7846:   case 7847:   case 7848:   case 7849:   case 7850:   case 7851:   case 7852:   case 7853:   case 7854:   case 7855:   case 7856:
            case 7857:   case 7858:   case 7859:   case 7860:   case 7861:   case 7862:   case 7863:   case 7864:   case 7865:   case 7866:   case 7867:   case 7868:
            case 7869:   case 7870:   case 7871:   case 7872:   case 7873:   case 7874:   case 7875:   case 7876:   case 7877:   case 7878:   case 7879:   case 7880:
            case 7881:   case 7882:   case 7883:   case 7884:   case 7885:   case 7886:   case 7887:   case 7888:   case 7889:   case 7890:   case 7891:   case 7892:
            case 7893:   case 7894:   case 7895:   case 7896:   case 7897:   case 7898:   case 7899:   case 7900:   case 7901:   case 7902:   case 7903:   case 7904:
            case 7905:   case 7906:   case 7907:   case 7908:   case 7909:   case 7910:   case 7911:   case 7912:   case 7913:   case 7914:   case 7915:   case 7916:
            case 7917:   case 7918:   case 7919:   case 7920:   case 7921:   case 7922:   case 7923:   case 7924:   case 7925:   case 7926:   case 7927:   case 7928:
            case 7929:
                if (((utf32) % 2 == 1)) {
                    lc = (utf32 - 1);
                }
                break;
            case 7936:   case 7937:   case 7938:   case 7939:   case 7940:   case 7941:   case 7942:   case 7943:   case 7952:
            case 7953:   case 7954:   case 7955:   case 7956:   case 7957:   case 7968:   case 7969:   case 7970:   case 7971:   case 7972:   case 7973:
            case 7974:   case 7975:   case 7984:   case 7985:   case 7986:   case 7987:   case 7988:   case 7989:   case 7990:   case 7991:
            case 8000:   case 8001:   case 8002:   case 8003:   case 8004:   case 8005:   case 8032:   case 8033:   case 8034:   case 8035:   case 8036:
            case 8037:   case 8038:   case 8039:   case 8064:   case 8065:   case 8066:   case 8067:   case 8068:   case 8069:   case 8070:   case 8071:
            case 8080:   case 8081:   case 8082:   case 8083:   case 8084:   case 8085:   case 8086:   case 8087:   case 8096:   case 8097:
            case 8098:   case 8099:   case 8100:   case 8101:   case 8102:   case 8103:
                lc = (utf32 + 8);
                break;
            case 8017:   case 8018:   case 8019:   case 8020:   case 8021:   case 8022:   case 8023:
                if (((utf32) % 2 == 1)) {
                    lc = (utf32 + 8);
                }
                break;
            case 8560:   case 8561:   case 8562:   case 8563:   case 8564:   case 8565:   case 8566:   case 8567:   case 8568:   case 8569:
            case 8570:   case 8571:   case 8572:   case 8573:   case 8574:   case 8575:
                lc = (utf32 - 16);
                break;
            case 9424:   case 9425:   case 9426:   case 9427:   case 9428:   case 9429:   case 9430:   case 9431:   case 9432:   case 9433:
            case 9434:   case 9435:   case 9436:   case 9437:   case 9438:   case 9439:   case 9440:   case 9441:   case 9442:   case 9443:   case 9444:   case 9445:
            case 9446:   case 9447:   case 9448:   case 9449:
                lc = (utf32 - 26);
                break;
            case 7835:
                lc = 7776;
                break;
            case 8048:
                lc = 8122;
                break;
            case 8049:
                lc = 8123;
                break;
            case 8050:
                lc = 8136;
                break;
            case 8051:
                lc = 8137;
                break;
            case 8052:
                lc = 8138;
                break;
            case 8053:
                lc = 8139;
                break;
            case 8054:
                lc = 8154;
                break;
            case 8055:
                lc = 8155;
                break;
            case 8056:
                lc = 8184;
                break;
            case 8057:
                lc = 8185;
                break;
            case 8058:
                lc = 8170;
                break;
            case 8059:
                lc = 8171;
                break;
            case 8060:
                lc = 8186;
                break;
            case 8061:
                lc = 8187;
                break;
            case 8112:
                lc = 8120;
                break;
            case 8113:
                lc = 8121;
                break;
            case 8115:
                lc = 8124;
                break;
            case 8126:
                lc = 921;
                break;
            case 8131:
                lc = 8140;
                break;
            case 8144:
                lc = 8152;
                break;
            case 8145:
                lc = 8153;
                break;
            case 8160:
                lc = 8168;
                break;
            case 8161:
                lc = 8169;
                break;
            case 8165:
                lc = 8172;
                break;
            case 8179:
                lc = 8188;
                break;
        default:
            break;
        }
    } else {
        switch (utf32) {
            case 97:   case 98:   case 99:   case 100:   case 101:   case 102:   case 103:   case 104:   case 105:   case 106:
            case 107:   case 108:   case 109:   case 110:   case 111:   case 112:   case 113:   case 114:   case 115:   case 116:   case 117:   case 118:
            case 119:   case 120:   case 121:   case 122:   case 224:   case 225:   case 226:   case 227:   case 228:   case 229:   case 230:
            case 231:   case 232:   case 233:   case 234:   case 235:   case 236:   case 237:   case 238:   case 239:   case 240:   case 241:   case 242:
            case 243:   case 244:   case 245:   case 246:   case 247:   case 248:   case 249:   case 250:   case 251:   case 252:   case 253:   case 254:
            case 945:   case 946:   case 947:   case 948:   case 949:   case 950:   case 951:   case 952:   case 953:   case 954:   case 955:
            case 956:   case 957:   case 958:   case 959:   case 960:   case 961:   case 963:   case 964:   case 965:   case 966:   case 967:
            case 968:   case 969:   case 970:   case 971:   case 1072:   case 1073:   case 1074:   case 1075:   case 1076:   case 1077:   case 1078:
            case 1079:   case 1080:   case 1081:   case 1082:   case 1083:   case 1084:   case 1085:   case 1086:   case 1087:   case 1088:   case 1089:   case 1090:
            case 1091:   case 1092:   case 1093:   case 1094:   case 1095:   case 1096:   case 1097:   case 1098:   case 1099:   case 1100:   case 1101:   case 1102:
            case 1103:
                if ((utf32 != 247)) {
                    lc = (utf32 - 32);
                }
                break;
            case 257:   case 258:   case 259:   case 260:   case 261:   case 262:   case 263:   case 264:   case 265:   case 266:
            case 267:   case 268:   case 269:   case 270:   case 271:   case 272:   case 273:   case 274:   case 275:   case 276:   case 277:   case 278:
            case 279:   case 280:   case 281:   case 282:   case 283:   case 284:   case 285:   case 286:   case 287:   case 288:   case 289:   case 290:
            case 291:   case 292:   case 293:   case 294:   case 295:   case 296:   case 297:   case 298:   case 299:   case 300:   case 301:   case 302:
            case 303:   case 304:   case 306:   case 307:   case 308:   case 309:   case 310:   case 311:   case 331:   case 332:
            case 333:   case 334:   case 335:   case 336:   case 337:   case 338:   case 339:   case 340:   case 341:   case 342:   case 343:   case 344:
            case 345:   case 346:   case 347:   case 348:   case 349:   case 350:   case 351:   case 352:   case 353:   case 354:   case 355:   case 356:
            case 357:   case 358:   case 359:   case 360:   case 361:   case 362:   case 363:   case 364:   case 365:   case 366:   case 367:   case 368:
            case 369:   case 370:   case 371:   case 372:   case 373:   case 374:   case 375:   case 417:   case 418:   case 419:   case 420:
            case 421:   case 481:   case 482:   case 483:   case 484:   case 485:   case 486:   case 487:   case 488:   case 489:   case 490:
            case 491:   case 492:   case 493:   case 494:   case 495:   case 507:   case 508:   case 509:   case 510:   case 511:
            case 513:   case 514:   case 515:   case 516:   case 517:   case 518:   case 519:   case 520:   case 521:   case 522:   case 523:   case 524:
            case 525:   case 526:   case 527:   case 528:   case 529:   case 530:   case 531:   case 532:   case 533:   case 534:   case 535:   case 536:
            case 537:   case 538:   case 539:   case 540:   case 541:   case 542:   case 543:   case 544:   case 546:   case 547:   case 548:
            case 549:   case 550:   case 551:   case 552:   case 553:   case 554:   case 555:   case 556:   case 557:   case 558:   case 559:   case 560:
            case 561:   case 562:   case 563:   case 985:   case 986:   case 987:   case 988:   case 989:   case 990:   case 991:   case 992:
            case 993:   case 994:   case 995:   case 996:   case 997:   case 998:   case 999:   case 1000:   case 1001:   case 1002:   case 1003:   case 1004:
            case 1005:   case 1006:   case 1007:   case 1121:   case 1122:   case 1123:   case 1124:   case 1125:   case 1126:   case 1127:   case 1128:
            case 1129:   case 1130:   case 1131:   case 1132:   case 1133:   case 1134:   case 1135:   case 1136:   case 1137:   case 1138:   case 1139:   case 1140:
            case 1141:   case 1142:   case 1143:   case 1144:   case 1145:   case 1146:   case 1147:   case 1148:   case 1149:   case 1150:   case 1151:   case 1152:
            case 1153:   case 1163:   case 1164:   case 1165:   case 1166:   case 1167:   case 1168:   case 1169:   case 1170:   case 1171:   case 1172:
            case 1173:   case 1174:   case 1175:   case 1176:   case 1177:   case 1178:   case 1179:   case 1180:   case 1181:   case 1182:   case 1183:   case 1184:
            case 1185:   case 1186:   case 1187:   case 1188:   case 1189:   case 1190:   case 1191:   case 1192:   case 1193:   case 1194:   case 1195:   case 1196:
            case 1197:   case 1198:   case 1199:   case 1200:   case 1201:   case 1202:   case 1203:   case 1204:   case 1205:   case 1206:   case 1207:   case 1208:
            case 1209:   case 1210:   case 1211:   case 1212:   case 1213:   case 1214:   case 1215:   case 1233:   case 1234:   case 1235:   case 1236:
            case 1237:   case 1238:   case 1239:   case 1240:   case 1241:   case 1242:   case 1243:   case 1244:   case 1245:   case 1246:   case 1247:   case 1248:
            case 1249:   case 1250:   case 1251:   case 1252:   case 1253:   case 1254:   case 1255:   case 1256:   case 1257:   case 1258:   case 1259:   case 1260:
            case 1261:   case 1262:   case 1263:   case 1264:   case 1265:   case 1266:   case 1267:   case 1268:   case 1269:   case 1281:   case 1282:
            case 1283:   case 1284:   case 1285:   case 1286:   case 1287:   case 1288:   case 1289:   case 1290:   case 1291:   case 1292:   case 1293:   case 1294:
            case 1295:
                if (((utf32) % 2 == 1)) {
                    lc = (utf32 - 1);
                }
                break;
            case 314:   case 315:   case 316:   case 317:   case 318:   case 319:   case 320:   case 321:   case 322:   case 323:
            case 324:   case 325:   case 326:   case 327:   case 328:   case 378:   case 379:   case 380:   case 381:   case 382:
            case 464:   case 465:   case 466:   case 467:   case 468:   case 469:   case 470:   case 471:   case 472:   case 473:   case 474:   case 475:
            case 476:   case 1218:   case 1219:   case 1220:   case 1221:   case 1222:   case 1223:   case 1224:   case 1225:   case 1226:   case 1227:
            case 1228:   case 1229:   case 1230:
                if ( !(((utf32) % 2 == 1))) {
                    lc = (utf32 - 1);
                }
                break;
            case 1104:   case 1105:   case 1106:   case 1107:   case 1108:   case 1109:   case 1110:   case 1111:   case 1112:   case 1113:
            case 1114:   case 1115:   case 1116:   case 1117:   case 1118:   case 1119:
                lc = (utf32 - 80);
                break;
            case 1377:   case 1378:   case 1379:   case 1380:   case 1381:   case 1382:   case 1383:   case 1384:   case 1385:   case 1386:
            case 1387:   case 1388:   case 1389:   case 1390:   case 1391:   case 1392:   case 1393:   case 1394:   case 1395:   case 1396:   case 1397:   case 1398:
            case 1399:   case 1400:   case 1401:   case 1402:   case 1403:   case 1404:   case 1405:   case 1406:   case 1407:   case 1408:   case 1409:   case 1410:
            case 1411:   case 1412:   case 1413:   case 1414:
                lc = (utf32 - 48);
                break;
            case 181:
                lc = 924;
                break;
            case 255:
                lc = 376;
                break;
            case 305:
                lc = 73;
                break;
            case 383:
                lc = 83;
                break;
            case 387:
                lc = 386;
                break;
            case 389:
                lc = 388;
                break;
            case 392:
                lc = 391;
                break;
            case 396:
                lc = 395;
                break;
            case 402:
                lc = 401;
                break;
            case 405:
                lc = 502;
                break;
            case 409:
                lc = 408;
                break;
            case 414:
                lc = 544;
                break;
            case 424:
                lc = 423;
                break;
            case 429:
                lc = 428;
                break;
            case 432:
                lc = 431;
                break;
            case 436:
                lc = 435;
                break;
            case 438:
                lc = 437;
                break;
            case 441:
                lc = 440;
                break;
            case 445:
                lc = 444;
                break;
            case 447:
                lc = 503;
                break;
            case 453:
                lc = 452;
                break;
            case 454:
                lc = 452;
                break;
            case 456:
                lc = 455;
                break;
            case 457:
                lc = 455;
                break;
            case 459:
                lc = 458;
                break;
            case 460:
                lc = 458;
                break;
            case 462:
                lc = 461;
                break;
            case 477:
                lc = 398;
                break;
            case 479:
                lc = 478;
                break;
            case 498:
                lc = 497;
                break;
            case 499:
                lc = 497;
                break;
            case 501:
                lc = 500;
                break;
            case 505:
                lc = 504;
                break;
            case 595:
                lc = 385;
                break;
            case 596:
                lc = 390;
                break;
            case 598:
                lc = 393;
                break;
            case 599:
                lc = 394;
                break;
            case 601:
                lc = 399;
                break;
            case 603:
                lc = 400;
                break;
            case 608:
                lc = 403;
                break;
            case 611:
                lc = 404;
                break;
            case 616:
                lc = 407;
                break;
            case 617:
                lc = 406;
                break;
            case 623:
                lc = 412;
                break;
            case 626:
                lc = 413;
                break;
            case 629:
                lc = 415;
                break;
            case 640:
                lc = 422;
                break;
            case 643:
                lc = 425;
                break;
            case 648:
                lc = 430;
                break;
            case 650:
                lc = 433;
                break;
            case 651:
                lc = 434;
                break;
            case 658:
                lc = 439;
                break;
            case 837:
                lc = 921;
                break;
            case 940:
                lc = 902;
                break;
            case 941:
                lc = 904;
                break;
            case 942:
                lc = 905;
                break;
            case 943:
                lc = 906;
                break;
            case 962:
                lc = 931;
                break;
            case 972:
                lc = 908;
                break;
            case 973:
                lc = 910;
                break;
            case 974:
                lc = 911;
                break;
            case 976:
                lc = 914;
                break;
            case 977:
                lc = 920;
                break;
            case 981:
                lc = 934;
                break;
            case 982:
                lc = 928;
                break;
            case 1008:
                lc = 922;
                break;
            case 1009:
                lc = 929;
                break;
            case 1010:
                lc = 1017;
                break;
            case 1013:
                lc = 917;
                break;
            case 1016:
                lc = 1015;
                break;
            case 1019:
                lc = 1018;
                break;
            case 1273:
                lc = 1272;
                break;
        default:
            break;
        }
    }
    return lc;
}

/**
 * Gets the UTF8 character 'utf8char' from the UTF8 string 'utf8str' from
 *             position 'pos'
 * @param    utf8str: utf8 string
 * @param    pos: position from where the utf8 character is copied
 *            (also output set as position of the next utf8 character in the utf8 string)
 * @param    utf8char: zero terminated utf8 character containing 1 to 4 bytes (output)
*/
static void picobase_get_utf8char (picoos_uint8 utf8[], picoos_int32 * pos, picobase_utf8char utf8char)
{

    int i;
    int l;

    utf8char[0] = 0;
    l = picobase_det_utf8_length(utf8[*pos]);
    i = 0;
    while ((((i < l) && (i < PICOBASE_UTF8_MAXLEN)) && (utf8[*pos] != 0))) {
        utf8char[i] = utf8[*pos];
        (*pos)++;
        i++;
    }
    utf8char[i] = 0;
}


picoos_uint8 picobase_get_next_utf8char(const picoos_uint8 *utf8s,
                                        const picoos_uint32 utf8slenmax,
                                        picoos_uint32 *pos,
                                        picobase_utf8char utf8char) {
    picoos_uint8 i;
    picoos_uint8 len;
    picoos_uint32 poscnt;

    utf8char[0] = 0;
    len = picobase_det_utf8_length(utf8s[*pos]);
    if ((((*pos) + len) > utf8slenmax) ||
        (len > PICOBASE_UTF8_MAXLEN)) {
        return FALSE;
    }

    poscnt = *pos;
    i = 0;
    while ((i < len) && (utf8s[poscnt] != 0)) {
        utf8char[i] = utf8s[poscnt];
        poscnt++;
        i++;
    }
    utf8char[i] = 0;
    if ((i < len) && (utf8s[poscnt] == 0)) {
        return FALSE;
    }
    *pos = poscnt;
    return TRUE;
}

picoos_uint8 picobase_get_next_utf8charpos(const picoos_uint8 *utf8s,
                                           const picoos_uint32 utf8slenmax,
                                           picoos_uint32 *pos) {
    picoos_uint8 i;
    picoos_uint8 len;
    picoos_uint32 poscnt;

    len = picobase_det_utf8_length(utf8s[*pos]);
    if ((((*pos) + len) > utf8slenmax) ||
        (len > PICOBASE_UTF8_MAXLEN)){
        return FALSE;
    }

    poscnt = *pos;
    i = 0;
    while ((i < len) && (utf8s[poscnt] != 0)) {
        poscnt++;
        i++;
    }
    if ((i < len) && (utf8s[poscnt] == 0)) {
        return FALSE;
    }
    *pos = poscnt;
    return TRUE;
}

picoos_uint8 picobase_get_prev_utf8char(const picoos_uint8 *utf8s,
                                        const picoos_uint32 utf8slenmin,
                                        picoos_uint32 *pos,
                                        picobase_utf8char utf8char) {
    picoos_uint8 i, j;
    picoos_uint8 len;
    picoos_uint32 poscnt;

    utf8char[0] = 0;
    if ((*pos) == 0) {
        return FALSE;
    }
    poscnt = (*pos) - 1;
    i = 1;
    while ((i <= PICOBASE_UTF8_MAXLEN) && (poscnt >= utf8slenmin) &&
           (utf8s[poscnt] != 0)) {
        len = picobase_det_utf8_length(utf8s[poscnt]);
        if (len == i) {
            for (j = 0; j < len; j++) {
                utf8char[j] = utf8s[poscnt + j];
            }
            utf8char[j] = 0;
            *pos = poscnt;
            return TRUE;
        }
        i++;
        poscnt--;
    }
    return FALSE;
}

picoos_uint8 picobase_get_prev_utf8charpos(const picoos_uint8 *utf8s,
                                           const picoos_uint32 utf8slenmin,
                                           picoos_uint32 *pos) {
    picoos_uint8 i;
    picoos_uint8 len;
    picoos_uint32 poscnt;

    if ((*pos) == 0) {
        return FALSE;
    }
    poscnt = (*pos) - 1;
    i = 1;
    while ((i <= PICOBASE_UTF8_MAXLEN) && (poscnt >= utf8slenmin) &&
           (utf8s[poscnt] != 0)) {
        len = picobase_det_utf8_length(utf8s[poscnt]);
        if (len == i) {
            *pos = poscnt;
            return TRUE;
        }
        i++;
        poscnt--;
    }
    return FALSE;
}

/**
 * Converts utf8 input to utf32
 * @param    utf8[] : character encoded in utf8
 * @param    done : boolean indicating the completion of the operation (FALSE: conversion not done)
 * @return   a single character encoded in UTF32
*/
static picobase_utf32 picobase_utf8_to_utf32 (picoos_uint8 utf8[], picoos_uint8 * done)
{
    (*done) = TRUE;
    if ((utf8[0] < (picoos_uint8)'\200')) {
        return utf8[0];
    } else if ((utf8[0] >= (picoos_uint8)'\370')) {
        return 0;
    } else if ((utf8[0] >= (picoos_uint8)'\360')) {
        return ((((262144 * (utf8[0] % 8)) + (4096 * (utf8[1] % 64))) + (64 * (utf8[2] % 64))) + (utf8[3] % 64));
    } else if ((utf8[0] >= (picoos_uint8)'\340')) {
        return (((4096 * (utf8[0] % 16)) + (64 * (utf8[1] % 64))) + (utf8[2] % 64));
    } else if ((utf8[(0)] >= (picoos_uint8)'\300')) {
        return ((64 * (utf8[0] % 32)) + (utf8[1] % 64));
    } else {
        (*done) = FALSE;
        return 0;
    }
}

static picoos_int32 picobase_utf32_to_utf8 (picobase_utf32 utf32, picobase_utf8 utf8[], picoos_int32 utf8MaxLen, picoos_uint8 * done)
{
    picoos_int32 len;

    (*done) = TRUE;
    if (utf8MaxLen >= 4) {
        if (utf32 < 128) {
            len = 1;
            utf8[0] = utf32;
        } else if (utf32 < 2048) {
            len = 2;
            utf8[1] = (128 + (utf32 % 64));
            utf32 = (utf32 / 64);
            utf8[0] = (192 + (utf32 % 32));
        } else if (utf32 < 65536) {
            len = 3;
            utf8[2] = (128 + (utf32 % 64));
            utf32 = (utf32 / 64);
            utf8[1] = (128 + (utf32 % 64));
            utf32 = (utf32 / 64);
            utf8[0] = (224 + utf32);
        } else if (utf32 < 1048576) {
            len = 4;
            utf8[3] = (128 + (utf32 % 64));
            utf32 = (utf32 / 64);
            utf8[2] = (128 + (utf32 % 64));
            utf32 = (utf32 / 64);
            utf8[1] = (128 + (utf32 % 64));
            utf32 = (utf32 / 64);
            utf8[0] = (240 + utf32);
        } else {
            (*done) = FALSE;
            return 0;
        }
        if (len <= (utf8MaxLen-1)) {
            utf8[len] = 0;
        }
        return len;
    } else {
        (*done) = FALSE;
        return 0;
    }
}


extern picoos_int32 picobase_lowercase_utf8_str (picoos_uchar utf8str[], picoos_char lowercase[], int lowercaseMaxLen, picoos_uint8 * done)
{
    picobase_utf8char utf8char;
    picoos_int32 i;
    picoos_int32 j;
    picoos_int32 k;
    picoos_int32 l;
    picobase_utf32 utf32;
    picoos_uint8 done1;

    k = 0;
    i = 0;
    (*done) = TRUE;
    while (utf8str[i] != 0) {
        picobase_get_utf8char(utf8str,& i,utf8char);
        utf32 = picobase_utf8_to_utf32(utf8char, & done1);
        utf32 = base_utf32_lowercase(utf32);
        l = picobase_utf32_to_utf8(utf32, utf8char, PICOBASE_UTF8_MAXLEN, & done1);
        j = 0;
        while ((j < l) && (k < (lowercaseMaxLen-1))) {
            lowercase[k] = utf8char[j];
            k++;
            j++;
        }
        *done = *done && (j == l);
    }
    lowercase[k] = 0;
    return k;
}


extern picoos_int32 picobase_uppercase_utf8_str (picoos_uchar utf8str[], picoos_char uppercase[], int uppercaseMaxLen, picoos_uint8 * done)
{
    picobase_utf8char utf8char;
    picoos_int32 i;
    picoos_int32 j;
    picoos_int32 k;
    picoos_int32 l;
    picobase_utf32 utf32;
    picoos_uint8 done1;

    k = 0;
    i = 0;
    (*done) = TRUE;
    while (utf8str[i] != 0) {
        picobase_get_utf8char(utf8str,& i,utf8char);
        utf32 = picobase_utf8_to_utf32(utf8char, & done1);
        utf32 = base_utf32_uppercase(utf32);
        l = picobase_utf32_to_utf8(utf32, utf8char, PICOBASE_UTF8_MAXLEN, & done1);
        j = 0;
        while ((j < l) && (k < (uppercaseMaxLen-1))) {
            uppercase[k] = utf8char[j];
            k++;
            j++;
        }
        *done = *done && (j == l);
    }
    uppercase[k] = 0;
    return k;
}


extern picoos_bool picobase_is_utf8_uppercase (picoos_uchar utf8str[], picoos_int32 utf8strmaxlen)
{
    picobase_utf8char utf8char;
    picoos_int32 i;
    picoos_uint32 utf32;
    picoos_bool done;
    picoos_bool isUpperCase;

    isUpperCase = TRUE;
    i = 0;
    while (isUpperCase && (i <= utf8strmaxlen-1) && (utf8str[i] != 0)) {
        picobase_get_utf8char(utf8str,& i,utf8char);
        utf32 = picobase_utf8_to_utf32(utf8char,& done);
        isUpperCase = isUpperCase && (utf32 == base_utf32_uppercase(utf32));
    }
    return isUpperCase;
}


extern picoos_bool picobase_is_utf8_lowercase (picoos_uchar utf8str[], picoos_int32 utf8strmaxlen)
{
    picobase_utf8char utf8char;
    picoos_int32 i;
    picoos_uint32 utf32;
    picoos_bool done;
    picoos_bool isLowerCase;

    isLowerCase = TRUE;
    i = 0;
    while (isLowerCase && (i <= utf8strmaxlen-1) && (utf8str[i] != 0)) {
        picobase_get_utf8char(utf8str,& i,utf8char);
        utf32 = picobase_utf8_to_utf32(utf8char,& done);
        isLowerCase = isLowerCase && (utf32 == base_utf32_lowercase(utf32));
    }
    return isLowerCase;
}


#ifdef __cplusplus
}
#endif



/* end */
