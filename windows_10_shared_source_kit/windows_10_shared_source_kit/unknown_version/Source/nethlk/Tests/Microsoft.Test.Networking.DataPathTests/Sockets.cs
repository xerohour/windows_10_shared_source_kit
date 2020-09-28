///
///
/// <copyright>
///  Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///
///
/// <file>
///  $File$
/// </file>
///
/// <author>  $Author$ </author>
/// <date>    $Date$ </date>
/// <version> $Revision$ </version>
///
/// Information:
///
///
///
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Globalization;
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.SafeHandles;
using Microsoft.Test.Networking.Kit.Tracing;


namespace HlkTest.DataPathTests
{



    internal class Sockets : IDisposable
    {
    	private static int WSAStartupCount = 0;
		
        private const int IPPROTO_IP = 0;
        private const int IP_MULTICAST_TTL = 10;
        private const int IP_ADD_MEMBERSHIP = 12;
        private const int IPPROTO_IPV6 = 41;
        private const int IPV6_ADD_MEMBERSHIP = 12;
        private const int IPV6_HOPLIMIT = 21;
        private const int SOL_SOCKET = 0xffff;
        private const int SO_BROADCAST = 0x0020;

        private Dictionary<IntPtr, ListenerData> udpReceiveListeners = new Dictionary<IntPtr, ListenerData>();
        private Dictionary<IntPtr, ListenerData> tcpAcceptListeners = new Dictionary<IntPtr, ListenerData>();
        private WlanHckTestLogger testLogger;

        public Sockets(WlanHckTestLogger testLogger)
        {
            this.testLogger = testLogger;
            WSAData wsaData;

            testLogger.LogTrace("Calling WSAStartup()");
            int retval = WSAStartup(0x0202, out wsaData);
            if (retval != 0)
            {
                var lastError = WSAGetLastError();
                throw new Win32CallException("WSAStartup", lastError);
            }
            WSAStartupCount++;
        }
        ~Sockets()
        {
            Dispose(false);
        }
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        private void Dispose(bool disposing)
        {
            if (disposing)
            {	
            	WSAStartupCount--;

                if (WSAStartupCount == 0)
                {
                    testLogger.LogTrace("Calling WSACleanup()");
                    WSACleanup();
                }
            }
        }

        public string FindIpAddress(Guid interfaceGuid, bool ipv6)
        {
            if (ipv6)
            {
                return FindIpAddressIpv6(interfaceGuid);
            }
            else
            {
                return FindIpAddressIpv4(interfaceGuid);
            }
        }
        public IntPtr CreateUdpSocket(string localIpAddress, ushort localPort, bool ipv6)
        {
            if (ipv6)
            {
                return CreateUdpSocketIpv6(localIpAddress, localPort);
            }
            else
            {
                return CreateUdpSocketIpv4(localIpAddress, localPort);
            }
        }
        public IntPtr CreateMulticastSocket(string localIpAddress, ushort localPort, string multicastAddress, ushort multicastPort)
        {
            return CreateMulticastSocketIpv4(localIpAddress, localPort, multicastAddress, multicastPort);
        }
        public IntPtr CreateMulticastSocket(string localIpAddress, ushort localPort, string multicastAddress, ushort multicastPort, uint interfaceIndex)
        {
            return CreateMulticastSocketIpv6(localIpAddress, localPort, multicastAddress, multicastPort, interfaceIndex);
        }
        public IntPtr CreateBroadcastSocket(string localIpAddress, ushort localPort)
        {
            testLogger.LogTrace("CreateBroadcastSocket Start {0}  {1}", localIpAddress, localPort);
            try
            {
                uint localAddrSize;

                testLogger.LogTrace("GetSockAddrIpv4");
                SOCKADDR_IN localSockAddr = GetSockAddrIpv4(localIpAddress, out localAddrSize, localPort);
                int lastError;

                testLogger.LogTrace("socket AfFamily.AF_INET SockType.SOCK_DGRAM ProtocolType.IPPROTO_UDP");
                var udpSocket = socket(AfFamily.AF_INET, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP);
                if (udpSocket == (IntPtr)(-1))
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("socket", lastError, "socket was called attempt to create a IPv4 UDP Socket.");
                }

                int broadcast = 1;
                testLogger.LogTrace("setsockopt AfFamily.SOL_SOCKET SO_BROADCAST");
                int retCode = setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, ref broadcast, Marshal.SizeOf<Int32>());
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("setsockopt", lastError, "setsockopt failed when attempt to set the SO_BROADCAST socket option.");
                }

                testLogger.LogTrace("bind");
                retCode = bind(udpSocket, ref localSockAddr, localAddrSize);
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("bind", lastError, string.Format(CultureInfo.InvariantCulture, "bind failed when attempting to bind the IPv4 UDP Broadcast socket to the local address {0}:{1}", localIpAddress, localPort));
                }

                testLogger.LogTrace("CreateBroadcastSocket End");
                return udpSocket;
            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }

        }
        public IntPtr CreateTcpSocket(string localIpAddress, ushort localPort, bool ipv6)
        {
            if (ipv6)
            {
                return CreateTcpSocketIpv6(localIpAddress, localPort);
            }
            else
            {
                return CreateTcpSocketIpv4(localIpAddress, localPort);
            }
        }

        public void CloseSocket(IntPtr socket)
        {
            testLogger.LogTrace("CloseSocket");
            closesocket(socket);
        }
        public void Connect(IntPtr socket, string destinationAddress, ushort destinationPort, bool ipv6)
        {
            if (ipv6)
            {
                ConnectIpv6(socket, destinationAddress, destinationPort);
            }
            else
            {
                ConnectIpv4(socket, destinationAddress, destinationPort);
            }
        }
        public void SendTo(IntPtr socket, byte[] buffer, string destinationAddress, ushort destinationPort, bool ipv6)
        {
            if (ipv6)
            {
                SendToIpv6(socket, buffer, destinationAddress, destinationPort);
            }
            else
            {
                SendToIpv4(socket, buffer, destinationAddress, destinationPort);
            }
        }
        public void Send(IntPtr socket, byte[] buffer)
        {
            testLogger.LogTrace("Send Start:  Buffer Size {0}", buffer.Length);
            using (var bufferSafeHandle = HGlobalMemorySafeHandle.Allocate(buffer))
            {
                int bytesSent = 0;
                while (true)
                {
                    using (var currentBuffer = InteropUtilities.GetSafeHandleToOffset(bufferSafeHandle, bytesSent))
                    {
                        int remainingBytes = buffer.Length - bytesSent;
                        testLogger.LogTrace("Calling send");
                        var iterationBytesSent = send(socket, currentBuffer, remainingBytes, 0);
                        if (iterationBytesSent == -1)
                        {
                            var errorcode = WSAGetLastError();
                            throw new Win32CallException("send", errorcode);
                        }
                        bytesSent += iterationBytesSent;
                        if (buffer.Length - bytesSent == 0)
                        {
                            //all packets sent
                            break;
                        }
                    }
                }
            }
            testLogger.LogTrace("Send End");
        }
        public void StartReceiveUdp(IntPtr socket)
        {
            testLogger.LogTrace("Enter StartReceiveUdp");
            if (udpReceiveListeners.ContainsKey(socket) == false)
            {
                Task receiveTask = new Task(Task_StartReceiveUdp, socket, TaskCreationOptions.LongRunning);
                IntPtr stopEvent = CreateEvent(IntPtr.Zero, true, false, null);
                if (stopEvent == IntPtr.Zero)
                {
                    throw new Win32CallException("CreateEvent");
                    
                }

                IntPtr finishedEvent = CreateEvent(IntPtr.Zero, true, false, null);
                if (finishedEvent == IntPtr.Zero)
                {
                    throw new Win32CallException("CreateEvent");
                }

                ListenerData listenerData = new ListenerData(stopEvent, finishedEvent, receiveTask);
                udpReceiveListeners.Add(socket, listenerData);
                receiveTask.Start();
                testLogger.LogTrace("Exit StartReceiveUdp");
            }
            else
            {
                throw new TestBugException("The test tried start start a UDP listener that was already started.  This is a test bug.");
            }



        }

        public int StopReceiveUdp(IntPtr socket, bool returnBytesRead)
        {
            testLogger.LogTrace("Enter StopReceiveUdp");
            int bytesRead = 0;
            int packetsRead = 0;
            if (udpReceiveListeners.ContainsKey(socket))
            {
                SetEvent(udpReceiveListeners[socket].CancelHandle);
                int waitResult = WaitForSingleObject(udpReceiveListeners[socket].FinishedHandle, 1000 * 60 * 2);
                if (waitResult != 0)
                {
                    throw new TestBugException("The test timed out while waiting for the UDP listener to stop.  This is most likely a test bug.");
                }


                bytesRead = udpReceiveListeners[socket].BytesRead;
                packetsRead = udpReceiveListeners[socket].PacketsRead;
                udpReceiveListeners.Remove(socket);
            }
            if (returnBytesRead)
            {
                testLogger.LogTrace("Exit StopReceiveUdp");
                return bytesRead;
            }
            else
            {
                testLogger.LogTrace("Exit StopReceiveUdp");
                return packetsRead;
            }
        }

        public void StartReceiveTcp(IntPtr socket, bool ipv6)
        {
            testLogger.LogTrace("Enter StartReceiveTcp");
            if (tcpAcceptListeners.ContainsKey(socket) == false)
            {
                Task receiveTask = new Task(Task_StartReceiveTcp, socket, TaskCreationOptions.LongRunning);
                IntPtr stopEvent = CreateEvent(IntPtr.Zero, true, false, null);
                if (stopEvent == IntPtr.Zero)
                {
                    throw new Win32CallException("CreateEvent");
                }

                IntPtr finishedEvent = CreateEvent(IntPtr.Zero, true, false, null);
                if (finishedEvent == IntPtr.Zero)
                {
                    throw new Win32CallException("CreateEvent");
                }

                ListenerData listenerData = new ListenerData(stopEvent, finishedEvent, receiveTask);
                listenerData.Ipv6 = ipv6;
                tcpAcceptListeners.Add(socket, listenerData);
                receiveTask.Start();
                testLogger.LogTrace("Exit StartReceiveTcp");
            }
            else
            {
                throw new TestBugException("The test tried start start a TCP listener that was already started.  This is a test bug.");
            }



        }

        public int StopReceiveTcp(IntPtr socket)
        {
            testLogger.LogTrace("Enter StopReceiveTcp");
            int bytesRead = 0;
            if (tcpAcceptListeners.ContainsKey(socket))
            {
                SetEvent(tcpAcceptListeners[socket].CancelHandle);
                int waitResult = WaitForSingleObject(tcpAcceptListeners[socket].FinishedHandle, 1000 * 60 * 2);
                if (waitResult != 0)
                {
                    throw new TestBugException("The test timed out while waiting for the TCP listener to stop.  This is most likely a test bug.");
                }
                bytesRead = tcpAcceptListeners[socket].BytesRead;
                tcpAcceptListeners.Remove(socket);
            }
            testLogger.LogTrace("Exit StopReceiveTcp");
            return bytesRead;
        }


        private string FindIpAddressIpv4(Guid interfaceGuid)
        {
            testLogger.LogTrace("Enter FindIpAddressIpv4  interfaceGuid: {0}", interfaceGuid.ToString());
            IntPtr pAdapterAddresses = IntPtr.Zero;
            List<IP_ADAPTER_ADDRESSES> adapters = new List<IP_ADAPTER_ADDRESSES>();


            try
            {
                uint outBufLen = 0;
                testLogger.LogTrace("GetAdaptersAddresses AfFamily.AF_INET, GaaFlag.GAA_FLAG_INCLUDE_PREFIX to get buffer size");
                Win32Error err = GetAdaptersAddresses(AfFamily.AF_INET, GaaFlag.GAA_FLAG_INCLUDE_PREFIX, IntPtr.Zero, IntPtr.Zero, ref outBufLen);

                if (Win32Error.ERROR_BUFFER_OVERFLOW == err)
                {
                    pAdapterAddresses = Marshal.AllocHGlobal((int)outBufLen);
                    testLogger.LogTrace("GetAdaptersAddresses AfFamily.AF_INET, GaaFlag.GAA_FLAG_INCLUDE_PREFIX outBufLen {0}", outBufLen);
                    err = GetAdaptersAddresses(AfFamily.AF_INET, GaaFlag.GAA_FLAG_INCLUDE_PREFIX, IntPtr.Zero, pAdapterAddresses, ref outBufLen);
                    if (Win32Error.ERROR_SUCCESS == err)
                    {
                        testLogger.LogTrace("Loop through each adapter and add to list");
                        IntPtr currPtr = pAdapterAddresses;
                        while (IntPtr.Zero != currPtr)
                        {
                            IP_ADAPTER_ADDRESSES addr = Marshal.PtrToStructure<IP_ADAPTER_ADDRESSES>(currPtr);
                            adapters.Add(addr);
                            testLogger.LogTrace("Adding adapter: {0}", addr.AdapterName);
                            currPtr = addr.Next;
                        }
                    }
                }

                foreach (var adapter in adapters)
                {
                    Guid adapterGuid = Guid.Parse(adapter.AdapterName);
                    if (adapterGuid == interfaceGuid)
                    {
                        testLogger.LogTrace("Found Interface with matching GUID: {0}", adapterGuid);
                        testLogger.LogTrace("Exit FindIpAddressIpv4");
                        return FindIpAddressIPv4(adapter);
                    }

                }
            }
            finally
            {
                if (pAdapterAddresses != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(pAdapterAddresses);
                }
            }

            throw new DriverBugException(String.Format(CultureInfo.InvariantCulture, "The test could not find a IPv4 address on the Interface {0}.  This could be a configuration error or a driver bug.  Verify the correct Interface GUID is being used.", interfaceGuid.ToString()));


        }
        private string FindIpAddressIpv6(Guid interfaceGuid)
        {
            testLogger.LogTrace("Enter FindIpAddressIpv6  interfaceGuid: {0}", interfaceGuid.ToString());
            IntPtr pAdapterAddresses = IntPtr.Zero;
            List<IP_ADAPTER_ADDRESSES> adapters = new List<IP_ADAPTER_ADDRESSES>();


            try
            {
                uint outBufLen = 0;
                testLogger.LogTrace("GetAdaptersAddresses AfFamily.AF_INET6, GaaFlag.GAA_FLAG_INCLUDE_PREFIX to get buffer size");
                Win32Error err = GetAdaptersAddresses(AfFamily.AF_INET6, GaaFlag.GAA_FLAG_INCLUDE_PREFIX, IntPtr.Zero, IntPtr.Zero, ref outBufLen);

                if (Win32Error.ERROR_BUFFER_OVERFLOW == err)
                {
                    pAdapterAddresses = Marshal.AllocHGlobal((int)outBufLen);
                    testLogger.LogTrace("GetAdaptersAddresses AfFamily.AF_INET6, GaaFlag.GAA_FLAG_INCLUDE_PREFIX outBufLen {0}", outBufLen);
                    err = GetAdaptersAddresses(AfFamily.AF_INET6, GaaFlag.GAA_FLAG_INCLUDE_PREFIX, IntPtr.Zero, pAdapterAddresses, ref outBufLen);
                    if (Win32Error.ERROR_SUCCESS == err)
                    {
                        testLogger.LogTrace("Loop through each adapter and add to list");

                        IntPtr currPtr = pAdapterAddresses;
                        while (IntPtr.Zero != currPtr)
                        {
                            IP_ADAPTER_ADDRESSES addr = Marshal.PtrToStructure<IP_ADAPTER_ADDRESSES>(currPtr);
                            adapters.Add(addr);
                            testLogger.LogTrace("Adding adapter: {0}", addr.AdapterName);
                            currPtr = addr.Next;
                        }
                    }
                }

                foreach (var adapter in adapters)
                {
                    Guid adapterGuid = Guid.Parse(adapter.AdapterName);
                    if (adapterGuid == interfaceGuid)
                    {
                        testLogger.LogTrace("Found Interface with matching GUID: {0}", adapterGuid);
                        testLogger.LogTrace("Exit FindIpAddressIpv4");

                        return FindIpAddressIPv6(adapter);
                    }

                }
            }
            finally
            {
                if (pAdapterAddresses != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(pAdapterAddresses);
                }
            }

            throw new DriverBugException(String.Format(CultureInfo.InvariantCulture, "The test could not find a IPv6 address on the Interface {0}.  This could be a configuration error or a driver bug.  Verify the correct Interface GUID is being used.", interfaceGuid.ToString()));


        }

        public UInt32 FindInterfaceIndexIpv6(Guid interfaceGuid)
        {
            testLogger.LogTrace("Enter FindInterfaceIndexIpv6  interfaceGuid: {0}", interfaceGuid.ToString());
            IntPtr pAdapterAddresses = IntPtr.Zero;
            List<IP_ADAPTER_ADDRESSES> adapters = new List<IP_ADAPTER_ADDRESSES>();


            try
            {
                uint outBufLen = 0;
                testLogger.LogTrace("GetAdaptersAddresses AfFamily.AF_INET6, GaaFlag.GAA_FLAG_INCLUDE_PREFIX to get buffer size");
                Win32Error err = GetAdaptersAddresses(AfFamily.AF_INET6, GaaFlag.GAA_FLAG_INCLUDE_PREFIX, IntPtr.Zero, IntPtr.Zero, ref outBufLen);

                if (Win32Error.ERROR_BUFFER_OVERFLOW == err)
                {
                    pAdapterAddresses = Marshal.AllocHGlobal((int)outBufLen);
                    testLogger.LogTrace("GetAdaptersAddresses AfFamily.AF_INET6, GaaFlag.GAA_FLAG_INCLUDE_PREFIX outBufLen {0}", outBufLen);
                    err = GetAdaptersAddresses(AfFamily.AF_INET6, GaaFlag.GAA_FLAG_INCLUDE_PREFIX, IntPtr.Zero, pAdapterAddresses, ref outBufLen);
                    if (Win32Error.ERROR_SUCCESS == err)
                    {
                        testLogger.LogTrace("Loop through each adapter and add to list");
                        IntPtr currPtr = pAdapterAddresses;
                        while (IntPtr.Zero != currPtr)
                        {
                            IP_ADAPTER_ADDRESSES addr = Marshal.PtrToStructure<IP_ADAPTER_ADDRESSES>(currPtr);
                            adapters.Add(addr);
                            testLogger.LogTrace("Adding adapter: {0}", addr.AdapterName);
                            currPtr = addr.Next;
                        }
                    }
                }

                foreach (var adapter in adapters)
                {
                    Guid adapterGuid = Guid.Parse(adapter.AdapterName);
                    if (adapterGuid == interfaceGuid)
                    {
                        testLogger.LogTrace("Found Interface with matching GUID: {0}", adapterGuid);
                        testLogger.LogTrace("Exit FindInterfaceIndexIpv6");

                        return adapter.Ipv6IfIndex;
                    }

                }
            }
            finally
            {
                if (pAdapterAddresses != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(pAdapterAddresses);
                }
            }

            throw new DriverBugException("The test could not find the IPv6 address on the the network Interfaces.  It attempts this by calling GetAdaptersAddresses.  To triage this attempt running IPConfig.exe when connected to the network.");


        }

        private string FindIpAddressIPv4(IP_ADAPTER_ADDRESSES adapterAddress)
        {
            testLogger.LogTrace("Enter FindIpAddressIPv4");

            List<string> address = new List<string>();
            if (IntPtr.Zero != adapterAddress.FirstUnicastAddress)
            {

                IP_ADAPTER_UNICAST_ADDRESS unicastAddr = Marshal.PtrToStructure<IP_ADAPTER_UNICAST_ADDRESS>(adapterAddress.FirstUnicastAddress);
                if (IntPtr.Zero != unicastAddr.Address.lpSockAddr)
                {
                    SOCKADDR_IN socketAddr = Marshal.PtrToStructure<SOCKADDR_IN>(unicastAddr.Address.lpSockAddr);
                    address.Add(ParseAddressFromSockAddr(socketAddr, unicastAddr.Address.iSockaddrLength));
                }
            }
            if (address.Count == 0)
            {
                throw new DriverBugException("The test could not find the IPv4 address on the the network Interfaces.  It attempts this by calling GetAdaptersAddresses.  To triage this attempt running IPConfig.exe when connected to the network.");
            }
            testLogger.LogTrace("Exit FindIpAddressIPv4: Return Address {0}", address[0]);
            return address[0];
        }
        private string FindIpAddressIPv6(IP_ADAPTER_ADDRESSES adapterAddress)
        {
            testLogger.LogTrace("Enter FindIpAddressIPv6");
            List<string> address = new List<string>();
            if (IntPtr.Zero != adapterAddress.FirstUnicastAddress)
            {

                IP_ADAPTER_UNICAST_ADDRESS unicastAddr = Marshal.PtrToStructure<IP_ADAPTER_UNICAST_ADDRESS>(adapterAddress.FirstUnicastAddress);
                if (IntPtr.Zero != unicastAddr.Address.lpSockAddr)
                {
                    SOCKADDR_IN6 socketAddr = Marshal.PtrToStructure<SOCKADDR_IN6>(unicastAddr.Address.lpSockAddr);
                    address.Add(ParseAddressFromSockAddr(socketAddr, unicastAddr.Address.iSockaddrLength));
                }
            }
            if (address.Count == 0)
            {
                throw new DriverBugException("The test could not find the IPv6 address on the the network Interfaces.  It attempts this by calling GetAdaptersAddresses.  To triage this attempt running IPConfig.exe when connected to the network.");
            }
            testLogger.LogTrace("Exit FindIpAddressIPv6: Return Address {0}", address[0]);
            return address[0];
        }

        private string ParseAddressFromSockAddr(SOCKADDR_IN socketAddr, uint length)
        {
            testLogger.LogTrace("Enter ParseAddressFromSockAddr IPv4");
            uint bufferSize = 128;
            IntPtr buffer = Marshal.AllocHGlobal(128 * 2);
            string returnValue = null;
            int retVal = WSAAddressToString(ref socketAddr, length, IntPtr.Zero, buffer, ref bufferSize);
            if (retVal == 0)
            {
                returnValue = Marshal.PtrToStringUni(buffer);
            }
            else
            {
                throw new DriverBugException("The test could not find the IPv4 address on the the network Interfaces.  It attempts this by calling GetAdaptersAddresses.  To triage this attempt running IPConfig.exe when connected to the network.");
            }

            Marshal.FreeHGlobal(buffer);
            testLogger.LogTrace("Exit ParseAddressFromSockAddr.  Return Address: {0}", returnValue);
            return returnValue;
        }
        private string ParseAddressFromSockAddr(SOCKADDR_IN6 socketAddr, uint length)
        {

            testLogger.LogTrace("Enter ParseAddressFromSockAddr IPv6");
            uint bufferSize = 128;
            IntPtr buffer = Marshal.AllocHGlobal(128 * 2);
            string returnValue = null;
            int retVal = WSAAddressToString(ref socketAddr, length, IntPtr.Zero, buffer, ref bufferSize);
            if (retVal == 0)
            {
                returnValue = Marshal.PtrToStringUni(buffer);
            }

            Marshal.FreeHGlobal(buffer);
            testLogger.LogTrace("Exit ParseAddressFromSockAddr.  Return Address: {0}", returnValue);
            return returnValue;
        }

        private IntPtr CreateUdpSocketIpv4(string localIpAddress, ushort localPort)
        {
            testLogger.LogTrace("Enter CreateUdpSocketIpv4 localIpAddress: {0}  localPort {1}", localIpAddress, localPort);
            try
            {
                uint localAddrSize;

                testLogger.LogTrace("Call GetSockAddrIpv4");
                SOCKADDR_IN localSockAddr = GetSockAddrIpv4(localIpAddress, out localAddrSize, localPort);
                int lastError;

                testLogger.LogTrace("Call socket AF_INET, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP");
                var udpSocket = socket(AfFamily.AF_INET, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP);
                if (udpSocket == (IntPtr)(-1))
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("socket", lastError, "socket was called attempt to create a IPv4 UDP Socket.");
                }

                testLogger.LogTrace("Call bind");
                int retCode = bind(udpSocket, ref localSockAddr, localAddrSize);
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("bind", lastError, string.Format(CultureInfo.InvariantCulture, "bind failed when attempting to bind the IPv4 UDP socket to the local address {0}:{1}", localIpAddress, localPort));
                }

                testLogger.LogTrace("Exit CreateUdpSocketIpv4");
                return udpSocket;
            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
        }
        private IntPtr CreateUdpSocketIpv6(string localIpAddress, ushort localPort)
        {
            testLogger.LogTrace("Enter CreateUdpSocketIpv6 localIpAddress: {0}  localPort {1}", localIpAddress, localPort);
            try
            {
                uint localAddrSize;

                testLogger.LogTrace("Call GetSockAddrIpv6");
                SOCKADDR_IN6 localSockAddr = GetSockAddrIpv6(localIpAddress, out localAddrSize, localPort);
                int lastError;

                testLogger.LogTrace("Call socket AF_INET6, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP");
                var udpSocket = socket(AfFamily.AF_INET6, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP);
                if (udpSocket == (IntPtr)(-1))
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("socket", lastError, "socket was called attempt to create a IPv4 UDP Socket.");
                }

                testLogger.LogTrace("Call bind");
                int retCode = bind(udpSocket, ref localSockAddr, localAddrSize);
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("bind", lastError, string.Format(CultureInfo.InvariantCulture, "bind failed when attempting to bind the IPv6 UDP socket to the local address {0}:{1}", localIpAddress, localPort));
                }

                testLogger.LogTrace("Exit CreateUdpSocketIpv6");
                return udpSocket;
            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
        }
        private IntPtr CreateTcpSocketIpv4(string localIpAddress, ushort localPort)
        {
            testLogger.LogTrace("Enter CreateTcpSocketIpv4 localIpAddress: {0}  localPort {1}", localIpAddress, localPort);
            try
            {
                uint localAddrSize;

                testLogger.LogTrace("Call GetSockAddrIpv4");
                SOCKADDR_IN localSockAddr = GetSockAddrIpv4(localIpAddress, out localAddrSize, localPort);
                int lastError;
                testLogger.LogTrace("Call socket AF_INET, SockType.SOCK_STREAM, ProtocolType.IPPROTO_TCP");
                var tcpSocket = socket(AfFamily.AF_INET, SockType.SOCK_STREAM, ProtocolType.IPPROTO_TCP);
                if (tcpSocket == (IntPtr)(-1))
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("socket", lastError, "socket was called attempt to create a IPv4 TCP Socket.");
                }

                testLogger.LogTrace("Call bind");
                int retCode = bind(tcpSocket, ref localSockAddr, localAddrSize);
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("bind", lastError, string.Format(CultureInfo.InvariantCulture, "bind failed when attempting to bind the IPv4 TCP socket to the local address {0}:{1}", localIpAddress, localPort));
                }

                testLogger.LogTrace("Exit CreateTcpSocketIpv4");
                return tcpSocket;
            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
        }
        private IntPtr CreateTcpSocketIpv6(string localIpAddress, ushort localPort)
        {
            testLogger.LogTrace("Enter CreateTcpSocketIpv6 localIpAddress: {0}  localPort {1}", localIpAddress, localPort);
            try
            {
                uint localAddrSize;

                testLogger.LogTrace("Call GetSockAddrIpv6");
                SOCKADDR_IN6 localSockAddr = GetSockAddrIpv6(localIpAddress, out localAddrSize, localPort);
                int lastError;

                testLogger.LogTrace("Call socket AF_INET6, SockType.SOCK_STREAM, ProtocolType.IPPROTO_TCP");
                var tcpSocket = socket(AfFamily.AF_INET6, SockType.SOCK_STREAM, ProtocolType.IPPROTO_TCP);
                if (tcpSocket == (IntPtr)(-1))
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("socket", lastError, "socket was called attempt to create a IPv6 TCP Socket.");
                }

                testLogger.LogTrace("Call bind");
                int retCode = bind(tcpSocket, ref localSockAddr, localAddrSize);
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("bind", lastError, string.Format(CultureInfo.InvariantCulture, "bind failed when attempting to bind the IPv6 TCP socket to the local address {0}:{1}", localIpAddress, localPort));
                }


                testLogger.LogTrace("Exit CreateTcpSocketIpv6");
                return tcpSocket;
            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
        }
        private IntPtr CreateMulticastSocketIpv4(string localIpAddress, ushort localPort, string multicastAddress, ushort multicastPort)
        {
            testLogger.LogTrace("Enter CreateTcpSocketIpv4 localIpAddress: {0}  localPort: {1}  multicastAddress: {2}  multicastPort: {3}", localIpAddress, localPort, multicastAddress, multicastPort);
            try
            {
                uint localAddrSize;

                testLogger.LogTrace("Call GetSockAddrIpv4");
                SOCKADDR_IN localSockAddr = GetSockAddrIpv4(localIpAddress, out localAddrSize, localPort);
                int lastError;

                testLogger.LogTrace("Call socket AF_INET, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP");
                var udpSocket = socket(AfFamily.AF_INET, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP);
                if (udpSocket == (IntPtr)(-1))
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("socket", lastError, "socket was called attempt to create a IPv4 UDP Socket.");
                }

                uint multicastAddrSize;
                testLogger.LogTrace("Call GetSockAddrIpv4");
                SOCKADDR_IN multicastSockAddr = GetSockAddrIpv4(multicastAddress, out multicastAddrSize, multicastPort);

                ip_mreq multicastAdd = new ip_mreq();
                multicastAdd.imr_multiaddr = multicastSockAddr.sin_addr;
                multicastAdd.imr_interface = localSockAddr.sin_addr;

                testLogger.LogTrace("Call setsockopt IPPROTO_IP, IPV6_ADD_MEMBERSHIP");
                int retCode = setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, ref multicastAdd, Marshal.SizeOf<ip_mreq>());
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("setsockopt", lastError, string.Format(CultureInfo.InvariantCulture, "setsockopt failed when attempt to set the IP_ADD_MEMBERSHIP socket option for the address {0}", multicastAddress));
                }

                int multicastTtl = 2;
                testLogger.LogTrace("Call setsockopt IPPROTO_IP, IP_MULTICAST_TTL");
                retCode = setsockopt(udpSocket, IPPROTO_IP, IP_MULTICAST_TTL, ref multicastTtl, Marshal.SizeOf<Int32>());
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("setsockopt", lastError, "setsockopt failed when attempt to set the IP_MULTICAST_TTL socket option.");
                }

                testLogger.LogTrace("Call bind");
                retCode = bind(udpSocket, ref localSockAddr, localAddrSize);
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("bind", lastError, string.Format(CultureInfo.InvariantCulture, "bind failed when attempting to bind the IPv4 UDP Multicast socket to the local address {0}:{1}", localIpAddress, localPort));
                }

                testLogger.LogTrace("Exit CreateTcpSocketIpv4");
                return udpSocket;
            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
        }
        private IntPtr CreateMulticastSocketIpv6(string localIpAddress, ushort localPort, string multicastAddress, ushort multicastPort, uint interfaceIndex)
        {
            testLogger.LogTrace("Enter CreateMulticastSocketIpv6 localIpAddress: {0}  localPort: {1}  multicastAddress: {2}  multicastPort: {3}  interfaceIndex: {4}", localIpAddress, localPort, multicastAddress, multicastPort, interfaceIndex);
            try
            {
                uint localAddrSize;

                testLogger.LogTrace("Call GetSockAddrIpv6");
                SOCKADDR_IN6 localSockAddr = GetSockAddrIpv6(localIpAddress, out localAddrSize, localPort);
                int lastError;

                testLogger.LogTrace("Call socket AF_INET6, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP");
                var udpSocket = socket(AfFamily.AF_INET6, SockType.SOCK_DGRAM, ProtocolType.IPPROTO_UDP);
                if (udpSocket == (IntPtr)(-1))
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("socket", lastError, "socket was called attempt to create a IPv6 UDP Socket.");
                }

                uint multicastAddrSize;
                testLogger.LogTrace("Call GetSockAddrIpv6");
                SOCKADDR_IN6 multicastSockAddr = GetSockAddrIpv6(multicastAddress, out multicastAddrSize, multicastPort);

                ipv6_mreq multicastAdd = new ipv6_mreq();
                multicastAdd.ipv6mr_multiaddr = multicastSockAddr.sin_addr;
                multicastAdd.ipv6mr_interface = interfaceIndex;

                testLogger.LogTrace("Call setsockopt IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP");
                int retCode = setsockopt(udpSocket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, ref multicastAdd, Marshal.SizeOf<ipv6_mreq>());
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("setsockopt", lastError, string.Format(CultureInfo.InvariantCulture, "setsockopt failed when attempt to set the IPV6_ADD_MEMBERSHIP socket option for the address {0}", multicastAddress));
                }

                int multicastTtl = 2;
                testLogger.LogTrace("Call setsockopt IPPROTO_IP, IPV6_HOPLIMIT");
                retCode = setsockopt(udpSocket, IPPROTO_IPV6, IPV6_HOPLIMIT, ref multicastTtl, Marshal.SizeOf<Int32>());
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("setsockopt", lastError, "setsockopt failed when attempt to set the IPV6_HOPLIMIT socket option.");
                }

                testLogger.LogTrace("Call bind");
                retCode = bind(udpSocket, ref localSockAddr, localAddrSize);
                if (retCode != 0)
                {
                    lastError = WSAGetLastError();
                    throw new Win32CallException("bind", lastError, string.Format(CultureInfo.InvariantCulture, "bind failed when attempting to bind the IPv6 UDP Multicast socket to the local address {0}:{1}", localIpAddress, localPort));
                }

                testLogger.LogTrace("Exit CreateMulticastSocketIpv6");
                return udpSocket;
            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
        }

        private SOCKADDR_IN GetSockAddrIpv4(string ipAddress, out uint size, ushort port)
        {
            testLogger.LogTrace("Enter GetSockAddrIpv4 {0} {1}", ipAddress, port);
            uint tempSize = 200;

            SOCKADDR_IN sockAddr;
            using (var addressSafeHandle = HGlobalMemorySafeHandle.Allocate(200))
            {
                int retCode = WSAStringToAddress(ipAddress, AfFamily.AF_INET, IntPtr.Zero, addressSafeHandle, ref tempSize);
                if (retCode != 0)
                {
                    int lastError = WSAGetLastError();
                    throw new Win32CallException("WSAStringToAddress", lastError, string.Format(CultureInfo.InvariantCulture, "WSAGetLastError failed when trying to parse {0}:{1}", ipAddress, port));
                }
                else
                {
                    sockAddr = InteropUtilities.PtrToStructure<SOCKADDR_IN>(addressSafeHandle);
                }
            }

            size = tempSize;

            // Translate Port to big/little endian
            byte[] reversePort = BitConverter.GetBytes(port);
            Array.Reverse(reversePort);
            ushort finalPort = BitConverter.ToUInt16(reversePort, 0);
            sockAddr.port = finalPort;

            testLogger.LogTrace("Exit GetSockAddrIpv4");
            return sockAddr;
        }
        private SOCKADDR_IN6 GetSockAddrIpv6(string ipAddress, out uint size, ushort port)
        {
            testLogger.LogTrace("Enter GetSockAddrIpv6 {0} {1}", ipAddress, port);
            uint tempSize = 200;

            SOCKADDR_IN6 sockAddr;
            using (var addressSafeHandle = HGlobalMemorySafeHandle.Allocate(200))
            {
                int retCode = WSAStringToAddress(ipAddress, AfFamily.AF_INET6, IntPtr.Zero, addressSafeHandle, ref tempSize);
                if (retCode != 0)
                {
                    int lastError = WSAGetLastError();
                    throw new Win32CallException("WSAStringToAddress", lastError, string.Format(CultureInfo.InvariantCulture, "WSAGetLastError failed when trying to parse {0}:{1}", ipAddress, port));
                }
                else
                {
                    sockAddr = InteropUtilities.PtrToStructure<SOCKADDR_IN6>(addressSafeHandle);
                }
            }

            size = tempSize;

            // Translate Port to big/little endian
            byte[] reversePort = BitConverter.GetBytes(port);
            Array.Reverse(reversePort);
            ushort finalPort = BitConverter.ToUInt16(reversePort, 0);
            sockAddr.port = finalPort;

            testLogger.LogTrace("Exit GetSockAddrIpv6");
            return sockAddr;
        }


        private void ConnectIpv4(IntPtr socket, string destinationAddress, ushort destinationPort)
        {
            testLogger.LogTrace("Enter ConnectIpv4");
            uint destinationAddrSize;
            SOCKADDR_IN destinationSockAddr = GetSockAddrIpv4(destinationAddress, out destinationAddrSize, destinationPort);

            int retCode = connect(socket, ref destinationSockAddr, destinationAddrSize);
            if (retCode != 0)
            {
                var lastError = WSAGetLastError();
                throw new Win32CallException("connect", lastError, string.Format(CultureInfo.InvariantCulture, "connect when trying to connect to {0}:{1}", destinationAddress, destinationPort));
            }
            testLogger.LogTrace("Exit ConnectIpv4");

        }

        private void ConnectIpv6(IntPtr socket, string destinationAddress, ushort destinationPort)
        {
            testLogger.LogTrace("Enter ConnectIpv6");

            uint destinationAddrSize;
            SOCKADDR_IN6 destinationSockAddr = GetSockAddrIpv6(destinationAddress, out destinationAddrSize, destinationPort);

            int retCode = connect(socket, ref destinationSockAddr, destinationAddrSize);
            if (retCode != 0)
            {
                var lastError = WSAGetLastError();
                throw new Win32CallException("connect", lastError, string.Format(CultureInfo.InvariantCulture, "connect when trying to connect to {0}:{1}", destinationAddress, destinationPort));
            }

            testLogger.LogTrace("Exit ConnectIpv6");

        }


        private void SendToIpv4(IntPtr socket, byte[] buffer, string destinationAddress, ushort destinationPort)
        {
            testLogger.LogTrace("Enter SendToIpv4 {0} {1}", destinationAddress, destinationPort);
            uint destinationAddrSize;

            testLogger.LogTrace("Call GetSockAddrIpv4");
            SOCKADDR_IN destinationSockAddr = GetSockAddrIpv4(destinationAddress, out destinationAddrSize, destinationPort);

            using (var bufferSafeHandle = HGlobalMemorySafeHandle.Allocate(buffer))
            {
                int bytesSent = 0;
                while (true)
                {
                    using (var currentBuffer = InteropUtilities.GetSafeHandleToOffset(bufferSafeHandle, bytesSent))
                    {
                        int remainingBytes = buffer.Length - bytesSent;
                        testLogger.LogTrace("Call sendto");
                        var iterationBytesSent = sendto(socket, currentBuffer, remainingBytes, 0, ref destinationSockAddr, destinationAddrSize);
                        if (iterationBytesSent == -1)
                        {
                            var errorcode = WSAGetLastError();
                            throw new Win32CallException("sendto", errorcode, string.Format(CultureInfo.InvariantCulture, "sendto when trying to send to {0}:{1}", destinationAddress, destinationPort));
                        }
                        bytesSent += iterationBytesSent;
                        if (buffer.Length - bytesSent == 0)
                        {
                            //all packets sent
                            break;
                        }
                    }
                }
            }
            testLogger.LogTrace("Exit SendToIpv4");
        }
        private void SendToIpv6(IntPtr socket, byte[] buffer, string destinationAddress, ushort destinationPort)
        {
            testLogger.LogTrace("Enter SendToIpv6 {0} {1}", destinationAddress, destinationPort);
            uint destinationAddrSize;

            testLogger.LogTrace("Call GetSockAddrIpv6");
            SOCKADDR_IN6 destinationSockAddr = GetSockAddrIpv6(destinationAddress, out destinationAddrSize, destinationPort);

            using (var bufferSafeHandle = HGlobalMemorySafeHandle.Allocate(buffer))
            {
                int bytesSent = 0;
                while (true)
                {
                    using (var currentBuffer = InteropUtilities.GetSafeHandleToOffset(bufferSafeHandle, bytesSent))
                    {
                        int remainingBytes = buffer.Length - bytesSent;
                        testLogger.LogTrace("Call sendto");
                        var iterationBytesSent = sendto(socket, currentBuffer, remainingBytes, 0, ref destinationSockAddr, destinationAddrSize);
                        if (iterationBytesSent == -1)
                        {
                            var errorcode = WSAGetLastError();
                            throw new Win32CallException("sendto", errorcode, string.Format(CultureInfo.InvariantCulture, "sendto when trying to send to {0}:{1}", destinationAddress, destinationPort));
                        }
                        bytesSent += iterationBytesSent;
                        if (buffer.Length - bytesSent == 0)
                        {
                            //all packets sent
                            break;
                        }
                    }
                }
            }
            testLogger.LogTrace("Exit SendToIpv6");
        }

        private void Task_StartReceiveUdp(object state)
        {
            testLogger.LogTrace("Enter Task_StartReceiveUdp");
            IntPtr socket = (IntPtr)state;
            ListenerData listenerData = udpReceiveListeners[socket];
            IntPtr cancelEvent = listenerData.CancelHandle;

            IntPtr buffer = Marshal.AllocHGlobal(4096);

            IntPtr[] handleArray = new IntPtr[2];
            handleArray[0] = cancelEvent;

            OVERLAPPED overLapped = new OVERLAPPED();

            testLogger.LogTrace("call WSACreateEvent");
            overLapped.EventHandle = WSACreateEvent();
            if (overLapped.EventHandle == IntPtr.Zero)
            {
                throw new Win32CallException("WSACreateEvent");
            }
            handleArray[1] = overLapped.EventHandle;

            WsaBuf wsaBuf = new WsaBuf();
            wsaBuf.len = 4096;
            wsaBuf.buf = buffer;
            IntPtr dataPointer = Marshal.AllocHGlobal(Marshal.SizeOf<WsaBuf>());
            Marshal.StructureToPtr(wsaBuf, dataPointer, false);

            int numberOfBytes = 0;
            int flags = 0;
            Int32 lastError = 0;

            try
            {
                while (true)
                {
                    numberOfBytes = 0;
                    flags = 0;
                    testLogger.LogTrace("call WSARecv");
                    int retVal = WSARecv(socket, dataPointer, 1, ref numberOfBytes, ref flags, ref overLapped, IntPtr.Zero);

                    if (retVal == 0)
                    {
                        if (numberOfBytes == 0)
                        {
                            testLogger.LogTrace("Task_StartReceiveUdp no more data");
                            break;
                        }
                        else
                       {
                            listenerData.PacketsRead++;
                            listenerData.BytesRead += numberOfBytes;
                            testLogger.LogTrace("Task_StartReceiveUdp Packets Read {0}", listenerData.PacketsRead);
                            testLogger.LogTrace("Task_StartReceiveUdp Bytes Read {0}", listenerData.BytesRead);
                        }
                    }
                    else if (retVal == -1)  // Error in WSA Recv
                    {
                        lastError = Marshal.GetLastWin32Error();
                        if (lastError == 10038)
                        {
                            // We are shutting down
                            testLogger.LogTrace("Task_StartReceiveUdp shutting down");
                            break;
                        }
                        else if (lastError == 997)  // Pending Result :)
                        {
                            testLogger.LogTrace("Task_StartReceiveUdp Result is pending");
                            retVal = WSAWaitForMultipleEvents(2, handleArray, false, 1000 * 60 * 5, false);
                            if (retVal == 0)
                            {
                                testLogger.LogTrace("Task_StartReceiveUdp shutting down");
                                break;
                            }
                            else if (retVal == 1)
                            {
                                int bytesRead;
                                retVal = GetOverlappedResult(socket, ref overLapped, out bytesRead, false);
                                if (retVal != 0)
                                {
                                    if (bytesRead == 0)
                                    {
                                        testLogger.LogTrace("Task_StartReceiveUdp no more data");
                                        return;
                                    }
                                    listenerData.BytesRead += bytesRead;
                                    listenerData.PacketsRead++;
                                    testLogger.LogTrace("Task_StartReceiveUdp Packets Read {0}", listenerData.PacketsRead);
                                    testLogger.LogTrace("Task_StartReceiveUdp Bytes Read {0}", listenerData.BytesRead);

                                }
                                else
                                {
                                    lastError = Marshal.GetLastWin32Error();
                                    throw new Win32CallException("GetOverlappedResult", lastError);
                                }
                            }
                            else
                            {
                                lastError = Marshal.GetLastWin32Error();
                                throw new Win32CallException("WaitForSingleObject", lastError);
                            }
                        }
                        else
                        {
                            throw new Win32CallException("WSARecv", lastError);
                        }
                    }

                }

            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
            finally
            {
                if (overLapped.EventHandle != IntPtr.Zero)
                {
                    testLogger.LogTrace("Calling WSACloseEvent");
                    WSACloseEvent(overLapped.EventHandle);
                }
                if (buffer != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(buffer);
                    buffer = IntPtr.Zero;
                }
                if (dataPointer != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(dataPointer);
                    dataPointer = IntPtr.Zero;
                }
                if (cancelEvent != IntPtr.Zero)
                {
                    CloseHandle(cancelEvent);
                    cancelEvent = IntPtr.Zero;
                }

                SetEvent(listenerData.FinishedHandle);
            }

            testLogger.LogTrace("Exit Task_StartReceiveUdp");
        }

        private void Task_StartReceiveTcp(object state)
        {
            testLogger.LogTrace("Enter Task_StartReceiveTcp");
            IntPtr listenSocket = (IntPtr)state;
            ListenerData listenerData = tcpAcceptListeners[listenSocket];
            IntPtr cancelEvent = listenerData.CancelHandle;

            IntPtr buffer = Marshal.AllocHGlobal(4096);
            int addressSize;
            if (listenerData.Ipv6)
            {
                addressSize = Marshal.SizeOf<SOCKADDR_IN6>() + 16;
            }
            else
            {
                addressSize = Marshal.SizeOf<SOCKADDR_IN>() + 16;
            }
            int bufferSize = 4096 - (addressSize * 2);

            IntPtr[] handleArray = new IntPtr[2];
            handleArray[0] = cancelEvent;

            OVERLAPPED overLapped = new OVERLAPPED();
            overLapped.EventHandle = WSACreateEvent();
            if (overLapped.EventHandle == IntPtr.Zero)
            {
                throw new Win32CallException("WSACreateEvent");
            }
            handleArray[1] = overLapped.EventHandle;
            Int32 lastError = 0;

            IntPtr acceptSocket = IntPtr.Zero;

            int numberOfBytes = 0;

            try
            {
                numberOfBytes = 0;
                testLogger.LogTrace("Calling listenSocket");
                int retVal = listen(listenSocket, 5);
                if (retVal != 0)
                {
                    lastError = Marshal.GetLastWin32Error();
                    throw new Win32CallException("listen", lastError);
                }

                while (true)
                {

                    if (acceptSocket != IntPtr.Zero)
                    {
                        testLogger.LogComment("Task_StartReceiveTcp Calling closesocket");
                        closesocket(acceptSocket);
                        acceptSocket = IntPtr.Zero;
                    }
                    if (listenerData.Ipv6)
                    {
                        testLogger.LogTrace("Task_StartReceiveTcp Creating Accept Socket AF_INET6, SockType.SOCK_STREAM, ProtocolType.IPPROTO_TCP");
                        acceptSocket = socket(AfFamily.AF_INET6, SockType.SOCK_STREAM, ProtocolType.IPPROTO_TCP);
                    }
                    else
                    {
                        testLogger.LogTrace("Task_StartReceiveTcp Creating Accept Socket AF_INET, SockType.SOCK_STREAM, ProtocolType.IPPROTO_TCP");
                        acceptSocket = socket(AfFamily.AF_INET, SockType.SOCK_STREAM, ProtocolType.IPPROTO_TCP);
                    }
                    if (acceptSocket == (IntPtr)(-1))
                    {
                        lastError = WSAGetLastError();
                        if (lastError == 10093)
                        {
                            // We are shutting down
                            break;
                        }
                        throw new Win32CallException("socket", lastError);
                    }

                    lastError = Marshal.GetLastWin32Error();
                    testLogger.LogTrace("Task_StartReceiveTcp Calling AcceptEx");
                    bool bVal = AcceptEx(listenSocket, acceptSocket, buffer, bufferSize, addressSize, addressSize, ref numberOfBytes, ref overLapped);
                    if (!bVal)  // Error in WSA Recv
                    {
                        lastError = Marshal.GetLastWin32Error();
                        if (lastError == 997)  // Pending Result :)
                        {
                            testLogger.LogTrace("Task_StartReceiveTcp Waiting for result");
                            retVal = WSAWaitForMultipleEvents(2, handleArray, false, 1000 * 60 * 5, false);
                            if (retVal == 0)
                            {
                                break;
                            }
                            else if (retVal == 1)
                            {
                                int bytesRead;
                                testLogger.LogTrace("Task_StartReceiveTcp calling GetOverlappedResult");
                                retVal = GetOverlappedResult(acceptSocket, ref overLapped, out bytesRead, false);
                                if (retVal != 0)
                                {
                                    listenerData.BytesRead += bytesRead;
                                    testLogger.LogTrace("Task_StartReceiveTcp Bytes Read {0}", listenerData.BytesRead);
                                }
                                else
                                {
                                    lastError = Marshal.GetLastWin32Error();
                                    throw new Win32CallException("GetOverlappedResult", lastError);
                                }
                                StartReceiveUdp(acceptSocket);
                                int wait = WaitForSingleObject(cancelEvent, 1000 * 60 * 3);
                                if (wait != 0)
                                {
                                    testLogger.LogTrace("Task_StartReceiveTcp didnt get stopped");
                                }
                                // If cancelEvent gets fired, we will get WAIT_OBJECT_0
                                bytesRead = StopReceiveUdp(acceptSocket, true);
                                listenerData.BytesRead += bytesRead;
                                testLogger.LogTrace("Task_StartReceiveTcp Bytes Read {0}", listenerData.BytesRead);
                                break;
                            }
                            else
                            {
                                lastError = Marshal.GetLastWin32Error();
                                throw new Win32CallException("WaitForSingleObject", lastError);
                            }
                        }
                        else if (lastError == 10093)
                        {
                            // We are shutting down
                            testLogger.LogTrace("Task_StartReceiveTcp Shutting down");
                            break;
                        }
                        else
                        {
                            throw new Win32CallException("AcceptEx", lastError);
                        }
                    }
                    else
                    {
                        throw new TestBugException("Unexpected return of AcceptEx");
                    }
                }


            }
            catch (Exception error)
            {
                testLogger.LogError(error.ToString());
                throw;
            }
            finally
            {
                if (overLapped.EventHandle != IntPtr.Zero)
                {
                    WSACloseEvent(overLapped.EventHandle);
                }
                if (buffer != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(buffer);
                    buffer = IntPtr.Zero;
                }
                if (cancelEvent != IntPtr.Zero)
                {
                    CloseHandle(cancelEvent);
                    cancelEvent = IntPtr.Zero;
                }
                if (acceptSocket != IntPtr.Zero)
                {
                    testLogger.LogTrace("Task_StartReceiveTcp Closing Accept Socket");
                    closesocket(acceptSocket);
                    acceptSocket = IntPtr.Zero;
                }
                SetEvent(listenerData.FinishedHandle);
            }
            testLogger.LogTrace("Exit Task_StartReceiveTcp");
        }



        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "WSAStartup", SetLastError = true)]
        private static extern Int32 WSAStartup(Int16 wVersionRequested, out WSAData wsaData);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "WSACleanup", SetLastError = true)]
        private static extern void WSACleanup();

        [DllImport("Iphlpapi.dll", ExactSpelling = true, EntryPoint = "GetAdaptersAddresses", SetLastError = true, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Winapi)]
        private static extern Win32Error GetAdaptersAddresses(
            AfFamily Family,
            GaaFlag Flags,
            IntPtr Reserved,
            IntPtr AdapterAddresses,
            ref uint SizePointer);

        [DllImport("Ws2_32.dll", CharSet = CharSet.Unicode, EntryPoint = "WSAAddressToStringW", SetLastError = true)]
        private static extern int WSAAddressToString(
                          ref SOCKADDR_IN lpsaAddress,
                          uint dwAddressLength,
                          IntPtr lpProtocolInfo,
                          IntPtr lpszAddressString,
                          ref uint lpdwAddressStringLength);

        [DllImport("Ws2_32.dll", CharSet = CharSet.Unicode, EntryPoint = "WSAAddressToStringW", SetLastError = true)]
        private static extern int WSAAddressToString(
                          ref SOCKADDR_IN6 lpsaAddress,
                          uint dwAddressLength,
                          IntPtr lpProtocolInfo,
                          IntPtr lpszAddressString,
                          ref uint lpdwAddressStringLength);

        [DllImport("ws2_32.dll", ExactSpelling = true, SetLastError = true)]
        private static extern void closesocket(IntPtr s);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "socket", SetLastError = true)]
        private static extern IntPtr socket(AfFamily af, SockType type, ProtocolType protocol);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "bind", SetLastError = true)]
        private static extern int bind(IntPtr s, ref SOCKADDR_IN name, uint length);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "bind", SetLastError = true)]
        private static extern int bind(IntPtr s, ref SOCKADDR_IN6 name, uint length);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "connect", SetLastError = true)]
        private static extern int connect(IntPtr s, ref SOCKADDR_IN name, uint length);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "connect", SetLastError = true)]
        private static extern int connect(IntPtr s, ref SOCKADDR_IN6 name, uint length);

        [DllImport("Mswsock.dll", ExactSpelling = true, EntryPoint = "AcceptEx", SetLastError = true)]
        private static extern bool AcceptEx(IntPtr sListenSocket, IntPtr sAcceptSocket, IntPtr lpOutputBuffer, Int32 dwReceiveDataLength, Int32 dwLocalAddressLength, Int32 dwRemoteAddressLength, ref Int32 lpdwBytesReceived, ref OVERLAPPED overLapped);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "listen", SetLastError = true)]
        private static extern int listen(IntPtr s, int backlog);


        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "WSAGetLastError", SetLastError = true)]
        private static extern int WSAGetLastError();


        [DllImport("Ws2_32.dll", CharSet = CharSet.Unicode, EntryPoint = "WSAStringToAddressW", SetLastError = true)]
        private static extern int WSAStringToAddress(
                          string AddressString,
                          AfFamily AddressFamily,
                          IntPtr lpProtocolInfo,
                          MemorySafeHandle lpAddress,
                          ref uint lpAddressLength);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "sendto", SetLastError = true)]
        private static extern int sendto(IntPtr s, MemorySafeHandle buf, int len, int flags, ref SOCKADDR_IN to, UInt32 tolen);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "sendto", SetLastError = true)]
        private static extern int sendto(IntPtr s, MemorySafeHandle buf, int len, int flags, ref SOCKADDR_IN6 to, UInt32 tolen);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "send", SetLastError = true)]
        private static extern int send(IntPtr s, MemorySafeHandle buf, int len, int flags);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "setsockopt", SetLastError = true)]
        private static extern int setsockopt(IntPtr s, int level, int optname, ref ip_mreq optval, int optlen);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "setsockopt", SetLastError = true)]
        private static extern int setsockopt(IntPtr s, int level, int optname, ref ipv6_mreq optval, int optlen);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "setsockopt", SetLastError = true)]
        private static extern int setsockopt(IntPtr s, int level, int optname, ref int optval, int optlen);

        [DllImport("ws2_32.dll", SetLastError = true, ExactSpelling = true, EntryPoint = "WSACreateEvent")]
        private static extern IntPtr WSACreateEvent();

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "WSACloseEvent")]
        private static extern void WSACloseEvent(IntPtr hObject);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "WSAWaitForMultipleEvents", SetLastError = true)]
        private static extern int WSAWaitForMultipleEvents(int cEvents, [MarshalAs(UnmanagedType.LPArray)] IntPtr[] lphEvents, bool fWaitAll, int dwTimeout, bool fAlertable);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "WSARecv", SetLastError = true)]
        private static extern int WSARecv(IntPtr s, IntPtr wsaBuf, Int32 dwBufferCount, ref int lpNumberOfBytesRecvd, ref int flags, ref OVERLAPPED overLapped, IntPtr lpCompletionRoutine);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "WSARecv", SetLastError = true)]
        private static extern bool WSAGetOverlappedResult(IntPtr s, ref OVERLAPPED overLapped, out int lpcbTransfer, bool fWait, ref int lpdwFlags);

        [DllImport("ws2_32.dll", ExactSpelling = true, EntryPoint = "recv", SetLastError = true)]
        private static extern int recv(IntPtr s, MemorySafeHandle buf, int len, int flags);

        [DllImportAttribute("api-ms-win-core-synch-l1-2-0", CharSet = CharSet.Unicode)]
        extern static IntPtr CreateEvent(IntPtr lpEventAttributes, bool bManualReset, bool bInitalState, string lpName);

        [DllImportAttribute("api-ms-win-core-synch-l1-2-0")]
        extern static bool CloseHandle(IntPtr hObject);

        [DllImportAttribute("api-ms-win-core-synch-l1-2-0")]
        extern static bool SetEvent(IntPtr hEvent);

        [DllImportAttribute("api-ms-win-core-synch-l1-2-0")]
        extern static Int32 WaitForSingleObject(IntPtr hObject, int dwMilliseconds);

        [DllImport("API-MS-Win-Core-Synch-L1-1-0", ExactSpelling = true, EntryPoint = "WaitForMultipleObjects")]
        private static extern int WaitForMultipleObjects(int nCount, [MarshalAs(UnmanagedType.LPArray)] IntPtr[] lpHandles, bool bWaitAll, int dwTimeout);

        [DllImportAttribute("api-ms-win-core-synch-l1-2-0")]
        extern static Int32 GetOverlappedResult(IntPtr hFile, ref OVERLAPPED overLapped, out int lpNumberOfBytesTransferred, bool bWait);




        private const int MAX_ADAPTER_NAME_LENGTH = 256;
        private const int MAX_ADAPTER_ADDRESS_LENGTH = 8;
        private const int MAX_DHCPV6_DUID_LENGTH = 130;


        private enum AfFamily : uint
        {
            /// <summary>
            /// IPv4
            /// </summary>
            AF_INET = 2,
            /// <summary>
            /// IPv6
            /// </summary>
            AF_INET6 = 23,
            /// <summary>
            /// Unpecified. Includes both IPv4 and IPv4
            /// </summary>
            AF_UNSPEC = 0
        }

        [Flags]
        private enum GaaFlag : uint
        {
            GAA_FLAG_SKIP_UNICAST = 0x0001,
            GAA_FLAG_SKIP_ANYCAST = 0x0002,
            GAA_FLAG_SKIP_MULTICAST = 0x0004,
            GAA_FLAG_SKIP_DNS_SERVER = 0x0008,
            GAA_FLAG_INCLUDE_PREFIX = 0x0010,
            GAA_FLAG_SKIP_FRIENDLY_NAME = 0x0020
        }

        private enum ProtocolType : uint
        {
            IPPROTO_ICMP = 1,
            IPPROTO_IGMP = 2,
            BTHPROTO_RFCOMM = 3,
            IPPROTO_TCP = 6,
            IPPROTO_UDP = 17,
            IPPROTO_ICMPV6 = 58,
            IPPROTO_RM = 113
        }

        private enum SockType : uint
        {
            SOCK_STREAM = 1,
            SOCK_DGRAM = 2,
            SOCK_RAW = 3,
            SOCK_RDM = 4,
            SOCK_SEQPACKET = 5
        }

        [StructLayout(LayoutKind.Sequential)]
        public class WsaBuf
        {
            public UInt32 len;
            public IntPtr buf;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct OVERLAPPED
        {
            public IntPtr InternalLow;
            public IntPtr InternalHigh;
            public int OffsetLow;
            public int OffsetHigh;
            public IntPtr EventHandle;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IP_ADAPTER_ADDRESSES
        {
            public UInt64 Alignment;
            public IntPtr Next;
            [System.Runtime.InteropServices.MarshalAs(UnmanagedType.LPStr)]
            public string AdapterName;
            public IntPtr FirstUnicastAddress;
            public IntPtr FirstAnycastAddress;
            public IntPtr FirstMulticastAddress;
            public IntPtr FirstDnsServerAddress;
            [MarshalAs(UnmanagedType.LPWStr)]
            public string DnsSuffix;
            [System.Runtime.InteropServices.MarshalAs(UnmanagedType.LPWStr)]
            public string Description;
            [System.Runtime.InteropServices.MarshalAs(UnmanagedType.LPWStr)]
            public string FriendlyName;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = MAX_ADAPTER_ADDRESS_LENGTH)]
            public byte[] PhysicalAddress;
            public uint PhysicalAddressLength;
            public uint Flags;
            public uint Mtu;
            public uint IfType;
            public uint OperStatus;
            public uint Ipv6IfIndex;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            public uint[] ZoneIndices;
            public IntPtr FirstPrefix;
            public UInt64 TrasmitLinkSpeed;
            public UInt64 ReceiveLinkSpeed;
            public IntPtr FirstWinsServerAddress;
            public IntPtr FirstGatewayAddress;
            public uint Ipv4Metric;
            public uint Ipv6Metric;
            public UInt64 Luid;
            public SOCKET_ADDRESS Dhcpv4Server;
            public uint CompartmentId;
            public Guid NetworkGuid;
            public uint ConnectionType;
            public uint TunnelType;
            public SOCKET_ADDRESS Dhcpv6Server;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = MAX_DHCPV6_DUID_LENGTH)]
            public byte[] Dhcpv6ClientDuid;
            public uint Dhcpv6ClientDuidLength;
            public uint Dhcpv6Iaid;
            public uint FirstDnsSuffix;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IP_ADAPTER_UNICAST_ADDRESS
        {
            public UInt64 Alignment;
            public IntPtr Next;
            public SOCKET_ADDRESS Address;
            public uint PrefixOrigin;
            public uint SuffixOrigin;
            public uint DadState;
            public uint ValidLifetime;
            public uint PreferredLifetime;
            public uint LeaseLifetime;
        }


        [StructLayout(LayoutKind.Sequential)]
        private struct SOCKADDR_IN
        {
            /// u_short->unsigned short
            public ushort sa_family;
            public ushort port;
            public in_addr sin_addr;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public byte[] sa_data;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SOCKADDR_IN6
        {
            public ushort sa_family;
            public ushort port;
            public UInt32 sin6_flowinfo;
            public in6_addr sin_addr;
            public UInt32 sin6_scope_id;
        }


        [StructLayout(LayoutKind.Sequential)]
        private struct SOCKET_ADDRESS
        {
            public IntPtr lpSockAddr;
            public uint iSockaddrLength;
        }


        [StructLayout(LayoutKind.Sequential)]
        private struct ip_mreq
        {
            public in_addr imr_multiaddr;
            public in_addr imr_interface;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct ipv6_mreq
        {
            public in6_addr ipv6mr_multiaddr;
            public uint ipv6mr_interface;
        }

        [StructLayout(LayoutKind.Explicit, Size = 4)]
        private struct in_addr
        {
            [FieldOffset(0)]
            internal byte s_b1;
            [FieldOffset(1)]
            internal byte s_b2;
            [FieldOffset(2)]
            internal byte s_b3;
            [FieldOffset(3)]
            internal byte s_b4;

            [FieldOffset(0)]
            internal ushort s_w1;
            [FieldOffset(2)]
            internal ushort s_w2;

            [FieldOffset(0)]
            internal uint S_addr;

        }

        [StructLayout(LayoutKind.Explicit, Size = 16)]
        public struct in6_addr
        {
            [FieldOffset(0)]
            internal byte Byte_0;
            [FieldOffset(1)]
            internal byte Byte_1;
            [FieldOffset(2)]
            internal byte Byte_2;
            [FieldOffset(3)]
            internal byte Byte_3;

            [FieldOffset(4)]
            internal byte Byte_4;
            [FieldOffset(5)]
            internal byte Byte_5;
            [FieldOffset(6)]
            internal byte Byte_6;
            [FieldOffset(7)]
            internal byte Byte_7;

            [FieldOffset(8)]
            internal byte Byte_8;
            [FieldOffset(9)]
            internal byte Byte_9;
            [FieldOffset(10)]
            internal byte Byte_10;
            [FieldOffset(11)]
            internal byte Byte_11;

            [FieldOffset(12)]
            internal byte Byte_12;
            [FieldOffset(13)]
            internal byte Byte_13;
            [FieldOffset(14)]
            internal byte Byte_14;
            [FieldOffset(15)]
            internal byte Byte_16;

            [FieldOffset(0)]
            internal short Word_0;
            [FieldOffset(2)]
            internal short Word_1;
            [FieldOffset(4)]
            internal short Word_2;
            [FieldOffset(6)]
            internal short Word_3;

            [FieldOffset(8)]
            internal short Word_4;
            [FieldOffset(10)]
            internal short Word_5;
            [FieldOffset(12)]
            internal short Word_6;
            [FieldOffset(14)]
            internal short Word_7;
        }

        private class ListenerData
        {
            public IntPtr CancelHandle { get; private set; }
            public IntPtr FinishedHandle { get; private set; }
            public Task Task { get; private set; }
            public bool Ipv6 { get; set; }
            public int BytesRead { get; set; }
            public int PacketsRead { get; set; }
            public ListenerData(IntPtr cancelHandle, IntPtr finishedHandle, Task task)
            {
                this.CancelHandle = cancelHandle;
                this.FinishedHandle = finishedHandle;
                this.Task = task;
                this.Ipv6 = false;
                this.BytesRead = 0;
                this.PacketsRead = 0;
            }
        }

    }

    internal abstract class DataPathTestException : Exception
    {
        public abstract string TestFailureOverview
        {
            get;
        }

        public abstract string TestFailureDetail
        {
            get;
        }

        public DataPathTestException()
        {
        }

        public override string ToString()
        {
            StringBuilder returnString = new StringBuilder();
            returnString.Append(TestFailureOverview);
            returnString.Append("\n");
            returnString.Append(TestFailureDetail);
            return returnString.ToString();
        }
    }

    internal class TestConfigException : DataPathTestException
    {
        public override string TestFailureOverview
        {
            get
            {
                return "The test failed because of a test configuration issue.  This is most likely not a WLAN driver bug or a bug in the test itself.";
            }
        }

        private string testFailureDetail;
        public override string TestFailureDetail
        {
            get
            {
                return testFailureDetail;
            }
        }
        public TestConfigException(string failureDetail)
        {
            this.testFailureDetail = failureDetail;
        }
    }

    internal class Win32CallException : DataPathTestException
    {
        public override string TestFailureOverview
        {
            get
            {
                return "The test failed because of a Win32 function call failed in an unexpected way.  This may be a driver bug or a configuration issue.";
            }
        }

        private string testFailureDetail;
        public override string TestFailureDetail
        {
            get
            {
                return testFailureDetail;
            }
        }
        public Win32CallException(string win32Function)
        {
            this.testFailureDetail = string.Format(CultureInfo.InvariantCulture, "The test called {0} To triage this failure lookup the Win32 function {0} in MSDN.", win32Function);
        }
        public Win32CallException(string win32Function, int errorCode)
        {
            this.testFailureDetail = string.Format(CultureInfo.InvariantCulture, "The test called {0} which returned the error code {1}.  To triage this failure lookup the Win32 function {0} in MSDN and the subsequent error code.", win32Function, errorCode);
        }
        public Win32CallException(string win32Function, int errorCode, string notes)
        {
            this.testFailureDetail = string.Format(CultureInfo.InvariantCulture, "The test called {0} which returned the error code {1}.  To triage this failure lookup the Win32 function {0} in MSDN and the subsequent error code.  {2}", win32Function, errorCode, notes);


        }


    }

    internal class TestBugException : DataPathTestException
    {
        public override string TestFailureOverview
        {
            get
            {
                return "The test failed because of a test bug.  This is most likely not a WLAN driver bug or a configuration issue.";
            }
        }

        private string testFailureDetail;
        public override string TestFailureDetail
        {
            get
            {
                return testFailureDetail;
            }
        }
        public TestBugException(string failureDetail)
        {
            this.testFailureDetail = failureDetail;
        }
    }
    internal class DriverBugException : DataPathTestException
    {
        public override string TestFailureOverview
        {
            get
            {
                return "The test failed because of a driver bug.  This is most likely not a test issue or a configuration issue.";
            }
        }

        private string testFailureDetail;
        public override string TestFailureDetail
        {
            get
            {
                return testFailureDetail;
            }
        }
        public DriverBugException(string failureDetail)
        {
            this.testFailureDetail = failureDetail;
        }
    }
}
