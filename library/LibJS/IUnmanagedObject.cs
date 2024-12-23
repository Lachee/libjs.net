using System;

namespace LibJS {
    public interface IUnmanagedObject : IDisposable {
        public IntPtr Ptr { get; }
    }
}