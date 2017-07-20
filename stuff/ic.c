#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint32_t Word;
typedef struct {
  Word* data;
  Word index;
  Word ref;
  size_t size;
} Net;

void reduce(Net* net) {
  Word p, n, x, y, x1, x2, y1, y2, z, a, b;
  Word* m = net->data;
  Word* M = &net->index;
  Word v[1024];
  Word V = 1;
  v[0] = net->ref;
  while (V > 0) {
    p = v[--V];
    n = m[p];
    p = m[n];
    x = n >> 2;
    y = p >> 2;
    if (m[y*4+3] & 1)
      continue;
    if ((p & 3) == 0) {
      if ((n & 3) == 0 && x != y) { 
        z = m[n + ((m[x * 4 + 3] >> 1) & 3)];
        if ((m[x*4+3]>>3) == (m[y*4+3]>>3)){
          a = m[x*4+1], b = m[y*4+1], m[a] = b, m[b] = a;
          a = m[x*4+2], b = m[y*4+2], m[a] = b, m[b] = a;
        } else {
          x1 = (*M += 4) / 4 - 1;
          x2 = (*M += 4) / 4 - 1;
          y1 = (*M += 4) / 4 - 1;
          y2 = (*M += 4) / 4 - 1;
          m[x1*4+0] = x1*4+0;
          m[x1*4+1] = x1*4+1;
          m[x1*4+2] = x1*4+2;
          m[x1*4+3] = m[x*4+3] & 0xFFFFFFF8;
          m[x2*4+0] = x2*4+0;
          m[x2*4+1] = x2*4+1;
          m[x2*4+2] = x2*4+2;
          m[x2*4+3] = m[x*4+3] & 0xFFFFFFF8;
          m[y1*4+0] = y1*4+0;
          m[y1*4+1] = y1*4+1;
          m[y1*4+2] = y1*4+2;
          m[y1*4+3] = m[y*4+3] & 0xFFFFFFF8;
          m[y2*4+0] = y2*4+0;
          m[y2*4+1] = y2*4+1;
          m[y2*4+2] = y2*4+2;
          m[y2*4+3] = m[y*4+3] & 0xFFFFFFF8;
          a = m[y1*4+0], b = m[x*4+1], m[a] = b, m[b] = a;
          a = m[y2*4+0], b = m[x*4+2], m[a] = b, m[b] = a;
          a = m[x1*4+0], b = m[y*4+1], m[a] = b, m[b] = a;
          a = m[x2*4+0], b = m[y*4+2], m[a] = b, m[b] = a;
          a = m[x1*4+1], b = m[y1*4+1], m[a] = b, m[b] = a;
          a = m[x1*4+2], b = m[y2*4+1], m[a] = b, m[b] = a;
          a = m[x2*4+1], b = m[y1*4+2], m[a] = b, m[b] = a;
          a = m[x2*4+2], b = m[y2*4+2], m[a] = b, m[b] = a;
        }
        v[V++] = m[z];
      } else {
        m[(p>>2)*4+3] = m[(p>>2)*4+3] | 1;
        v[V++] = m[p|2];
        v[V++] = m[p|1];
      };
    } else {
      m[(p>>2)*4+3] = (m[(p>>2)*4+3] & 0xFFFFFFF9) | ((p&3)<<1);
      v[V++] = m[(p>>2)*4+0];
    }
  }
}

Word checksum(Net* net) {
  int sum = 0;
  for (int i = 0; i < net->index; ++i) {
    sum = (sum + net->data[i]) % 65536;
  }
  return sum;
}

int main() {
  Word init[] = {6,5464,2,8,10,132,0,8,12,88,4,8,8,84,20,8,13,18,17,0,14,81,30,8,21,26,25,0,37,34,22,8,38,42,29,8,45,28,32,16,46,50,33,8,53,36,40,24,54,58,41,8,61,44,48,32,62,66,49,8,69,52,56,40,70,74,57,8,77,60,64,48,78,82,65,8,85,68,72,56,86,21,73,8,13,76,80,64,9,128,96,8,89,94,93,0,90,125,106,8,97,102,101,0,113,110,98,8,114,118,105,8,121,104,108,72,122,126,109,8,129,112,116,80,130,97,117,8,89,120,124,88,5,140,142,8,133,138,137,0,133,144,134,8,141,5460,152,8,145,150,149,0,146,5456,160,8,153,158,157,0,154,980,168,8,161,166,165,0,162,976,176,8,169,174,173,0,170,1620,184,8,177,182,181,0,178,1616,192,8,185,190,189,0,186,2260,200,8,193,198,197,0,194,2256,208,8,201,206,205,0,202,2900,216,8,209,214,213,0,210,2896,224,8,217,222,221,0,218,3540,232,8,225,230,229,0,226,3536,240,8,233,238,237,0,234,4180,248,8,241,246,245,0,242,4176,256,8,249,254,253,0,250,4820,264,8,257,262,261,0,258,4816,272,8,265,270,269,0,266,340,282,8,273,278,277,0,286,5142,274,8,290,4826,280,8,294,4502,284,8,298,4186,288,8,302,3862,292,8,306,3546,296,8,310,3222,300,8,314,2906,304,8,318,2582,308,8,322,2266,312,8,326,1942,316,8,330,1626,320,8,334,1302,324,8,338,986,328,8,342,662,332,8,273,346,336,8,350,977,341,8,352,981,344,8,348,380,360,8,353,358,357,0,354,369,370,8,361,366,365,0,374,361,362,8,378,568,368,8,382,476,372,8,353,384,376,8,381,400,394,8,385,390,389,0,398,452,386,8,402,428,392,8,385,404,396,8,401,408,412,8,405,410,409,0,406,422,420,8,413,418,417,0,414,424,413,8,421,426,425,0,397,432,436,8,429,434,433,0,430,440,444,8,437,442,441,0,438,446,445,8,445,450,449,0,393,470,460,8,453,458,457,0,454,464,468,8,461,466,465,0,462,472,453,8,469,474,473,0,377,492,486,8,477,482,481,0,490,544,478,8,494,520,484,8,477,496,488,8,493,514,504,8,497,502,501,0,498,508,512,8,505,510,509,0,506,516,497,8,513,518,517,0,489,538,528,8,521,526,525,0,522,532,536,8,529,534,533,0,530,540,521,8,537,542,541,0,485,548,552,8,545,550,549,0,546,562,560,8,553,558,557,0,554,564,553,8,561,566,565,0,373,584,578,8,569,574,573,0,582,636,570,8,586,612,576,8,569,588,580,8,585,592,596,8,589,594,593,0,590,606,604,8,597,602,601,0,598,608,597,8,605,610,609,0,581,616,620,8,613,618,617,0,614,624,628,8,621,626,625,0,622,630,629,8,629,634,633,0,577,640,644,8,637,642,641,0,638,648,652,8,645,650,649,0,646,654,653,8,653,658,657,0,666,982,337,8,668,978,660,8,664,696,676,8,669,674,673,0,670,685,686,8,677,682,681,0,690,677,678,8,694,884,684,8,698,792,688,8,669,700,692,8,697,716,710,8,701,706,705,0,714,768,702,8,718,744,708,8,701,720,712,8,717,724,728,8,721,726,725,0,722,738,736,8,729,734,733,0,730,740,729,8,737,742,741,0,713,748,752,8,745,750,749,0,746,756,760,8,753,758,757,0,754,762,761,8,761,766,765,0,709,786,776,8,769,774,773,0,770,780,784,8,777,782,781,0,778,788,769,8,785,790,789,0,693,808,802,8,793,798,797,0,806,860,794,8,810,836,800,8,793,812,804,8,809,830,820,8,813,818,817,0,814,824,828,8,821,826,825,0,822,832,813,8,829,834,833,0,805,854,844,8,837,842,841,0,838,848,852,8,845,850,849,0,846,856,837,8,853,858,857,0,801,864,868,8,861,866,865,0,862,878,876,8,869,874,873,0,870,880,869,8,877,882,881,0,689,900,894,8,885,890,889,0,898,952,886,8,902,928,892,8,885,904,896,8,901,908,912,8,905,910,909,0,906,922,920,8,913,918,917,0,914,924,913,8,921,926,925,0,897,932,936,8,929,934,933,0,930,940,944,8,937,942,941,0,938,946,945,8,945,950,949,0,893,956,960,8,953,958,957,0,954,964,968,8,961,966,965,0,962,970,969,8,969,974,973,0,169,345,665,96,161,349,661,104,990,1617,333,8,992,1621,984,8,988,1020,1000,8,993,998,997,0,994,1009,1010,8,1001,1006,1005,0,1014,1001,1002,8,1018,1208,1008,8,1022,1116,1012,8,993,1024,1016,8,1021,1040,1034,8,1025,1030,1029,0,1038,1092,1026,8,1042,1068,1032,8,1025,1044,1036,8,1041,1048,1052,8,1045,1050,1049,0,1046,1062,1060,8,1053,1058,1057,0,1054,1064,1053,8,1061,1066,1065,0,1037,1072,1076,8,1069,1074,1073,0,1070,1080,1084,8,1077,1082,1081,0,1078,1086,1085,8,1085,1090,1089,0,1033,1110,1100,8,1093,1098,1097,0,1094,1104,1108,8,1101,1106,1105,0,1102,1112,1093,8,1109,1114,1113,0,1017,1132,1126,8,1117,1122,1121,0,1130,1184,1118,8,1134,1160,1124,8,1117,1136,1128,8,1133,1154,1144,8,1137,1142,1141,0,1138,1148,1152,8,1145,1150,1149,0,1146,1156,1137,8,1153,1158,1157,0,1129,1178,1168,8,1161,1166,1165,0,1162,1172,1176,8,1169,1174,1173,0,1170,1180,1161,8,1177,1182,1181,0,1125,1188,1192,8,1185,1190,1189,0,1186,1202,1200,8,1193,1198,1197,0,1194,1204,1193,8,1201,1206,1205,0,1013,1224,1218,8,1209,1214,1213,0,1222,1276,1210,8,1226,1252,1216,8,1209,1228,1220,8,1225,1232,1236,8,1229,1234,1233,0,1230,1246,1244,8,1237,1242,1241,0,1238,1248,1237,8,1245,1250,1249,0,1221,1256,1260,8,1253,1258,1257,0,1254,1264,1268,8,1261,1266,1265,0,1262,1270,1269,8,1269,1274,1273,0,1217,1280,1284,8,1277,1282,1281,0,1278,1288,1292,8,1285,1290,1289,0,1286,1294,1293,8,1293,1298,1297,0,1306,1622,329,8,1308,1618,1300,8,1304,1336,1316,8,1309,1314,1313,0,1310,1325,1326,8,1317,1322,1321,0,1330,1317,1318,8,1334,1524,1324,8,1338,1432,1328,8,1309,1340,1332,8,1337,1356,1350,8,1341,1346,1345,0,1354,1408,1342,8,1358,1384,1348,8,1341,1360,1352,8,1357,1364,1368,8,1361,1366,1365,0,1362,1378,1376,8,1369,1374,1373,0,1370,1380,1369,8,1377,1382,1381,0,1353,1388,1392,8,1385,1390,1389,0,1386,1396,1400,8,1393,1398,1397,0,1394,1402,1401,8,1401,1406,1405,0,1349,1426,1416,8,1409,1414,1413,0,1410,1420,1424,8,1417,1422,1421,0,1418,1428,1409,8,1425,1430,1429,0,1333,1448,1442,8,1433,1438,1437,0,1446,1500,1434,8,1450,1476,1440,8,1433,1452,1444,8,1449,1470,1460,8,1453,1458,1457,0,1454,1464,1468,8,1461,1466,1465,0,1462,1472,1453,8,1469,1474,1473,0,1445,1494,1484,8,1477,1482,1481,0,1478,1488,1492,8,1485,1490,1489,0,1486,1496,1477,8,1493,1498,1497,0,1441,1504,1508,8,1501,1506,1505,0,1502,1518,1516,8,1509,1514,1513,0,1510,1520,1509,8,1517,1522,1521,0,1329,1540,1534,8,1525,1530,1529,0,1538,1592,1526,8,1542,1568,1532,8,1525,1544,1536,8,1541,1548,1552,8,1545,1550,1549,0,1546,1562,1560,8,1553,1558,1557,0,1554,1564,1553,8,1561,1566,1565,0,1537,1572,1576,8,1569,1574,1573,0,1570,1580,1584,8,1577,1582,1581,0,1578,1586,1585,8,1585,1590,1589,0,1533,1596,1600,8,1593,1598,1597,0,1594,1604,1608,8,1601,1606,1605,0,1602,1610,1609,8,1609,1614,1613,0,185,985,1305,112,177,989,1301,120,1630,2257,325,8,1632,2261,1624,8,1628,1660,1640,8,1633,1638,1637,0,1634,1649,1650,8,1641,1646,1645,0,1654,1641,1642,8,1658,1848,1648,8,1662,1756,1652,8,1633,1664,1656,8,1661,1680,1674,8,1665,1670,1669,0,1678,1732,1666,8,1682,1708,1672,8,1665,1684,1676,8,1681,1688,1692,8,1685,1690,1689,0,1686,1702,1700,8,1693,1698,1697,0,1694,1704,1693,8,1701,1706,1705,0,1677,1712,1716,8,1709,1714,1713,0,1710,1720,1724,8,1717,1722,1721,0,1718,1726,1725,8,1725,1730,1729,0,1673,1750,1740,8,1733,1738,1737,0,1734,1744,1748,8,1741,1746,1745,0,1742,1752,1733,8,1749,1754,1753,0,1657,1772,1766,8,1757,1762,1761,0,1770,1824,1758,8,1774,1800,1764,8,1757,1776,1768,8,1773,1794,1784,8,1777,1782,1781,0,1778,1788,1792,8,1785,1790,1789,0,1786,1796,1777,8,1793,1798,1797,0,1769,1818,1808,8,1801,1806,1805,0,1802,1812,1816,8,1809,1814,1813,0,1810,1820,1801,8,1817,1822,1821,0,1765,1828,1832,8,1825,1830,1829,0,1826,1842,1840,8,1833,1838,1837,0,1834,1844,1833,8,1841,1846,1845,0,1653,1864,1858,8,1849,1854,1853,0,1862,1916,1850,8,1866,1892,1856,8,1849,1868,1860,8,1865,1872,1876,8,1869,1874,1873,0,1870,1886,1884,8,1877,1882,1881,0,1878,1888,1877,8,1885,1890,1889,0,1861,1896,1900,8,1893,1898,1897,0,1894,1904,1908,8,1901,1906,1905,0,1902,1910,1909,8,1909,1914,1913,0,1857,1920,1924,8,1917,1922,1921,0,1918,1928,1932,8,1925,1930,1929,0,1926,1934,1933,8,1933,1938,1937,0,1946,2262,321,8,1948,2258,1940,8,1944,1976,1956,8,1949,1954,1953,0,1950,1965,1966,8,1957,1962,1961,0,1970,1957,1958,8,1974,2164,1964,8,1978,2072,1968,8,1949,1980,1972,8,1977,1996,1990,8,1981,1986,1985,0,1994,2048,1982,8,1998,2024,1988,8,1981,2000,1992,8,1997,2004,2008,8,2001,2006,2005,0,2002,2018,2016,8,2009,2014,2013,0,2010,2020,2009,8,2017,2022,2021,0,1993,2028,2032,8,2025,2030,2029,0,2026,2036,2040,8,2033,2038,2037,0,2034,2042,2041,8,2041,2046,2045,0,1989,2066,2056,8,2049,2054,2053,0,2050,2060,2064,8,2057,2062,2061,0,2058,2068,2049,8,2065,2070,2069,0,1973,2088,2082,8,2073,2078,2077,0,2086,2140,2074,8,2090,2116,2080,8,2073,2092,2084,8,2089,2110,2100,8,2093,2098,2097,0,2094,2104,2108,8,2101,2106,2105,0,2102,2112,2093,8,2109,2114,2113,0,2085,2134,2124,8,2117,2122,2121,0,2118,2128,2132,8,2125,2130,2129,0,2126,2136,2117,8,2133,2138,2137,0,2081,2144,2148,8,2141,2146,2145,0,2142,2158,2156,8,2149,2154,2153,0,2150,2160,2149,8,2157,2162,2161,0,1969,2180,2174,8,2165,2170,2169,0,2178,2232,2166,8,2182,2208,2172,8,2165,2184,2176,8,2181,2188,2192,8,2185,2190,2189,0,2186,2202,2200,8,2193,2198,2197,0,2194,2204,2193,8,2201,2206,2205,0,2177,2212,2216,8,2209,2214,2213,0,2210,2220,2224,8,2217,2222,2221,0,2218,2226,2225,8,2225,2230,2229,0,2173,2236,2240,8,2233,2238,2237,0,2234,2244,2248,8,2241,2246,2245,0,2242,2250,2249,8,2249,2254,2253,0,201,1625,1945,128,193,1629,1941,136,2270,2897,317,8,2272,2901,2264,8,2268,2300,2280,8,2273,2278,2277,0,2274,2289,2290,8,2281,2286,2285,0,2294,2281,2282,8,2298,2488,2288,8,2302,2396,2292,8,2273,2304,2296,8,2301,2320,2314,8,2305,2310,2309,0,2318,2372,2306,8,2322,2348,2312,8,2305,2324,2316,8,2321,2328,2332,8,2325,2330,2329,0,2326,2342,2340,8,2333,2338,2337,0,2334,2344,2333,8,2341,2346,2345,0,2317,2352,2356,8,2349,2354,2353,0,2350,2360,2364,8,2357,2362,2361,0,2358,2366,2365,8,2365,2370,2369,0,2313,2390,2380,8,2373,2378,2377,0,2374,2384,2388,8,2381,2386,2385,0,2382,2392,2373,8,2389,2394,2393,0,2297,2412,2406,8,2397,2402,2401,0,2410,2464,2398,8,2414,2440,2404,8,2397,2416,2408,8,2413,2434,2424,8,2417,2422,2421,0,2418,2428,2432,8,2425,2430,2429,0,2426,2436,2417,8,2433,2438,2437,0,2409,2458,2448,8,2441,2446,2445,0,2442,2452,2456,8,2449,2454,2453,0,2450,2460,2441,8,2457,2462,2461,0,2405,2468,2472,8,2465,2470,2469,0,2466,2482,2480,8,2473,2478,2477,0,2474,2484,2473,8,2481,2486,2485,0,2293,2504,2498,8,2489,2494,2493,0,2502,2556,2490,8,2506,2532,2496,8,2489,2508,2500,8,2505,2512,2516,8,2509,2514,2513,0,2510,2526,2524,8,2517,2522,2521,0,2518,2528,2517,8,2525,2530,2529,0,2501,2536,2540,8,2533,2538,2537,0,2534,2544,2548,8,2541,2546,2545,0,2542,2550,2549,8,2549,2554,2553,0,2497,2560,2564,8,2557,2562,2561,0,2558,2568,2572,8,2565,2570,2569,0,2566,2574,2573,8,2573,2578,2577,0,2586,2902,313,8,2588,2898,2580,8,2584,2616,2596,8,2589,2594,2593,0,2590,2605,2606,8,2597,2602,2601,0,2610,2597,2598,8,2614,2804,2604,8,2618,2712,2608,8,2589,2620,2612,8,2617,2636,2630,8,2621,2626,2625,0,2634,2688,2622,8,2638,2664,2628,8,2621,2640,2632,8,2637,2644,2648,8,2641,2646,2645,0,2642,2658,2656,8,2649,2654,2653,0,2650,2660,2649,8,2657,2662,2661,0,2633,2668,2672,8,2665,2670,2669,0,2666,2676,2680,8,2673,2678,2677,0,2674,2682,2681,8,2681,2686,2685,0,2629,2706,2696,8,2689,2694,2693,0,2690,2700,2704,8,2697,2702,2701,0,2698,2708,2689,8,2705,2710,2709,0,2613,2728,2722,8,2713,2718,2717,0,2726,2780,2714,8,2730,2756,2720,8,2713,2732,2724,8,2729,2750,2740,8,2733,2738,2737,0,2734,2744,2748,8,2741,2746,2745,0,2742,2752,2733,8,2749,2754,2753,0,2725,2774,2764,8,2757,2762,2761,0,2758,2768,2772,8,2765,2770,2769,0,2766,2776,2757,8,2773,2778,2777,0,2721,2784,2788,8,2781,2786,2785,0,2782,2798,2796,8,2789,2794,2793,0,2790,2800,2789,8,2797,2802,2801,0,2609,2820,2814,8,2805,2810,2809,0,2818,2872,2806,8,2822,2848,2812,8,2805,2824,2816,8,2821,2828,2832,8,2825,2830,2829,0,2826,2842,2840,8,2833,2838,2837,0,2834,2844,2833,8,2841,2846,2845,0,2817,2852,2856,8,2849,2854,2853,0,2850,2860,2864,8,2857,2862,2861,0,2858,2866,2865,8,2865,2870,2869,0,2813,2876,2880,8,2873,2878,2877,0,2874,2884,2888,8,2881,2886,2885,0,2882,2890,2889,8,2889,2894,2893,0,217,2265,2585,144,209,2269,2581,152,2910,3537,309,8,2912,3541,2904,8,2908,2940,2920,8,2913,2918,2917,0,2914,2929,2930,8,2921,2926,2925,0,2934,2921,2922,8,2938,3128,2928,8,2942,3036,2932,8,2913,2944,2936,8,2941,2960,2954,8,2945,2950,2949,0,2958,3012,2946,8,2962,2988,2952,8,2945,2964,2956,8,2961,2968,2972,8,2965,2970,2969,0,2966,2982,2980,8,2973,2978,2977,0,2974,2984,2973,8,2981,2986,2985,0,2957,2992,2996,8,2989,2994,2993,0,2990,3000,3004,8,2997,3002,3001,0,2998,3006,3005,8,3005,3010,3009,0,2953,3030,3020,8,3013,3018,3017,0,3014,3024,3028,8,3021,3026,3025,0,3022,3032,3013,8,3029,3034,3033,0,2937,3052,3046,8,3037,3042,3041,0,3050,3104,3038,8,3054,3080,3044,8,3037,3056,3048,8,3053,3074,3064,8,3057,3062,3061,0,3058,3068,3072,8,3065,3070,3069,0,3066,3076,3057,8,3073,3078,3077,0,3049,3098,3088,8,3081,3086,3085,0,3082,3092,3096,8,3089,3094,3093,0,3090,3100,3081,8,3097,3102,3101,0,3045,3108,3112,8,3105,3110,3109,0,3106,3122,3120,8,3113,3118,3117,0,3114,3124,3113,8,3121,3126,3125,0,2933,3144,3138,8,3129,3134,3133,0,3142,3196,3130,8,3146,3172,3136,8,3129,3148,3140,8,3145,3152,3156,8,3149,3154,3153,0,3150,3166,3164,8,3157,3162,3161,0,3158,3168,3157,8,3165,3170,3169,0,3141,3176,3180,8,3173,3178,3177,0,3174,3184,3188,8,3181,3186,3185,0,3182,3190,3189,8,3189,3194,3193,0,3137,3200,3204,8,3197,3202,3201,0,3198,3208,3212,8,3205,3210,3209,0,3206,3214,3213,8,3213,3218,3217,0,3226,3542,305,8,3228,3538,3220,8,3224,3256,3236,8,3229,3234,3233,0,3230,3245,3246,8,3237,3242,3241,0,3250,3237,3238,8,3254,3444,3244,8,3258,3352,3248,8,3229,3260,3252,8,3257,3276,3270,8,3261,3266,3265,0,3274,3328,3262,8,3278,3304,3268,8,3261,3280,3272,8,3277,3284,3288,8,3281,3286,3285,0,3282,3298,3296,8,3289,3294,3293,0,3290,3300,3289,8,3297,3302,3301,0,3273,3308,3312,8,3305,3310,3309,0,3306,3316,3320,8,3313,3318,3317,0,3314,3322,3321,8,3321,3326,3325,0,3269,3346,3336,8,3329,3334,3333,0,3330,3340,3344,8,3337,3342,3341,0,3338,3348,3329,8,3345,3350,3349,0,3253,3368,3362,8,3353,3358,3357,0,3366,3420,3354,8,3370,3396,3360,8,3353,3372,3364,8,3369,3390,3380,8,3373,3378,3377,0,3374,3384,3388,8,3381,3386,3385,0,3382,3392,3373,8,3389,3394,3393,0,3365,3414,3404,8,3397,3402,3401,0,3398,3408,3412,8,3405,3410,3409,0,3406,3416,3397,8,3413,3418,3417,0,3361,3424,3428,8,3421,3426,3425,0,3422,3438,3436,8,3429,3434,3433,0,3430,3440,3429,8,3437,3442,3441,0,3249,3460,3454,8,3445,3450,3449,0,3458,3512,3446,8,3462,3488,3452,8,3445,3464,3456,8,3461,3468,3472,8,3465,3470,3469,0,3466,3482,3480,8,3473,3478,3477,0,3474,3484,3473,8,3481,3486,3485,0,3457,3492,3496,8,3489,3494,3493,0,3490,3500,3504,8,3497,3502,3501,0,3498,3506,3505,8,3505,3510,3509,0,3453,3516,3520,8,3513,3518,3517,0,3514,3524,3528,8,3521,3526,3525,0,3522,3530,3529,8,3529,3534,3533,0,233,2905,3225,160,225,2909,3221,168,3550,4177,301,8,3552,4181,3544,8,3548,3580,3560,8,3553,3558,3557,0,3554,3569,3570,8,3561,3566,3565,0,3574,3561,3562,8,3578,3768,3568,8,3582,3676,3572,8,3553,3584,3576,8,3581,3600,3594,8,3585,3590,3589,0,3598,3652,3586,8,3602,3628,3592,8,3585,3604,3596,8,3601,3608,3612,8,3605,3610,3609,0,3606,3622,3620,8,3613,3618,3617,0,3614,3624,3613,8,3621,3626,3625,0,3597,3632,3636,8,3629,3634,3633,0,3630,3640,3644,8,3637,3642,3641,0,3638,3646,3645,8,3645,3650,3649,0,3593,3670,3660,8,3653,3658,3657,0,3654,3664,3668,8,3661,3666,3665,0,3662,3672,3653,8,3669,3674,3673,0,3577,3692,3686,8,3677,3682,3681,0,3690,3744,3678,8,3694,3720,3684,8,3677,3696,3688,8,3693,3714,3704,8,3697,3702,3701,0,3698,3708,3712,8,3705,3710,3709,0,3706,3716,3697,8,3713,3718,3717,0,3689,3738,3728,8,3721,3726,3725,0,3722,3732,3736,8,3729,3734,3733,0,3730,3740,3721,8,3737,3742,3741,0,3685,3748,3752,8,3745,3750,3749,0,3746,3762,3760,8,3753,3758,3757,0,3754,3764,3753,8,3761,3766,3765,0,3573,3784,3778,8,3769,3774,3773,0,3782,3836,3770,8,3786,3812,3776,8,3769,3788,3780,8,3785,3792,3796,8,3789,3794,3793,0,3790,3806,3804,8,3797,3802,3801,0,3798,3808,3797,8,3805,3810,3809,0,3781,3816,3820,8,3813,3818,3817,0,3814,3824,3828,8,3821,3826,3825,0,3822,3830,3829,8,3829,3834,3833,0,3777,3840,3844,8,3837,3842,3841,0,3838,3848,3852,8,3845,3850,3849,0,3846,3854,3853,8,3853,3858,3857,0,3866,4182,297,8,3868,4178,3860,8,3864,3896,3876,8,3869,3874,3873,0,3870,3885,3886,8,3877,3882,3881,0,3890,3877,3878,8,3894,4084,3884,8,3898,3992,3888,8,3869,3900,3892,8,3897,3916,3910,8,3901,3906,3905,0,3914,3968,3902,8,3918,3944,3908,8,3901,3920,3912,8,3917,3924,3928,8,3921,3926,3925,0,3922,3938,3936,8,3929,3934,3933,0,3930,3940,3929,8,3937,3942,3941,0,3913,3948,3952,8,3945,3950,3949,0,3946,3956,3960,8,3953,3958,3957,0,3954,3962,3961,8,3961,3966,3965,0,3909,3986,3976,8,3969,3974,3973,0,3970,3980,3984,8,3977,3982,3981,0,3978,3988,3969,8,3985,3990,3989,0,3893,4008,4002,8,3993,3998,3997,0,4006,4060,3994,8,4010,4036,4000,8,3993,4012,4004,8,4009,4030,4020,8,4013,4018,4017,0,4014,4024,4028,8,4021,4026,4025,0,4022,4032,4013,8,4029,4034,4033,0,4005,4054,4044,8,4037,4042,4041,0,4038,4048,4052,8,4045,4050,4049,0,4046,4056,4037,8,4053,4058,4057,0,4001,4064,4068,8,4061,4066,4065,0,4062,4078,4076,8,4069,4074,4073,0,4070,4080,4069,8,4077,4082,4081,0,3889,4100,4094,8,4085,4090,4089,0,4098,4152,4086,8,4102,4128,4092,8,4085,4104,4096,8,4101,4108,4112,8,4105,4110,4109,0,4106,4122,4120,8,4113,4118,4117,0,4114,4124,4113,8,4121,4126,4125,0,4097,4132,4136,8,4129,4134,4133,0,4130,4140,4144,8,4137,4142,4141,0,4138,4146,4145,8,4145,4150,4149,0,4093,4156,4160,8,4153,4158,4157,0,4154,4164,4168,8,4161,4166,4165,0,4162,4170,4169,8,4169,4174,4173,0,249,3545,3865,176,241,3549,3861,184,4190,4817,293,8,4192,4821,4184,8,4188,4220,4200,8,4193,4198,4197,0,4194,4209,4210,8,4201,4206,4205,0,4214,4201,4202,8,4218,4408,4208,8,4222,4316,4212,8,4193,4224,4216,8,4221,4240,4234,8,4225,4230,4229,0,4238,4292,4226,8,4242,4268,4232,8,4225,4244,4236,8,4241,4248,4252,8,4245,4250,4249,0,4246,4262,4260,8,4253,4258,4257,0,4254,4264,4253,8,4261,4266,4265,0,4237,4272,4276,8,4269,4274,4273,0,4270,4280,4284,8,4277,4282,4281,0,4278,4286,4285,8,4285,4290,4289,0,4233,4310,4300,8,4293,4298,4297,0,4294,4304,4308,8,4301,4306,4305,0,4302,4312,4293,8,4309,4314,4313,0,4217,4332,4326,8,4317,4322,4321,0,4330,4384,4318,8,4334,4360,4324,8,4317,4336,4328,8,4333,4354,4344,8,4337,4342,4341,0,4338,4348,4352,8,4345,4350,4349,0,4346,4356,4337,8,4353,4358,4357,0,4329,4378,4368,8,4361,4366,4365,0,4362,4372,4376,8,4369,4374,4373,0,4370,4380,4361,8,4377,4382,4381,0,4325,4388,4392,8,4385,4390,4389,0,4386,4402,4400,8,4393,4398,4397,0,4394,4404,4393,8,4401,4406,4405,0,4213,4424,4418,8,4409,4414,4413,0,4422,4476,4410,8,4426,4452,4416,8,4409,4428,4420,8,4425,4432,4436,8,4429,4434,4433,0,4430,4446,4444,8,4437,4442,4441,0,4438,4448,4437,8,4445,4450,4449,0,4421,4456,4460,8,4453,4458,4457,0,4454,4464,4468,8,4461,4466,4465,0,4462,4470,4469,8,4469,4474,4473,0,4417,4480,4484,8,4477,4482,4481,0,4478,4488,4492,8,4485,4490,4489,0,4486,4494,4493,8,4493,4498,4497,0,4506,4822,289,8,4508,4818,4500,8,4504,4536,4516,8,4509,4514,4513,0,4510,4525,4526,8,4517,4522,4521,0,4530,4517,4518,8,4534,4724,4524,8,4538,4632,4528,8,4509,4540,4532,8,4537,4556,4550,8,4541,4546,4545,0,4554,4608,4542,8,4558,4584,4548,8,4541,4560,4552,8,4557,4564,4568,8,4561,4566,4565,0,4562,4578,4576,8,4569,4574,4573,0,4570,4580,4569,8,4577,4582,4581,0,4553,4588,4592,8,4585,4590,4589,0,4586,4596,4600,8,4593,4598,4597,0,4594,4602,4601,8,4601,4606,4605,0,4549,4626,4616,8,4609,4614,4613,0,4610,4620,4624,8,4617,4622,4621,0,4618,4628,4609,8,4625,4630,4629,0,4533,4648,4642,8,4633,4638,4637,0,4646,4700,4634,8,4650,4676,4640,8,4633,4652,4644,8,4649,4670,4660,8,4653,4658,4657,0,4654,4664,4668,8,4661,4666,4665,0,4662,4672,4653,8,4669,4674,4673,0,4645,4694,4684,8,4677,4682,4681,0,4678,4688,4692,8,4685,4690,4689,0,4686,4696,4677,8,4693,4698,4697,0,4641,4704,4708,8,4701,4706,4705,0,4702,4718,4716,8,4709,4714,4713,0,4710,4720,4709,8,4717,4722,4721,0,4529,4740,4734,8,4725,4730,4729,0,4738,4792,4726,8,4742,4768,4732,8,4725,4744,4736,8,4741,4748,4752,8,4745,4750,4749,0,4746,4762,4760,8,4753,4758,4757,0,4754,4764,4753,8,4761,4766,4765,0,4737,4772,4776,8,4769,4774,4773,0,4770,4780,4784,8,4777,4782,4781,0,4778,4786,4785,8,4785,4790,4789,0,4733,4796,4800,8,4793,4798,4797,0,4794,4804,4808,8,4801,4806,4805,0,4802,4810,4809,8,4809,4814,4813,0,265,4185,4505,192,257,4189,4501,200,4830,5457,285,8,4832,5461,4824,8,4828,4860,4840,8,4833,4838,4837,0,4834,4849,4850,8,4841,4846,4845,0,4854,4841,4842,8,4858,5048,4848,8,4862,4956,4852,8,4833,4864,4856,8,4861,4880,4874,8,4865,4870,4869,0,4878,4932,4866,8,4882,4908,4872,8,4865,4884,4876,8,4881,4888,4892,8,4885,4890,4889,0,4886,4902,4900,8,4893,4898,4897,0,4894,4904,4893,8,4901,4906,4905,0,4877,4912,4916,8,4909,4914,4913,0,4910,4920,4924,8,4917,4922,4921,0,4918,4926,4925,8,4925,4930,4929,0,4873,4950,4940,8,4933,4938,4937,0,4934,4944,4948,8,4941,4946,4945,0,4942,4952,4933,8,4949,4954,4953,0,4857,4972,4966,8,4957,4962,4961,0,4970,5024,4958,8,4974,5000,4964,8,4957,4976,4968,8,4973,4994,4984,8,4977,4982,4981,0,4978,4988,4992,8,4985,4990,4989,0,4986,4996,4977,8,4993,4998,4997,0,4969,5018,5008,8,5001,5006,5005,0,5002,5012,5016,8,5009,5014,5013,0,5010,5020,5001,8,5017,5022,5021,0,4965,5028,5032,8,5025,5030,5029,0,5026,5042,5040,8,5033,5038,5037,0,5034,5044,5033,8,5041,5046,5045,0,4853,5064,5058,8,5049,5054,5053,0,5062,5116,5050,8,5066,5092,5056,8,5049,5068,5060,8,5065,5072,5076,8,5069,5074,5073,0,5070,5086,5084,8,5077,5082,5081,0,5078,5088,5077,8,5085,5090,5089,0,5061,5096,5100,8,5093,5098,5097,0,5094,5104,5108,8,5101,5106,5105,0,5102,5110,5109,8,5109,5114,5113,0,5057,5120,5124,8,5117,5122,5121,0,5118,5128,5132,8,5125,5130,5129,0,5126,5134,5133,8,5133,5138,5137,0,5146,5462,281,8,5148,5458,5140,8,5144,5176,5156,8,5149,5154,5153,0,5150,5165,5166,8,5157,5162,5161,0,5170,5157,5158,8,5174,5364,5164,8,5178,5272,5168,8,5149,5180,5172,8,5177,5196,5190,8,5181,5186,5185,0,5194,5248,5182,8,5198,5224,5188,8,5181,5200,5192,8,5197,5204,5208,8,5201,5206,5205,0,5202,5218,5216,8,5209,5214,5213,0,5210,5220,5209,8,5217,5222,5221,0,5193,5228,5232,8,5225,5230,5229,0,5226,5236,5240,8,5233,5238,5237,0,5234,5242,5241,8,5241,5246,5245,0,5189,5266,5256,8,5249,5254,5253,0,5250,5260,5264,8,5257,5262,5261,0,5258,5268,5249,8,5265,5270,5269,0,5173,5288,5282,8,5273,5278,5277,0,5286,5340,5274,8,5290,5316,5280,8,5273,5292,5284,8,5289,5310,5300,8,5293,5298,5297,0,5294,5304,5308,8,5301,5306,5305,0,5302,5312,5293,8,5309,5314,5313,0,5285,5334,5324,8,5317,5322,5321,0,5318,5328,5332,8,5325,5330,5329,0,5326,5336,5317,8,5333,5338,5337,0,5281,5344,5348,8,5341,5346,5345,0,5342,5358,5356,8,5349,5354,5353,0,5350,5360,5349,8,5357,5362,5361,0,5169,5380,5374,8,5365,5370,5369,0,5378,5432,5366,8,5382,5408,5372,8,5365,5384,5376,8,5381,5388,5392,8,5385,5390,5389,0,5386,5402,5400,8,5393,5398,5397,0,5394,5404,5393,8,5401,5406,5405,0,5377,5412,5416,8,5409,5414,5413,0,5410,5420,5424,8,5417,5422,5421,0,5418,5426,5425,8,5425,5430,5429,0,5373,5436,5440,8,5433,5438,5437,0,5434,5444,5448,8,5441,5446,5445,0,5442,5450,5449,8,5449,5454,5453,0,153,4825,5145,208,145,4829,5141,216,1,5532,5474,8,5465,5470,5469,0,5478,5896,5466,8,5482,5872,5472,8,5486,5848,5476,8,5490,5824,5480,8,5494,5800,5484,8,5498,5776,5488,8,5502,5752,5492,8,5506,5728,5496,8,5510,5704,5500,8,5514,5680,5504,8,5518,5656,5508,8,5522,5632,5512,8,5526,5608,5516,8,5530,5584,5520,8,5534,5560,5524,8,5465,5536,5528,8,5533,5554,5544,8,5537,5542,5541,0,5538,5548,5552,8,5545,5550,5549,0,5546,5556,5537,8,5553,5558,5557,0,5529,5578,5568,8,5561,5566,5565,0,5562,5572,5576,8,5569,5574,5573,0,5570,5580,5561,8,5577,5582,5581,0,5525,5588,5592,8,5585,5590,5589,0,5586,5602,5600,8,5593,5598,5597,0,5594,5604,5593,8,5601,5606,5605,0,5521,5612,5616,8,5609,5614,5613,0,5610,5620,5624,8,5617,5622,5621,0,5618,5626,5625,8,5625,5630,5629,0,5517,5650,5640,8,5633,5638,5637,0,5634,5644,5648,8,5641,5646,5645,0,5642,5652,5633,8,5649,5654,5653,0,5513,5660,5664,8,5657,5662,5661,0,5658,5674,5672,8,5665,5670,5669,0,5666,5676,5665,8,5673,5678,5677,0,5509,5684,5688,8,5681,5686,5685,0,5682,5698,5696,8,5689,5694,5693,0,5690,5700,5689,8,5697,5702,5701,0,5505,5708,5712,8,5705,5710,5709,0,5706,5716,5720,8,5713,5718,5717,0,5714,5722,5721,8,5721,5726,5725,0,5501,5746,5736,8,5729,5734,5733,0,5730,5740,5744,8,5737,5742,5741,0,5738,5748,5729,8,5745,5750,5749,0,5497,5756,5760,8,5753,5758,5757,0,5754,5770,5768,8,5761,5766,5765,0,5762,5772,5761,8,5769,5774,5773,0,5493,5780,5784,8,5777,5782,5781,0,5778,5788,5792,8,5785,5790,5789,0,5786,5794,5793,8,5793,5798,5797,0,5489,5804,5808,8,5801,5806,5805,0,5802,5812,5816,8,5809,5814,5813,0,5810,5818,5817,8,5817,5822,5821,0,5485,5842,5832,8,5825,5830,5829,0,5826,5836,5840,8,5833,5838,5837,0,5834,5844,5825,8,5841,5846,5845,0,5481,5866,5856,8,5849,5854,5853,0,5850,5860,5864,8,5857,5862,5861,0,5858,5868,5849,8,5865,5870,5869,0,5477,5876,5880,8,5873,5878,5877,0,5874,5890,5888,8,5881,5886,5885,0,5882,5892,5881,8,5889,5894,5893,0,5473,5900,5904,8,5897,5902,5901,0,5898,5908,5912,8,5905,5910,5909,0,5906,5914,5913,8,5913,5918,5917,0};

  Net net;
  net.ref = 2;
  net.index = sizeof(init) / sizeof(Word);
  net.size = 400000000;
  net.data = malloc(sizeof(Word) * net.size);
  for (int i = 0; i < net.index; ++i)
    net.data[i] = init[i];

  reduce(&net);
  printf("%d", checksum(&net));

}
