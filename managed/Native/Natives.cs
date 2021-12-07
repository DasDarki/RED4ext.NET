using RED4ext.NET.Runtime;

namespace RED4ext.NET.Native
{
    /// <summary>
    /// A collection of native delegates for the access to the native unmanaged side.
    /// </summary>
    internal static unsafe partial class Natives
    {
        internal static void Initialize(BindingWrapper binding)
        {
            #region Utility

            binding.FetchArea();
            Utility.ConsoleLog = (delegate* unmanaged[Cdecl]<string, void>) binding.GetFunction();

            #endregion
        }
    }
}
