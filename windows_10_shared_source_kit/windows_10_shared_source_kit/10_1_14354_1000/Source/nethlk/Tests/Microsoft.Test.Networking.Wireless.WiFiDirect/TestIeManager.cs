///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Diagnostics;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    //
    // Class for encoding, parsing, and managing test IEs used in the discovery tests to validate custom IEs are passed correctly.
    //
    // The test IE will contain a GUID value, and have the following format:
    //
    // 0xDD  // Vendor Specific IE
    // 0x14  // IE Length 20 Bytes: 3 Byte OUI, 1 Byte OUI Type, 16 Byte GUID value
    //     0x0F, 0x24, 0x5B  // 3 Byte OUI (Chosen randomly)
    //     0x00              // 1 Byte OUI type
    //     <16 Byte GUID Value>
    internal sealed class TestIeManager
    {
        private const byte ieBufferLength = 22;
        private const byte ieType = 0xDD;
        private const byte ieLength = 20;
        private static readonly byte[] ouiAndTypeBytes = {0x0F, 0x24, 0x5B, 0x00};
        private const byte offsetToGuid = 6;

        public Guid Guid {get; private set;}
        public byte[] IeBytes{get; private set;}

        public TestIeManager()
        {
            this.Guid = Guid.NewGuid();
            this.IeBytes = new byte[TestIeManager.ieBufferLength];

            this.IeBytes[0] = TestIeManager.ieType;
            this.IeBytes[1] = TestIeManager.ieLength;
            Array.Copy(TestIeManager.ouiAndTypeBytes, 0, this.IeBytes, 2, TestIeManager.ouiAndTypeBytes.Length);

            this.encodeGuid();
        }

        public void SetNewIeGuid(Guid guid)
        {
            this.Guid = guid;
            this.encodeGuid();
        }

        private void encodeGuid()
        {
            byte[] guidBytes = this.Guid.ToByteArray();
            Debug.Assert(guidBytes.Length == 16);

            Array.Copy(guidBytes, 0, this.IeBytes, TestIeManager.offsetToGuid, guidBytes.Length);
        }

        public static bool TryParseIeBuffer(byte[] ieBuffer, out Guid guid)
        {
            bool parsingSuccessful = false;
            guid = Guid.Empty;

            uint currentOffset = 0;
            while (currentOffset < ieBuffer.Length)
            {
                // Parse IE type and length
                if (ieBuffer.Length > 2 &&
                    currentOffset < (ieBuffer.Length - 2))
                {
                    byte currentIeType = ieBuffer[currentOffset];
                    currentOffset++;

                    byte currentIeLength = ieBuffer[currentOffset];
                    currentOffset++;

                    uint remainingBytes = (uint) (ieBuffer.Length - (currentOffset + 1));
                    if (remainingBytes < currentIeLength)
                    {
                        break;
                    }

                    if (currentIeType == TestIeManager.ieType &&
                        currentIeLength == TestIeManager.ieLength)
                    {
                        parsingSuccessful = TryParseSingleIeInternal(ieBuffer, currentOffset, ref guid);
                        if (parsingSuccessful) break;
                    }

                    currentOffset += currentIeLength;
                }
                else
                {
                    break;
                }
            }

            return parsingSuccessful;
        }

        private static bool TryParseSingleIeInternal(byte[] ieBuffer, uint currentOffset, ref Guid guid)
        {
            for (uint index = 0; index < TestIeManager.ouiAndTypeBytes.Length; index++)
            {
                if (ieBuffer[currentOffset + index] != TestIeManager.ouiAndTypeBytes[index])
                {
                    return false;
                }
            }

            byte[] guidBytes = new byte[16];
            Array.Copy(ieBuffer, ((int) currentOffset) + TestIeManager.ouiAndTypeBytes.Length, guidBytes, 0, 16);

            guid = new Guid(guidBytes);
            return true;
        }
    }
}
