using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LibJS.Intrinsics
{
    internal interface IIntrinsic
	{
		/// <summary>
		/// Registers the intrinsic to the document
		/// </summary>
		/// <param name="document"></param>
		public void Register(Document document);
	}
}
