///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
namespace HlkTest.Stress
{
    using System;
    using System.Text;
    using System.Diagnostics.CodeAnalysis;
    using System.Diagnostics;
    using System.Threading;
    using System.Net;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using WEX.Logging.Interop;
    using WEX.TestExecution;
    using WEX.TestExecution.Markup;
    using Microsoft.Test.Networking.Kit.Tracing;

    internal abstract class StressModule
    {
        public string Name { get; protected set; }
        protected Random Random { get; private set; }
        protected Collection<Guid> routers;
        protected WlanHckTestLogger testLogger;

        public StressModule(Random random, Collection<Guid> routers, WlanHckTestLogger testLogger)
        {
            this.Random = random;
            this.routers = routers;
            this.testLogger = testLogger;
        }

        public abstract void Run(string profileName1, string profileName2);
        public abstract void PrintResults();


    }
}