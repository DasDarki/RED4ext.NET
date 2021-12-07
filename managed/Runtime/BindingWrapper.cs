using System;
using System.Security;

namespace RED4ext.NET.Runtime
{
    /// <summary>
    /// The binding wrapper is used for binding the native functions to
    /// managed C# delegates.
    /// </summary>
    [SuppressUnmanagedCodeSecurity]
    internal unsafe class BindingWrapper : IDisposable
    {
        private bool _isDisposed;
        private int _currentHead;
        private int _currentPosition;
        private IntPtr* _currentArea;
        private readonly IntPtr* _areas;

        internal BindingWrapper(IntPtr pointer)
        {
            _areas = (IntPtr*)pointer;
        }

        /// <summary>
        /// Fetches a new area of the area array from the native side.
        /// The opposite of NewArea on the native side.
        /// </summary>
        public void FetchArea()
        {
            if (_isDisposed) return;
            _currentArea = (IntPtr*)_areas[_currentPosition++];
            _currentHead = 0;
        }

        /// <summary>
        /// Gets the function on the current position in the current area
        /// and returns the pointer to it.
        /// </summary>
        /// <returns>The pointer to the current function</returns>
        public IntPtr GetFunction()
        {
            return _isDisposed ? IntPtr.Zero : _currentArea[_currentHead++];
        }


        /// <summary>
        /// Disposes the wrapper.
        /// </summary>
        public void Dispose()
        {
            if (_isDisposed) return;
            _isDisposed = true;
        }
    }
}
