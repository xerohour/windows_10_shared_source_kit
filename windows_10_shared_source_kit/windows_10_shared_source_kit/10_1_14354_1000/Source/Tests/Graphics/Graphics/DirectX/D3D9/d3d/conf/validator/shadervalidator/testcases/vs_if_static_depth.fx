#include "Test_Include.fx"

int Test_Count = 21;

string TestInfo
<
    string TestType = "VS";
>
= "VS_if_static_depth";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_2_0 - depth = 8 <= 16 is legal";
string VS_001 =
    "vs_2_0 "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "endif endif endif endif endif endif endif endif";
    
VS_CRITERIA VS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_2_0 - depth = 16 <= 16 is legal";
string VS_002 =
    "vs_2_0 "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "                  if b8 mov oPos, c8 "
    "                    if b9 mov oPos, c9 "
    "                      if b10 mov oPos, c10 "
    "                        if b11 mov oPos, c11 "
    "                          if b12 mov oPos, c12 "
    "                            if b13 mov oPos, c13 "
    "                              if b14 mov oPos, c14 "
    "                                if b15 mov oPos, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA VS_003_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_2_0 - depth = 17 > 16 is outright illegal";
string VS_003 =
    "vs_2_0 "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "                  if b8 mov oPos, c8 "
    "                    if b9 mov oPos, c9 "
    "                      if b10 mov oPos, c10 "
    "                        if b11 mov oPos, c11 "
    "                          if b12 mov oPos, c12 "
    "                            if b13 mov oPos, c13 "
    "                              if b14 mov oPos, c14 "
    "                                if b15 mov oPos, c15 "
    "                                  if b0 mov oPos, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_2_x - depth = 8 <= 16 is legal";
string VS_004 =
    "vs_2_x "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "endif endif endif endif endif endif endif endif";
    
VS_CRITERIA VS_005_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_2_x - depth = 16 <= 16 is legal";
string VS_005 =
    "vs_2_x "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "                  if b8 mov oPos, c8 "
    "                    if b9 mov oPos, c9 "
    "                      if b10 mov oPos, c10 "
    "                        if b11 mov oPos, c11 "
    "                          if b12 mov oPos, c12 "
    "                            if b13 mov oPos, c13 "
    "                              if b14 mov oPos, c14 "
    "                                if b15 mov oPos, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA VS_006_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_2_x - depth = 17 > 16 is outright illegal";
string VS_006 =
    "vs_2_x "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "                  if b8 mov oPos, c8 "
    "                    if b9 mov oPos, c9 "
    "                      if b10 mov oPos, c10 "
    "                        if b11 mov oPos, c11 "
    "                          if b12 mov oPos, c12 "
    "                            if b13 mov oPos, c13 "
    "                              if b14 mov oPos, c14 "
    "                                if b15 mov oPos, c15 "
    "                                  if b0 mov oPos, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif";

VS_CRITERIA VS_007_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_2_sw - depth = 8 <= 24 is legal";
string VS_007 =
    "vs_2_sw "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "endif endif endif endif endif endif endif endif";
    
VS_CRITERIA VS_008_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_2_sw - depth = 16 <= 24 is legal";
string VS_008 =
    "vs_2_sw "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "                  if b8 mov oPos, c8 "
    "                    if b9 mov oPos, c9 "
    "                      if b10 mov oPos, c10 "
    "                        if b11 mov oPos, c11 "
    "                          if b12 mov oPos, c12 "
    "                            if b13 mov oPos, c13 "
    "                              if b14 mov oPos, c14 "
    "                                if b15 mov oPos, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA VS_009_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_2_sw - depth = 17 <= 24 is legal";
string VS_009 =
    "vs_2_sw "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "            if b5 mov oPos, c5 "
    "              if b6 mov oPos, c6 "
    "                if b7 mov oPos, c7 "
    "                  if b8 mov oPos, c8 "
    "                    if b9 mov oPos, c9 "
    "                      if b10 mov oPos, c10 "
    "                        if b11 mov oPos, c11 "
    "                          if b12 mov oPos, c12 "
    "                            if b13 mov oPos, c13 "
    "                              if b14 mov oPos, c14 "
    "                                if b15 mov oPos, c15 "
    "                                  if b0 mov oPos, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif";

VS_CRITERIA VS_010_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_2_sw - depth = 24 <= 24 is illegal";
string VS_010 =
    "vs_2_sw "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "          if b5 mov oPos, c5 "
    "            if b6 mov oPos, c6 "
    "              if b7 mov oPos, c7 "
    "                if b8 mov oPos, c8 "
    "                  if b9 mov oPos, c9 "
    "                    if b10 mov oPos, c10 "
    "                      if b11 mov oPos, c11 "
    "                        if b12 mov oPos, c12 "
    "                          if b13 mov oPos, c13 "
    "                            if b14 mov oPos, c14 "
    "                              if b15 mov oPos, c15 "
    "                                if b16 mov oPos, c16 "
    "                                  if b17 mov oPos, c16 "
    "                                    if b18 mov oPos, c16 "
    "                                      if b19 mov oPos, c16 "
    "                                        if b20 mov oPos, c16 "
    "                                          if b21 mov oPos, c16 "
    "                                            if b22 mov oPos, c16 "
    "                                              if b23 mov oPos, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif endif endif endif ";
    
VS_CRITERIA VS_011_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_2_sw - depth = 25 > 24 is illegal";
string VS_011 =
    "vs_2_sw "
    "if b0 mov oPos, c0 "
    "  if b1 mov oPos, c1 "
    "    if b2 mov oPos, c2 "
    "      if b3 mov oPos, c3 "
    "        if b4 mov oPos, c4 "
    "          if b5 mov oPos, c5 "
    "            if b6 mov oPos, c6 "
    "              if b7 mov oPos, c7 "
    "                if b8 mov oPos, c8 "
    "                  if b9 mov oPos, c9 "
    "                    if b10 mov oPos, c10 "
    "                      if b11 mov oPos, c11 "
    "                        if b12 mov oPos, c12 "
    "                          if b13 mov oPos, c13 "
    "                            if b14 mov oPos, c14 "
    "                              if b15 mov oPos, c15 "
    "                                if b16 mov oPos, c16 "
    "                                  if b17 mov oPos, c16 "
    "                                    if b18 mov oPos, c16 "
    "                                      if b19 mov oPos, c16 "
    "                                        if b20 mov oPos, c16 "
    "                                          if b21 mov oPos, c16 "
    "                                            if b22 mov oPos, c16 "
    "                                              if b23 mov oPos, c16 "
    "                                                if b24 mov oPos, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA VS_012_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_3_0 - depth = 8 <= 24 is legal";
string VS_012 =
    "vs_3_0 "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "endif endif endif endif endif endif endif endif";
    
VS_CRITERIA VS_013_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_3_0 - depth = 16 <= 24 is legal";
string VS_013 =
    "vs_3_0 "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "                  if b8 mov o0, c8 "
    "                    if b9 mov o0, c9 "
    "                      if b10 mov o0, c10 "
    "                        if b11 mov o0, c11 "
    "                          if b12 mov o0, c12 "
    "                            if b13 mov o0, c13 "
    "                              if b14 mov o0, c14 "
    "                                if b15 mov o0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA VS_014_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_3_0 - depth = 17 <= 24 is legal";
string VS_014 =
    "vs_3_0 "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "                  if b8 mov o0, c8 "
    "                    if b9 mov o0, c9 "
    "                      if b10 mov o0, c10 "
    "                        if b11 mov o0, c11 "
    "                          if b12 mov o0, c12 "
    "                            if b13 mov o0, c13 "
    "                              if b14 mov o0, c14 "
    "                                if b15 mov o0, c15 "
    "                                  if b0 mov o0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif";

VS_CRITERIA VS_015_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_3_0 - depth = 24 <= 24 is legal";
string VS_015 =
    "vs_3_0 "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "                  if b8 mov o0, c8 "
    "                    if b9 mov o0, c9 "
    "                      if b10 mov o0, c10 "
    "                        if b11 mov o0, c11 "
    "                          if b12 mov o0, c12 "
    "                            if b13 mov o0, c13 "
    "                              if b14 mov o0, c14 "
    "                                if b15 mov o0, c15 "
    "                                  if b0 mov o0, c16 "
    "                                    if b1 mov o0, c16 "
    "                                      if b2 mov o0, c16 "
    "                                        if b3 mov o0, c16 "
    "                                          if b4 mov o0, c16 "
    "                                            if b5 mov o0, c16 "
    "                                              if b6 mov o0, c16 "
    "                                                if b7 mov o0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA VS_016_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_3_0 - depth = 25 > 24 is illegal";
string VS_016 =
    "vs_3_0 "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "          if b5 mov o0, c5 "
    "            if b6 mov o0, c6 "
    "              if b7 mov o0, c7 "
    "                if b8 mov o0, c8 "
    "                  if b9 mov o0, c9 "
    "                    if b10 mov o0, c10 "
    "                      if b11 mov o0, c11 "
    "                        if b12 mov o0, c12 "
    "                          if b13 mov o0, c13 "
    "                            if b14 mov o0, c14 "
    "                              if b15 mov o0, c15 "
    "                                if b0 mov o0, c16 "
    "                                  if b1 mov o0, c16 "
    "                                    if b2 mov o0, c16 "
    "                                      if b3 mov o0, c16 "
    "                                        if b4 mov o0, c16 "
    "                                          if b5 mov o0, c16 "
    "                                            if b6 mov o0, c16 "
    "                                              if b7 mov o0, c16 "
    "                                                if b8 mov o0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif ";
    
VS_CRITERIA VS_017_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_3_sw - depth = 8 <= 24 is legal";
string VS_017 =
    "vs_3_sw "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "endif endif endif endif endif endif endif endif";
    
VS_CRITERIA VS_018_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_3_sw - depth = 16 <= 24 is legal";
string VS_018 =
    "vs_3_sw "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "                  if b8 mov o0, c8 "
    "                    if b9 mov o0, c9 "
    "                      if b10 mov o0, c10 "
    "                        if b11 mov o0, c11 "
    "                          if b12 mov o0, c12 "
    "                            if b13 mov o0, c13 "
    "                              if b14 mov o0, c14 "
    "                                if b15 mov o0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA VS_019_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_3_sw - depth = 17 <= 24 is legal";
string VS_019 =
    "vs_3_sw "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "                  if b8 mov o0, c8 "
    "                    if b9 mov o0, c9 "
    "                      if b10 mov o0, c10 "
    "                        if b11 mov o0, c11 "
    "                          if b12 mov o0, c12 "
    "                            if b13 mov o0, c13 "
    "                              if b14 mov o0, c14 "
    "                                if b15 mov o0, c15 "
    "                                  if b0 mov o0, c15 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif";

VS_CRITERIA VS_020_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_3_sw - depth = 24 <= 24 is legal";
string VS_020 =
    "vs_3_sw "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "                  if b8 mov o0, c8 "
    "                    if b9 mov o0, c9 "
    "                      if b10 mov o0, c10 "
    "                        if b11 mov o0, c11 "
    "                          if b12 mov o0, c12 "
    "                            if b13 mov o0, c13 "
    "                              if b14 mov o0, c14 "
    "                                if b15 mov o0, c15 "
    "                                  if b16 mov o0, c16 "
    "                                    if b17 mov o0, c16 "
    "                                      if b18 mov o0, c16 "
    "                                        if b19 mov o0, c16 "
    "                                          if b20 mov o0, c16 "
    "                                            if b21 mov o0, c16 "
    "                                              if b22 mov o0, c16 "
    "                                                if b23 mov o0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif endif endif endif ";

VS_CRITERIA VS_021_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_3_sw - depth = 25 > 24 is illegal";
string VS_021 =
    "vs_3_sw "
    "dcl_position o0 "
    "if b0 mov o0, c0 "
    "  if b1 mov o0, c1 "
    "    if b2 mov o0, c2 "
    "      if b3 mov o0, c3 "
    "        if b4 mov o0, c4 "
    "            if b5 mov o0, c5 "
    "              if b6 mov o0, c6 "
    "                if b7 mov o0, c7 "
    "                  if b8 mov o0, c8 "
    "                    if b9 mov o0, c9 "
    "                      if b10 mov o0, c10 "
    "                        if b11 mov o0, c11 "
    "                          if b12 mov o0, c12 "
    "                            if b13 mov o0, c13 "
    "                              if b14 mov o0, c14 "
    "                                if b15 mov o0, c15 "
    "                                  if b16 mov o0, c16 "
    "                                    if b17 mov o0, c16 "
    "                                      if b18 mov o0, c16 "
    "                                        if b19 mov o0, c16 "
    "                                          if b20 mov o0, c16 "
    "                                            if b21 mov o0, c16 "
    "                                              if b22 mov o0, c16 "
    "                                                if b23 mov o0, c16 "
    "                                                  if b24 mov o0, c16 "
    "endif endif endif endif endif endif endif endif endif endif endif endif endif endif endif "
    "endif endif endif endif endif endif endif endif endif ";