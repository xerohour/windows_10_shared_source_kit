///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    /// <summary>
    /// Wi-Fi Direct defined roles.
    /// </summary>
    public enum RoleType
    {
        None,
        Device,
        GroupOwner,
        Client,
        Max
    }

    /// <summary>
    /// Legacy defined roles.
    /// </summary>
    public enum LegacyRoleType
    {
        None,
        GroupOwner,
        STA,
        Max
    }

    /// <summary>
    /// Scenarios for establishing a Wi-Fi Direct session.
    /// </summary>
    public enum ScenarioType
    {
        GONegotiation,
        Invitation,
        JoinExistingGO,
        MultiPort,
        WiFiDScenario
    }

    /// <summary>
    /// Type of connection the device should stablish with an access point.
    /// </summary>
    public enum APConnectSetting
    {
        NoAP,
        PreAP24,
        PreAP5,
        PostAP24,
        PostAP5,
        None
    }

    /// <summary>
    /// Frequencies of access points.
    /// </summary>
    public enum APfrequencies
    {
        F_2_4Ghz,
        F_5_0Ghz
    }

    /// <summary>
    /// Width of the Wireless Channel.
    /// </summary>
    public enum ChannelWidth : uint
    {
        CW_20Mhz = 20,
        CW_40Mhz = 40,
        CW_80Mhz = 80,
        CW_160Mhz = 160
    }
}
