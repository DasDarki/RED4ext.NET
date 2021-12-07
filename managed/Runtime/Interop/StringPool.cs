using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace RED4ext.NET.Runtime.Interop
{
    /// <summary>
    /// An utility class which allows string functions which can be used for interoperating strings.
    /// </summary>
    internal static class StringPool
    {
        /// <summary>
        /// The thread-unique string buffer allocated for the next string communication.
        /// </summary>
        [ThreadStatic]
        private static byte[] _stringBuffer;

        /// <summary>
        /// Returns the existing or creates a new string buffer for the next communication.
        /// </summary>
        /// <returns>The string buffer</returns>
        internal static byte[] GetStringBuffer()
        {
            return _stringBuffer ??= GC.AllocateUninitializedArray<byte>(8192, true);
        }

        /// <summary>
        /// If there are any strings which has the wrong unicode, we can change the unicode marshalling here.
        /// </summary>
        internal static string PtrToString(IntPtr ptr)
        {
            return Marshal.PtrToStringUTF8(ptr);
        }

        /// <summary>
        /// Converting the given string value to an UTF8 byte array.
        /// </summary>
        /// <param name="value">The value to be converted</param>
        /// <returns>The converted byte array</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        internal static byte[] StringToBytes(this string value)
        {
            return value != null ? Encoding.UTF8.GetBytes(value) : null;
        }
    }
}
