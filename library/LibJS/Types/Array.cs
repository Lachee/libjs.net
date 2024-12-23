using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace LibJS.Types
{
    public sealed class Array : Object, IEnumerable<Value>, IList<Value>
    {
		[DllImport(Consts.LibraryName)] private static extern Value js_array_create(Value[] values, int length);

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
			if (array == null)
				throw new ArgumentNullException(nameof(array));
			if (arrayIndex < 0)
				throw new ArgumentOutOfRangeException(nameof(arrayIndex));
			if (array.Length - arrayIndex < Count)
				throw new ArgumentException("The number of elements in the source array is greater than the available space from arrayIndex to the end of the destination array.");

			for (int i = 0; i < Count; i++)
			{
				array[arrayIndex + i] = GetProperty(i);
			}
		}

        public bool Remove(Value item)
        {
            throw new NotImplementedException();
        }

		public static Array Create(params Value[] values) 
        {
            var value = js_array_create(values, values.Length);
            return new Array(value);
        }
    }
}