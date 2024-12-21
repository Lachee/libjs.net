using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LibJS.Intrinsics
{
	internal interface IIntrinsic
	{
		public void Register(Document documnet);
	}
}
