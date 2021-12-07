using System;
using System.IO;
using System.Text;
using RED4ext.NET.Native;

namespace RED4ext.NET.Runtime
{
    /// <summary>
    /// An internally used wrapper for the <see cref="TextWriter"/> class to allow redirecting of the output.
    /// </summary>
    internal class ConsoleLogger : TextWriter
    {
        public override Encoding Encoding => Encoding.UTF8;

        public override void Write(string value)
        {
            unsafe
            {
                Natives.Utility.ConsoleLog(value);
            }
        }

        public override void WriteLine(string value)
        {
            Write(value + Environment.NewLine);
        }
    }
}
