// Copyright (C) Microsoft Corporation. All rights reserved.

#include "common.h"

void WaitForEnter() {
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    bool enterPressed = false;
    bool error = false;
    while (!enterPressed && !error) {
        DWORD dwWaitResult = WaitForSingleObject(hStdIn, INFINITE);

        switch (dwWaitResult) {
            case WAIT_OBJECT_0: {
                // see if any of the input records was an Enter key-up event
                INPUT_RECORD rInput[128] = {};
                DWORD nEvents;
                if (!ReadConsoleInput(hStdIn, rInput, ARRAYSIZE(rInput), &nEvents)) {
                    //Log::Comment(L"ReadConsoleInput failed: last error is %u", GetLastError());
                    error = true;
                    break;
                } else {
                    for (DWORD i = 0; i < nEvents; i++) {
                        if (
                            KEY_EVENT == rInput[i].EventType &&
                            VK_RETURN == rInput[i].Event.KeyEvent.wVirtualKeyCode &&
                            !rInput[i].Event.KeyEvent.bKeyDown
                         ) {
                            enterPressed = true;
                            break;
                        }
                    }
                }
                break;
            }

            default: {
     /*           Log::Comment(
                    L"WaitForSingleObject returned unexpected value 0x%08x: last error is %u",
                    dwWaitResult, GetLastError()
                );*/
                error = true;
                break;
            }
        } // switch
    } // while
}

void WaitForYorN(bool *yPressed, bool* nPressed) {
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    *yPressed = false;
    *nPressed = false;
    bool error = false;
    while (!(*yPressed || *nPressed) && !error) {
        DWORD dwWaitResult = WaitForSingleObject(hStdIn, INFINITE);

        switch (dwWaitResult) {
            case WAIT_OBJECT_0: {
                // see if any of the input records was an Enter key-up event
                INPUT_RECORD rInput[128] = {};
                DWORD nEvents;
                if (!ReadConsoleInput(hStdIn, rInput, ARRAYSIZE(rInput), &nEvents)) {
                    //Log::Comment(L"ReadConsoleInput failed: last error is %u", GetLastError());
                    error = true;
                    break;
                } else {
                    for (DWORD i = 0; i < nEvents; i++) {
                        if (
                            KEY_EVENT == rInput[i].EventType &&
                            0x59 == rInput[i].Event.KeyEvent.wVirtualKeyCode &&
                            !rInput[i].Event.KeyEvent.bKeyDown
                         ) {
                            *yPressed = true;
                            break;
                        }
                        else if (
                            KEY_EVENT == rInput[i].EventType &&
                            0x4E == rInput[i].Event.KeyEvent.wVirtualKeyCode &&
                            !rInput[i].Event.KeyEvent.bKeyDown
                         ) {
                            *nPressed = true;
                            break;
                        }
                    }
                }
                break;
            }

            default: {
     /*           Log::Comment(
                    L"WaitForSingleObject returned unexpected value 0x%08x: last error is %u",
                    dwWaitResult, GetLastError()
                );*/
                error = true;
                break;
            }
        } // switch
    } // while
}

void ReadUserInput(WCHAR *pStr)
{        
    DWORD dwCtl;
    WCHAR ch;
    WCHAR *bufPtr = pStr;
    DWORD buflen = 65536; 
    DWORD len = 0;


    do
    {
        ReadConsole( GetStdHandle(STD_INPUT_HANDLE), &ch, 1, &dwCtl, NULL );        // Check if end of the line
        if ((ch == '\n') || (ch == 0xffff))
        {
            *bufPtr = '\0';
            break;
        }
        else
        {
            *bufPtr = ch;
            if (len < buflen)
                bufPtr++ ;                   // don't overflow buf
            (len)++;                        // always increment len
        }
    }while(ch != '\n' );

 }

