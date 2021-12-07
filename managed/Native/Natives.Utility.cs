namespace RED4ext.NET.Native
{
    /// <summary>
    /// This class is containing all utility natives.
    /// </summary>
    internal static unsafe partial class Natives
    {
        internal static class Utility
        {
            internal static delegate* unmanaged[Cdecl]<string, void> ConsoleLog;
        }
    }
}
