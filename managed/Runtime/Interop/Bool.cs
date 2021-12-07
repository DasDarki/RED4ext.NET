namespace RED4ext.NET.Runtime.Interop
{
    /// <summary>
    /// A bool struct which can be passed through the managed interface bridge without corruption.
    /// </summary>
    internal readonly struct Bool
    {
        private readonly byte _value;

        internal Bool(byte value) => _value = value;

        public static implicit operator bool(Bool value) => value._value != 0;

        public static implicit operator Bool(bool value) => !value ? new(0) : new(1);

        public override int GetHashCode() => _value.GetHashCode();
    }
}
