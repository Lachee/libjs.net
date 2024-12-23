using System;
using System.Collections;
using System.Collections.Generic;

namespace LibJS.Types
{
    public sealed class Array : Object, IEnumerable<Value>, IList<Value>
    {
        private const string LengthName = "length";

        public bool IsReadOnly => true;

        public int Count => GetCount();

        public Value this[int index]
        {
            get
            {
                if (index < 0 || index >= Count)
                    throw new IndexOutOfRangeException();
                return GetProperty(index);
            }
            set => throw new NotImplementedException();
        }

        internal Array(Value value) : base(value) {
            if (!value.IsArray)
                throw new InvalidTypeException("array");
        }

        /// <summary>
        /// Gets the number of elements in the array
        /// </summary>
        /// <returns></returns>
        public int GetCount()
        {
            var val = GetProperty(LengthName);
            return val.AsInt();
        }

        public IEnumerator<Value> GetEnumerator()
        {
            int count = GetCount();
            for (int i = 0; i < count; i++)
            {
                yield return GetProperty(i);
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public int IndexOf(Value item)
        {
            int count = GetCount();
            for (int i = 0; i < count; i++)
            {
                if (GetProperty(i) == item)
                    return i;
            }
            return -1;
        }

        public void Insert(int index, Value item)
        {
            throw new NotImplementedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

        public void Add(Value item)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public bool Contains(Value item)
        {
            return IndexOf(item) >= 0;
        }

        public void CopyTo(Value[] array, int arrayIndex)
        {
            throw new NotImplementedException();
        }

        public bool Remove(Value item)
        {
            throw new NotImplementedException();
        }
    }
}