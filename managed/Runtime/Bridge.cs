using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Threading;
using RED4ext.NET.Native;
using RED4ext.NET.Runtime.Interop;

namespace RED4ext.NET.Runtime
{
    /// <summary>
    /// The bridge is connecting unit which manages the communication between the native layer and the managed layer.
    /// From here the API gets initialized and the managed process gets started.
    /// </summary>
    [SuppressUnmanagedCodeSecurity]
    internal static class Bridge
    {
        /// <summary>
        /// The absolute path the working directory, which is games root directory.
        /// </summary>
        internal static string WorkingDirectory { get; private set; }

        /// <summary>
        /// The id of the main threading. Can be used for main thread invocation assertion.
        /// </summary>
        private static int _mainThreadId;

        /// <summary>
        /// Starts the bridge and initializes the first managed parts.
        /// </summary>
        /// <param name="workingDirPtr">The pointer pointing to the working directory string</param>
        [UnmanagedCallersOnly]
        internal static void Start(IntPtr workingDirPtr)
        {
            WorkingDirectory = StringPool.PtrToString(workingDirPtr);

            Console.WriteLine("C# initialized! YEAH!!!!!!");
        }

        /// <summary>
        /// Stops the managed process and shuts down the bridge.
        /// </summary>
        [UnmanagedCallersOnly]
        internal static void Stop()
        {

        }

        /// <summary>
        /// Binds the functions in the array of the given pointer.
        /// </summary>
        /// <param name="functionsPtr">The pointer which points to the functions array</param>
        [UnmanagedCallersOnly]
        internal static void BindFunctions(IntPtr functionsPtr)
        {
            _mainThreadId = Thread.CurrentThread.ManagedThreadId;
            using BindingWrapper wrapper = new BindingWrapper(functionsPtr);
            Natives.Initialize(wrapper);
            Console.SetOut(new ConsoleLogger());
        }
    }
}
