using System;
using System.Runtime.InteropServices;

namespace Fuji
{
	public struct MFMatrix
	{
		public MFVector x;
		public MFVector y;
		public MFVector z;
		public MFVector t;

		public static readonly MFMatrix identity = new MFMatrix { x = MFVector.right, y = MFVector.up, z = MFVector.forward, t = MFVector.identity };
	}
}
