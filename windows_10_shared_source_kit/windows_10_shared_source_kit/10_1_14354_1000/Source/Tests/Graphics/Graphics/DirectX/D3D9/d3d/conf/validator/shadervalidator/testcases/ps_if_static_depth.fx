#include "Test_Include.fx"

int Test_Count = 20;

string TestInfo
<
    string TestType = "PS";
>
= "PS_if_static_depth";

VS_CRITERIA PS_001_Criteria = { false, false, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_0 - if-else is not supported";
string PS_001 =
    "ps_2_0 "
    "if b0 "
    "  mov oC0, c0 "
    "else "
    "  mov oC0, c1 ";

VS_CRITERIA PS_002_Criteria = { true, false, 0x0200, 8, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_x - depth = 8 <= 24 is legal";
string PS_002 =
    "ps_2_x "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "          if b5 mov oC0, c5 "
    "            if b6 mov oC0, c6 "
    "              if b7 mov oC0, c7 "
    "endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_003_Criteria = { true, false, 0x0200, 16, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_2_x - depth = 16 <= 24 is legal";
string PS_003 =
    "ps_2_x "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_004_Criteria = { true, false, 0x0200, 24, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_2_x - depth = 24 <= 24 is legal";
string PS_004 =
    "ps_2_x "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "          if b5 mov oC0, c5 "
    "            if b6 mov oC0, c6 "
    "              if b7 mov oC0, c7 "
    "                if b8 mov oC0, c8 "
    "                  if b9 mov oC0, c9 "
    "                    if b10 mov oC0, c10 "
    "                      if b11 mov oC0, c11 "
    "                        if b12 mov oC0, c12 "
    "                          if b13 mov oC0, c13 "
    "                            if b14 mov oC0, c14 "
    "                              if b15 mov oC0, c15 "
    "                                if b0 mov oC0, c16 "
    "                                  if b1 mov oC0, c16 "
    "                                    if b2 mov oC0, c16 "
    "                                      if b3 mov oC0, c16 "
    "                                        if b4 mov oC0, c16 "
    "                                          if b5 mov oC0, c16 "
    "                                            if b6 mov oC0, c16 "
    "                                              if b7 mov oC0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_005_Criteria = { false, false, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_2_x - depth = 25 > 24 is illegal";
string PS_005 =
    "ps_2_x "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "          if b5 mov oC0, c5 "
    "            if b6 mov oC0, c6 "
    "              if b7 mov oC0, c7 "
    "                if b8 mov oC0, c8 "
    "                  if b9 mov oC0, c9 "
    "                    if b10 mov oC0, c10 "
    "                      if b11 mov oC0, c11 "
    "                        if b12 mov oC0, c12 "
    "                          if b13 mov oC0, c13 "
    "                            if b14 mov oC0, c14 "
    "                              if b15 mov oC0, c15 "
    "                                if b0 mov oC0, c16 "
    "                                  if b1 mov oC0, c16 "
    "                                    if b2 mov oC0, c16 "
    "                                      if b3 mov oC0, c16 "
    "                                        if b4 mov oC0, c16 "
    "                                          if b5 mov oC0, c16 "
    "                                            if b6 mov oC0, c16 "
    "                                              if b7 mov oC0, c16 "
    "                                                if b8 mov oC0, c17 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif"
    "endif endif endif endif endif endif endif endif endif endif endif endif ";
    
VS_CRITERIA PS_006_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_2_sw - depth = 8 <= 24 is legal";
string PS_006 =
    "ps_2_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "endif endif endif endif endif endif endif endif";
    
VS_CRITERIA PS_007_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_2_sw - depth = 16 <= 24 is legal";
string PS_007 =
    "ps_2_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_008_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_2_sw - depth = 17 <= 24 is legal";
string PS_008 =
    "ps_2_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "                                  if b0 mov oC0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif";

VS_CRITERIA PS_009_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_2_sw - depth = 24 <= 24 is illegal";
string PS_009 =
    "ps_2_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "          if b5 mov oC0, c5 "
    "            if b6 mov oC0, c6 "
    "              if b7 mov oC0, c7 "
    "                if b8 mov oC0, c8 "
    "                  if b9 mov oC0, c9 "
    "                    if b10 mov oC0, c10 "
    "                      if b11 mov oC0, c11 "
    "                        if b12 mov oC0, c12 "
    "                          if b13 mov oC0, c13 "
    "                            if b14 mov oC0, c14 "
    "                              if b15 mov oC0, c15 "
    "                                if b16 mov oC0, c16 "
    "                                  if b17 mov oC0, c16 "
    "                                    if b18 mov oC0, c16 "
    "                                      if b19 mov oC0, c16 "
    "                                        if b20 mov oC0, c16 "
    "                                          if b21 mov oC0, c16 "
    "                                            if b22 mov oC0, c16 "
    "                                              if b23 mov oC0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif endif endif endif ";
    
VS_CRITERIA PS_010_Criteria = { false, false, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_2_sw - depth = 25 > 24 is illegal";
string PS_010 =
    "ps_2_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "          if b5 mov oC0, c5 "
    "            if b6 mov oC0, c6 "
    "              if b7 mov oC0, c7 "
    "                if b8 mov oC0, c8 "
    "                  if b9 mov oC0, c9 "
    "                    if b10 mov oC0, c10 "
    "                      if b11 mov oC0, c11 "
    "                        if b12 mov oC0, c12 "
    "                          if b13 mov oC0, c13 "
    "                            if b14 mov oC0, c14 "
    "                              if b15 mov oC0, c15 "
    "                                if b16 mov oC0, c16 "
    "                                  if b17 mov oC0, c16 "
    "                                    if b18 mov oC0, c16 "
    "                                      if b19 mov oC0, c16 "
    "                                        if b20 mov oC0, c16 "
    "                                          if b21 mov oC0, c16 "
    "                                            if b22 mov oC0, c16 "
    "                                              if b23 mov oC0, c16 "
    "                                                if b24 mov oC0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_011_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_3_0 - depth = 8 <= 24 is legal";
string PS_011 =
    "ps_3_0 "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "          if b5 mov oC0, c5 "
    "            if b6 mov oC0, c6 "
    "              if b7 mov oC0, c7 "
    "endif endif endif endif endif endif endif endif";
    
VS_CRITERIA PS_012_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_3_0 - depth = 16 <= 24 is legal";
string PS_012 =
    "ps_3_0 "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_013_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_3_0 - depth = 17 <= 24 is legal";
string PS_013 =
    "ps_3_0 "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "                                  if b0 mov oC0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif";

VS_CRITERIA PS_014_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_3_0 - depth = 24 <= 24 is legal";
string PS_014 =
    "ps_3_0 "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "                                  if b0 mov oC0, c16 "
    "                                    if b1 mov oC0, c16 "
    "                                      if b2 mov oC0, c16 "
    "                                        if b3 mov oC0, c16 "
    "                                          if b4 mov oC0, c16 "
    "                                            if b5 mov oC0, c16 "
    "                                              if b6 mov oC0, c16 "
    "                                                if b7 mov oC0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_015_Criteria = { false, false, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_3_0 - depth = 25 > 24 is illegal";
string PS_015 =
    "ps_3_0 "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "          if b5 mov oC0, c5 "
    "            if b6 mov oC0, c6 "
    "              if b7 mov oC0, c7 "
    "                if b8 mov oC0, c8 "
    "                  if b9 mov oC0, c9 "
    "                    if b10 mov oC0, c10 "
    "                      if b11 mov oC0, c11 "
    "                        if b12 mov oC0, c12 "
    "                          if b13 mov oC0, c13 "
    "                            if b14 mov oC0, c14 "
    "                              if b15 mov oC0, c15 "
    "                                if b0 mov oC0, c16 "
    "                                  if b1 mov oC0, c16 "
    "                                    if b2 mov oC0, c16 "
    "                                      if b3 mov oC0, c16 "
    "                                        if b4 mov oC0, c16 "
    "                                          if b5 mov oC0, c16 "
    "                                            if b6 mov oC0, c16 "
    "                                              if b7 mov oC0, c16 "
    "                                                if b8 mov oC0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif ";
    
VS_CRITERIA PS_016_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_3_sw - depth = 8 <= 24 is legal";
string PS_016 =
    "ps_3_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "endif endif endif endif endif endif endif endif";
    
VS_CRITERIA PS_017_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_3_sw - depth = 16 <= 24 is legal";
string PS_017 =
    "ps_3_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_018_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_3_sw - depth = 17 <= 24 is legal";
string PS_018 =
    "ps_3_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "                                  if b0 mov oC0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif";

VS_CRITERIA PS_019_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_3_sw - depth = 24 <= 24 is legal";
string PS_019 =
    "ps_3_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "                                  if b16 mov oC0, c16 "
    "                                    if b17 mov oC0, c16 "
    "                                      if b18 mov oC0, c16 "
    "                                        if b19 mov oC0, c16 "
    "                                          if b20 mov oC0, c16 "
    "                                            if b21 mov oC0, c16 "
    "                                              if b22 mov oC0, c16 "
    "                                                if b23 mov oC0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA PS_020_Criteria = { false, false, -1, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_3_sw - depth = 25 > 24 is illegal";
string PS_020 =
    "ps_3_sw "
    "if b0 mov oC0, c0 "
    "  if b1 mov oC0, c1 "
    "    if b2 mov oC0, c2 "
    "      if b3 mov oC0, c3 "
    "        if b4 mov oC0, c4 "
    "            if b5 mov oC0, c5 "
    "              if b6 mov oC0, c6 "
    "                if b7 mov oC0, c7 "
    "                  if b8 mov oC0, c8 "
    "                    if b9 mov oC0, c9 "
    "                      if b10 mov oC0, c10 "
    "                        if b11 mov oC0, c11 "
    "                          if b12 mov oC0, c12 "
    "                            if b13 mov oC0, c13 "
    "                              if b14 mov oC0, c14 "
    "                                if b15 mov oC0, c15 "
    "                                  if b16 mov oC0, c16 "
    "                                    if b17 mov oC0, c16 "
    "                                      if b18 mov oC0, c16 "
    "                                        if b19 mov oC0, c16 "
    "                                          if b20 mov oC0, c16 "
    "                                            if b21 mov oC0, c16 "
    "                                              if b22 mov oC0, c16 "
    "                                                if b23 mov oC0, c16 "
    "                                                  if b24 mov oC0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif ";